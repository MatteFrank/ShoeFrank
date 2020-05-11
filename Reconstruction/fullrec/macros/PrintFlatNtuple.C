

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>

#include "TAMCntuEve.hxx"
#include "TAMCntuHit.hxx"


#include "TASTparGeo.hxx"
#include "TASTntuRaw.hxx"


#include "TABMparGeo.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuHit.hxx"
#include "TABMntuTrack.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAITparGeo.hxx"
#include "TAITntuCluster.hxx"

#include "TAMSDparGeo.hxx"
#include "TAMSDntuCluster.hxx"

#include "TATWparGeo.hxx"
#include "TATW_ContainerPoint.hxx"
#include "TATW_Point.hxx"


#include "TAGgeoTrafo.hxx"

//#include "KFitter.hxx"

#endif

// main
void PrintFlatNtuple(TString nameFile = "", Int_t nentries = 0)

{
  //  bool refittrack = false;
  // bool  printntuple = true;

  TAGroot gTAGroot;
  
  TAGgeoTrafo *fGeoTrafo = new TAGgeoTrafo();   
  fGeoTrafo->FromFile();
  

 
   TAGparaDsc* parGeoSt = new TAGparaDsc(TASTparGeo::GetDefParaName(), new TASTparGeo());
   TASTparGeo* stparGeo = (TASTparGeo*)parGeoSt->Object();
   stparGeo->FromFile();
  
   
   TAGparaDsc* parGeoBm = new TAGparaDsc(TABMparGeo::GetDefParaName(), new TABMparGeo());
   TABMparGeo* bmparGeo = (TABMparGeo*)parGeoBm->Object();
   bmparGeo->FromFile();
   
   
   TAGparaDsc* parGeoVtx = new TAGparaDsc(TAVTparGeo::GetDefParaName(), new TAVTparGeo());
   TAVTparGeo* vtparGeo = (TAVTparGeo*)parGeoVtx->Object();
   vtparGeo->FromFile();
  

   TAGparaDsc* parGeoIt = new TAGparaDsc(TAITparGeo::GetDefParaName(), new TAITparGeo());
   TAITparGeo* itparGeo = (TAITparGeo*)parGeoIt->Object();
   itparGeo->FromFile();
   
   TAGparaDsc* parGeoMsd = new TAGparaDsc(TAMSDparGeo::GetDefParaName(), new TAMSDparGeo());
   TAMSDparGeo* msdparGeo = (TAMSDparGeo*)parGeoMsd->Object();
   msdparGeo->FromFile();
   
   TAGparaDsc* parGeoTw = new TAGparaDsc(TATWparGeo::GetDefParaName(), new TATWparGeo());
   TATWparGeo* twparGeo = (TATWparGeo*)parGeoTw->Object();
   twparGeo->FromFile();

  
   TTree *tree = 0;
   TFile *f = new TFile(nameFile.Data());
   tree = (TTree*)f->Get("tree");
   
   
   TASTntuRaw *stHit = new TASTntuRaw();
   tree->SetBranchAddress(TASTntuRaw::GetBranchName(), &stHit);
   
   TABMntuRaw*  bmHit = new TABMntuRaw(); 
   tree->SetBranchAddress(TABMntuRaw::GetBranchName(), &bmHit);
   
   TABMntuTrack*  bmTrack = new TABMntuTrack();
   tree->SetBranchAddress(TABMntuTrack::GetBranchName(), &bmTrack);

//    TAVTntuCluster *vtClus = new TAVTntuCluster();
// // vtClus->SetParGeo(vtparGeo);
//    tree->SetBranchAddress(TAVTntuCluster::GetBranchName(), &vtClus);
  
   TAITntuCluster *itClus = new TAITntuCluster();
   tree->SetBranchAddress(TAITntuCluster::GetBranchName(), &itClus);
   
   TAMSDntuCluster *msdClus = new TAMSDntuCluster();
   tree->SetBranchAddress(TAMSDntuCluster::GetBranchName(), &msdClus);


   // TATW_ContainerPoint *twpoint = new TATW_ContainerPoint();
   // tree->SetBranchAddress(TATW_ContainerPoint::GetBranchName(), &twpoint);

 
   TAVTntuVertex* vtx      = new TAVTntuVertex();
   tree->SetBranchAddress(TAVTntuVertex::GetBranchName(), &vtx);
   


   TAVTntuTrack *vttrack = new TAVTntuTrack();
   tree->SetBranchAddress(TAVTntuTrack::GetBranchName(), &vttrack);
   TAGdataDsc* track = new TAGdataDsc("vttrack", vttrack);
   

   TAMCntuEve *eve = new TAMCntuEve();
   tree->SetBranchAddress(TAMCntuEve::GetBranchName(), &eve);
   
  
   TAMCntuHit *vtMc = new TAMCntuHit();
   tree->SetBranchAddress(TAMCntuHit::GetVtxBranchName(), &vtMc);
   
   if (nentries == 0)
     nentries = tree->GetEntries();
   // printf("Processed Events: %d\n", ev);
   
   // KFitter* m_kFitter = new KFitter();
   
   

   
   vector<float> st_charge, st_time, st_pos_x, st_pos_y, st_pos_z;
   vector<TVector3> st_pos;
   
   vector<int> bm_trkind;
   vector<TVector3> bm_Pvers, bm_R0;
   vector<float> bm_Pvers_x, bm_Pvers_y, bm_Pvers_z;
   vector<float> bm_R0_x, bm_R0_y, bm_R0_z, bmtrk_chi2;

   vector<TVector3> vtx_coll;
   vector<float> vtx_x, vtx_y, vtx_z;


   vector<double> mc_trk_charge, mc_trk_mass, mc_trk_tof, mc_trk_length;
   vector<float> mc_trk_intpos_x,  mc_trk_intpos_y,  mc_trk_intpos_z;
   vector<float> mc_trk_finpos_x,  mc_trk_finpos_y,  mc_trk_finpos_z;
   
   vector<int> trk_ind;
   vector<TVector3>  trk_vtx_clus, trk_slopez, trk_origin;
   vector<vector<TVector3> > trk_vtx_clus_2;
  
   vector<int> trk_vtx_clus_n, trk_vtx_clus_tothit;
   vector<float>  trk_vtx_clus_x, trk_vtx_clus_y, trk_vtx_clus_z, trk_chi2; 
   
   Int_t nsthit;
   Int_t nbmtrack;
   Int_t nvtx;
   Int_t nvttrack;
   Int_t mctrack;


   Int_t pos = nameFile.Last('.');
   TString nameOut = nameFile(0, pos);  
   nameOut.Append("_rob.root");
  

   TFile *file_out = new TFile(nameOut,"RECREATE");
   TTree *tree_out = new TTree("OuTree","Reco Event Tree");
   tree_out->Branch("st_time",             &st_time);
   tree_out->Branch("st_charge",           &st_charge);
   tree_out->Branch("st_hit_n",            &nsthit   );
   tree_out->Branch("st_pos_x",            &st_pos_x );
   tree_out->Branch("st_pos_y",            &st_pos_y );
   tree_out->Branch("st_pos_z",            &st_pos_z );
   tree_out->Branch("bm_trk_n",            &nbmtrack );
   tree_out->Branch("bm_trk_chi2",         &bmtrk_chi2 );
   tree_out->Branch("bm_Pvers_x",          &bm_Pvers_x );
   tree_out->Branch("bm_Pvers_y",          &bm_Pvers_y );
   tree_out->Branch("bm_Pvers_z",          &bm_Pvers_z );
   tree_out->Branch("bm_R0_x",             &bm_R0_x );
   tree_out->Branch("bm_R0_y",             &bm_R0_y );
   tree_out->Branch("bm_R0_z",             &bm_R0_z );
   tree_out->Branch("vertex_n",            &nvtx );
   tree_out->Branch("vertex_x",            &vtx_x );
   tree_out->Branch("vertex_y",            &vtx_y );
   tree_out->Branch("vertex_z",            &vtx_z );
   tree_out->Branch("vt_trk_n",            &nvttrack );
   tree_out->Branch("vt_trk_chi2",         &trk_chi2 );
   tree_out->Branch("vt_trk_slopez",       &trk_slopez );
   tree_out->Branch("vt_trk_origin",       &trk_origin );
   tree_out->Branch("vt_trk_clus",         &trk_vtx_clus_n ); 
   tree_out->Branch("vt_trk_clus_hits",    &trk_vtx_clus_tothit); 
   tree_out->Branch("vt_trk_clus_x",       &trk_vtx_clus_x );
   tree_out->Branch("vt_trk_clus_y",       &trk_vtx_clus_y );
   tree_out->Branch("vt_trk_clus_z",       &trk_vtx_clus_z );
   tree_out->Branch("mc_trk_n",            &mctrack );
   tree_out->Branch("mc_trk_charge",       &mc_trk_charge );
   tree_out->Branch("mc_trk_mass",         &mc_trk_mass );
   tree_out->Branch("mc_trk_tof",          &mc_trk_tof );
   tree_out->Branch("mc_trk_length",       &mc_trk_length );
   tree_out->Branch("mc_trk_intpos_x",     &mc_trk_intpos_x );
   tree_out->Branch("mc_trk_intpos_y",     &mc_trk_intpos_y );
   tree_out->Branch("mc_trk_intpos_z",     &mc_trk_intpos_z );
   tree_out->Branch("mc_trk_finpos_x",     &mc_trk_finpos_x );
   tree_out->Branch("mc_trk_finpos_y",     &mc_trk_finpos_y );
   tree_out->Branch("mc_trk_finpos_z",     &mc_trk_finpos_z );
 
   
   
   for (Int_t ev = 0; ev < nentries; ++ev) {
     if(ev%100==0) printf("Processed Events: %d\n", ev);
     
     stHit->Clear();
     bmHit->Clear();
     bmTrack->Clear();

     //vtClus->Clear();
     itClus->Clear();
     msdClus->Clear();

     
     // twpoint->Clear();
     vttrack->Clear();
     vtx->Clear();
     
      
     vtMc->Clear();
     eve->Clear();
     
     
     tree->GetEntry(ev);
     
     st_charge.clear();
     st_time.clear();
     st_pos.clear();
     st_pos_x.clear();
     st_pos_y.clear();
     st_pos_z.clear();
     
     bm_trkind.clear();
     bmtrk_chi2.clear();
     bm_Pvers.clear();
     bm_R0.clear();
     bm_Pvers_x.clear();
     bm_Pvers_y.clear(); 
     bm_Pvers_z.clear();
     bm_R0_x.clear();
     bm_R0_y.clear(); 
     bm_R0_z.clear();
     

     vtx_coll.clear();
     vtx_x.clear();
     vtx_y.clear();
     vtx_z.clear();


     mc_trk_charge.clear(); 
     mc_trk_mass.clear();
     mc_trk_tof.clear();
     mc_trk_length.clear();
     mc_trk_intpos_x.clear();
     mc_trk_intpos_y.clear();
     mc_trk_intpos_z.clear();
     mc_trk_finpos_x.clear();
     mc_trk_finpos_y.clear();
     mc_trk_finpos_z.clear();
   

     trk_ind.clear();
     trk_vtx_clus.clear();
     trk_vtx_clus_2.clear();
     trk_vtx_clus_n.clear();
     trk_vtx_clus_tothit.clear();
     trk_chi2.clear();
     trk_slopez.clear();
     trk_origin.clear();
     trk_vtx_clus_x.clear();
     trk_vtx_clus_y.clear();
     trk_vtx_clus_z.clear();


     
      // Int_t nPlanes = vtparGeo->GetSensorsN();
     // cout << " iPlane  " <<   nPlanes  << endl;
      
     
     // for( Int_t iPlane = 0; iPlane < nPlanes; iPlane++) {
       
     //   Int_t nclus = vtClus->GetClustersN(iPlane);
     //   //totClus += nclus;
     //   cout << " nclus " <<  nclus << endl;
     //   if (nclus == 0) continue;
       
     //   for (Int_t iClus = 0; iClus < nclus; ++iClus) {
     // 	 TAVTcluster *clus = vtClus->GetCluster(iPlane, iClus);
     // 	 if (!clus->IsValid()) continue;
	 
	 
	 
     // 	 TVector3 pos = clus->GetPositionG();
     // 	 cout << "pos0 " << pos.X()  << endl;
     // 	 cout << "pos1 " << pos.Y()  << endl;
     // 	 cout << "pos2 " << pos.Z()  << endl;
	 
	 
     //   }
     // }
     
    


     Int_t       nstHits  = stHit->GetHitsN();
     
     //hits
     for (Int_t i = 0; i < nstHits; i++) {
       
       TASTntuHit* hit = stHit->Hit(i);
       Float_t charge = hit->GetCharge();
       Float_t time = hit->GetTime();
       
       TVector3 posHit(0,0,0); // center
       
       TVector3 posHitG = fGeoTrafo->FromSTLocalToGlobal(posHit);
       
       //  cout << " time  " << time << endl;
       // cout << " charge  " << charge << endl;
       // cout << " posHitG x " <<  posHitG.X() << endl;
       // cout << " posHitG y " <<  posHitG.Y() << endl;
       // cout << " posHitG z " <<  posHitG.Z() << endl;
   
       st_charge.push_back(charge);
       st_time.push_back(time);
       st_pos.push_back(posHitG);
 }
  
  

 
    Int_t       nbmHits  = bmHit->GetHitsN();

    // cout << " nbmHits   " << nbmHits  << endl;
   // //hits
    for (Int_t i = 0; i < nbmHits; i++) {
      TABMntuHit* hit = bmHit->Hit(i);
      
      Int_t view  = hit->View();
      Int_t lay  = hit->Plane();
      Int_t cell  = hit->Cell();
      
      Float_t x = bmparGeo->GetWireX(bmparGeo->GetSenseId(cell),lay,view);
      Float_t y = bmparGeo->GetWireY(bmparGeo->GetSenseId(cell),lay,view);
      Float_t z = bmparGeo->GetWireZ(bmparGeo->GetSenseId(cell),lay,view);
      
      TVector3 posHit(x, y, z);
      
      // cout << "poshit0 " << posHit.X()  << endl;
      // cout << "poshit1 " << posHit.Y()  << endl;
      // cout << "poshit2 " << posHit.Z()  << endl;
      
      
    }       

   
  
   ///Track in BM
  int  Nbmtrack = bmTrack->GetTracksN();
  // cout << " ntrack  BM  " << Nbmtrack  << endl;
  if( bmTrack->GetTracksN() > 0 ) {
    for( Int_t iTrack = 0; iTrack < bmTrack->GetTracksN(); ++iTrack ) {
      
      bm_trkind.push_back(iTrack);
      
      TABMntuTrackTr* track = bmTrack->Track(iTrack);
            
      Int_t nHits = track->GetNhit();
      // cout << " nHits  BM  " <<  nHits << endl;

      TVector3 Pvers = track->GetPvers();  //direction of the track from mylar1_pos to mylar2_pos
      TVector3 R0 = track->GetR0();              //position of the track on the xy plane at z=0
     


      // Double_t chi2 = track->GetChi2New();
      // cout << " chi2   " <<  chi2  << endl;
      // Double_t chi2Red = track->GetChi2NewRed();
      // cout << " chi2Red   " <<  chi2Red  << endl;
      // Double_t mychi2 = track->GetMyChi2();
      // cout << " mychi2   " <<   mychi2 << endl;
      Double_t mychi2Red = track->GetMyChi2Red();
      //      cout << " mychi2Red   " << mychi2Red  << endl;

      bmtrk_chi2.push_back(mychi2Red);
      

      bm_Pvers.push_back(Pvers);
      bm_R0.push_back(R0);

    } // end loop on tracks
    
  } // nTracks > 0



   







   ///Vertex collection
   TAVTvertex*    vtxPD   = 0x0;//NEW
   TVector3 vtxPositionPD = TVector3(0.,0.,0.);
   // cout << " vtx->GetVertexN() " << vtx->GetVertexN() <<endl;
   
   for (Int_t iVtx = 0; iVtx < vtx->GetVertexN(); ++iVtx) {
     vtxPD = vtx->GetVertex(iVtx);
       
     	if (vtxPD == 0x0) continue;
     	vtxPositionPD = vtxPD->GetVertexPosition();
	
     	// cout << " vtxPositionPD  local " << vtxPositionPD[2]  << endl;
     	vtxPositionPD = fGeoTrafo->FromVTLocalToGlobal(vtxPositionPD);
	// cout << " vtxPositionPD  global " << vtxPositionPD[2]  << endl;
	
	vtx_coll.push_back(vtxPositionPD);
   }
   


    //truth track coll
   int  Nmctrack = eve->GetHitsN();
   mctrack = Nmctrack;
   // cout << " Nmctrack   " << Nmctrack  << endl;
   for( Int_t iTrack = 0; iTrack < eve->GetHitsN(); ++iTrack ) {
     TAMCeveTrack* track = eve->GetHit(iTrack);
     //     printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
     Double_t Charge = track->GetCharge();
     Double_t Mass = track->GetMass();
  
     mc_trk_charge.push_back(Charge);
     mc_trk_mass.push_back(Mass);

     //  cout << " Charge   " << Charge  << "  Mass  "<< Mass << endl;
    
     TVector3 InitPos = track->GetInitPos();
     // cout << " InitPos x   " << InitPos.x()<< endl;
     // cout << " InitPos y   " << InitPos.y()<< endl;
     // cout << " InitPos z   " << InitPos.z()<< endl;

     mc_trk_intpos_x.push_back(InitPos.x());
     mc_trk_intpos_y.push_back(InitPos.y());
     mc_trk_intpos_z.push_back(InitPos.z());

     TVector3 FinalPos = track->GetFinalPos();
     // cout << " FinalPos x   " << FinalPos.x()<< endl;
     // cout << " FinalPos y   " << FinalPos.y()<< endl;
     // cout << " FinalPos z   " << FinalPos.z()<< endl;

     
     mc_trk_finpos_x.push_back(FinalPos.x());
     mc_trk_finpos_y.push_back(FinalPos.y());
     mc_trk_finpos_z.push_back(FinalPos.z());



     Double_t      Tof =      track->GetTof();              // time of flight
     Double_t      TrkLength =  track->GetTrkLength(); 
     // cout << " Tof   " << Tof   << "  TrkLength  "<<  TrkLength << endl;
     mc_trk_tof.push_back(Tof);
     mc_trk_length.push_back(TrkLength);
     

   }
   
    ///Vertex track collection
   //cout << " vertex tracks " << vttrack->GetTracksN() <<endl;
   if( vttrack->GetTracksN() > 0 ) {
     for( Int_t iTrack = 0; iTrack < vttrack->GetTracksN(); ++iTrack ) {
       TAVTtrack* track = vttrack->GetTrack(iTrack);
       
       Float_t Chi2 = track-> GetChi2();  
       trk_chi2.push_back(Chi2);

       TVector3 slopez = track->GetSlopeZ();
       trk_slopez.push_back(slopez);
       TVector3 origin = trackline.GetOrigin();
       trk_origin.push_back(origin);

       Float_t nCluster =  track->GetClustersN();
       // cout << " n cluster of vertex tracks " <<  nCluster <<endl;
       Int_t TotHits = 0;
       for( Int_t iclus = 0; iclus < nCluster; ++iclus ) {
	 TAVTcluster* clus = track->GetCluster(iclus);
	 TVector3 p_clus = clus->GetPositionG();
	 
	 
	 Int_t nHits = clus->GetPixelsN();
	 // cout << " n  nHits   " <<   nHits <<endl;
	 TotHits += nHits;
	 
	
	 // for (Int_t j = 0; j < nHits; ++j) {
	 //   TAVTntuHit* hit = clus->GetPixel(j);
	 //   for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
	 //     Int_t id = hit->GetMcTrackIdx(k);
	 //     Int_t idx = hit->GetMcIndex(k);
	 //     cout << " TrackMcIdx  " <<   id   <<endl;
	 //     cout << "  McIndex   " <<   idx    <<endl;
	 //     // printf("TrackMcId %d ", id);
	 //     // printf("McIndex   %d ", idx);
	 //     TAMCeveTrack* track = eve->GetHit(id);
	 //     printf("charge %d mass %g ", track->GetCharge(), track->GetMass());
	 //     // TAMChit* mcHit = vtMc->GetHit(idx);
	 //     // TVector3 pos = mcHit->GetPosition();
	 //     // printf("MC pos (%.4f %.4f %.4f)\n", pos[0], pos[1], pos[2]);
	     

	 //   }
	 // }



	 // cout << " p_clus.z() local   i  " << iclus <<  "  z  " << p_clus.z()<< endl;
	 p_clus = fGeoTrafo->FromVTLocalToGlobal(p_clus);
	 // cout << " p_clus.z() global  i  " << iclus <<  "  z  " << p_clus.z()<< endl;

	 trk_vtx_clus.push_back(p_clus);
       }
       // cout << " TotHits   " <<   TotHits  <<endl;
       trk_vtx_clus_tothit.push_back(TotHits);
       trk_ind.push_back(iTrack);
       trk_vtx_clus_2.push_back(trk_vtx_clus);
       trk_vtx_clus.clear();
     }
   }


  
 

   
   // Float_t nTWpoint = twpoint->GetPointN();
   //  // cout << " n point in Tofwall " << nTWpoint  <<endl;
   // if(  twpoint->GetPointN() > 0 ) {
   //   for (int i = 0; i < twpoint->GetPointN(); i++) {
     	  
   //     TATW_Point *point = twpoint->GetPoint(i);
   
   //     TVector3 posG = point->GetPosition();
       
   //     // posG = fpFootGeo->FromTWLocalToGlobal(posG);
	  
   //     posG = fGeoTrafo->FromTWLocalToGlobal(posG);
   //     // cout << " posG  x  "<<  posG.X()  << endl;
   //     // cout << " posG  y  "<<  posG.Y()  << endl;
   //     // cout << " posG  z  "<<  posG.Z()  << endl;


   //     Float_t edep1  = point->GetEnergyLoss1();
   //     Float_t edep2  = point->GetEnergyLoss2();
   //     Float_t edep   = point->GetEnergyLoss();
   //     Float_t timeTW = point->GetTime();

   //     cout << " edep    "<<  edep  << endl;
   //     cout << " timeTW  "<< timeTW   << endl;
       
   //   }
     
     
     
   // }
   
   
   
   // if (refittrack)    m_kFitter->MakeFit( ev );
   
   
   nsthit = st_pos.size();
   
   for (Int_t i = 0; i < nsthit; i++) {
     st_pos_x.push_back(st_pos.at(i).X());
     st_pos_y.push_back(st_pos.at(i).Y());
     st_pos_z.push_back(st_pos.at(i).Z());
     
   }
   
   
   nbmtrack = bm_trkind.size();
   
   for (Int_t i = 0; i < nbmtrack; i++) {
     bm_Pvers_x.push_back(bm_Pvers.at(i).X());
     bm_Pvers_y.push_back(bm_Pvers.at(i).Y());
     bm_Pvers_z.push_back(bm_Pvers.at(i).Z());
     
     
     bm_R0_x.push_back(bm_R0.at(i).X());
     bm_R0_y.push_back(bm_R0.at(i).Y());
     bm_R0_z.push_back(bm_R0.at(i).Z());
     
   }
   
   nvtx = vtx_coll.size();
   for (Int_t i = 0; i < nvtx; i++) {
     vtx_x.push_back(vtx_coll.at(i).X());
     vtx_y.push_back(vtx_coll.at(i).Y());
     vtx_z.push_back(vtx_coll.at(i).Z());
   }
   
   
   // // cout<<  "ntrack   " << trk_ind.size() << endl;
   nvttrack = trk_ind.size();
   for (Int_t i = 0; i <nvttrack ; i++) {	 
     
     trk_vtx_clus_n.push_back(trk_vtx_clus_2.at(i).size());	
     // cout<<  "trk_vtx_clus_n " << trk_vtx_clus_n.at(i)  << endl;
     
     for (Int_t j = 0; j < trk_vtx_clus_n.at(i); j++) {
       
       // cout << "j " << j << endl; 
       // cout << " trk_vtx_clus_2.at(i).at(j).X()   " << trk_vtx_clus_2.at(i).at(j).X() << endl;
       trk_vtx_clus_x.push_back(trk_vtx_clus_2.at(i).at(j).X());
       trk_vtx_clus_y.push_back(trk_vtx_clus_2.at(i).at(j).Y());
       trk_vtx_clus_z.push_back(trk_vtx_clus_2.at(i).at(j).Z());
     }
	   
   }
   
   tree_out->Fill();
   
   
   
   }//Loop on event
   
   
   
   file_out->Write();
   file_out->Close();
}






