/*!
  \file
  \version $Id: TACAactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TACAactNtuMC.
*/
#include "TVector3.h"
#include "TGeoElement.h"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAMCparTools.hxx"

#include "TACAparGeo.hxx"
#include "TACAntuRaw.hxx"
#include "TACAactNtuMC.hxx"
#include "TACAdigitizer.hxx"


const char* getPartNamefromID(int id);

/*!
  \class TACAactNtuMC TACAactNtuMC.hxx "TACAactNtuMC.hxx"
  \brief NTuplizer for Calo raw hits. **
*/

ClassImp(TACAactNtuMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAactNtuMC::TACAactNtuMC(const char* name, TAGdataDsc* p_datraw, TAGparaDsc* pGeoMap, EVENT_STRUCT* evStr)
  : TAGaction(name, "TACAactNtuMC - NTuplize CA raw data"),
    fpGeoMap(pGeoMap),
    fpNtuMC(p_datraw),
    fpEvtStr(evStr)
{
   AddDataOut(p_datraw, "TACAntuRaw");
   
   CreateDigitizer();
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TACAactNtuMC::CreateHistogram()
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
   fpHisEnergy = new TH1F("caEnergy", "Energy Deposition per Event; Energy;", 1500, 0, 15 );
   AddHistogram(fpHisEnergy);

   // 1
   fpHisEnergyReleasePosXY = new TH2F("caEnergyReleasePosXY", " Energy Deposition position; X; Y",
                                      nBinCry, -calosize.X()/2., calosize.X()/2.,nBinCry, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram( fpHisEnergyReleasePosXY );
   
   // 2
   fpHisEnergyReleasePosZY_in = new TH2F("caEnergyReleasePosZY_in", " Energy Deposition position; Z; Y",
                                         (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2,nBinCry*3, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram( fpHisEnergyReleasePosZY_in );

   fpHisEnergyReleasePosZY_out = new TH2F("caEnergyReleasePosZY_out", " Energy Deposition position; Z; Y",
                                          (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2,nBinCry*3, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram( fpHisEnergyReleasePosZY_out  );

   fpHisMass = new TH1F("caMass", "Calorimeter - Mass particles; Mass [u]",211,-0.5,209.5);
   AddHistogram(fpHisMass);

   fpHisChargeVsMass = new TH2F("caChargeVsMass", " Charge versus Mass; Z; Mass",
                                101, 0, 100, 211, 0., 210.) ;
   AddHistogram( fpHisChargeVsMass );

   fpHisFinalPositionVsMass = new TH2F("caFinalPositionVsMass", " Final position versus mass; zf; mass",
                                       (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2, 211, 0., 210.) ;
   AddHistogram( fpHisFinalPositionVsMass );

   TGeoElementTable table;
   table.BuildDefaultElements();
   
  fpHistypeParticleVsRegion = new TH2I("caTypeParticleVsRegion",
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
      fpHisEnergyIon[z-1] = new TH1F(Form( "caEnergyIon%d", z),
                                     Form( "^{%d}%s ; EDep/Ek; Events Norm", elem->N(), elem->GetName() ),
                                     121, 0, 1.2) ;
      AddHistogram( fpHisEnergyIon[z-1] );
   }

   // 12-19   Energy spectrum
   for(int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      fpHisEnergyIonSpect[z-1] = new TH1F(Form( "caEnergyIonSpect%d", z),
                                          Form( "^{%d}%s ; Ekin [GeV]; Events Norm", elem->N(), elem->GetName() ),
                                          600, 0, 12);
      AddHistogram( fpHisEnergyIonSpect[z-1]  );
   }

   for(int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      fpHisEnergyDep[z-1] = new TH1F(Form( "caEnergyDep%d", z),
                                     Form( "^{%d}%s ; EDep [GeV]; Events Norm", elem->N(), elem->GetName() ),
                                     400, 0, 14) ;
      AddHistogram( fpHisEnergyDep[z-1] );
   }

   // 20-27   Energy Dep vs p
   for(int z=1; z<=8; ++z) {
      TGeoElement * elem = table.GetElement( z );
      fpHisP_vs_EDepIon[z-1] = new TH2F( Form( "caP_vs_EDepIon%d", z),
                                        Form( "^{%d}%s; EDep; p", elem->N(), elem->GetName() ),
                                        400, 0, 14,
                                        400, 0, 22
                                        );
      AddHistogram( fpHisP_vs_EDepIon[z-1] );
   }

   fpHistimeFirstHit =  new TH1F("catimeFirstHit", "Time of Calorimeter; Time [ns];",100, 0, 30) ;
   AddHistogram( fpHistimeFirstHit );

   fpHisEnergyNeutron =  new TH1F("caEnergyNeutron", "En Dep Neutron; En [MeV];", 1000, 0., 10) ;
   AddHistogram( fpHisEnergyNeutron );

   // AddHistogram( new TH1F("caTof", "Time between first and last hit in the Calo; Tof [ns];",100, 0, 5) );


   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TACAactNtuMC::CreateDigitizer()
{
   TACAntuRaw* pNtuRaw = (TACAntuRaw*) fpNtuMC->Object();

   fDigitizer = new TACAdigitizer(pNtuRaw);
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACAactNtuMC::~TACAactNtuMC()
{
   delete fDigitizer;
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TACAactNtuMC::Action()
{  

   TGeoElementTable table;
   table.BuildDefaultElements();

   TACAparGeo* parGeo    = (TACAparGeo*) fpGeoMap->Object();
   TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   // Sum all energy dep. of the same particle
   TObjArray dep(fpEvtStr->TRn);
   double energyEvent = 0;     // Total Energy deposition on the Event
   for (Int_t i = 0; i < fpEvtStr->CALn; i++) { 

      // Get particle index
      Int_t id      = fpEvtStr->CALicry[i];
      Int_t trackId = fpEvtStr->CALid[i] - 1;  // id della particella nel Calo
      double ener   = fpEvtStr->CALde[i]*TAGgeoTrafo::GevToMev();;
      energyEvent  += fpEvtStr->CALde[i];
      
      // Returns the object at position trackId. Returns 0 if trackId is out of range.
      energyDep* endep = (energyDep*)dep.At(trackId);   
      
      if( !endep ) {
         Float_t timeFirstHit  = fpEvtStr->CALtim[i]*TAGgeoTrafo::SecToNs();
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
      Int_t ipart_mother = fpEvtStr->TRpaid[endep->fid]; // index mother in TR 
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
   fDigitizer->ClearMap();
   
   for (int i=0; i<fpEvtStr->CALn; ++i) {
      
      Int_t trackId = fpEvtStr->CALid[i] - 1;
      Int_t id      = fpEvtStr->CALicry[i];
      Float_t x0_i  = fpEvtStr->CALxin[i];
      Float_t x0_f  = fpEvtStr->CALxout[i];
      Float_t y0_i  = fpEvtStr->CALyin[i];
      Float_t y0_f  = fpEvtStr->CALyout[i];
      Float_t z0_i  = fpEvtStr->CALzin[i];
      Float_t z0_f  = fpEvtStr->CALzout[i];
      Float_t time  = fpEvtStr->CALtim[i]*TAGgeoTrafo::SecToNs();
      Float_t edep  = fpEvtStr->CALde[i]*TAGgeoTrafo::GevToMev();;

      TVector3 posIn(x0_i, y0_i, z0_i);
      TVector3 posInLoc = geoTrafo->FromGlobalToCALocal(posIn);

      TVector3 posOut(x0_f, y0_f, z0_f);
      TVector3 posOutLoc = geoTrafo->FromGlobalToCALocal(posOut);

      // don't use z for the moment
      fDigitizer->Process(edep, posInLoc[0], posInLoc[1], z0_i, z0_f, time, id);
      TACAntuHit* hit = fDigitizer->GetCurrentHit();
      
      if (hit) {
         hit->AddMcTrackIdx(trackId, i);
         
         Float_t thick = -parGeo->GetCrystalThick();
         TVector3 positionCry(0, 0, thick);
         
         positionCry = parGeo->Crystal2Detector(id, positionCry);
         hit->SetPosition(positionCry);
      }
   }

   fpNtuMC->SetBit(kValid);

   if (!ValidHistogram()) {
      return kTRUE;
   }

   // ********************************************************************
   // **************           Histograming         **********************

   fpHisEnergy->Fill( energyEvent );
   npart = dep.GetEntriesFast();
   for (int i=0; i<npart; ++i) {

      energyDep* endep = (energyDep*)dep.At(i);
      int index = endep->index;

      Float_t x0_i = fpEvtStr->CALxin[index];
      Float_t x0_f = fpEvtStr->CALxout[index];
      Float_t y0_i = fpEvtStr->CALyin[index];
      Float_t y0_f = fpEvtStr->CALyout[index];
      Float_t z0_i = fpEvtStr->CALzin[index];
      Float_t z0_f = fpEvtStr->CALzout[index];

      TVector3 posIn(x0_i, y0_i, z0_i);
      TVector3 posInLoc = geoTrafo->FromGlobalToCALocal(posIn);

      TVector3 posOut(x0_f, y0_f, z0_f);
      TVector3 posOutLoc = geoTrafo->FromGlobalToCALocal(posOut);

      fpHisEnergyReleasePosXY->Fill(posInLoc.X(), posInLoc.Y());
      fpHisEnergyReleasePosZY_in->Fill(posInLoc.Z(), posInLoc.Y());
      fpHisEnergyReleasePosZY_out->Fill(posOutLoc.Z(), posOutLoc.Y());
     
      int fluID = fpEvtStr->TRfid[endep->fid];

      int z = fpEvtStr->TRcha[endep->fid];
      float zf    = fpEvtStr->CALzout[endep->index];
      double mass = fpEvtStr->TRmass[endep->fid];

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

            double p = sqrt( fpEvtStr->TRipx[endep->fid]*fpEvtStr->TRipx[endep->fid] + 
                         fpEvtStr->TRipy[endep->fid]*fpEvtStr->TRipy[endep->fid] +
                         fpEvtStr->TRipz[endep->fid]*fpEvtStr->TRipz[endep->fid] );
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
