/*!
  \file
  \version $Id: TACAactNtuHitMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TACAactNtuHitMC.
*/
#include "TVector3.h"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGparGeo.hxx"

#include "TAMCparTools.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuTrack.hxx"

#include "TACAparGeo.hxx"
#include "TACAparCal.hxx"
#include "TACAntuRaw.hxx"
#include "TACAactNtuHitMC.hxx"
#include "TACAdigitizer.hxx"

#include "TAMCflukaParser.hxx"

/*!
  \class TACAactNtuHitMC TACAactNtuHitMC.hxx "TACAactNtuHitMC.hxx"
  \brief NTuplizer for Calo raw hits. **
*/

ClassImp(TACAactNtuHitMC);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAactNtuHitMC::TACAactNtuHitMC(const char* name,  TAGdataDsc* p_ntuMC, TAGdataDsc* p_ntuEve,
                                 TAGdataDsc* p_nturaw, TAGparaDsc* p_geomap,  TAGparaDsc* p_calmap, TAGparaDsc* p_geomapG, EVENT_STRUCT* evStr)
  : TAGaction(name, "TACAactNtuHitMC - NTuplize CA raw data"),
    fpNtuMC(p_ntuMC),
    fpNtuEve(p_ntuEve),
    fpNtuRaw(p_nturaw),
    fpCalMap(p_calmap),
    fpGeoMap(p_geomap),
    fpGeoMapG(p_geomapG),
    fEventStruct(evStr)
{
   if (fEventStruct == 0x0) {
     AddDataIn(p_ntuMC, "TAMCntuHit");
     AddDataIn(p_ntuEve, "TAMCntuTrack");
   } 
   AddDataOut(p_nturaw, "TACAntuRaw");
   AddPara(p_geomap,"TACAparGeo");
   AddPara(p_calmap,"TACAparCal");

   CreateDigitizer();
   
   fpGeoTrafo  = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TACAactNtuHitMC::CreateHistogram()
{
   
   DeleteHistogram();
   
   TACAparGeo* parGeo   = (TACAparGeo*) fpGeoMap->Object();
   TAGparGeo* tagParGeo = (TAGparGeo*)  fpGeoMapG->Object();;
   
   TGeoElementTable table;
   table.BuildDefaultElements();
   
   double energyBeam = tagParGeo->GetBeamPar().Energy; //*TAGgeoTrafo::GevToMev();
   int nNucleonBeam  = tagParGeo->GetBeamPar().AtomicMass;
   
   //Get calorimeter and crystal dimension
   Int_t nCrystal       = parGeo->GetCrystalsN();             //Total number of crystals
   Float_t heightback   = parGeo->GetCrystalHeightBack();     //Height of one crystal (back face)
   Float_t heightfront  = parGeo->GetCrystalHeightFront();    //Height of one crystal (front face)
   TVector3 calosize    = parGeo->GetCaloSize();              //Total size of Calo
   
   //get number of crystals
   int nBinCry = calosize.X()/(2*heightback);
   
   //printout
   if (FootDebugLevel(1)) {
      cout << "Number of crystals: " << nCrystal << endl;
      cout << "nBinCry: " << nBinCry << endl;
      cout << "x dim: " << -calosize.X()/2. << "  " << calosize.X()/2. << endl;
      cout << "y dim: " << -calosize.Y()/2. << "  " << calosize.Y()/2. << endl;
      cout << "Energy Beam: " << energyBeam << " GeV/n | "<< "  atomic number: " << nNucleonBeam << endl;
   }
   
   // 0
   fpHisDeTotMc = new TH1F("caDeTotMc", "Energy deposition per event; Energy;", 1500, 0, energyBeam );
   AddHistogram(fpHisDeTotMc);
   
   // 1
   fpHisDeTot = new TH1F("caDeTot", "Energy deposition per event (smeared); Energy;", 1500, 0, energyBeam );
   AddHistogram(fpHisDeTot);
   
   // 2
   fpHisHitMapXY = new TH2F("caHitMapXY", " Energy deposition position; X; Y",
                            nBinCry, -calosize.X()/2., calosize.X()/2., nBinCry, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram(fpHisHitMapXY);
   
   // 3
   fpHisHitMapZYin = new TH2F("caHitMapZYin", " Energy deposition position; Z; Y",
                              (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2., nBinCry*3, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram(fpHisHitMapZYin);
   
   // 4
   fpHisHitMapZYout = new TH2F("caHitMapZYout", " Energy deposition position; Z; Y",
                               (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2,nBinCry*3, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram(fpHisHitMapZYout);
   
   // 5
   fpHisMass = new TH1F("caMass", "Calorimeter - Mass particles; [GeV]", 211,-0.5,209.5);
   AddHistogram(fpHisMass);
   
   // 6
   fpHisTime = new TH1F("caTime", "Time of Calorimeter; Time [ns];",100, 0, 30);
   AddHistogram(fpHisTime);
   
   fpHisRange = new TH1F("caRange", "Range inside crystal; Range [cm];", 240, -calosize.Z()/2., calosize.Z()/2.);
   AddHistogram(fpHisRange);
   
   // 7
   fpHisNeutron_dE = new TH1F("caDeNeutron", "En Dep Neutron; En [GeV];",400, 0., 0.1);
   AddHistogram(fpHisNeutron_dE);
   
   // 8
   // fpHisParticleVsRegion = new TH2I("caParticleVsRegion",
   //                                  "Type of particles vs crystal ID; Particle ID; CAL crystal ID",
   //                                  272, -200, 70,
   //                                  nCrystal, 0, nCrystal-1);
   fpHisParticleVsRegion = new TH2I("caParticleVsRegion",
                                    "Type of particles vs crystal ID; Region; Particle",
                                    500, 1, 501, 90, 0, 90);
   AddHistogram(fpHisParticleVsRegion);
   
   
   // TAxis* xaxis = fpHisParticleVsRegion->GetXaxis();
   // for( int i=-6; i<0; i++ ) xaxis->SetBinLabel( xaxis->FindFixBin(i), TAMCparTools::GetFlukaPartName(i));
   // for( int i=1; i<17; i++ ) xaxis->SetBinLabel( xaxis->FindFixBin(i), TAMCparTools::GetFlukaPartName(i));
   // xaxis->LabelsOption("v"); // "v" draw labels vertical
   // xaxis->SetLabelSize(0.02);
   // TAxis* yaxis = fpHisParticleVsRegion->GetYaxis();
   // yaxis->SetLabelSize(0.02);
   // AddHistogram(fpHisParticleVsRegion);
   
   // fpHisParticleVsRegion = new TH2F("caHisParticleVsRegion",
   //                                  "")
   
   
   // 9
   fpHisCryHitVsEnDep = new TH2F("caCryHitVsEnDep", "En Dep Vs #CryHit; #Hit; En Dep [GeV]", 40, 0, 40, 400, 0, 4);
   AddHistogram(fpHisCryHitVsEnDep);
   
   fpHisRangeVsMass = new TH2F("caRangeVsMass", "Range Vs Mass; Range [cm]; Mass", 240, -calosize.Z()/2., calosize.Z()/2., 211,-0.5,209.5);
   AddHistogram(fpHisRangeVsMass);
   
   // 10
   fpHisCryHitVsZ = new TH2F("caCryHitVsZ", "Z Vs #CryHit; #Hit; Z", 40, 0, 40, nNucleonBeam/2+1, 0, nNucleonBeam/2+1);
   AddHistogram(fpHisCryHitVsZ);
   
   // 11
   fpHisEnDepVsZ = new TH2F("caEnDepVsZ", "En Dep Vs Z; Z; En Dep [GeV]", nNucleonBeam/2, 0, nNucleonBeam/2, 400, 0, 4);
   AddHistogram(fpHisEnDepVsZ);
   
   // 12-20
   // for(int z=0; z<nNucleonBeam/2; z++) {
   for(int z=0; z<nNucleonBeam; z++) {
      TGeoElement * elem = table.GetElement(z+1);
      fpHisIon_Ek[z] = new TH1F(Form( "caDeIonSpectrum%d", z+1),
                                Form( "^{%d}%s ; Ekin [GeV]; Events Norm", elem->N(), elem->GetName() ),
                                600, 0, 6);
      AddHistogram(fpHisIon_Ek[z]);
   }
   
   // 21-29
   for(int z=0; z<nNucleonBeam; z++) {
      // for(int z=0; z<6; z++) {
      TGeoElement * elem = table.GetElement(z+1);
      fpHisIon_dE[z] = new TH1F(Form( "caDeIon%d", z+1),
                                Form( "^{%d}%s ; EDep [GeV]; Events Norm", elem->N(), elem->GetName() ),
                                400, 0, energyBeam*nNucleonBeam);
      AddHistogram(fpHisIon_dE[z]);
   }
   
   
   //30 - 30+nCrystal
   for (int i=0; i<nCrystal; i++){
      fpHisEnPerCry[i] = new TH1F(Form( "caEnPerCry%d", i),
                                  Form( "Cry %d ; Ekin [GeV]; Events Norm", i),
                                  1000, 0., 5.);
      AddHistogram(fpHisEnPerCry[i]);
   }
   
   
   // //30 - 30+nCrystal
   // for (int i=0; i<nCrystal; i++){
   //    fpHisEnVsPositionPerCry[i] = new TH1F(Form( "caEnVsPositionPerCry%d", i),
   //                            Form( "Cry %d ; Pos [cm]; Energy [MeV]", i),
   //                            1200, 0., 24.);
   //    AddHistogram(fpHisEnVsPositionPerCry[i]);
   // }
   
   
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
   TAMCntuHit* pNtuMC   = 0x0;
   TAMCntuTrack* pNtuEve  = 0x0;
  
   if (fEventStruct == 0x0) {
     pNtuMC  = (TAMCntuHit*) fpNtuMC->Object();
     pNtuEve = (TAMCntuTrack*) fpNtuEve->Object();
   } else {
     pNtuMC  = TAMCflukaParser::GetCalHits(fEventStruct, fpNtuMC);
     pNtuEve = TAMCflukaParser::GetTracks(fEventStruct, fpNtuEve);
   }
  
  TACAparGeo* parGeo = (TACAparGeo*) fpGeoMap->Object();
  TACAparCal* parCal = (TACAparCal*) fpCalMap->Object();

   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hitMC = pNtuMC->GetHit(i);

      
      // Get particle index
      Int_t trackId = hitMC->GetTrackIdx()-1;
      Int_t cryId   = hitMC->GetCrystalId();
      Float_t edep  = hitMC->GetDeltaE()*TAGgeoTrafo::GevToMev();;
     
      Float_t thres = parCal->GetCrystalThres(cryId);

      TVector3 posIn(hitMC->GetInPosition());
      TVector3 posOut(hitMC->GetOutPosition());
      
      Float_t x0_i  = posIn[0];       //initial x position
      Float_t y0_i  = posIn[1];       //initial y position
      Float_t z0_i  = posIn[2];       //initial z position
      Float_t x0_f  = posOut[0];      //final x position
      Float_t y0_f  = posOut[1];      //final y position
      Float_t z0_f  = posOut[2];      //final z position
      
      TVector3 posInV(x0_i, y0_i, z0_i);
      TVector3 posInLoc = fpGeoTrafo->FromGlobalToCALocal(posInV);
      
      TVector3 posOutV(x0_f, y0_f, z0_f);
      TVector3 posOutLoc = fpGeoTrafo->FromGlobalToCALocal(posOutV);

      TAMCtrack*  track = pNtuEve->GetTrack(trackId);
      int fluID   = track->GetFlukaID();
      Int_t reg   = track->GetRegion();
      int z       = track->GetCharge();
      double mass = track->GetMass();
      double px   = track->GetInitP().X();
      double py   = track->GetInitP().Y();
      double pz   = track->GetInitP().Z();
      
      if (ValidHistogram()) {
         fpHisHitMapXY    ->Fill(posInLoc.X(), posInLoc.Y());
         fpHisHitMapZYin  ->Fill(posInLoc.Z(), posInLoc.Y());
         fpHisHitMapZYout ->Fill(posOutLoc.Z(), posOutLoc.Y());
         fpHisParticleVsRegion ->Fill(reg, z);
      }
      
      // Fill fDigitizer with energy in MeV
      fDigitizer->Process(edep, posInLoc[0], posInLoc[1], posInLoc[2], posOutLoc[2], 0, cryId);
      TACAntuHit* hit = fDigitizer->GetCurrentHit();
      if (hit){
         hit->AddMcTrackIdx(trackId, i);
         // hit->SetPosition(posInLoc);
         // TVector3 hitpos(posInLoc[0], posInLoc[1], posOutLoc[2]-posInLoc[2]);
         // hit->SetPosition(hitpos);
         Float_t thick = -parGeo->GetCrystalThick();
         TVector3 positionCry(0, 0, thick);
         
         positionCry = parGeo->Crystal2Detector(cryId, positionCry);
         hit->SetPosition(positionCry);
        
        if (hit->GetCharge() < thres)
          hit->SetValid(false);
        else
          hit->SetValid(true);
      }
      
      // Select Neutrons
      if (fluID == 8) {
         if (ValidHistogram())
            fpHisNeutron_dE->Fill(edep);
      }
      
      // Select Heavy-Ions
      if ( fluID <= -2 || fluID == 1 ) {
         
         // Select Heavy-Ions with charge from 2 to 8 and Protons (z=1)
         if ( (z > 1 && z <= 8) || fluID == 1 ) {
            
            double tof = 0.;
            double p = sqrt( px*px + py*py + pz*pz);
            double ek = sqrt( p*p + mass*mass ) - mass;
            
            if (z == 1 && fluID != 1) continue;  // skip triton, deuteron
            if (z == 2 && fluID == -5) continue; // skip He3
            
            if (ValidHistogram()) {
               fpHisIon_dE[z-1]        ->Fill(edep);
               fpHisIon_Ek[z-1]        ->Fill(ek);
               // // fpHisTime              ->Fill(time);
               fpHisEnDepVsZ          ->Fill(z,edep);
               fpHisRange             ->Fill(z0_f-z0_i);
               fpHisRangeVsMass       ->Fill(z0_f-z0_i,mass);
            }
         }
         
         const char* flukaName = TAMCparTools::GetFlukaPartName(fluID);
         if( fluID == -2 ) { // shift HEAVYION by z
            fluID = -40 - z;
            // check out of range
            if( fluID < -200 ) cout << "fluID " << fluID <<endl;
         }
         
      }
      
   }
   
   if (fEventStruct != 0x0) {
     fpNtuMC->SetBit(kValid);
     fpNtuEve->SetBit(kValid);
   }
   fpNtuRaw->SetBit(kValid);
  
   return kTRUE;
}




