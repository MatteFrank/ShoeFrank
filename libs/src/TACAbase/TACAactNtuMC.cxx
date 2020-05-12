/*!
  \file
  \version $Id: TACAactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TACAactNtuMC.
*/
#include "TVector3.h"

#include "TAGroot.hxx"
#include "TAGparGeo.hxx"

#include "TAGgeoTrafo.hxx"
#include "TAMCparTools.hxx"

#include "TACAntuRaw.hxx"
#include "TACAactNtuMC.hxx"
#include "TGeoElement.h"

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
    fpEvtStr(evStr),
    fListOfParticles(0x0)
{
   AddDataOut(p_datraw, "TACAntuRaw");
   CreateDigitizer();
}


//------------------------------------------+-----------------------------------
//! Destructor.
TACAactNtuMC::~TACAactNtuMC()
{
   delete fDigitizer;
   delete fListOfParticles;
}


//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TACAactNtuMC::CreateHistogram()
{
   
   DeleteHistogram();
   
   TAGparGeo*  tagParGeo = (TAGparGeo*) gTAGroot->FindDataDsc(TAGparGeo::GetDefParaName());
   geoTrafo              = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   parGeo                = (TACAparGeo*) fpGeoMap->Object();

   TGeoElementTable table;
   table.BuildDefaultElements();
   
   //if you run only ReadCARawMC.C and not DecodeMC create a new TAGparaDsc
   if(tagParGeo == NULL) {
      TAGparaDsc *paradsc = new TAGparaDsc("gGeo", new TAGparGeo());
      tagParGeo = (TAGparGeo*) paradsc->Object();
      TString parFile = "./geomaps/TAGdetector.map";
      tagParGeo->FromFile(parFile.Data());
   }

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
   cout << "Number of crystals: " << nCrystal << endl;
   cout << "nBinCry: " << nBinCry << endl;
   cout << "x dim: " << -calosize.X()/2. << "  " << calosize.X()/2. << endl;
   cout << "y dim: " << -calosize.Y()/2. << "  " << calosize.Y()/2. << endl;
   cout << "Energy Beam: " << energyBeam << " GeV/n | "<< "  atomic number: " << nNucleonBeam << endl;
   // for (int i=0; i<nCrystal; i++){
   //    cout << "ID CRYSTAL: " << i << endl;

   //    TVector3 positionCry_Local = parGeo->GetCrystalPosition(i);
   //    cout << "   LOCAL - posx: " << positionCry_Local.X() << " posy: " << positionCry_Local.Y() << " posz: " << positionCry_Local.Z() << endl;

   //    TVector3 positionCry_Global = geoTrafo->FromCALocalToGlobal(positionCry_Local);
   //    cout << "   GLOBAL - posx: " << positionCry_Global.X() << " posy: " << positionCry_Global.Y() << " posz: " << positionCry_Global.Z() << endl;
   // }

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
                         (int)calosize.Z(), -calosize.Z()/2., calosize.Z()/2,nBinCry*3, -calosize.Y()/2., calosize.Y()/2.);
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

   // 7
   fpHisDeNeutron = new TH1F("caDeNeutron", "En Dep Neutron; En [GeV];",400, 0., 0.1);
   AddHistogram(fpHisDeNeutron);

   // 8
   fpHisParticleVsRegion = new TH2I("caParticleVsRegion", 
                                    "Type of particles vs crystal ID; Particle ID; CAL crystal ID", 
                                    272, -200, 70, 
                                    nCrystal, 0, nCrystal-1);


   TAxis* xaxis = fpHisParticleVsRegion->GetXaxis();
   for( int i=-6; i<0; i++ ) xaxis->SetBinLabel( xaxis->FindFixBin(i), TAMCparTools::GetFlukaPartName(i));
   for( int i=1; i<17; i++ ) xaxis->SetBinLabel( xaxis->FindFixBin(i), TAMCparTools::GetFlukaPartName(i));
   xaxis->LabelsOption("v"); // "v" draw labels vertical
   xaxis->SetLabelSize(0.02);
   TAxis* yaxis = fpHisParticleVsRegion->GetYaxis();
   yaxis->SetLabelSize(0.02);
   AddHistogram(fpHisParticleVsRegion);
   

   // 9
   fpHisCryHitVsEnDep = new TH2F("caCryHitVsEnDep", "En Dep Vs #CryHit; #Hit; En Dep [GeV]", 40, 0, 40, 400, 0, 4);
   AddHistogram(fpHisCryHitVsEnDep);

   // 10
   fpHisCryHitVsZ = new TH2F("caCryHitVsZ", "Z Vs #CryHit; #Hit; Z", 40, 0, 40, nNucleonBeam/2+1, 0, nNucleonBeam/2+1);
   AddHistogram(fpHisCryHitVsZ);

   // 11
   fpHisEnDepVsZ = new TH2F("caEnDepVsZ", "En Dep Vs Z; Z; En Dep [GeV]", nNucleonBeam/2, 0, nNucleonBeam/2, 400, 0, 4);
   AddHistogram(fpHisEnDepVsZ);

   // 12-20
   for(int z=0; z<nNucleonBeam/2; z++) {
      TGeoElement * elem = table.GetElement(z+1);
      fpHisDeIonSpectrum[z] = new TH1F(Form( "caDeIonSpectrum%d", z+1), 
                              Form( "^{%d}%s ; Ekin [GeV]; Events Norm", elem->N(), elem->GetName() ),
                              600, 0, 6);
      AddHistogram(fpHisDeIonSpectrum[z]);
   }

   // 21-29
   for(int z=0; z<nNucleonBeam/2; z++) {
      TGeoElement * elem = table.GetElement(z+1);
      fpHisDeIon[z] = new TH1F(Form( "caDeIon%d", z+1), 
                              Form( "^{%d}%s ; EDep [GeV]; Events Norm", elem->N(), elem->GetName() ),
                              400, 0, energyBeam*nNucleonBeam);
      AddHistogram(fpHisDeIon[z]);
   }


   //30 - 30+nCrystal
   for (int i=0; i<nCrystal; i++){
      fpHisEnPerCry[i] = new TH1F(Form( "caEnPerCry%d", i), 
                              Form( "Cry %d ; Ekin [GeV]; Events Norm", i),
                              1200, 0, 4);
      AddHistogram(fpHisEnPerCry[i]);
   }


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
//! Action.
Bool_t TACAactNtuMC::Action(){  
      
   TGeoElementTable table;
   table.BuildDefaultElements();

   geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   // Sum all energy dep. of the same particle
   //TObject array of size of the particles created in one event
   fListOfParticles = new TObjArray(fpEvtStr->TRn); 

   //Fill TObjArr with partID, cryID and EnDep for each release in the calorimeter
   for (Int_t i = 0; i<fpEvtStr->CALn; i++) { 

      // Get particle index
      Int_t id      = fpEvtStr->CALicry[i];      //cry id
      Int_t trackId = fpEvtStr->CALid[i] - 1;    //particle id
      Float_t edep  = fpEvtStr->CALde[i];        //edep of the "i"-esimo release (GeV)
      
      //Returns the event(object) at position "trackId". Returns 0 if idx is out of range
      if (!fListOfParticles->At(trackId)) fListOfParticles->AddAt(new ParticleEnergyDep(trackId,i),trackId);
      ParticleEnergyDep* obj = (ParticleEnergyDep*) fListOfParticles->At(trackId);
      obj->addEnergyDep(id, edep);

   }
   fListOfParticles->SetOwner(true);

   int npart = fListOfParticles->GetEntriesFast();

   // Sum all daughter energy dep. with their mother
   for ( int i=npart-1; i>0; --i ) { // assume that particles are sort by creation
      ParticleEnergyDep* obj = (ParticleEnergyDep*)fListOfParticles->At(i);

      //skip the empty slots
      if ( !obj ) continue; 
      
      // Get particle mother
      Int_t ipart_mother = fpEvtStr->TRpaid[obj->partId]; // index of the mother in TRpaid
      ParticleEnergyDep* objMother = (ParticleEnergyDep*)fListOfParticles->At(ipart_mother);

      // Skip empty slots: not mother found in CAL
      if ( !objMother ) continue;

      //don't count the mother two times
      if (obj->partId == objMother->partId) continue; 

      //copy the block of energy-cryID from daughter to mother particle
      objMother->energyDeps.insert(objMother->energyDeps.end(),obj->energyDeps.begin(),obj->energyDeps.end());
      
      // skip if mother and daughter are in different crystals
      // if (objPart->fCryid != objPartMother->fCryid) continue;  

      //Remove daughter slot
      fListOfParticles->RemoveAt(obj->partId); // remove the daughter particle
            
   }

   //Remove empty slots
   fListOfParticles->Compress(); // Remove empty slots

   //new size of the TObj after compression
   npart = fListOfParticles->GetEntriesFast(); 
   
   //Final loop
   for (int i=0; i<npart; ++i) {
      
      ParticleEnergyDep* obj = (ParticleEnergyDep*)fListOfParticles->At(i);

      Int_t index           = obj->iRelease;
      Int_t trackId         = obj->partId;
      Int_t cryid           = obj->energyDeps[0].crystalId;
      Float_t endep         = obj->getTotalEnergyDep();
      std::set<int> set_cry = obj->getUniqueCryIds();
      float ncryhit         = obj->getNCrystals();
     

      Float_t x0_i  = fpEvtStr->CALxin[index];
      Float_t y0_i  = fpEvtStr->CALyin[index];
      Float_t z0_i  = fpEvtStr->CALzin[index];
      Float_t x0_f  = fpEvtStr->CALxout[index];
      Float_t y0_f  = fpEvtStr->CALyout[index];
      Float_t z0_f  = fpEvtStr->CALzout[index];

      TVector3 posIn(x0_i, y0_i, z0_i);
      TVector3 posInLoc = geoTrafo->FromGlobalToCALocal(posIn);

      TVector3 posOut(x0_f, y0_f, z0_f);
      TVector3 posOutLoc = geoTrafo->FromGlobalToCALocal(posOut);


      // Fill fDigitizer with energy in MeV
      fDigitizer->Process(endep, posInLoc[0], posInLoc[1], z0_i, z0_f, 0, cryid);
      TACAntuHit* hit = fDigitizer->GetCurrentHit();
      hit->AddMcTrackIdx(trackId, i);
      hit->SetPosition(posInLoc);
      //double chargeBGO = hit->GetCharge();


      fpHisDeTot        ->Fill(hit->GetEnDep());
      fpHisDeTotMc      ->Fill(endep);
      fpHisHitMapXY     ->Fill(posInLoc.X(), posInLoc.Y());
      fpHisHitMapZYin   ->Fill(posInLoc.Z(), posInLoc.Y());
      fpHisHitMapZYout  ->Fill(posOutLoc.Z(), posOutLoc.Y());
      // fpHisEnPerCry[cryid]->Fill(endep);


      int fluID   = fpEvtStr->TRfid[trackId];
      int z       = fpEvtStr->TRcha[trackId];
      float zf    = fpEvtStr->CALzout[trackId];
      double mass = fpEvtStr->TRmass[trackId];


      // Select Neutrons
      if ( fluID == 8 ) {
         fpHisDeNeutron->Fill(endep); 
      }


      // Select Heavy-Ions
      if ( fluID <= -2 || fluID == 1 ) {


         // Select Heavy-Ions with charge from 2 to 8 and Protons (z=1)             
         if ( (z > 1 && z <= 8) || fluID == 1 ) {  
                             
            double tof = 0.;

            double p = sqrt( fpEvtStr->TRipx[trackId]*fpEvtStr->TRipx[trackId] + 
                         fpEvtStr->TRipy[trackId]*fpEvtStr->TRipy[trackId] +
                         fpEvtStr->TRipz[trackId]*fpEvtStr->TRipz[trackId] );
            double ek = sqrt( p*p + mass*mass ) - mass; 

            if (z == 1 && fluID != 1) continue;  // skip triton, deuteron
            if (z == 2 && fluID == -5) continue; // skip He3
            
            fpHisDeIon[z-1]        ->Fill(endep);
            fpHisDeIonSpectrum[z-1]->Fill(ek);
            // fpHisTime              ->Fill(time);
            fpHisCryHitVsZ         ->Fill(ncryhit,z);
            fpHisCryHitVsEnDep     ->Fill(ncryhit,endep);
            fpHisEnDepVsZ          ->Fill(z,endep);
            fpHisEnPerCry[cryid]   ->Fill(hit->GetEnDep());
         }

         const char* flukaName = TAMCparTools::GetFlukaPartName(fluID);
         if( fluID == -2 ) { // shift HEAVYION by z
            fluID = -40 - z; 
            // check out of range
            if( fluID < -200 ) cout << "fluID " << fluID <<endl; 
         }

      }

      fpHisParticleVsRegion->Fill( fluID, cryid, 1 );
      fpHisMass->Fill(mass);
   }

   fpNtuMC->SetBit(kValid);

   if (!ValidHistogram()) {
      return kTRUE;
   }

   return kTRUE;
}

