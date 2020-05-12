/*!
  \file
  \version $Id: TACAactNtuHitMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TACAactNtuHitMC.
*/
#include "TVector3.h"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAMCparTools.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuEve.hxx"

#include "TACAparGeo.hxx"
#include "TACAntuRaw.hxx"
#include "TACAactNtuHitMC.hxx"
#include "TACAdigitizer.hxx"
#include "TGeoElement.h"

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
   fpHisEnergy = new TH1F("caMcEnergy", "Energy Deposition per Event; Energy;", 1500, 0, 15 );
   AddHistogram( fpHisEnergy );

   // 1
   fpHisEnergyReleasePosXY = new TH2F("caMcEnergyReleasePosXY", " Energy Deposition position; X; Y",
                                     nBinCry, -calosize.X()/2., calosize.X()/2.,nBinCry, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram( fpHisEnergyReleasePosXY );
   
   // 2
   fpHisEnergyReleasePosZY_in = new TH2F("caMcEnergyReleasePosZY_in", " Energy Deposition position; Z; Y",
                                        (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2,nBinCry*3, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram( fpHisEnergyReleasePosZY_in );

   fpHisEnergyReleasePosZY_out = new TH2F("caMcEnergyReleasePosZY_out", " Energy Deposition position; Z; Y",
                                         (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2,nBinCry*3, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram( fpHisEnergyReleasePosZY_out );

   fpHisMass = new TH1F("caMcMass","Calorimeter - Mass particles; Mass [u]",211,-0.5,209.5);
   AddHistogram( fpHisMass );

   fpHisChargeVsMass = new TH2F("caMcChargeVsMass", " Charge versus Mass; Z; Mass",
                               101, 0, 100, 211, 0., 210.);
   AddHistogram( fpHisChargeVsMass );

   fpHisFinalPositionVsMass = new TH2F("caMcFinalPositionVsMass", " Final position versus mass; zf; mass",
                                      (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2, 211, 0., 210.);
   AddHistogram( fpHisFinalPositionVsMass );

   TGeoElementTable table;
   table.BuildDefaultElements();
   
   fpHistypeParticleVsRegion = new TH2I("caMctypeParticleVsRegion",
                                       "Type of particles vs crystal ID; Particle ID; CAL crystal ID",
                                       272, -200, 70,
                                       nCrystal, 0, nCrystal-1);
                                          
   TAxis* xaxis = fpHistypeParticleVsRegion->GetXaxis();
   for( int i=-6; i<0; ++i )
     xaxis->SetBinLabel( xaxis->FindFixBin(i), TAMCparTools::GetFlukaPartName(i));
   for( int i=1; i<17; ++i )
     xaxis->SetBinLabel( xaxis->FindFixBin(i), TAMCparTools::GetFlukaPartName(i));
   xaxis->LabelsOption("v"); // "v" draw labels vertical
   xaxis->SetLabelSize(0.02);
   TAxis* yaxis = fpHistypeParticleVsRegion->GetYaxis();
   yaxis->SetLabelSize(0.02);
   AddHistogram( fpHistypeParticleVsRegion );
   
   // 4-11   Energy release per ion fragment
   for (int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      fpHisEnergyIon[z-1] = new TH1F(Form( "caMcEnergyIon%d", z),
               Form( "^{%d}%s ; EDep/Ek; Events Norm", elem->N(), elem->GetName() ),
                                  121, 0, 1.2);
      AddHistogram( fpHisEnergyIon[z-1] );
   }

   // 12-19   Energy spectrum
   for(int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      fpHisEnergyIonSpect[z-1] = new TH1F(Form( "caMcEnergyIonSpect%d", z),
               Form( "^{%d}%s ; Ekin [GeV]; Events Norm", elem->N(), elem->GetName() ),
                                       600, 0, 12);
      AddHistogram( fpHisEnergyIonSpect[z-1] );
   }

   for(int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      fpHisEnergyDep[z-1] = new TH1F(Form( "caMcEnergyDep%d", z),
               Form( "^{%d}%s ; EDep [GeV]; Events Norm", elem->N(), elem->GetName() ),
                                  400, 0, 14);
      AddHistogram( fpHisEnergyDep[z-1] );
   }

   // 20-27   Energy Dep vs p
   for(int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      fpHisP_vs_EDepIon[z-1] = new TH2F( Form( "caMcP_vs_EDepIon%d", z),
                                     Form( "^{%d}%s; EDep; p", elem->N(), elem->GetName() ),
                                     400, 0, 14, 400, 0, 22);
      AddHistogram( fpHisP_vs_EDepIon[z-1] );
   }

   fpHistimeFirstHit = new TH1F("caMctimeFirstHit", "Time of Calorimeter; Time [ns];",100, 0, 30);
   AddHistogram( fpHistimeFirstHit );

   fpHisEnergyNeutron = new TH1F("caMcEnergyNeutron", "En Dep Neutron; En [GeV];",400, 0., 0.1);
   AddHistogram( fpHisEnergyNeutron );

   // AddHistogram( new TH1F("caMcTof", "Time between first and last hit in the Calo; Tof [ns];",100, 0, 5) );


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
   dep.SetOwner(true);
   double energyEvent = 0;     // Total Energy deposition on the Event
   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hitMC = pNtuMC->GetHit(i);

      // Get particle index
      Int_t id      = hitMC->GetCrystalId();
      Int_t trackId = hitMC->GetTrackIdx()-1;  // id della particella nel Calo
      double ener   = hitMC->GetDeltaE()*TAGgeoTrafo::GevToMev();;
      energyEvent  += hitMC->GetDeltaE();
      
      // Returns the object at position trackId. Returns 0 if trackId is out of range.
      EnergyDep_t* endep = (EnergyDep_t*)dep.At(trackId);
      
      if( !endep ) {
         Float_t timeFirstHit  = hitMC->GetTof()*TAGgeoTrafo::SecToNs();
         dep.AddAt( new EnergyDep_t(i, id, trackId, timeFirstHit, ener), trackId );   //Add object at position trackId.
      } 
      else {
         endep->fDE += ener;    //con l'espressione endep -> accedo ai vari membri della classe EnergyDep_t
         endep->fn++;
      }

   } // end first loop


   // Sum all dauthers energy dep. with their mother
   int npart = dep.GetEntriesFast();
   for ( int i=npart-1; i>0; --i ) { // assume that particles are sort by creation
      EnergyDep_t* endep = (EnergyDep_t*)dep.At(i);
      if ( !endep ) continue;
  
      // Get particle mother
      TAMCntuEve* pNtuEve  = (TAMCntuEve*) fpNtuEve->Object();
      TAMCeveTrack*  track = pNtuEve->GetTrack(endep->fid);
      Int_t ipart_mother = track->GetMotherID();

      EnergyDep_t* endepM = (EnergyDep_t*)dep.At(ipart_mother);
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
      EnergyDep_t* endep = (EnergyDep_t*)dep.At(i);
      int index = endep->index;
      TAMChit* hitMC = pNtuMC->GetHit(i);
      TAMChit* hitMC_f = pNtuMC->GetHit(index);

      TVector3 posIn(hitMC_f->GetInPosition());
      TVector3 posOut(hitMC_f->GetOutPosition());

      Int_t trackId = hitMC->GetTrackIdx()-1;
      Float_t z0_i  = posIn.Z();
      Float_t z0_f  = posOut.Z();
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

   fpHisEnergy->Fill( energyEvent );

   for (int i=0; i<npart; ++i) {

      EnergyDep_t* endep = (EnergyDep_t*)dep.At(i);
      int index = endep->index;
      TAMChit* hitMC_f = pNtuMC->GetHit(index);

      TVector3 posIn(hitMC_f->GetInPosition());
      TVector3 posOut(hitMC_f->GetOutPosition());
      
      Float_t z0_i = posIn.Z();
      Float_t z0_f = posOut.Z();
      
      TVector3 posInLoc = geoTrafo->FromGlobalToCALocal(posIn);
      TVector3 posOutLoc = geoTrafo->FromGlobalToCALocal(posOut);

      fpHisEnergyReleasePosXY->Fill(posInLoc.X(), posInLoc.Y());
      fpHisEnergyReleasePosZY_in->Fill(posInLoc.Z(), posInLoc.Y());
      fpHisEnergyReleasePosZY_out->Fill(posOutLoc.Z(), posOutLoc.Y());
      
      TAMCntuEve* pNtuEve  = (TAMCntuEve*) fpNtuEve->Object();
      TAMCeveTrack*  track = pNtuEve->GetTrack(endep->fid);
      
      int fluID   = track->GetFlukaID();
      int z       = track->GetCharge();
      double mass = track->GetMass();
      double px   = track->GetInitP().X();
      double py   = track->GetInitP().Y();
      double pz   = track->GetInitP().Z();

     // TAMChit* hitMC = pNtuMC->GetHit(endep->fid); could not be endep->fid, index of tracks not hits !
      TAMChit* hitMC = pNtuMC->GetHit(index);
      float zf    = hitMC->GetOutPosition().Z();

      // Select Neutrons
      if ( fluID == 8 ) {
         fpHisEnergyNeutron->Fill(endep->fDE);
      }

      // Select Heavy-ions
      if ( fluID <= -2 || fluID == 1 ) {

         // select heavy ion with charge from 2 to 8 and protons (z=1)
         if ( (z > 1 && z <= 8) || fluID == 1 ) {

            double eDep = endep->fDE;
            double tof = 0.;

            double p = sqrt( px*px + py*py + pz*pz);
            double ek = sqrt( p*p + mass*mass ) - mass;

            double eRatio = eDep/ek;

            if (z == 1 && fluID != 1) continue; // skip triton, deuteron
            if (z == 2 && fluID == -5) continue; // skip He3

            fpHisEnergyIon[z-1]->Fill( eRatio );
            fpHisEnergyDep[z-1]->Fill( eDep );
            fpHisEnergyIonSpect[z-1]->Fill( ek );
            fpHisP_vs_EDepIon[z-1]->Fill( eDep, p );
            fpHistimeFirstHit->Fill(endep->fTimeFirstHit);
         }

         const char* flukaName = TAMCparTools::GetFlukaPartName(fluID);
         if( fluID == -2 ) { // shift HEAVYION by z
            fluID = -40 - z;
            // check out of range
            if( fluID < -200 ) cout << "fluID " << fluID <<endl;
         }

      }

      fpHistypeParticleVsRegion->Fill( fluID, endep->fCryid, 1 );
      fpHisFinalPositionVsMass->Fill(zf, mass);
      fpHisMass->Fill(mass);
      fpHisChargeVsMass->Fill(z, mass);
   }

   return kTRUE;
}




