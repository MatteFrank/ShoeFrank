/*!
  \file
  \version $Id: TACAactNtuHitMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TACAactNtuHitMC.
*/
#include "TVector3.h"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

#include "TACAparGeo.hxx"
#include "TACAntuRaw.hxx"
#include "TACAactNtuHitMC.hxx"
#include "TACAdigitizer.hxx"
#include "TGeoElement.h"

const char* getPartNamefromID(int id);

/*!
  \class TACAactNtuHitMC TACAactNtuHitMC.hxx "TACAactNtuHitMC.hxx"
  \brief NTuplizer for Calo raw hits. **
*/

ClassImp(TACAactNtuHitMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAactNtuHitMC::TACAactNtuHitMC(const char* name,  TAGdataDsc* p_ntuMC, TAGdataDsc* p_ntuEve,
                                 TAGdataDsc* p_nturaw, TAGparaDsc* p_geomap)
  : TAGaction(name, "TACAactNtuHitMC - NTuplize CA raw data"),
    fpNtuMC(p_ntuMC),
    fpNtuEve(p_ntuEve),
    fpNtuRaw(p_nturaw),
    fpGeoMap(p_geomap)
{
   AddDataIn(p_ntuMC, "TAMCntuHit");
   AddDataIn(p_ntuEve, "TAMCntuEve");
   AddDataOut(p_nturaw, "TACAntuRaw");
   AddPara(p_geomap,"TACAparGeo");

   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TACAactNtuHitMC::CreateHistogram()
{
   
   DeleteHistogram();
   
   TACAparGeo* parGeo = (TACAparGeo*) fpGeoMap->Object();

   Float_t heightback   = parGeo->GetCrystalHeightBack();
   Float_t heightfront  = parGeo->GetCrystalHeightFront();
   TVector3 calosize    = parGeo->GetCaloSize();  
   TVector3 positionCry = parGeo->GetCrystalPosition(0);

   int nBinCry = calosize.X()/(2*heightback);
   int nCrystal = parGeo->GetCrystalsN();
   
   // 0    
   AddHistogram( new TH1F("hCA_Energy", "Energy Deposition per Event; Energy;", 1500, 0, 15 ) );

   // 1
   AddHistogram( new TH2F("hCA_EnergyReleasePosXY", " Energy Deposition position; X; Y", 
                        nBinCry, -calosize.X()/2., calosize.X()/2.,nBinCry, -calosize.Y()/2., calosize.Y()/2.) );
   // 2
   AddHistogram( new TH2F("hCA_EnergyReleasePosZY_in", " Energy Deposition position; Z; Y", 
                         (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2,nBinCry*3, -calosize.Y()/2., calosize.Y()/2.) );

   AddHistogram( new TH2F("hCA_EnergyReleasePosZY_out", " Energy Deposition position; Z; Y", 
                         (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2,nBinCry*3, -calosize.Y()/2., calosize.Y()/2.) );

   // fpHisMass = new TH1F("caMass", "Calorimeter - Mass particles", 201 ,0. ,200);
   // AddHistogram(fpHisMass);

   AddHistogram( new TH1F("hCA_Mass","Calorimeter - Mass particles; Mass [u]",211,-0.5,209.5));

   AddHistogram( new TH2F("hCA_ChargeVsMass", " Charge versus Mass; Z; Mass", 
                         101, 0, 100, 211, 0., 210.) );

   AddHistogram( new TH2F("hCA_FinalPositionVsMass", " Final position versus mass; zf; mass", 
                         (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2, 211, 0., 210.) );

   TGeoElementTable table;
   table.BuildDefaultElements();
   
   TH2I* hCA_typeParticleVsRegion = new TH2I("hCA_typeParticleVsRegion", 
                                              "Type of particles vs crystal ID; Particle ID; CAL crystal ID", 
                                              272, -200, 70, 
                                              nCrystal, 0, nCrystal-1);
                                          
   TAxis* xaxis = hCA_typeParticleVsRegion->GetXaxis();
   for( int i=-6; i<0; ++i )
     xaxis->SetBinLabel( xaxis->FindFixBin(i), getPartNamefromID(i));
   for( int i=1; i<17; ++i )
     xaxis->SetBinLabel( xaxis->FindFixBin(i), getPartNamefromID(i));
   xaxis->LabelsOption("v"); // "v" draw labels vertical
   xaxis->SetLabelSize(0.02);
   TAxis* yaxis = hCA_typeParticleVsRegion->GetYaxis();
   yaxis->SetLabelSize(0.02);
   AddHistogram( hCA_typeParticleVsRegion );
   
   // 4-11   Energy release per ion fragment
   for (int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      AddHistogram( new TH1F(Form( "hCA_EnergyIon%d", z), 
                           Form( "^{%d}%s ; EDep/Ek; Events Norm", elem->N(), elem->GetName() ),
                           121, 0, 1.2) );
   }

   // 12-19   Energy spectrum
   for(int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      AddHistogram( new TH1F(Form( "hCA_EnergyIonSpect%d", z), 
                              Form( "^{%d}%s ; Ekin [GeV]; Events Norm", elem->N(), elem->GetName() ),
                              600, 0, 12) );
   }

   for(int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      AddHistogram( new TH1F(Form( "hCA_EnergyDep%d", z), 
                              Form( "^{%d}%s ; EDep [GeV]; Events Norm", elem->N(), elem->GetName() ),
                              400, 0, 14) );
   }

   // 20-27   Energy Dep vs p
   for(int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      AddHistogram( new TH2F( Form( "hCA_P_vs_EDepIon%d", z), 
                              Form( "^{%d}%s; EDep; p", elem->N(), elem->GetName() ),
                              400, 0, 14,
                              400, 0, 22
                           ) );
   }

   AddHistogram( new TH1F("hCA_timeFirstHit", "Time of Calorimeter; Time [ns];",100, 0, 30) );

   AddHistogram( new TH1F("hCA_EnergyNeutron", "En Dep Neutron; En [GeV];",400, 0., 0.1) );

   // AddHistogram( new TH1F("hCA_Tof", "Time between first and last hit in the Calo; Tof [ns];",100, 0, 5) );


   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TACAactNtuHitMC::CreateDigitizer()
{
   TACAntuRaw* pNtuRaw = (TACAntuRaw*) fpNtuRaw->Object();

   fDigitizer = new TACAdigitizer(pNtuRaw);
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACAactNtuHitMC::~TACAactNtuHitMC()
{
   delete fDigitizer;
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TACAactNtuHitMC::Action()
{  
   TAMCntuHit* pNtuMC  = (TAMCntuHit*) fpNtuMC->Object();

   TGeoElementTable table;
   table.BuildDefaultElements();

   TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   // Sum all energy dep. of the same particle
   TAMCntuEve* pNtuEve  = (TAMCntuEve*) fpNtuEve->Object();

   TObjArray dep(pNtuEve->GetTracksN());
   double energyEvent = 0;     // Total Energy deposition on the Event
   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hitMC = pNtuMC->GetHit(i);

      // Get particle index
      Int_t id      = hitMC->GetCrystalId();
      Int_t trackId = hitMC->GetTrackId();  // id della particella nel Calo
      double ener   =  hitMC->GetDeltaE()*TAGgeoTrafo::GevToMev();;
      energyEvent  +=  hitMC->GetDeltaE();
      
      // Returns the object at position trackId. Returns 0 if trackId is out of range.
      energyDep* endep = (energyDep*)dep.At(trackId);   
      
      if( !endep ) {
         Float_t timeFirstHit  = hitMC->GetTof()*TAGgeoTrafo::SecToNs();
         dep.AddAt( new energyDep(i, id, trackId, timeFirstHit, ener), trackId );   //Add object at position trackId.
      } 
      else {
         endep->fDE += ener;    //con l'espressione endep -> accedo ai vari membri della classe energyDep
         endep->fn++;
      }

   } // end first loop


   // Sum all dauthers energy dep. with their mother
   int npart = dep.GetEntriesFast();
   for ( int i=npart-1; i>0; --i ) { // assume that particles are sort by creation
      energyDep* endep = (energyDep*)dep.At(i);
      if ( !endep ) continue;
  
      // Get particle mother
      TAMCntuEve* pNtuEve  = (TAMCntuEve*) fpNtuEve->Object();
      TAMCeveTrack*  track = pNtuEve->GetTrack(endep->fid);
      Int_t ipart_mother = track->GetMotherID();

      energyDep* endepM = (energyDep*)dep.At(ipart_mother);
      if ( !endepM ) { // Not mother found in CAL (entering particle)
         continue;
      }
      // skip if mother and daughter are in different crystals
      if (endep->fCryid != endepM->fCryid) continue;  

      endepM->fDE += endep->fDE;  // Merge with mother
      endepM->fn += endep->fn++;

      dep.RemoveAt( endep->fid ); // remove thedauther  particle
   }
   dep.Compress(); // Remove empty slots


   /////// Fill Hit tree (Digitizer)
   npart = dep.GetEntriesFast();
   fDigitizer->ClearMap();
   
   for (int i=0; i<npart; ++i) {
      energyDep* endep = (energyDep*)dep.At(i);
      int index = endep->index;
      TAMChit* hitMC = pNtuMC->GetHit(i);
      TAMChit* hitMC_f = pNtuMC->GetHit(index);

      TVector3 posIn(hitMC_f->GetInPosition());
      TVector3 posOut(hitMC_f->GetOutPosition());

      Int_t trackId =  hitMC->GetTrackId();
      Float_t z0_i  =  posIn.Z();
      Float_t z0_f  =  posOut.Z();
      Float_t time  = hitMC_f->GetTof()*TAGgeoTrafo::SecToNs();

      TVector3 posInLoc = geoTrafo->FromGlobalToCALocal(posIn);
      TVector3 posOutLoc = geoTrafo->FromGlobalToCALocal(posOut);

      // don't use z for the moment
      fDigitizer->Process(endep->fDE, posInLoc[0], posInLoc[1], z0_i, z0_f, time, endep->fCryid);
      TACAntuHit* hit = fDigitizer->GetCurrentHit();
      hit->AddMcTrackIdx(trackId, i);
      hit->SetPosition(posInLoc);   
   }

   fpNtuRaw->SetBit(kValid);

   if (!ValidHistogram()) {
      return kTRUE;
   }

   // ********************************************************************
   // **************           Histograming         **********************
   const TList* histList = GetHistogrammList();

   TH1F*  hCA_Mass                 = (TH1F*)histList->FindObject("hCA_Mass");
   TH1F*  hCA_Energy               = (TH1F*)histList->FindObject("hCA_Energy");
   TH2F*  hCA_EnergyReleasePosXY =  (TH2F*)histList->FindObject("hCA_EnergyReleasePosXY");
   TH2F*  hCA_EnergyReleasePosZY_in  =  (TH2F*)histList->FindObject("hCA_EnergyReleasePosZY_in");
   TH2F*  hCA_EnergyReleasePosZY_out =  (TH2F*)histList->FindObject("hCA_EnergyReleasePosZY_out");
   TH2F*  hCA_FinalPositionVsMass  = (TH2F*)histList->FindObject("hCA_FinalPositionVsMass");
   TH2F*  hCA_ChargeVsMass         = (TH2F*)histList->FindObject("hCA_ChargeVsMass");
   TH2I*  hCA_typeParticleVsRegion = (TH2I*)histList->FindObject("hCA_typeParticleVsRegion");
   TH1F*  hCA_EnergyNeutron        = (TH1F*)histList->FindObject("hCA_EnergyNeutron");
   TH1F*  hCA_timeFirstHit         = (TH1F*)histList->FindObject("hCA_timeFirstHit");

   TH1F* hCA_EnergyIon[8];
   for (int z=1; z<=8; ++z) { 
      hCA_EnergyIon[z-1] = (TH1F*)histList->FindObject(Form( "hCA_EnergyIon%d", z));
   }

   TH1F* hCA_EnergyIonSpect[8];
   for (int z=1; z<=8; ++z) {
      hCA_EnergyIonSpect[z-1] = (TH1F*)histList->FindObject(Form( "hCA_EnergyIonSpect%d", z)); 
   }

   TH1F* hCA_EnergyDep[8];
   for (int z=1; z<=8; ++z) {
      hCA_EnergyDep[z-1] = (TH1F*)histList->FindObject(Form( "hCA_EnergyDep%d", z)); 
   }

   TH2F* hCA_P_vs_EDepIon[8];
   for (int z=1; z<=8; ++z) {
      hCA_P_vs_EDepIon[z-1] = (TH2F*)histList->FindObject(Form( "hCA_P_vs_EDepIon%d", z));
   }

   TH2F* hCA_Edx_vs_EDepIon[8];
   for (int z=1; z<=8; ++z) {
      hCA_Edx_vs_EDepIon[z-1] = (TH2F*)histList->FindObject(Form( "hCA_Edx_vs_EDepIon%d", z));
   }

   hCA_Energy->Fill( energyEvent );

   for (int i=0; i<npart; ++i) {

      energyDep* endep = (energyDep*)dep.At(i);
      int index = endep->index;
      TAMChit* hitMC_f = pNtuMC->GetHit(index);

      TVector3 posIn(hitMC_f->GetInPosition());
      TVector3 posOut(hitMC_f->GetOutPosition());
      
      Float_t z0_i  =  posIn.Z();
      Float_t z0_f  =  posOut.Z();
      
      TVector3 posInLoc = geoTrafo->FromGlobalToCALocal(posIn);
      TVector3 posOutLoc = geoTrafo->FromGlobalToCALocal(posOut);

      hCA_EnergyReleasePosXY->Fill(posInLoc.X(), posInLoc.Y());
      hCA_EnergyReleasePosZY_in->Fill(posInLoc.Z(), posInLoc.Y());
      hCA_EnergyReleasePosZY_out->Fill(posOutLoc.Z(), posOutLoc.Y());
     
      // Reconstruction is NOT analysis moreover incompatible with Geant4 !!!
      
//      int fluID = fpEvtStr->TRfid[endep->fid];
//
//      int z = fpEvtStr->TRcha[endep->fid];
//      float zf    = fpEvtStr->CALzout[endep->fid];
//      double mass = fpEvtStr->TRmass[endep->fid];
//
//      // Select Neutrons
//      if ( fluID == 8 ) {
//         hCA_EnergyNeutron->Fill(endep->fDE);
//      }
//
//      // Select Heavy-ions
//      if ( fluID <= -2 || fluID == 1 ) {
//
//         // select heavy ion with charge from 2 to 8 and protons (z=1)
//         if ( (z > 1 && z <= 8) || fluID == 1 ) {
//
//            double eDep = endep->fDE;
//            double tof = 0.;
//
//            double p = sqrt( fpEvtStr->TRipx[endep->fid]*fpEvtStr->TRipx[endep->fid] +
//                         fpEvtStr->TRipy[endep->fid]*fpEvtStr->TRipy[endep->fid] +
//                         fpEvtStr->TRipz[endep->fid]*fpEvtStr->TRipz[endep->fid] );
//            double ek = sqrt( p*p + mass*mass ) - mass;
//
//            double eRatio = eDep/ek;
//
//            if (z == 1 && fluID != 1) continue; // skip triton, deuteron
//            if (z == 2 && fluID == -5) continue; // skip He3
//            hCA_EnergyIon[z-1]->Fill( eRatio );
//            hCA_EnergyDep[z-1]->Fill( eDep );
//            hCA_EnergyIonSpect[z-1]->Fill( ek );
//            hCA_P_vs_EDepIon[z-1]->Fill( eDep, p );
//            hCA_timeFirstHit->Fill(endep->fTimeFirstHit);
//         }
//
//         const char* flukaName = getPartNamefromID(fluID);
//         if( fluID == -2 ) { // shift HEAVYION by z
//            fluID = -40 - z;
//            // check out of range
//            if( fluID < -200 ) cout << "fluID " << fluID <<endl;
//         }
//
//      }
//
//      hCA_typeParticleVsRegion->Fill( fluID, endep->fCryid, 1 );
//      hCA_FinalPositionVsMass->Fill(zf, mass);
//      hCA_Mass->Fill(mass);
//      hCA_ChargeVsMass->Fill(z, mass);
   }

   return kTRUE;
}




