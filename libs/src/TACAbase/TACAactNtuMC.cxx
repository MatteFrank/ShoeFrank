/*!
  \file
  \version $Id: TACAactNtuMC.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TACAactNtuMC.
*/
#include "TVector3.h"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TACAparGeo.hxx"
#include "TACAntuRaw.hxx"
#include "TACAactNtuMC.hxx"
#include "TACAdigitizer.hxx"
#include "TGeoElement.h"

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

   // Helper class to sum MC hit of the same particle
   class energyDep : public TObject {
   public:    
      energyDep( int iEvn, int icry, int idpart, float ti, double de ) : 
         index(iEvn), fn(1), fCryid(icry), fid(idpart), fTimeFirstHit(ti), fDE(de) {}
      int index;             // index in EvnStr
      int fn;                // number of Edep  
      int fCryid;            // crystal index hit 
      int fid;               // index in the particle block
      float fTimeFirstHit;   // dep. time at FIRST hit
      double fDE;            // sum Edep
   };
   
   TGeoElementTable table;
   table.BuildDefaultElements();

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
   npart = dep.GetEntriesFast();
   for (int i=0; i<npart; ++i) {
      energyDep* endep = (energyDep*)dep.At(i);
      int index = endep->index;

      Int_t trackId = fpEvtStr->CALid[i] - 1;
      Float_t x0_i  = fpEvtStr->CALxin[index];
      Float_t x0_f  = fpEvtStr->CALxout[index];
      Float_t y0_i  = fpEvtStr->CALyin[index];
      Float_t y0_f  = fpEvtStr->CALyout[index];
      Float_t z0_i  = fpEvtStr->CALzin[index];
      Float_t z0_f  = fpEvtStr->CALzout[index];
      Float_t time  = fpEvtStr->CALtim[index]*TAGgeoTrafo::SecToNs();

      TVector3 posIn(x0_i, y0_i, z0_i);
      TVector3 posInLoc = geoTrafo->FromGlobalToCALocal(posIn);

      TVector3 posOut(x0_f, y0_f, z0_f);
      TVector3 posOutLoc = geoTrafo->FromGlobalToCALocal(posOut);

      // don't use z for the moment
      fDigitizer->Process(endep->fDE, posInLoc[0], posInLoc[1], z0_i, z0_f, time, endep->fCryid);
      TACAntuHit* hit = fDigitizer->GetCurrentHit();
      hit->AddMcTrackId(trackId, i);
      hit->SetPosition(posInLoc);   
   }

   fpNtuMC->SetBit(kValid);

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

      hCA_EnergyReleasePosXY->Fill(posInLoc.X(), posInLoc.Y());
      hCA_EnergyReleasePosZY_in->Fill(posInLoc.Z(), posInLoc.Y());
      hCA_EnergyReleasePosZY_out->Fill(posOutLoc.Z(), posOutLoc.Y());
     
      int fluID = fpEvtStr->TRfid[endep->fid];

      int z = fpEvtStr->TRcha[endep->fid];
      float zf    = fpEvtStr->CALzout[endep->fid];
      double mass = fpEvtStr->TRmass[endep->fid];

      // Select Neutrons
      if ( fluID == 8 ) {
         hCA_EnergyNeutron->Fill(endep->fDE); 
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
            hCA_EnergyIon[z-1]->Fill( eRatio );
            hCA_EnergyDep[z-1]->Fill( eDep );
            hCA_EnergyIonSpect[z-1]->Fill( ek );
            hCA_P_vs_EDepIon[z-1]->Fill( eDep, p );
            hCA_timeFirstHit->Fill(endep->fTimeFirstHit);
         }

         const char* flukaName = getPartNamefromID(fluID);
         if( fluID == -2 ) { // shift HEAVYION by z
            fluID = -40 - z; 
            // check out of range
            if( fluID < -200 ) cout << "fluID " << fluID <<endl; 
         }

      }

      hCA_typeParticleVsRegion->Fill( fluID, endep->fCryid, 1 );
      hCA_FinalPositionVsMass->Fill(zf, mass);
      hCA_Mass->Fill(mass);
      hCA_ChargeVsMass->Fill(z, mass);
   }

   return kTRUE;
}


//------------------------------------------+-----------------------------------

const char* getPartNamefromID(int id)
{
   //Fluka number  Fluka name       

   const char* negPartNames[] = {
      "",
      "OPTIPHOT",       //[-1]
      "HEAVYION",       //[-2]
      "DEUTERON",       //[-3]
      "TRITON  ",       //[-4]
      "3-HELIUM",       //[-5]
      "4-HELIUM"        //[-6]
   };

   const char* partNames[] = {
      "RAY     ",        //[ 0]
      "PROTON  ",        //[ 1]
      "APROTON ",        //[ 2]
      "ELECTRON",        //[ 3]
      "POSITRON",        //[ 4]
      "NEUTRIE ",        //[ 5]
      "ANEUTRIE",        //[ 6]
      "PHOTON  ",        //[ 7]
      "NEUTRON ",        //[ 8]
      "ANEUTRON",        //[ 9]
      "MUON+   ",        //[10]
      "MUON-   ",        //[11]
      "KAONLONG",        //[12]
      "PION+   ",        //[13]
      "PION-   ",        //[14]
      "KAON+   ",        //[15]
      "KAON-   ",        //[16]
      "LAMBDA  ",        //[17]
      "ALAMBDA ",        //[18]
      "KAONSHRT",        //[19]
      "SIGMA-  ",        //[20]
      "SIGMA+  ",        //[21]
      "SIGMAZER",        //[22]
      "PIZERO  ",        //[23]
      "KAONZERO",        //[24]
      "AKAONZER",        //[25]
      "Reserved",        //[26]
      "NEUTRIM ",        //[27]
      "ANEUTRIM",        //[28]
      "Blank   ",        //[29]
      "Reserved",        //[30]
      "ASIGMA- ",        //[31]
      "ASIGMAZE",        //[32]
      "ASIGMA+ ",        //[33]
      "XSIZERO ",        //[34]
      "AXSIZERO",        //[35]
      "XSI-    ",        //[36]
      "AXSI+   ",        //[37]
      "OMEGA-  ",        //[38]
      "AOMEGA+ ",        //[39]
      "Reserved",        //[40]
      "TAU+    ",        //[41]
      "TAU-    ",        //[42]
      "NEUTRIT ",        //[43]
      "ANEUTRIT",        //[44]
      "D+      ",        //[45]
      "D-      ",        //[46]
      "D0      ",        //[47]
      "D0BAR   ",        //[48]
      "DS+     ",        //[49]
      "DS-     ",        //[50]
      "LAMBDAC+",        //[51]
      "XSIC+   ",        //[52]
      "XSIC0   ",        //[53]
      "XSIPC+  ",        //[54]
      "XSIPC0  ",        //[55]
      "OMEGAC0 ",        //[56]
      "ALAMBDC-",        //[57]
      "AXSIC-  ",        //[58]
      "AXSIC0  ",        //[59]
      "AXSIPC- ",        //[60]
      "AXSIPC0 ",        //[61]
      "AOMEGAC0"         //[62]
   };

   if( id < -40 || id >   63 ) { return Form("Invalid Particle ID %d", id); }
   if( id <  -6 && id >= -40 ) { return "NUC-EVAP"; }

   return ((id < 0) ? negPartNames[-id] : partNames[id]);
}

