/*!
  \file TACAactNtuHitMC.cxx
  \brief   Implementation of TACAactNtuHitMC.
*/
#include "TVector3.h"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGparGeo.hxx"

#include "TAMCparTools.hxx"
#include "TAMCntuHit.hxx"
#include "TAMCntuPart.hxx"

#include "TACAparGeo.hxx"
#include "TACAparCal.hxx"
#include "TACAntuHit.hxx"
#include "TACAactNtuHitMC.hxx"
#include "TACAdigitizer.hxx"

#include "TAMCflukaParser.hxx"

/*!
  \class TACAactNtuHitMC
  \brief NTuplizer for Calo raw hits. **
*/

//! Class Imp
ClassImp(TACAactNtuHitMC);

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[in] p_ntuMC MC input container descriptor
//! \param[in] p_ntuEve MC particle input container descriptor
//! \param[out] p_nturaw raw data output container descriptor
//! \param[in] p_geomap geometry parameter descriptor
//! \param[in] p_calmap calibration parameter descriptor
//! \param[in] p_geomapG beam geometry parameter descriptor
//! \param[in] evStr old Fluka container descriptor
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
     AddDataIn(p_ntuEve, "TAMCntuPart");
   }
   AddDataOut(p_nturaw, "TACAntuHit");
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
   TAGparGeo* tagParGeo = (TAGparGeo*)  fpGeoMapG->Object();

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
   fpHisDeTotMc = new TH1F("caDeTotMc", "Energy deposition per event; Energy;", 500, 0, 10 );
   AddHistogram(fpHisDeTotMc);

   // 1
   fpHisDeTot = new TH1F("caDeTot", "Energy deposition per event (smeared); Energy;", 500, 0, 10 );
   AddHistogram(fpHisDeTot);

   // 2
   fpHisHitMapXY = new TH2F("caHitMapMc", "MC Hit Map",
                            nBinCry, -calosize.X()/2., calosize.X()/2., nBinCry, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram(fpHisHitMapXY);

   
   // 2
   fpHisHitMap = new TH2F("caHitMap", " Hit map",
                            nBinCry, -calosize.X()/2., calosize.X()/2., nBinCry, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram(fpHisHitMap);
   
   // 2
   fpHisRawHitMap = new TH2F("caHitRawMap", " Raw Hit map",
                          nBinCry, 0, nBinCry, nBinCry, 0, nBinCry);
   AddHistogram(fpHisRawHitMap);
   
   // 3
   fpHisHitMapZYin = new TH2F("caHitMapZYinMc", " Energy deposition position; Z; Y",
                              (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2., nBinCry*3, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram(fpHisHitMapZYin);

   // 4
   fpHisHitMapZYout = new TH2F("caHitMapZYoutMc", " Energy deposition position; Z; Y",
                               (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2,nBinCry*3, -calosize.Y()/2., calosize.Y()/2.);
   AddHistogram(fpHisHitMapZYout);

   // 5
   fpHisMass = new TH1F("caMassMc", "Calorimeter - Mass particles; [GeV]", 20, 0.5, 20.5);
   AddHistogram(fpHisMass);

   // 6
   fpHisTime = new TH1F("caTimeMc", "Time of Calorimeter; Time [ns];",100, 0, 30);
   AddHistogram(fpHisTime);

   fpHisRange = new TH1F("caRangeMc", "Range inside crystal; Range [cm];", 240, 0., calosize.Z());
   AddHistogram(fpHisRange);

   // 7
   fpHisNeutrondE = new TH1F("caDeNeutron", "En Dep Neutron; En [GeV];",400, 0., 0.1);
   AddHistogram(fpHisNeutrondE);

   fpHisParticleVsRegion = new TH2I("caParticleVsRegionMc",
                                    "Type of particles vs crystal ID; Region; Particle",
                                    500, 1, 501, 90, 0, 90);
   AddHistogram(fpHisParticleVsRegion);

   fpHisRangeVsMass = new TH2F("caRangeVsMassMc", "Range Vs Mass; Range [cm]; Mass", 240, 0, calosize.Z(), 20, 0.5, 20.5);
   AddHistogram(fpHisRangeVsMass);

   SetValidHistogram(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Create digitizer
void TACAactNtuHitMC::CreateDigitizer()
{
   TACAntuHit* pNtuRaw = (TACAntuHit*) fpNtuRaw->Object();
   TACAparCal* parCal  = (TACAparCal*) fpCalMap->Object();

   fDigitizer = new TACAdigitizer(pNtuRaw, parCal);
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
   TAMCntuPart* pNtuEve  = 0x0;
   TAGparGeo* tagParGeo = (TAGparGeo*)  fpGeoMapG->Object();
   
   if (fEventStruct == 0x0) {
     pNtuMC  = (TAMCntuHit*) fpNtuMC->Object();
     pNtuEve = (TAMCntuPart*) fpNtuEve->Object();
   } else {
     pNtuMC  = TAMCflukaParser::GetCalHits(fEventStruct, fpNtuMC);
     pNtuEve = TAMCflukaParser::GetTracks(fEventStruct, fpNtuEve);
   }

   TACAparGeo* parGeo = (TACAparGeo*) fpGeoMap->Object();

   fDigitizer->ClearMap();

   for (Int_t i = 0; i < pNtuMC->GetHitsN(); i++) {
      TAMChit* hitMC = pNtuMC->GetHit(i);

      // Get particle index
      Int_t trackId = hitMC->GetTrackIdx()-1;
      Int_t cryId   = hitMC->GetCrystalId();
      Float_t edep  = hitMC->GetDeltaE();

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

      TAMCpart*  track = pNtuEve->GetTrack(trackId);
      int fluID   = track->GetFlukaID();
      Int_t reg   = track->GetRegion();
      int z       = track->GetCharge();
      double mass = track->GetMass();
      double px   = track->GetInitP().X();
      double py   = track->GetInitP().Y();
      double pz   = track->GetInitP().Z();

      if (z < 1) continue;
      if (ValidHistogram()) {
         fpHisMass->Fill(mass);
         fpHisDeTotMc->Fill(edep);
         fpHisHitMapXY->Fill(posInLoc.X(), posInLoc.Y());
         fpHisHitMapZYin->Fill(posInLoc.Z(), posInLoc.Y());
         fpHisHitMapZYout->Fill(posOutLoc.Z(), posOutLoc.Y());
         fpHisParticleVsRegion->Fill(reg, z);
         fpHisRange->Fill(z0_f-z0_i);
         fpHisRangeVsMass->Fill(z0_f-z0_i,mass);
      }
      
      // Fill fDigitizer with energy in GeV
      fDigitizer->Process(edep, posInLoc[0], posInLoc[1], posInLoc[2], posOutLoc[2], 0, cryId);
      TACAhit* hit = fDigitizer->GetCurrentHit();
      if (hit){
         hit->AddMcTrackIdx(trackId, i);
         Float_t thick = -parGeo->GetCrystalThick();
         TVector3 positionCry(0, 0, thick);

         positionCry = parGeo->Crystal2Detector(cryId, positionCry);
         hit->SetPosition(positionCry);
      }
   }//end of mc loop on calo hits
   
   // histograms
   TACAntuHit* ntuRaw  = (TACAntuHit*) fpNtuRaw->Object();

   for (Int_t i = 0; i < ntuRaw->GetHitsN(); ++i) {
      TACAhit* hit = ntuRaw->GetHit(i);
      Float_t charge = hit->GetCharge();
      TVector3 pos = hit->GetPosition();
      if (ValidHistogram()) {
         fpHisDeTot->Fill(charge);
         fpHisHitMap->Fill(pos.X(), pos.Y());
         Int_t id = hit->GetCrystalId();
         Int_t line = parGeo->GetCrystalLine(id);
         Int_t col  = parGeo->GetCrystalCol(id);
         fpHisRawHitMap->Fill(line, col);
      }
   }

   if (fEventStruct != 0x0) {
     fpNtuMC->SetBit(kValid);
     fpNtuEve->SetBit(kValid);
   }
   fpNtuRaw->SetBit(kValid);

   return kTRUE;
}
