
#include <iostream>
#include <fstream>
#include <Riostream.h>
#include <stdio.h>

#include "TGeoBBox.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoTrd2.h"
#include "TColor.h"
#include "TList.h"
#include "TMath.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TSystem.h"

#include "TAGgeoTrafo.hxx"
#include "TAGionisMaterials.hxx"

#include "TACAparGeo.hxx"
#include "TAGroot.hxx"

#include "GlobalPar.hxx"

//##############################################################################

const TString TACAparGeo::fgkDefParaName     = "caGeo";
const TString TACAparGeo::fgkBaseName        = "CA";
const Color_t TACAparGeo::fgkDefaultCryCol   = kAzure+6;
const Color_t TACAparGeo::fgkDefaultCryColOn = kRed-5;
const Color_t TACAparGeo::fgkDefaultModCol   = kGray;
const TString TACAparGeo::fgkDefaultCrysName = "caCrys";
const Int_t   TACAparGeo::fgkCrystalsNperModule = 9;


//_____________________________________________________________________________
TACAparGeo::TACAparGeo() 
: TAGparTools(),
  fIonisation(new TAGionisMaterials())
{  
   fkDefaultGeoName = "./geomaps/TACAdetector.map";
}

//______________________________________________________________________________
TACAparGeo::~TACAparGeo()
{
   delete fIonisation;
   Clear();
}

//______________________________________________________________________________
Bool_t TACAparGeo::FromFile(const TString& name)
{ 
   // Read config map created with macro BuildCaGeoFile.C
   cout << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

   TString nameExp;

   fDebugLevel = GlobalPar::GetDebugLevel(ClassName());
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;

   if (fDebugLevel)
      cout << "\nCalorimeter Config File: " << fkDefaultGeoName.Data() << endl; 
   
   if (!Open(nameExp)) return false;

   ReadStrings(fConfigTypeGeo);
   if (fDebugLevel)
      cout << "   Geometry Type: " << fConfigTypeGeo << endl;

   // BGO crystals
   ReadStrings(fCrystalMat);
   if (fDebugLevel)
      cout << "   Crystals material : " <<  fCrystalMat << endl;
   
   ReadItem(fCrystalDensity);
   if (fDebugLevel)
      cout << "   Crystals density : " <<  fCrystalDensity << endl;

   ReadItem(fCrystalIonisMat);
   if (fDebugLevel)
      cout << "   Crystals mean exciation energy : " <<  fCrystalIonisMat << endl;
      
   TVector3 crystalFront;
   ReadVector3(crystalFront);
   if (fDebugLevel) 
      cout << "   Crystal front size: "
           <<  crystalFront[0]  << " " <<  crystalFront[1] << " " << crystalFront[2] << endl;

   TVector3 crystalBack;
   ReadVector3(crystalBack);
   if (fDebugLevel) 
      cout << "   Crystal back size: "
           << crystalBack[0] << " " << crystalBack[1] << " " << crystalBack[2] << endl;

   fCrystalSize[0] = crystalFront[0];
   fCrystalSize[1] = crystalBack[0];
   fCrystalSize[2] = crystalFront[1];
   fCrystalSize[3] = crystalBack[1];
   fCrystalSize[4] = crystalBack[2];

   ReadItem(fCrystalDelta);
   if (fDebugLevel)
      cout << "   Crystals in-between distance : " <<  fCrystalDelta << endl;

   ReadStrings(fSupportMat);
   if (fDebugLevel)
      cout << "   Support material : " <<  fSupportMat << endl;
   
   ReadItem(fSupportDensity);
   if (fDebugLevel)
      cout << "   Support density : " <<  fSupportDensity << endl;

   // Support    
   TVector3 supportFront;
   ReadVector3(supportFront);
   if (fDebugLevel) 
      cout << "   Support front size: "
           << supportFront[0] << " " << supportFront[1] << " " << supportFront[2] << endl;
   TVector3 supportBack;
   ReadVector3(supportBack);
   if (fDebugLevel) 
      cout << "   Support back size: "
           << supportBack[0] << " " << supportBack[1] << " " << supportBack[2] << endl;
   fSupportSize[0] = supportFront[0];
   fSupportSize[1] = supportBack[0];
   fSupportSize[2] = supportFront[1];
   fSupportSize[3] = supportBack[1];
   fSupportSize[4] = supportBack[2];

   ReadItem(fSupportPositionZ);
   if (fDebugLevel)
      cout  << "   Support PositionZ : " <<  fSupportPositionZ << endl;

   // define material
   DefineMaterial();

   // Air around each module. It is need ONLY by FLUKA geometry
   // it will be remove if a truncate piramid body is implemented in FLUKA    
   TVector3 airModflukaFront;
   ReadVector3(airModflukaFront);
   if (fDebugLevel) 
      cout << "   AIR region around module front size: "
           << airModflukaFront[0] << " " << airModflukaFront[1] << " " << airModflukaFront[2] << endl;
   TVector3 airModflukaBack;
   ReadVector3(airModflukaBack);
   if (fDebugLevel) 
      cout << "   AIR region around module back size: "
           << airModflukaBack[0] << " " << airModflukaBack[1] << " " << airModflukaBack[2] << endl;
   fModAirFlukaSize[0] = airModflukaFront[0];
   fModAirFlukaSize[1] = airModflukaBack[0];
   fModAirFlukaSize[2] = airModflukaFront[1];
   fModAirFlukaSize[3] = airModflukaBack[1];
   fModAirFlukaSize[4] = airModflukaBack[2];

   ReadItem(fModAirFlukaPositionZ);
   if (fDebugLevel)
      cout  << "   AIR region around module PositionZ : " <<  fModAirFlukaPositionZ << endl;

   // Calorimeter dimensions and position of box around
   ReadVector3(fCaloSize);
   if(fDebugLevel)
      cout << "   Calorimeter size: "
      << fCaloSize[0] << " " << fCaloSize[1] << " " << fCaloSize[2] << endl;
   ReadItem(fCaloBoxPositionZ);
   if (fDebugLevel)
      cout  << "   AIR region around detector PositionZ : " <<  fCaloBoxPositionZ << endl;

   TVector3 position;
   TVector3 tilt;

   // Crystals
   ReadItem(fCrystalsN);
   if (fDebugLevel)
      cout  << "Number of crystals: " <<  fCrystalsN << endl;
   ReadItem(fModulesN);
   if (fDebugLevel)
      cout  << "Number of modules: " <<  fModulesN << endl;  

   // Set number of matrices
   SetupMatrices(fCrystalsN + fModulesN);
   fListOfCrysAng.resize(fCrystalsN);

   // Read transformtion info
   Int_t idCry = 0;
   for (Int_t iCry = 0; iCry < fCrystalsN; ++iCry) {
      
      ReadItem(idCry);
      if(fDebugLevel)
         cout  << "Crystal id " << idCry << endl;
      
      // read  position
      ReadVector3(position);
      if(fDebugLevel)
         cout << "   Position: "
         << position[0] << " " << position[1] << " " << position[2] << endl;
      
      ReadVector3(tilt);
      if(fDebugLevel)
         cout  << "   tilt: " << tilt[0] << " " << tilt[1] << " " << tilt[2] << endl;

      fListOfCrysAng[idCry] = tilt;

      TGeoRotation rot;
      rot.RotateX(tilt[0]);
      rot.RotateY(tilt[1]);
      rot.RotateZ(tilt[2]);
      
      TGeoTranslation trans(position[0], position[1], position[2]);
      
      TGeoHMatrix  transfo;
      transfo  = trans;
      transfo *= rot;
      AddTransMatrix(new TGeoHMatrix(transfo), idCry);
   }

   // Modules 
   // Read transformtion info
   Int_t nModule = 0;
   fListOfModAng.resize(fModulesN);
   for (Int_t imod = 0; imod < fModulesN; ++imod) {
      
      ReadItem(nModule);
      if(fDebugLevel)
         cout  << "Module id "<< nModule << endl;
      
      // read  position
      ReadVector3(position);
      if(fDebugLevel)
         cout << "   Position: "
         << position[0] << " " << position[1] << " " << position[2] << endl;

      ReadVector3(tilt);
      if(fDebugLevel)
         cout  << "   tilt: " << tilt[0] << " " << tilt[1] << " " << tilt[2] << endl;

      fListOfModAng[nModule] = tilt;

      TGeoRotation rot;
      rot.RotateX(tilt[0]);
      rot.RotateY(tilt[1]);
      rot.RotateZ(tilt[2]);
      
      TGeoTranslation trans(position[0], position[1], position[2]);
      
      TGeoHMatrix  transfo;
      transfo  = trans;
      transfo *= rot;
      AddTransMatrix(new TGeoHMatrix(transfo), fCrystalsN + nModule);
   }
   
   return true;
}

//_____________________________________________________________________________
TGeoVolume*  TACAparGeo::BuildCalorimeter(const char *caName)  
{
   if (fDebugLevel) {
      cout << "\n\nTACAparGeo::BuildCalorimeter" << endl << endl;
   }

   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager(TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   TGeoVolume* detector = gGeoManager->FindVolumeFast(caName);
   if ( detector == 0x0 ) {
      detector = new TGeoVolumeAssembly(caName);
   }
   
   // half dimensions of BGO crystal
   double xdim1 = fCrystalSize[0];
   double xdim2 = fCrystalSize[1]; 
   double ydim1 = fCrystalSize[2]; 
   double ydim2 = fCrystalSize[3];
   double zdim  = fCrystalSize[4];

   // half dimensions of module support
   double xdim1s = fSupportSize[0];
   double xdim2s = fSupportSize[1];
   double ydim1s = fSupportSize[2];
   double ydim2s = fSupportSize[3];
   double zdims  = fSupportSize[4];
   
   const Char_t* matName = fCrystalMat.Data();
   TGeoMedium*   medBGO  = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);

   const Char_t* matSupName = fSupportMat.Data();
   TGeoMedium*   medSup     = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matSupName);

   for (Int_t iCry = 0; iCry < fCrystalsN; ++iCry) {
      TGeoCombiTrans* hm = GetCombiTransfo(iCry);
      if (hm) {
         TGeoVolume *caloCristal = gGeoManager->MakeTrd2(GetDefaultCrysName(iCry), medBGO, xdim1, xdim2, ydim1, ydim2, zdim);
         caloCristal->SetLineColor(fgkDefaultCryCol);
         caloCristal->SetFillColor(fgkDefaultCryCol);
         caloCristal->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
         detector->AddNode(caloCristal, iCry, hm);

	 /*
	   AS:: Turned off the 'crystal per crystal construction'
	   if (iCry % fgkCrystalsNperModule == 0) {
	   TGeoVolume *support = gGeoManager->MakeTrd2("modSup", medSup, xdim1s, xdim2s, ydim1s, ydim2s, zdims);
	   support->SetLineColor(fgkDefaultModCol);
	   support->SetFillColor(fgkDefaultModCol);
	   // support->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
           
	   // Add Z shift to the support
	   TGeoCombiTrans* hms = new TGeoCombiTrans(*hm);
	   *hms *=  TGeoTranslation(0, 0, fSupportPositionZ);
	   detector->AddNode(support, iCry+fCrystalsN, hms);
	   }
	 */
      }
   }

   for (Int_t imod = 0; imod < fModulesN; ++imod) {
     TGeoCombiTrans* hm = GetCombiTransfo(fCrystalsN+imod);
     if (hm) {
       TGeoVolume *support = gGeoManager->MakeTrd2("modSup", medSup, xdim1s, xdim2s, ydim1s, ydim2s, zdims);
       support->SetLineColor(fgkDefaultModCol);
       support->SetFillColor(fgkDefaultModCol);
       // support->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
       // Add Z shift to the support
       *hm *=  TGeoTranslation(0, 0, fSupportPositionZ);
       detector->AddNode(support, imod, hm);
     }
   }

   return detector;
}

//_____________________________________________________________________________
TVector3 TACAparGeo::GetCrystalPosition(Int_t idx)
{
   TGeoHMatrix* hm = GetTransfo(idx);
   if (hm) {
      TVector3 local(0,0,0);
      fCurrentPosition =  Crystal2Detector(idx, local);
   }
   return fCurrentPosition;   
}

//_____________________________________________________________________________
TVector3 TACAparGeo::GetCrystalAngle(Int_t idx)
{
   if (idx < 0 || idx > fCrystalsN) {
      Warning("GetCrystalAngle()","Wrong crystal id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return fListOfCrysAng[idx];
}

//_____________________________________________________________________________
TVector3 TACAparGeo::GetModulePosition(Int_t idx)
{
   TGeoHMatrix* hm = GetTransfo(fCrystalsN + idx);
   if (hm) {
      TVector3 local(0,0,0);
      fCurrentPosition =  LocalToMaster(fCrystalsN + idx, local);
   }
   return fCurrentPosition;   
}

//_____________________________________________________________________________
TVector3 TACAparGeo::GetModuleAngle(Int_t idx)
{
   if (idx < 0 || idx > fCrystalsN) {
      Warning("GetCrystalAngle()","Wrong crystal id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return fListOfModAng[idx];
}

//_____________________________________________________________________________
TVector3 TACAparGeo::Crystal2Detector(Int_t idx, TVector3& loc) const
{
   if (idx < 0 || idx > fCrystalsN) {
      Warning("Crystal2Detector()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return LocalToMaster(idx, loc);
}


//_____________________________________________________________________________
TVector3 TACAparGeo::Crystal2DetectorVect(Int_t idx, TVector3& loc) const
{
   if (idx < 0 || idx > fCrystalsN) {
      Warning("Crystal2DetectorVect()","Wrong detector id number: %d ", idx);
      TVector3(0,0,0);
   }
   
   return LocalToMasterVect(idx, loc);
}

//_____________________________________________________________________________
TVector3 TACAparGeo::Detector2Crystal(Int_t idx, TVector3& glob) const
{
   if (idx < 0 || idx > fCrystalsN) {
      Warning("Detector2Sensor()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return MasterToLocal(idx, glob);
}

//_____________________________________________________________________________
TVector3 TACAparGeo::Detector2CrystalVect(Int_t idx, TVector3& glob) const
{
   if (idx < 0 || idx > fCrystalsN) {
      Warning("Detector2SensorVect()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return MasterToLocalVect(idx, glob);
}

//_____________________________________________________________________________
void TACAparGeo::DefineMaterial()
{
   
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // CA material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fCrystalMat, fCrystalDensity);
   if (fDebugLevel) {
      printf("Calorimeter material:\n");
      mat->Print();
   }

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
   fIonisation->SetMaterial(mat);
   fIonisation->AddBirksFactor(fCrystalIonisMat); 
#else
   fIonisation->SetMeanExcitationEnergy(fCrystalIonisMat);
   
   // put it under Cerenkov since only this EM property is available
   mat->SetCerenkovProperties(fIonisation); 
#endif

}


//_____________________________________________________________________________
//! set color on for fired bars
void TACAparGeo::SetCrystalColorOn(Int_t idx)
{
   if (!gGeoManager) {
      Error("SetBarcolorOn()", "No Geo manager defined");
      return;
   }
   
   TString name = GetDefaultCrysName(idx);
   
   TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
   if (vol)
      vol->SetLineColor(GetDefaultCryColOn());
}

//_____________________________________________________________________________
//! reset color for unfired bars
void TACAparGeo::SetCrystalColorOff(Int_t idx)
{
   if (!gGeoManager) {
      Error("SetBarcolorOn()", "No Geo manager defined");
      return;
   }
   
   TString name = GetDefaultCrysName(idx);
   
   TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
   if (vol)
      vol->SetLineColor(GetDefaultCryCol());
}


//_____________________________________________________________________________
string TACAparGeo::PrintRotations()
{
   // Defines rotations and translations to be applied 
   stringstream ss;

   if (GlobalPar::GetPar()->IncludeCA()) {

      TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   
      TVector3 center = fpFootGeo->GetCACenter();
      TVector3  angle = fpFootGeo->GetCAAngles();

      if ( angle.Mag()!=0 ) {

         // put the detector in local coord before the rotation
         ss << PrintCard("ROT-DEFI", "", "", "",
               Form("%f",-center.X()),
               Form("%f",-center.Y()),
               Form("%f",-center.Z()),
               Form("air_rot") ) << endl;

         if (angle.X() != 0) {
            ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",angle.X()),"", "", 
                  "", Form("air_rot")) << endl;
         }
         if (angle.Y() != 0) {
            ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",angle.Y()),"", "", 
                  "", Form("air_rot")) << endl;
         }
         if (angle.Z() !=0) {
            ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",angle.Z()),"", "", 
                  "", Form("air_rot")) << endl;
         }

         //put back the detector in global coord
         ss << PrintCard("ROT-DEFI", "", "", "",
               Form("%f",center.X()), 
               Form("%f",center.Y()),
               Form("%f",center.Z()), 
               Form("air_rot")) << endl;
      }
   }

   return ss.str();

}


//_____________________________________________________________________________
string TACAparGeo::PrintBodies() 
{

   stringstream outstr;  
   outstr << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

   if ( !GlobalPar::GetPar()->IncludeCA())
      return outstr.str();

   outstr << "* ***Calorimeter" << endl;

   // get global calorimter position
   TVector3 center, angle;
   TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   if (fpFootGeo) {
      center = fpFootGeo->GetCACenter();
      angle  = fpFootGeo->GetCAAngles();
   }

   if ( angle.Mag()!=0 )
	   outstr << "$start_transform " << Form("air_rot") << endl;
        
   // Add an air box for calorimeter
   outstr << TString::Format("RPP air_cal  %.12f %.12f %.12f %.12f %.12f %.12f\n", 
                     center[0] - fCaloSize[0]/2.- 2, center[0] + fCaloSize[0]/2. + 2, 
                     center[1] - fCaloSize[1]/2.- 2, center[1] + fCaloSize[1]/2. + 2, 
                     // TODO fix 0.08
                     center[2] - fCaloSize[2]/2.+ fCaloBoxPositionZ, center[2] + fCaloSize[2]/2. + fCaloBoxPositionZ+5);

   if ( angle.Mag()!=0 )
      outstr << "$end_transform "  << endl;

   // Add crystal bodies (6 PLA bodies)
   TString cryBodyName = "P";
   for (Int_t iCry=0; iCry<fCrystalsN; iCry++) {  
      TGeoCombiTrans* hm = GetCombiTransfo(iCry);
      if (hm) {
         outstr << SPrintCrystalBody( iCry, hm, cryBodyName, fCrystalSize );
      }
   }    

   // air regions around each module (6 PLA bodies)
   // AS: restored a 'per module' positioning
   TString modBodyName = "AP";
   for (Int_t imod=0; imod<fModulesN; imod++) {  
      TGeoCombiTrans* hm = GetCombiTransfo(fCrystalsN + imod);
      if (hm) {
         *hm *=  TGeoTranslation(0, 0, fModAirFlukaPositionZ);
         outstr << SPrintCrystalBody( imod, hm, modBodyName, fModAirFlukaSize );
      }
   } 

   // FULL Detector: 
   // Divide air_cal with planes in order to have
   // only few modules in each region
   if (fConfigTypeGeo.CompareTo("FULL_DET") == 0) {
      TString plaName = "MP";
      int dir[2];
      // Vertical planes
      dir[0] = 1; dir[1] = 0; //dir[0] =1 -> right plane
      int id = 16;
      TGeoCombiTrans* hm = GetCombiTransfo(fCrystalsN + id);
      outstr << SPrintParallelPla( id, hm, plaName, fModAirFlukaSize, dir );
      id = 4;
      hm = GetCombiTransfo(fCrystalsN + id);
      outstr << SPrintParallelPla( id, hm, plaName, fModAirFlukaSize, dir );
      id = 0;
      hm = GetCombiTransfo(fCrystalsN + id);
      outstr << SPrintParallelPla( id, hm, plaName, fModAirFlukaSize, dir );
      id = 6; dir[0] = -1; //dir[0] =-1 -> left plane
      hm = GetCombiTransfo(fCrystalsN + id);
      outstr << SPrintParallelPla( id, hm, plaName, fModAirFlukaSize, dir );
      id = 18;
      hm = GetCombiTransfo(fCrystalsN + id);
      outstr << SPrintParallelPla( id, hm, plaName, fModAirFlukaSize, dir );
      // horizontal planes
      dir[0] = 0; dir[1] = 1; //dir[1] =1 -> top plane
      id = 9;
      hm = GetCombiTransfo(fCrystalsN + id);
      outstr << SPrintParallelPla( id, hm, plaName, fModAirFlukaSize, dir );
      id = 11;
      hm = GetCombiTransfo(fCrystalsN + id);
      outstr << SPrintParallelPla( id, hm, plaName, fModAirFlukaSize, dir );
      dir[1] = -1;  //dir[1] =1 -> botton plane
      id = 8;
      hm = GetCombiTransfo(fCrystalsN + id);
      outstr << SPrintParallelPla( id, hm, plaName, fModAirFlukaSize, dir );
      id = 10;
      hm = GetCombiTransfo(fCrystalsN + id);
      outstr << SPrintParallelPla( id, hm, plaName, fModAirFlukaSize, dir );
   }

   return outstr.str();
}

//-----------------------------------------------------------------------------
// Print the planes parallel to a module
TString TACAparGeo::SPrintParallelPla( int id, TGeoCombiTrans* hm, TString bodyName, 
                                       Double_t *trd2Size, int * dir )
{
   // dir[0] X direction could be (-1, 0, 1)
   // dir[1] Y direction could be (-1, 0, 1)

   if ( dir[0] != 0 && dir[1] != 0 ) {
      cout << "ErrorTACAparGeo::SPrintParallelPla: Incompatible \"dir\" parameter\n" << endl;
      exit(-1);
   }

   TString outstr;
   Double_t * local    = new Double_t [3];
   local[0] = local[1] = local[2] = 0;
   Double_t * normal   = new Double_t [3];
   Double_t * point    = new Double_t [3];

   // Get global transformation
   TVector3 center, angle;
   TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   if (fpFootGeo) {
      center = fpFootGeo->GetCACenter();
      angle  = fpFootGeo->GetCAAngles();
   }

   TGeoTranslation detTrasl(center.X(), center.Y(), center.Z()); 
   TGeoRotation detRot;
   detRot.RotateX(-angle.X());
   detRot.RotateY(-angle.Y());
   detRot.RotateZ(-angle.Z());
   TGeoCombiTrans detPos(detTrasl, detRot);

   TGeoHMatrix  matCurrent(detPos);
   matCurrent.Multiply(hm);

   double xdim1 = trd2Size[0];
   double xdim2 = trd2Size[1]; 
   double ydim1 = trd2Size[2]; 
   double ydim2 = trd2Size[3];
   double zdim  = trd2Size[4];

   const Double_t* mat = matCurrent.GetRotationMatrix();
   Double_t angleX = TMath::ATan2(mat[7], mat[8]);
   Double_t angleY = TMath::ASin(-mat[6]);

   double dim1, dim2, angleM;
   if (dir[0] != 0) {
      dim1 = xdim1;
      dim2 = xdim2;
      angleM = angleX;
   }
   if (dir[1] != 0) {
      dim1 = ydim1;
      dim2 = ydim2;
      angleM = angleY;
   }
   ////////////////////////////////
   // Planes normal to  dir


   double deltaX = (dim2 - dim1);
   double trp_hipot = TMath::Sqrt( zdim * zdim  + deltaX * deltaX );
   double alfaX = TMath::ASin (deltaX / trp_hipot);
   local[0] = dir[0] * TMath::Sin(alfaX) * zdim;
   local[1] = dir[1] * TMath::Sin(alfaX) * zdim;
   local[2] = -zdim * (1.0-TMath::Cos(alfaX));
   matCurrent.LocalToMasterVect(local, normal); // normal to face pointed out
   local[0] = dir[0] * (dim1 + 0.95 * fCrystalDelta * TMath::Cos(angleM)); 
   local[1] = dir[1] * (dim1 + 0.95 * fCrystalDelta * TMath::Cos(angleM)); 
   local[2] = -zdim; 
   matCurrent.LocalToMaster(local, point); // point on face
   outstr += TString::Format("PLA %s%03d  %.12f %.12f %.12f %.12f %.12f %.12f\n", bodyName.Data(), id, normal[0], normal[1], normal[2] 
                                   ,point[0], point[1], point[2]);

   return outstr;
}

//-----------------------------------------------------------------------------
// Print the body definition in FLUKA FREE format
TString TACAparGeo::SPrintCrystalBody( int id, TGeoCombiTrans* hm, TString bodyName, Double_t *trd2Size ) 
{
   // There is no truncate piramid in FLUKA, so we need to define 
   // 6 half planes

   TString outstr;

   Double_t * local    = new Double_t [3];
   local[0] = local[1] = local[2] = 0;
   Double_t * normal   = new Double_t [3];
   Double_t * point    = new Double_t [3];

   // Get global transformation
   TVector3 center, angle;
   TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   if (fpFootGeo) {
      center = fpFootGeo->GetCACenter();
      angle  = fpFootGeo->GetCAAngles();
   }

   TGeoTranslation detTrasl(center.X(), center.Y(), center.Z()); 
   TGeoRotation detRot;
   detRot.RotateX(-angle.X());
   detRot.RotateY(-angle.Y());
   detRot.RotateZ(-angle.Z());
   TGeoCombiTrans detPos(detTrasl, detRot);

   TGeoHMatrix  matCurrent(detPos);
   matCurrent.Multiply(hm);

   double xdim1 = trd2Size[0];
   double xdim2 = trd2Size[1]; 
   double ydim1 = trd2Size[2]; 
   double ydim2 = trd2Size[3];
   double zdim  = trd2Size[4];
 
   ////////////////////////////////
   // Planes normal to Z
   local[2] =  -zdim;
   matCurrent.LocalToMasterVect(local, normal); // normal to front face pointed out
   matCurrent.LocalToMaster(local, point); // point on front face
   outstr = TString::Format("PLA %s%03d_1  %.12f %.12f %.12f %.12f %.12f %.12f\n", bodyName.Data(), id, normal[0] ,normal[1], normal[2] 
                                   ,point[0], point[1], point[2]);

   local[2] =  zdim;
   matCurrent.LocalToMasterVect(local, normal); 
   matCurrent.LocalToMaster(local, point); // point on back face
   outstr += TString::Format("PLA %s%03d_2  %.12f %.12f %.12f %.12f %.12f %.12f\n", bodyName.Data(), id, normal[0] ,normal[1], normal[2] 
                                   ,point[0], point[1], point[2]);

   ////////////////////////////////
   // Planes normal to X
   double deltaX = (xdim2 - xdim1);
   double trp_hipot = TMath::Sqrt( zdim * zdim  + deltaX * deltaX );
   double alfaX = TMath::ASin (deltaX / trp_hipot);
   //double simAlfaX = (deltaX / trp_hipot);
   //double cosAlfaX = TMath::Sqrt(1.0 - simAlfaX*simAlfaX);
   local[0] = TMath::Sin(alfaX) * zdim;
   local[1] = 0;
   local[2] = -zdim * (1.0-TMath::Cos(alfaX));
   matCurrent.LocalToMasterVect(local, normal); // normal to right face pointed out
   local[0] = xdim1; 
   local[1] = 0; 
   local[2] = -zdim; 
   matCurrent.LocalToMaster(local, point); // point on right face
   outstr += TString::Format("PLA %s%03d_3  %.12f %.12f %.12f %.12f %.12f %.12f\n", bodyName.Data(), id, normal[0], normal[1], normal[2] 
                                   ,point[0], point[1], point[2]);
 
   
   local[0] = -(TMath::Sin(alfaX) * zdim);
   local[1] = 0;
   local[2] = -zdim*(1.0 - TMath::Cos(alfaX));
   matCurrent.LocalToMasterVect(local, normal); // normal to left face  pointed out 
   local[0] = -xdim1;
   local[1] = 0; 
   local[2] = -zdim; 
   matCurrent.LocalToMaster(local, point); // point on left face
   outstr += TString::Format("PLA %s%03d_4  %.12f %.12f %.12f %.12f %.12f %.12f\n", bodyName.Data(), id,  normal[0], normal[1], normal[2] 
                                   ,point[0], point[1], point[2]);

   ////////////////////////////////
   // Planes normal to Y
   double deltaY = (ydim2 - ydim1);
   trp_hipot = TMath::Sqrt( zdim * zdim  + deltaY * deltaY );
   double alfaY = TMath::ASin (deltaY / trp_hipot);
   local[0] = 0;
   local[1] = TMath::Sin(alfaY) * zdim;
   local[2] = -zdim*(1.0-TMath::Cos(alfaY));
   matCurrent.LocalToMasterVect(local, normal); // normal to top face pointed out 
   local[0] = 0;
   local[1] = ydim1;
   local[2] = -zdim;
   matCurrent.LocalToMaster(local, point); // point on top face
   outstr += TString::Format("PLA %s%03d_5  %.12f %.12f %.12f %.12f %.12f %.12f\n", bodyName.Data(), id, normal[0], normal[1], normal[2] 
                                   ,point[0], point[1], point[2]);

 
   local[0] = 0;      
   local[1] = -(TMath::Sin(alfaY) * zdim);
   local[2] = -zdim*(1.0-TMath::Cos(alfaY));
   matCurrent.LocalToMasterVect(local, normal); // normal to bottom face pointed out
   local[0] = 0; 
   local[1] = -ydim1;
   local[2] = -zdim;
   matCurrent.LocalToMaster(local, point); // point on bottom face
   outstr += TString::Format("PLA %s%03d_6  %.12f %.12f %.12f %.12f %.12f %.12f\n", bodyName.Data(), id, normal[0], normal[1], normal[2] 
                                   ,point[0], point[1], point[2]);

   delete [] local;
   delete [] point;
   delete [] normal;

   return outstr;
 
}

//_____________________________________________________________________________
string TACAparGeo::PrintRegions() 
{
   stringstream outstr;  

   if ( !GlobalPar::GetPar()->IncludeCA())
      return outstr.str();
 
   outstr << "* ***Calorimeter" << endl;

   TString line;
   for (Int_t id=0; id<fCrystalsN; id++) {
      line.Form("CAL%03d 5 +P%03d_1 +P%03d_2 +P%03d_3 +P%03d_4 +P%03d_5 +P%03d_6\n", 
                                     id, id, id, id, id, id, id );
      outstr << line.Data();
   }

   outstr << PrintModuleAirRegions();

   return outstr.str();
}

//_____________________________________________________________________________
TString TACAparGeo::PrintModuleAirRegions() 
{
   TString modRegion = "";

   if (fModulesN <= 0) return modRegion;

   TString line;
   for( int im=0; im<fModulesN; im++ ) {

      line.Form("ACAL_%02d       5 ", im);
      modRegion += line.Data();

      // create piramid in front and back of each crystal
      Int_t iCry = im * fgkCrystalsNperModule;
      for (Int_t id=0; id<fgkCrystalsNperModule; id++) {
         line.Form(" | +AP%03d_1 -P%03d_1 +P%03d_3 +P%03d_4 +P%03d_5 +P%03d_6\n", 
                  im, iCry+id, iCry+id, iCry+id, iCry+id, iCry+id);
         modRegion += line.Data();
         line.Form(" | +AP%03d_2 -P%03d_2 +P%03d_3 +P%03d_4 +P%03d_5 +P%03d_6\n", 
                  im, iCry+id, iCry+id, iCry+id, iCry+id, iCry+id);
         modRegion += line.Data();
      }
      
      // create regions between crytals in a RAW
      Int_t nRows = 3; 
      iCry = im * fgkCrystalsNperModule;
      for (Int_t ir=0; ir<nRows; ir++) {
         line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_3 -P%03d_4 +P%03d_5 +P%03d_6\n", 
                  im, im, iCry, iCry+1, iCry+1, iCry+1);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_4 -P%03d_3 +P%03d_5 +P%03d_6\n", 
                  im, im, iCry, iCry+2, iCry+2, iCry+2);
         modRegion += line.Data();
         iCry += 3;
      }

      // create regions between RAWs in a module
      iCry = im * fgkCrystalsNperModule;
      // central
      line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_6 -P%03d_5 +P%03d_4 +P%03d_3\n", 
               im, im, iCry+3, iCry, iCry+3, iCry+3);
      modRegion += line.Data();
      // left
      line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_6 -P%03d_5 +P%03d_4 -P%03d_4 -(P%03d_4 +P%03d_5)\n", 
               im, im, iCry+5, iCry+2, iCry+5, iCry+3, iCry, iCry);
      modRegion += line.Data();
      // right
      line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_6 -P%03d_5 +P%03d_3 -P%03d_3 -(P%03d_3 +P%03d_5)\n", 
               im, im, iCry+4, iCry+1, iCry+4, iCry+3, iCry, iCry);
      modRegion += line.Data();

      // central
      line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_5 -P%03d_6 +P%03d_4 +P%03d_3\n", 
               im, im, iCry+6, iCry, iCry+6, iCry+6);
      modRegion += line.Data();
      // left
      line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_5 -P%03d_6 +P%03d_4 -P%03d_4 -(P%03d_4 +P%03d_6)\n", 
               im, im, iCry+8, iCry+2, iCry+8, iCry+6, iCry, iCry);
      modRegion += line.Data();
      // right
      line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_5 -P%03d_6 +P%03d_3 -P%03d_3 -(P%03d_3 +P%03d_6)\n", 
               im, im, iCry+7, iCry+1, iCry+7, iCry+6, iCry, iCry);
      modRegion += line.Data();
      
   
      //////////////////////////////////////////////////////////
      // Fill everything outside crystals
      iCry = im * fgkCrystalsNperModule;
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_3 -P%03d_3 +P%03d_5 +P%03d_6\n", 
                 im, im, im, iCry+1, iCry+1, iCry+1);
      modRegion += line.Data();
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_4 -P%03d_4 +P%03d_5 +P%03d_6\n", 
                 im, im, im, iCry+2, iCry+2, iCry+2);
      modRegion += line.Data();
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_5 -P%03d_5 +P%03d_4 +P%03d_3\n", 
                 im, im, im, iCry+3, iCry+3, iCry+3);
      modRegion += line.Data();
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_6 -P%03d_6 +P%03d_4 +P%03d_3\n", 
                 im, im, im, iCry+6, iCry+6, iCry+6);
      modRegion += line.Data();

      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_3 +AP%03d_5 -P%03d_3 -P%03d_5\n", 
                 im, im, im, im,  iCry+4, iCry+1);
      modRegion += line.Data();
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_3 +AP%03d_6 -P%03d_3 -P%03d_6\n", 
                 im, im, im, im,  iCry+7, iCry+1);
      modRegion += line.Data();
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_4 +AP%03d_5 -P%03d_4 -P%03d_5\n", 
                 im, im, im, im,  iCry+5, iCry+2);
      modRegion += line.Data();
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_4 +AP%03d_6 -P%03d_4 -P%03d_6\n", 
                 im, im, im, im,  iCry+8, iCry+2);
      modRegion += line.Data();

      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_5 -P%03d_5 +P%03d_4 -P%03d_4\n", 
                 im, im, im, iCry+5, iCry+5, iCry+3);
      modRegion += line.Data();
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_5 -P%03d_5 +P%03d_3 -P%03d_3\n", 
                 im, im, im, iCry+4, iCry+4, iCry+3);
      modRegion += line.Data();
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_6 -P%03d_6 +P%03d_3 -P%03d_3\n", 
                 im, im, im, iCry+7, iCry+7, iCry+6);
      modRegion += line.Data();
      line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_6 -P%03d_6 +P%03d_4 -P%03d_4\n", 
                 im, im, im, iCry+8, iCry+8, iCry+6);
      modRegion += line.Data();
   }

   return modRegion;
}

//_____________________________________________________________________________
string TACAparGeo::PrintSubtractBodiesFromAir() 
{

   stringstream outstr;  

   if ( !GlobalPar::GetPar()->IncludeCA())
      return outstr.str(); 

   outstr << TString::Format(" - air_cal\n");
   outstr << TString::Format("AIR_CAL0     5");

   TString line; 

   if ( fConfigTypeGeo.CompareTo("ONE_CRY") == 0 ) {
      line.Form(" +air_cal -(P000_1 + P000_2 +P000_3 +P000_4 +P000_5 +P000_6)\n");
      outstr << line.Data();
      return outstr.str();
   }

   // ONE Module
   if ( fConfigTypeGeo.CompareTo("ONE_MOD") == 0 ) {
      line.Form(" +air_cal -(AP000_1 + AP000_2 +AP000_3 +AP000_4 +AP000_5 +AP000_6)\n");
      outstr << line.Data();
      return outstr.str();
   }

   // CENTRAL_DET 2x2 Modules
   if ( fConfigTypeGeo.CompareTo("CENTRAL_DET") == 0 || fConfigTypeGeo.CompareTo("FULL_DET") != 0 ) {
      line.Form(" +air_cal -(AP000_1 + AP000_2 +AP000_3 +AP000_4 +AP000_5 +AP000_6)\n");
      outstr << line.Data();
      line.Form(" +air_cal -(AP001_1 + AP001_2 +AP001_3 +AP001_4 +AP001_5 +AP001_6)\n");
      outstr << line.Data();
      line.Form(" +air_cal -(AP002_1 + AP002_2 +AP002_3 +AP002_4 +AP002_5 +AP002_6)\n");
      outstr << line.Data();
      line.Form(" +air_cal -(AP003_1 + AP003_2 +AP003_3 +AP003_4 +AP003_5 +AP003_6)\n");
      outstr << line.Data();
      return outstr.str();
   }

   // FULL Detector 
   if (fConfigTypeGeo.CompareTo("FULL_DET") == 0) {

      // 4 top modules
      int id = 29;
      line.Form(" +air_cal -MP009 +MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 21;
      line.Form(" +air_cal -MP009 +MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 23;
      line.Form("AIR_CAL1    5 | +air_cal -MP011 -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 31;
      line.Form(" +air_cal -MP011 -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();

      // 4 botton modules
      id = 28;
      line.Form("AIR_CAL2    5 | +air_cal -MP008 +MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 20;
      line.Form(" +air_cal -MP008 +MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 22;
      line.Form("AIR_CAL3    5 | +air_cal -MP010 -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 30;
      line.Form(" +air_cal -MP010 -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();

      // 6 blocks of 4 modules from left to right
      id = 25;
      line.Form("AIR_CAL4    5 | +air_cal +MP016 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 17;
      line.Form(" +air_cal +MP016 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 16;
      line.Form(" +air_cal +MP016 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 24;
      line.Form(" +air_cal +MP016 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      //
      id = 13;
      line.Form("AIR_CAL5    5 | +air_cal -MP016 +MP004 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 5;
      line.Form(" +air_cal -MP016 +MP004 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 4;
      line.Form(" +air_cal -MP016 +MP004 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 12;
      line.Form(" +air_cal -MP016 +MP004 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      //
      id = 9;
      line.Form("AIR_CAL6    5 | +air_cal -MP004 +MP000 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 1;
      line.Form(" +air_cal -MP004 +MP000 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 0;
      line.Form(" +air_cal -MP004 +MP000 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 8;
      line.Form(" +air_cal -MP004 +MP000 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      //
      id = 11;
      line.Form("AIR_CAL7    5 | +air_cal -MP000 -MP006 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 3;
      line.Form(" +air_cal -MP000 -MP006 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 2;
      line.Form(" +air_cal -MP000 -MP006 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 10;
      line.Form(" +air_cal -MP000 -MP006 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      //
      id = 15;
      line.Form("AIR_CAL8    5 | +air_cal +MP006 -MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 7;
      line.Form(" +air_cal +MP006 -MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 6;
      line.Form(" +air_cal +MP006 -MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 14;
      line.Form(" +air_cal +MP006 -MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      //
      id = 27;
      line.Form("AIR_CAL9    5 | +air_cal +MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 19;
      line.Form(" +air_cal +MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 18;
      line.Form(" +air_cal +MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 26;
      line.Form(" +air_cal +MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
   }

   return outstr.str();

}

//_____________________________________________________________________________
string TACAparGeo::PrintParameters() 
{ 
   stringstream outstr;
   outstr << setiosflags(ios::fixed) << setprecision(5);

   outstr << "c     CALORIMETER PARAMETERS " << endl;
   outstr << endl;    
   
   string ncrystal = "ncryCAL";
   outstr << "      integer " << ncrystal << endl;
   outstr << "      parameter(" << ncrystal << " = " << fCrystalsN << ")" << endl;
   outstr << endl;    
   
   return outstr.str();
}


//_____________________________________________________________________________
string TACAparGeo::PrintAssignMaterial(TAGmaterials *Material) 
{
   stringstream outstr;  

   if ( !GlobalPar::GetPar()->IncludeCA())
      return outstr.str(); 

   TString flkmat;  
    
   if (Material == NULL) {
      TAGmaterials::Instance()->PrintMaterialFluka();
      flkmat = TAGmaterials::Instance()->GetFlukaMatName(fCrystalMat.Data());
   } else
      flkmat = Material->GetFlukaMatName(fCrystalMat.Data());

   bool magnetic = false;
   if (GlobalPar::GetPar()->IncludeDI())
      magnetic = true;

   
   if (fConfigTypeGeo.CompareTo("FULL_DET") == 0) {
      outstr << PrintCard("ASSIGNMA", "AIR", "AIR_CAL0", "AIR_CAL9", 
                          "1.", Form("%d",magnetic), "", "") << endl;
   }  else
      outstr << PrintCard("ASSIGNMA", "AIR", "AIR_CAL0", 
                          "", "", Form("%d",magnetic), "", "") << endl;
   
   outstr << PrintCard("ASSIGNMA", flkmat, "CAL000", Form("CAL%03d", fCrystalsN-1), 
                       "1.", Form("%d",magnetic), "", "") << endl;
   
   if (fModulesN == 1) {
      outstr << PrintCard("ASSIGNMA", "AIR", "ACAL_00", 
                          "", "", Form("%d",magnetic), "", "") << endl;
   }
   if (fModulesN > 1) {
      outstr << PrintCard("ASSIGNMA", "AIR", "ACAL_00", Form("ACAL_%02d", fModulesN-1), 
                          "1.", Form("%d",magnetic), "", "") << endl;
   }

   return outstr.str();
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TACAparGeo::Clear(Option_t*)
{
   return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TACAparGeo::ToStream(ostream& os, Option_t*) const
{
   //  os << "TACAparGeo " << GetName() << endl;
   //  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
   //     << "   ver_x   ver_y   ver_z  width" << endl;

   return;
}

//-----------------------------------------------------------------------------
TVector3 TACAparGeo::GetCaloSize()
{
   return fCaloSize;
}

















