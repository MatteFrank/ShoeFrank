
#include <TCanvas.h>
#include <TError.h>
#include <TMath.h>

#include <TH2.h>
#include <TH1.h>

#include "TAGaction.hxx"
#include "TAGview.hxx"


//MC block
#include "TAMCntuEve.hxx"
#include "TAMCactNtuEve.hxx"


//Start counter
#include "TASTdatRaw.hxx"
#include "TASTactNtuMC.hxx"

//Beam Monitor
#include "TABMparGeo.hxx"
#include "TABMparCon.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuTrack.hxx"
#include "TABMactNtuMC.hxx"
#include "TABMactNtuTrack.hxx"
#include "TABMvieTrackFOOT.hxx"

//Vertex
#include "TAVTparMap.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
// #include "TAVTparCal.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
// #include "TAVTntuTrack.hxx"
// #include "TAVTntuVertex.hxx"
// #include "TAVTactNtuVertex.hxx"
// #include "TAVTactNtuVertexPD.hxx"
#include "TAVTactNtuMC.hxx"
#include "TAVTactNtuClusterF.hxx"
// #include "TAVTactNtuTrack.hxx"
// #include "TAVTactNtuTrackH.hxx"
// #include "TAVTactNtuTrackF.hxx"

//Inner tracker
#include "TAITparMap.hxx"
#include "TAITparGeo.hxx"
#include "TAITparConf.hxx"
#include "TAITparCal.hxx"
#include "TAITntuRaw.hxx"
#include "TAITntuCluster.hxx"
#include "TAITntuTrack.hxx"
#include "TAITactNtuMC.hxx"
#include "TAITactNtuClusterF.hxx"
#include "TAITactNtuTrackF.hxx"

//MicroStrip Detector
#include "TAMSDparGeo.hxx"
#include "TAMSDparConf.hxx"
#include "TAMSDparMap.hxx"
#include "TAMSDntuRaw.hxx"
#include "TAMSDactNtuMC.hxx"


//Tof Wall (scintillator)
#include "TATWparGeo.hxx"
#include "TATWparMap.hxx"
#include "TATWdatRaw.hxx"
#include "TATWntuRaw.hxx"
#include "TATWactNtuMC.hxx"

//Calorimeter
#include "TACAparGeo.hxx"
#include "TACAparMap.hxx"
#include "TACAntuRaw.hxx"
#include "TACAactNtuMC.hxx"

#include "foot_geo.h"

#include "Materials.hxx"
#include "FootField.hxx"
#include "MeasurementCreator.h"

#include <iostream>
#include <vector>



#include "Booter.hxx"

using namespace std;





//------------------------------------------------------------------------------
void Booter::Initialize( EVENT_STRUCT* evStr ) {


    // debug fie
    if ( GlobalPar::GetPar()->Debug() > 1 ) {
        eventListFile.open( ((string)getenv("FOOTFULLREC")+"/"+"eventListFile.dat").c_str(), fstream::trunc | fstream::out );
        if ( !eventListFile.is_open() )        cout<< "ERROR  -->  eventListFile.dat cannot open file."<< endl, exit(0);
    }

    gROOT->SetStyle("Plain");
    gStyle->SetFrameBorderMode(0);
    gStyle->SetStatW(0.2);                  // Set width of stat-box (fraction of pad size)
    gStyle->SetStatH(0.1);                  // Set height of stat-box (fraction of pad size)

	//Initializing the Geometry class that handles the
    //detector positioning and global to local transformations
    fGeoTrafo = new TAGgeoTrafo();   
    TString filename = m_wd + "/geomap/FOOT_geo.map";   // obsolete, to be removed carefully
    //fGeoTrafo->InitGeo(filename.Data()); // avoid to load an unexisting file !

	cout << "Make Geo" << endl;
    TGeoManager *masterGeo = new TGeoManager("genfitGeom", "GENFIT geometry");
    
	Materials* listMaterials = new Materials() ;
    if ( GlobalPar::GetPar()->Debug() > 1 ) {
       listMaterials->PrintCompMap();
       listMaterials->PrintMatMap();
    }

    top = gGeoManager->MakeBox("TOPPER", gGeoManager->GetMedium("AIR"), 25., 25., 120.);
    gGeoManager->SetTopVolume(top); // mandatory !


    string magFieldMapName = GlobalPar::GetPar()->MagFieldInputMapName();
    // genfit::FieldManager::getInstance()->init(new genfit::ConstField(0. ,10., 0.)); // 1 T
    // genfit::FieldManager::getInstance()->init(new genfit::ConstField(0. ,0., 0.)); // no mag
    // genfit::FieldManager::getInstance()->init( new FootField( 7 ) ); // const field
    genfit::FieldManager::getInstance()->init( new FootField(magFieldMapName.c_str()) ); // variable field

    // include the nucleon into the genfit pdg repository
    if ( GlobalPar::GetPar()->IncludeBM() || GlobalPar::GetPar()->IncludeKalman() )
        UpdatePDG::Instance();

    // Setting up the detectors that we want to decode.    
    // Initialization of detectors parameters, geometry and materials   
    if( GlobalPar::GetPar()->IncludeEvent() )           FillMCEvent(evStr);
    if( GlobalPar::GetPar()->IncludeBM() )              FillMCBeamMonitor(evStr);
    if( GlobalPar::GetPar()->IncludeIR() )              FillMCInteractionRegion(evStr);
    if( GlobalPar::GetPar()->IncludeInnerTracker() )    FillMCInnerTracker(evStr);
    if( GlobalPar::GetPar()->IncludeVertex() )          FillMCVertex(evStr);
    if( GlobalPar::GetPar()->IncludeMSD() )             FillMCMSD(evStr);
    if( GlobalPar::GetPar()->IncludeTW() )              FillMCTofWall(evStr);
    if( GlobalPar::GetPar()->IncludeCA() )              FillMCCalorimeter(evStr);

    // set material and geometry into genfit
    MaterialEffects* materialEffects = MaterialEffects::getInstance();
    materialEffects->init(new TGeoMaterialInterface());

    if ( GlobalPar::GetPar()->Debug() > 0 )       MagFieldTest();

    //--- draw the ROOT box
    GeoPrint();

    // Initialisation of KFfitter
    if ( GlobalPar::GetPar()->Debug() > 1 )       cout << "KFitter init!" << endl;
    m_kFitter = new KFitter();
    if ( GlobalPar::GetPar()->Debug() > 1 )       cout << "KFitter init done!" << endl;
    // // cluster test  -  myn_vtclus
    // pos2D = new TH2F( "pos2D", "pos2D", 500, -4, 4 , 500, -4, 4 );
    // pos2D = new TH2F( "pos2D", "pos2D", 500, -1, 1 , 500, -1, 1 );

}







//------------------------------------------------------------------------------
void Booter::Process( Long64_t jentry ) {


    if ( GlobalPar::GetPar()->IncludeBM() ) {
            MonitorBMNew(jentry); // Yun
    }

    

    // start time
    start_kal = clock();

    // Kalman Filter
    int isKalmanConverged = 0;
    if ( GlobalPar::GetPar()->IncludeKalman() ) {
         // check other tracking systems are enabled
        if ( GlobalPar::GetPar()->Debug() > 0 )         cout << "MakeFit" << endl;
        isKalmanConverged = m_kFitter->MakeFit( jentry );
        if ( GlobalPar::GetPar()->Debug() > 0 )         cout << "MakeFit done. Converged = " << isKalmanConverged << endl;

        if ( isKalmanConverged == 1 && GlobalPar::GetPar()->Debug() > 1 )    eventListFile << jentry<< endl;

    }


    // // cluster test  -  myn_vtclus
    // // TAVTntuCluster* ntup = (TAVTntuCluster*)myn_vtclus->Object();
    // TAVTntuCluster* ntupVT = (TAVTntuCluster*) gTAGroot->FindDataDsc("vtClus", "TAVTntuCluster")->Object();
    // // for (int nSensor = 0; nSensor < ntup->GetNSensors(); nSensor++) {   // over all sensors

    // for (int nSensor = 0; nSensor < 4; nSensor++) {   // over all sensors
    //     if ( m_debug > 0 )      
    //     // cout << "N vertex pixel in sensor " << nSensor << ": " << ntup->GetClustersN( nSensor ) << endl;

    //     for (int nPx = 0; nPx < ntupVT->GetClustersN( nSensor ); nPx++)  {     // over all pixels for each sensor
    //         // cout << "Cluster Test :: cluster number = " << ntup->GetClustersN(nSensor) << " and sensorID = " << ntup->GetCluster( nSensor, nPx )->GetSensorID() << endl;
    //         TClonesArray* arra = ntupVT->GetCluster( nSensor, nPx )->GetListOfPixels();
    //         for ( int n=0; n<arra->GetEntries(); n++ ) {
    //             TVector3 vPos = ( (TAVTntuHit*) arra->At(n) )->GetPixelPosition_footFrame();
    //             if ( nSensor == 0 )
    //                 ControlPlotsRepository::GetControlObject( "BooterProcess" )->SetClusterView( "clusterVT",vPos.x(), vPos.y() );

    //                 // pos2D->Fill( vPos.x(), vPos.y() );
    //         }
    //     }
    // }

    // // cluster test  -  myn_vtclus
    // // TAVTntuCluster* ntup = (TAVTntuCluster*)myn_vtclus->Object();
    // TAITntuCluster* ntup = (TAITntuCluster*) gTAGroot->FindDataDsc("itClus", "TAITntuCluster")->Object();
    // // for (int nSensor = 0; nSensor < ntup->GetNSensors(); nSensor++) {   // over all sensors

    // for (int nSensor = 0; nSensor < 32; nSensor++) {   // over all sensors
    //     // if ( m_debug > 0 )      
    //     // cout << "N vertex pixel in sensor " << nSensor << ": " << ntup->GetClustersN( nSensor ) << endl;

    //     for (int nPx = 0; nPx < ntup->GetClustersN( nSensor ); nPx++)  {     // over all pixels for each sensor
    //         // cout << "Cluster Test :: cluster number = " << ntup->GetClustersN(nSensor) << " and sensorID = " << ntup->GetCluster( nSensor, nPx )->GetSensorID() << endl;
    //         TClonesArray* arra = ntup->GetCluster( nSensor, nPx )->GetListOfPixels();
    //         for ( int n=0; n<arra->GetEntries(); n++ ) {
    //             TVector3 vPos = ( (TAITntuHit*) arra->At(n) )->GetPixelPosition_footFrame();
    //             if ( nSensor < 16 )
    //                 ControlPlotsRepository::GetControlObject( "BooterProcess" )->SetClusterView( "clusterIT",vPos.x(), vPos.y() );

    //                 // pos2D->Fill( vPos.x(), vPos.y() );
    //         }
    //     }
    // }



    // stop time
    end_kal = clock();
    m_tempo_kal+=(double)(end_kal-start_kal);

}






//------------------------------------------------------------------------------
void Booter::Finalize() {

    // // cluster test
    // TCanvas* quadrante = new TCanvas( "q","q", 1000, 800 );
    // pos2D->Draw("colz");
    // quadrante->SaveAs("cluster.png");


  	if ( GlobalPar::GetPar()->IncludeKalman() )      m_kFitter->Finalize();

    if ( GlobalPar::GetPar()->IsPrintOutputFile() )         
        ControlPlotsRepository::GetControlObject( "BooterFinalize" )->PrintOutputFile();
    else                        
        ControlPlotsRepository::GetControlObject( "BooterFinalize" )->PrintMap();
    
    if( GlobalPar::GetPar()->IsPrintOutputNtuple() )        
        ControlPlotsRepository::GetControlObject( "BooterFinalize" )->PrintOutputNtuple();


    if (GlobalPar::GetPar()->Debug() > 1)   eventListFile.close();

}









//------------------------------------------------------------------------------
void Booter::MagFieldTest() {

    FootField * ff = new FootField( GlobalPar::GetPar()->MagFieldInputMapName().c_str() );
    cout << endl << "Magnetic Field in kGauss test in 0,0,14 : ", genfit::FieldManager::getInstance()->getFieldVal( TVector3( 0,0,14 ) ).Print();
    cout << endl << "Magnetic no Field in kGauss test in 0,0,0 : ", genfit::FieldManager::getInstance()->getFieldVal( TVector3( 0,0,0 ) ).Print();
    // cout << "Total mag field on the FOOT axis (from 0 to 40 cm) = " << ff->IntegralField( 4000, 0, 40 ) << " kG*cm" << endl;
  //  cout << "Total mag field on the FOOT axis (from VT to MDS cm) = " << ff->IntegralField( 400, ((TAVTparGeo*) myp_vtgeo->Object())->GetSensorPosition(0).z(), ((TAMSDparGeo*) myp_msdgeo->Object())->GetLayerCenter(2).z() ) << " kG*cm" << endl;


    // print out of the magnetic field
    if ( GlobalPar::GetPar()->Debug() > 1 ) {
        ofstream bFieldTest;
        bFieldTest.open("bField.test", std::ofstream::out | std::ofstream::trunc );

        bFieldTest << setiosflags(ios::fixed) << setprecision(8);

        float zCoord = -29.75 + 14;     // global coordinates
        for (int k=0; k<120; k++) {
            float yCoord = -4.75;
            for (int j=0; j<20; j++) {
                float xCoord = -4.75;
                for (int i=0; i<20; i++) {

                    TVector3 vecB = genfit::FieldManager::getInstance()->getFieldVal( TVector3( xCoord, yCoord, zCoord ) );
                    // bFieldTest << vecB.x() << " " << vecB.y() << " " << vecB.z() << endl;
                    bFieldTest << " " << xCoord << " " << yCoord << " " << zCoord-14;
                    bFieldTest << "  " << genfit::FieldManager::getInstance()->getFieldVal( TVector3( xCoord, yCoord, zCoord ) ).x() / 10
                                << "  " << genfit::FieldManager::getInstance()->getFieldVal( TVector3( xCoord, yCoord, zCoord ) ).y() / 10
                                << "  " << genfit::FieldManager::getInstance()->getFieldVal( TVector3( xCoord, yCoord, zCoord ) ).z() / 10 << endl;
                    xCoord += 0.5;
                }
                yCoord += 0.5;
            }
            zCoord += 0.5;
        }
        bFieldTest.close();
    }


}





//------------------------------------------------------------------------------
void Booter::GeoPrint() {

    //--- close the geometry
    gGeoManager->CloseGeometry();

    //--- draw the ROOT box
    gGeoManager->SetVisLevel(10);

    // save an image of the foot geometry
    //top->Draw("ogl");
    TCanvas* mirror = new TCanvas("footGeometry", "footGeometry",  700, 700);
    // gGeoManager->GetMasterVolume()->Draw();
    // top->Draw("");
    top->Draw("ap");
    mirror->SaveAs("footGeometry.png");
    mirror->SaveAs("footGeometry.root");

    

    // save the geometry info in .root
    TFile *outfile = TFile::Open("genfitGeomFOOT.root","RECREATE");
    gGeoManager->Write();
        
    outfile->Close();

}







//----------------------------------------------------------------------------------------------------
void Booter::AssociateHitsToParticle() {

  //   // to be done for all particles ---  full revision
  //   // TAMCntuEve*  p_ntumceve = (TAMCntuEve*)   myn_mceve->GenerateObject();
  //   TAMCntuEve*  mcNtup = (TAMCntuEve*)   myn_mceve->Object();

  //   // // vector<int> FragIdxs;
  //   // for(int i=0; i<mcNtup->GetHitN(); i++){
  //   //     TAMCntuEveHit *myPart = mcNtup->Hit(i);

  //   //     int part_reg = myPart->Reg();

  //   //     //Require that particle is produced inside the TG
  //   //     if(part_reg == 3) {
  //   //         FragIdxs.push_back(i);
  //   //     }
  //   // }

  //   //Pixels stuff
  //   TAVTntuRaw*  vtxNtup = (TAVTntuRaw*)   myn_vtraw->GenerateObject();
  //   int tmp_vtxid(0);
  //   TAVTntuHit* vtxHit;

  //   //inner tracker stuff
  //   TAITntuRaw*  p_itnturaw = (TAITntuRaw*)   myn_itraw->GenerateObject();

  //   int tmp_itid(0);
  //   TAITntuHit* hitIT;

  //   // associate maybe a pointer instead?
  //   for(int i=0; i<mcNtup->GetHitN(); i++){

  //   //Require that particle is produced inside the TG
  //   //     if(part_reg == 3) {
  //   //         FragIdxs.push_back(i);
  //   //     }

  //   //Check VTX pixels
  //   for (int nSensor = 0; nSensor < ntup->GetNSensors(); nSensor++) {   // over all sensors

  //       for (int nPx = 0; nPx < ntup->GetPixelsN( nSensor, "mc_hit" ); nPx++)       // over all pixels for each sensor

  //           vtxHit = p_nturaw->GetPixel( nSensor, nPx );
  //           tmp_vtxid = vtxHit->GetMCid(); 

  //       //     if(tmp_vtxid == FragIdxs.at(t_frg)){

  //       // if(m_debug) cout<<" Vtx hit associated to part "<<t_frg<<" That is a:: "<<mcNtup->Hit(t_frg)->FlukaID()<<"and has charge, mass:: "<<mcNtup->Hit(t_frg)->Chg()<<" "<<mcNtup->Hit(t_frg)->Mass()<<" "<<endl;
  //       //   }
  //   }

  //   //Check IT pixels
  //   for(int i=0; i<p_itnturaw->GetPixelsN(0); i++){
  //     hitIT = p_itnturaw->GetPixel(0,i);
  //     tmp_itid = hitIT->GetMCid()-1;
  //     if(tmp_itid == FragIdxs.at(t_frg)){
  //   if(m_debug) cout<<" IT hit associated to part "<<t_frg<<" That is a:: "<<mcNtup->Hit(t_frg)->FlukaID()<<"and has charge, mass:: "<<mcNtup->Hit(t_frg)->Chg()<<" "<<mcNtup->Hit(t_frg)->Mass()<<" "<<endl;
  //     }
  //   }

  // }//Loop on fragments

}

// void Booter::DisplayIRMonitor(TAGpadGroup* pg, EVENT_STRUCT *myStr) {}






// //----------------------------------------------------------------------------------------------------
// void Booter::DisplayBeamMonitor(TAGpadGroup* pg) {

//   TCanvas *c_bmhview;
//   c_bmhview = new TCanvas("bmhview", "Beam Monitor - horizontal view",20,20,700,900);
//   pg->AddPad(c_bmhview);

//   TAGview* pbmh_view = new TABMvieTrackFOOT(myn_bmtrk,
// 					     myn_bmraw,
// 					     myp_bmgeo);
//   pbmh_view->Draw();
// }






//----------------------------------------------------------------------------------------------------
void Booter::FillMCEvent(EVENT_STRUCT *myStr) {

  /*Ntupling the general MC event information*/
  myn_mceve    = new TAGdataDsc("myn_mceve", new TAMCntuEve());
  mya_mceve    = new TAMCactNtuEve("mya_mceve", myn_mceve, myStr);
  
  gTAGroot->AddRequiredItem("mya_mceve");
  // gTAGroot->AddRequiredItem("myn_mceve");

  // my_out->SetupElementBranch(myn_mceve,     "mceve.");

  /*Ntupling the general MC mimosa information*/
  // myn_mcmimo    = new TAGdataDsc("myn_mcmimo", new TAGntuMCmimo());
  // new TAGactNtuMCmimo("an_mcmimo", myn_mcmimo, myStr);
  // my_out->SetupElementBranch(myn_mcmimo,     "mcmimo.");

}





//----------------------------------------------------------------------------------------------------
void Booter::FillMCInteractionRegion(EVENT_STRUCT *myStr) {

    // Geo to be defined ...  
    // m_irgeo->InitGeo();
    // top->AddNode( m_irgeo->GetVolume(), 0, new TGeoCombiTrans( 0, 0,  m_irgeo->GetCenter().z(), new TGeoRotation("Marghe",0,0,0)) );-}

    /*Ntupling the MC Beam Monitor information*/
    myn_scraw    = new TAGdataDsc("myn_scraw", new TASTdatRaw());
    new TASTactNtuMC("an_scraw", myn_scraw, myStr);

    gTAGroot->AddRequiredItem("myn_scraw");

}






//----------------------------------------------------------------------------------------------------
void Booter::FillMCBeamMonitor(EVENT_STRUCT *myStr) {

  /*Ntupling the MC Beam Monitor information*/
  myn_bmraw    = new TAGdataDsc("myn_bmraw", new TABMntuRaw());
  myp_bmcon  = new TAGparaDsc("myp_bmcon", new TABMparCon());

  initBMCon(myp_bmcon);

  myp_bmgeo  = new TAGparaDsc("p_bmgeo", new TABMparGeo());

  initBMGeo(myp_bmgeo);

  // ?? @ Yun, serve ??
  new TABMactNtuMC("an_bmraw", myn_bmraw, myp_bmcon, myp_bmgeo, myStr);

  // my_out->SetupElementBranch(myn_bmraw,     "bmrh.");

  myn_bmtrk    = new TAGdataDsc("myn_bmtrk", new TABMntuTrack());

  new TABMactNtuTrack("an_bmtrk", myn_bmtrk, myn_bmraw, myp_bmgeo, myp_bmcon);

  // my_out->SetupElementBranch(myn_bmtrk,     "bmtrk.");

}










//----------------------------------------------------------------------------------------------------
void Booter::FillMCVertex(EVENT_STRUCT *myStr) {

    myp_vtgeo    = new TAGparaDsc("vtGeo", new TAVTparGeo());  // put fist!!!!!!!
    ((TAVTparGeo*) myp_vtgeo->Object())->FromFile();
  //  top->AddNode( ((TAVTparGeo*) myp_vtgeo->Object())->GetVolume(), 0, new TGeoCombiTrans( 0,0,0,new TGeoRotation("Vertex",0,0,0)) );
    
    /*Ntupling the MC Vertex information*/
    myn_vtraw    = new TAGdataDsc("vtRaw", new TAVTntuRaw());
    myn_vtclus   = new TAGdataDsc("vtClus", new TAVTntuCluster());
    // myn_vtrk     = new TAGdataDsc("vtTrack", new TAVTntuTrack());


    myp_vtconf  = new TAGparaDsc("vtConf", new TAVTparConf());
    TAVTparConf* parconf = (TAVTparConf*) myp_vtconf->Object();
    TString filename = m_wd + "/config/TAVTdetector.cfg";
    parconf->FromFile(filename.Data());

    
    mya_vtraw   = new TAVTactNtuMC("vtActRaw", myn_vtraw, myp_vtgeo, myStr);
    mya_vtclus = new TAVTactNtuClusterF("vtxActCluster", myn_vtraw, myn_vtclus, myp_vtconf, myp_vtgeo);
 
    gTAGroot->AddRequiredItem("vtRaw");
   gTAGroot->AddRequiredItem("vtClus");
}






//----------------------------------------------------------------------------------------------------
void Booter::FillMCInnerTracker(EVENT_STRUCT *myStr) {

    myp_itgeo    = new TAGparaDsc("itGeo", new TAITparGeo());
    ((TAITparGeo*) myp_itgeo->Object())->FromFile();
    // TVector3 transf (0,0,0);    ((TAITparGeo*) myp_itgeo->Object())->Local2Global( &transf );
    // top->AddNode( ((TAITparGeo*) myp_itgeo->Object())->GetVolume(), 0, new TGeoCombiTrans( transf.x(),transf.y(),transf.z(), new TGeoRotation("InnerTracker",0,0,0)) );
   // top->AddNode( ((TAITparGeo*) myp_itgeo->Object())->GetVolume(), 0, new TGeoCombiTrans( 0, 0,  0, new TGeoRotation("InnerTracker",0,0,0)) );

    // ((TAITparGeo*) myp_itgeo->Object())->PrintBodies("geppo");
    // ((TAITparGeo*) myp_itgeo->Object())->PrintRegions("geppo");
    // ((TAITparGeo*) myp_itgeo->Object())->PrintAssignMaterial("geppo");

    
    /*Ntupling the MC Vertex information*/
    myn_itraw    = new TAGdataDsc("itRaw", new TAITntuRaw());
    myn_itclus   = new TAGdataDsc("itClus", new TAITntuCluster());

    // remove?
    myp_itconf  = new TAGparaDsc("itConf", new TAITparConf());
    TAITparConf* parconf = (TAITparConf*) myp_itconf->Object();
    TString filename = m_wd + "/config/TAITdetector.cfg";
    parconf->FromFile(filename.Data());

    mya_itraw   = new TAITactNtuMC("itActRaw", myn_itraw, myp_itgeo, myStr);
    mya_itclus = new TAITactNtuClusterF("itActCluster", myn_itraw, myn_itclus, myp_itconf, myp_itgeo);

    gTAGroot->AddRequiredItem("itRaw");
    gTAGroot->AddRequiredItem("itClus");

}







//----------------------------------------------------------------------------------------------------
void Booter::FillMCMSD(EVENT_STRUCT *myStr) {

    myp_msdgeo    = new TAGparaDsc("msdGeo", new TAMSDparGeo());
    ((TAMSDparGeo*) myp_msdgeo->Object())->FromFile();
    // TVector3 transf (0,0,0);    ((TAMSDparGeo*) myp_msdgeo->Object())->Local2Global( &transf );
    // top->AddNode( ((TAMSDparGeo*) myp_msdgeo->Object())->GetVolume(), 0, new TGeoCombiTrans( transf.x(),transf.y(),transf.z(), new TGeoRotation("Strip",0,0,0)) );
  //  top->AddNode( ((TAMSDparGeo*) myp_msdgeo->Object())->GetVolume(), 0, new TGeoCombiTrans( 0, 0,  0, new TGeoRotation("Strip",0,0,0)) );

    /*Ntupling the MC Vertex information*/
    myn_msdraw    = new TAGdataDsc("msdRaw", new TAMSDntuRaw());
    // myn_msdclus   = new TAGdataDsc("msdClus", new TAMSDntuCluster());

//    myp_msdmap    = new TAGparaDsc("msdMap", new TAMSDparMap());

    myp_msdconf  = new TAGparaDsc("msdConf", new TAMSDparConf());
    TAMSDparConf* parconf = (TAMSDparConf*) myp_msdconf->Object();
    TString filename = m_wd + "/config/TAMSDdetector.cfg";
    parconf->FromFile(filename.Data());

    mya_msdraw   = new TAMSDactNtuMC("msdActRaw", myn_msdraw, myp_msdgeo, myStr);
    gTAGroot->AddRequiredItem("msdRaw");

}







//----------------------------------------------------------------------------------------------------
void Booter::FillMCTofWall(EVENT_STRUCT *myStr) {

    myp_twgeo    = new TAGparaDsc("twGeo", new TATWparGeo());
    ((TATWparGeo*) myp_twgeo->Object())->FromFile();
    // TVector3 transf (0,0,0);    ((TATWparGeo*) myp_twgeo->Object())->Local2Global( &transf );
    // top->AddNode( ((TATWparGeo*) myp_twgeo->Object())->GetVolume(), 0, new TGeoCombiTrans( transf.x(),transf.y(),transf.z(), new TGeoRotation("Scint",0,0,0)) );
  //  top->AddNode( ((TATWparGeo*) myp_twgeo->Object())->GetVolume(), 0, new TGeoCombiTrans( 0, 0, 0, new TGeoRotation("Scint",0,0,0)) );

    /*Ntupling the MC Tof Wall information*/
    containerHit    = new TAGdataDsc("containerHit", new TATWntuRaw());
   

    new TATWactNtuMC("an_twraw", containerHit, myStr);

    // gTAGroot->AddRequiredItem("myn_twraw");
    gTAGroot->AddRequiredItem("containerHit");
    gTAGroot->AddRequiredItem("containerPoint");
    gTAGroot->AddRequiredItem("an_twraw");   // prova --> funge!!!

    // my_out->SetupElementBranch(myn_twraw,     "twrh.");
}






//----------------------------------------------------------------------------------------------------
void Booter::FillMCCalorimeter(EVENT_STRUCT *myStr) {

    // Geo to be defined ...  
    // m_cageo->InitGeo();
    // top->AddNode( m_cageo->GetVolume(), 0, new TGeoCombiTrans( 0, 0,  m_cageo->GetCenter().z(), new TGeoRotation("Strip",0,0,0)) );

   myp_cageo    = new TAGparaDsc("caGeo", new TACAparGeo());

   
    /*Ntupling the MC Calorimeter information*/
    myn_caraw    = new TAGdataDsc("myn_caraw", new TACAntuRaw());

    new TACAactNtuMC("an_caraw", myn_caraw, myp_cageo, myStr);
    // my_out->SetupElementBranch(myn_caraw,     "carh.");

    gTAGroot->AddRequiredItem("myn_caraw");
}









void Booter::MonitorBM() {}
//Yun new graphs:
void Booter::MonitorBMNew(Long64_t jentry) {}
// void Booter::MonitorBMVTMat() {}
// void Booter::CalibBMVT() {}






//----------------------------------------------------------------------------------------------------
void Booter::initBMCon(TAGparaDsc* beamcon)  {

  Int_t i_run = gTAGroot->CurrentRunNumber();
  Int_t i_cam = gTAGroot->CurrentCampaignNumber();

  cout << "Loading beamcon for cam/run = " << i_cam << "/" << i_run << endl;

  TABMparCon* o_beamcon = (TABMparCon*) beamcon->Object();

  o_beamcon->Clear();

  Bool_t b_bad = kTRUE;

  TString filename = m_wd + "/config/TABMdetector.cfg";

  cout << "   from file " << filename << endl;

  b_bad = o_beamcon->FromFile(filename);

//  filename = m_wd + "/config/beammonitor_t0s.cfg";
//
//  o_beamcon->loadT0s(filename);
//
//  filename = m_wd + "/config/file_stlist_FIRST.txt";
//  //  filename = "config/file_stlist_8020_Cst1_1750.txt";
//
//  o_beamcon->LoadSTrel(filename);
//
//  o_beamcon->SetIsMC(true);
//
//  o_beamcon->ConfigureTrkCalib();


  if (!b_bad) beamcon->SetBit(TAGparaDsc::kValid);

  return;
}


void Booter::initBMGeo(TAGparaDsc* p_bmgeo)  {

  p_bmgeo = gTAGroot->FindParaDsc("p_bmgeo", "TABMparGeo");
  if (p_bmgeo == 0) {
    cout << "p_bmgeo not found or holding wrong parameter object type" << endl;
    return;
  }

  Int_t i_run = gTAGroot->CurrentRunNumber();
  Int_t i_cam = gTAGroot->CurrentCampaignNumber();

  Info("Booter::initBMGeo","Loading p_bmgeo for cam/run = %d / %d",i_cam,i_run);

  TABMparGeo* o_bmgeo = (TABMparGeo*) p_bmgeo->Object();

  //Initialization of BM parameters
  o_bmgeo->InitGeo();

  p_bmgeo->SetBit(TAGparaDsc::kValid);

  return;

}


void Booter::bookHisto(TFile *f) {}





































