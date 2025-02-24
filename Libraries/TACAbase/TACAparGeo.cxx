
/*!
 \file TACAparGeo.cxx
 \brief Implementation of TACAparGeo
 */

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

#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGionisMaterials.hxx"

#include "TACAparGeo.hxx"
#include "TAGroot.hxx"

#include "TAGrecoManager.hxx"

//##############################################################################

/*!
  \class TACAparGeo
  \brief Geometry parameters of the Calorimeter
*/

const TString TACAparGeo::fgkBaseName        = "CA";
const Color_t TACAparGeo::fgkDefaultCryCol   = kAzure+6;
const Color_t TACAparGeo::fgkDefaultCryColOn = kRed-5;
const Color_t TACAparGeo::fgkDefaultModCol   = kGray;
const TString TACAparGeo::fgkDefaultCrysName = "caCrys";
const Int_t   TACAparGeo::fgkCrystalsNperModule = 9;

//! Class Imp
ClassImp(TACAparGeo)

//_____________________________________________________________________________
//! Default constructor
TACAparGeo::TACAparGeo()
: TAGparTools(),
  fIonisation(new TAGionisMaterials())
{
   fkDefaultGeoName = "./geomaps/TACAdetector.geo";
   fDummyModulesN = 0;
}

//______________________________________________________________________________
//! Default destructor
TACAparGeo::~TACAparGeo()
{
   delete fIonisation;
   Clear();
}

//------------------------------------------+-----------------------------------
//! Read geometric parameters from file
//!
//! \param[in] name file name
Bool_t TACAparGeo::FromFile(const TString& name)
{
   // Read config map created with macro BuildCaGeoFile.C
   cout << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

   TString nameExp;

   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;

   if (FootDebugLevel(1))
      cout << "\nCalorimeter Config File: " << nameExp.Data() << endl;

   if (!Open(nameExp)) return false;

   Info("FromFile()", "Open file %s for geometry", name.Data());

   ReadStrings(fConfigTypeGeo);
   if (FootDebugLevel(1))
      cout << "   Geometry Type: " << fConfigTypeGeo << endl;

   // BGO crystals
   ReadStrings(fCrystalMat);

   if (FootDebugLevel(1))
      cout << "   Crystals material : " <<  fCrystalMat << endl;

   ReadItem(fCrystalDensity);
   if (FootDebugLevel(1))
      cout << "   Crystals density : " <<  fCrystalDensity << endl;

   ReadItem(fCrystalIonisMat);
   if (FootDebugLevel(1))
      cout << "   Crystals mean excitation energy : " <<  fCrystalIonisMat << endl;

   TVector3 crystalFront;
   ReadVector3(crystalFront);
   if (FootDebugLevel(1))
      cout << "   Crystal front size: "
           <<  crystalFront[0]  << " " <<  crystalFront[1] << " " << crystalFront[2] << endl;

   TVector3 crystalBack;
   ReadVector3(crystalBack);
   if (FootDebugLevel(1))
      cout << "   Crystal back size: "
           << crystalBack[0] << " " << crystalBack[1] << " " << crystalBack[2] << endl;

   fCrystalSize[0] = crystalFront[0];
   fCrystalSize[1] = crystalBack[0];
   fCrystalSize[2] = crystalFront[1];
   fCrystalSize[3] = crystalBack[1];
   fCrystalSize[4] = crystalBack[2];

   ReadItem(fCrystalDelta);
   if (FootDebugLevel(1))
      cout << "   Crystals in-between distance : " <<  fCrystalDelta << endl;

   ReadStrings(fSupportMat);
   if (FootDebugLevel(1))
      cout << "   Support material : " <<  fSupportMat << endl;

   ReadItem(fSupportDensity);
   if (FootDebugLevel(1))
      cout << "   Support density : " <<  fSupportDensity << endl;

   // Support
   TVector3 supportFront;
   ReadVector3(supportFront);
   if (FootDebugLevel(1))
      cout << "   Support front size: "
           << supportFront[0] << " " << supportFront[1] << " " << supportFront[2] << endl;
   TVector3 supportBack;
   ReadVector3(supportBack);
   if (FootDebugLevel(1))
      cout << "   Support back size: "
           << supportBack[0] << " " << supportBack[1] << " " << supportBack[2] << endl;
   fSupportSize[0] = supportFront[0];
   fSupportSize[1] = supportBack[0];
   fSupportSize[2] = supportFront[1];
   fSupportSize[3] = supportBack[1];
   fSupportSize[4] = supportBack[2];

   ReadItem(fSupportPositionZ);
   if (FootDebugLevel(1))
      cout  << "   Support PositionZ : " <<  fSupportPositionZ << endl;

   // define material
   DefineMaterial();

   // Air around each module. It is need ONLY by FLUKA geometry
   // it will be remove if a truncate pyramid body is implemented in FLUKA
   TVector3 airModflukaFront;
   ReadVector3(airModflukaFront);
   if (FootDebugLevel(1))
      cout << "   AIR region around module front size: "
           << airModflukaFront[0] << " " << airModflukaFront[1] << " " << airModflukaFront[2] << endl;
   TVector3 airModflukaBack;
   ReadVector3(airModflukaBack);
   if (FootDebugLevel(1))
      cout << "   AIR region around module back size: "
           << airModflukaBack[0] << " " << airModflukaBack[1] << " " << airModflukaBack[2] << endl;
   fModAirFlukaSize[0] = airModflukaFront[0];
   fModAirFlukaSize[1] = airModflukaBack[0];
   fModAirFlukaSize[2] = airModflukaFront[1];
   fModAirFlukaSize[3] = airModflukaBack[1];
   fModAirFlukaSize[4] = airModflukaBack[2];

   ReadItem(fModAirFlukaPositionZ);
   if (FootDebugLevel(1))
      cout << "   AIR region around module PositionZ : " <<  fModAirFlukaPositionZ << endl;

   // Calorimeter dimensions and position of box around
   ReadVector3(fCaloSize);
   if (FootDebugLevel(1))
      cout << "   Calorimeter size: "
      << fCaloSize[0] << " " << fCaloSize[1] << " " << fCaloSize[2] << endl;
   ReadItem(fCaloBoxPositionZ);
   if (FootDebugLevel(1))
      cout << "   AIR region around detector PositionZ : " <<  fCaloBoxPositionZ << endl;

   TVector3 position;
   TVector3 tilt;

   // ------- Crystals
   ReadItem(fCrystalsN);
   if (FootDebugLevel(1))
      cout << "Number of crystals: " <<  fCrystalsN << endl;
   ReadItem(fModulesN);
   if (FootDebugLevel(1))
      cout << "Number of modules: " <<  fModulesN << endl;

   // Set number of matrices
   SetupMatrices(fCrystalsN + fModulesN);
   fListOfCrysAng.resize(fCrystalsN);

   // Read transformation info
   Int_t idCry = 0;
   for (Int_t iCry = 0; iCry < fCrystalsN; ++iCry) {

      ReadItem(idCry);
      if (FootDebugLevel(1))
         cout << "Crystal id " << idCry << endl;

      // read  position
      ReadVector3(position);
      if (FootDebugLevel(1))
         cout << "   Position: "
         << position[0] << " " << position[1] << " " << position[2] << endl;

      ReadVector3(tilt);
      if (FootDebugLevel(1))
         cout << "   tilt: " << tilt[0] << " " << tilt[1] << " " << tilt[2] << endl;

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

   // -------  Modules
   // Read transformation info
   Int_t nModule = 0;
   fListOfModAng.resize(fModulesN);
   for (Int_t imod = 0; imod < fModulesN; ++imod) {

      ReadItem(nModule);
      if (FootDebugLevel(1))
         cout  << "Module id "<< nModule << endl;

      // read  position
      ReadVector3(position);
      if (FootDebugLevel(1))
         cout << "   Position: "
         << position[0] << " " << position[1] << " " << position[2] << endl;

      ReadVector3(tilt);
      if (FootDebugLevel(1))
         cout << "   tilt: " << tilt[0] << " " << tilt[1] << " " << tilt[2] << endl;

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

   ReadItem(fDummyModulesN);
   if (fDummyModulesN > 0) {
      fListDummyModules.resize(fDummyModulesN);
      if (FootDebugLevel(1))
         cout << "Number of dummy modules:  " << fDummyModulesN << endl;
      for (Int_t imod = 0; imod < fDummyModulesN; ++imod) {
         ReadItem(nModule);
         if (FootDebugLevel(1))
            cout  << "    Dummy Module id "<< nModule << endl;
         fListDummyModules.push_back(nModule);
      }
   }

   ComputeCrystalIndexes();

   return true;
}


//_____________________________________________________________________________
//! Build a row/column index to by used in clustering
//! line (row) increase from left to right
//! column increase from bottom to top
void TACAparGeo::ComputeCrystalIndexes()
{

   TVector3 local;
   TVector3 point;

   // Find number of X and Y different positions of calorimeter crystals

   Double_t width = 0;
   Float_t zdim = GetCrystalThick();
   double xdim1 = GetCrystalWidthFront();
   double ydim1 = GetCrystalHeightFront();
   vector<double> xcry;
   vector<double> ycry;

   for (Int_t iCry = 0; iCry < fCrystalsN; ++iCry) {
      // central point in front
      local[0] = 0 ;
      local[1] = 0 ;
      local[2] = -zdim;

      point =  Crystal2Detector(iCry, local);

      double x = point[0];
      double y = point[1];
      if (iCry == 0) {
         xcry.push_back( x );
         ycry.push_back( y );
      } else {
         bool foundX = false;
         for(std::size_t i = 0; i < xcry.size(); ++i) {
            if ( TMath::Abs(xcry[i] - x) < xdim1 ) foundX = true;
         }
         if (!foundX) xcry.push_back(x);

         bool foundY = false;
         for(std::size_t ii = 0; ii < ycry.size(); ++ii) {
            if ( TMath::Abs(ycry[ii] - y) < ydim1 ) foundY = true;
         }
         if (!foundY) ycry.push_back(y);
      }
   }

   sort(xcry.begin(), xcry.end());
   sort(ycry.begin(), ycry.end());

   fNumCol  = xcry.size();
   fNumLine = ycry.size();

   width    = TMath::Abs( (xcry.back()-xcry[0])/(fNumCol-1) );
   double widthHalf = width/2.;

   if (FootDebugLevel(1)) {
      cout  << "   width: " << width << "   rows: " << fNumLine << " cols: " << fNumCol << endl << endl;
      cout << "Crystal Index map for clustering:" << endl;
   }

   for (Int_t iCry = 0; iCry < fCrystalsN; ++iCry) {
      // central point in front
      local[0] = 0 ;
      local[1] = 0 ;
      local[2] = -zdim;
      point =  Crystal2Detector(iCry, local);

      for (Int_t i = 0; i<fNumCol; ++i) {
         for (Int_t j = 0; j<fNumLine; ++j) {

            if ( (point[0] >= i*width + xcry[0] - widthHalf && point[0] <= (i+1)*width + xcry[0] - widthHalf) &&
                 (point[1] >= j*width + ycry[0] - widthHalf && point[1] <= (j+1)*width + ycry[0] - widthHalf)) {

               pair<int, int> idx(i, j);
               fMapIndexes[iCry] = idx;
               if (FootDebugLevel(1)) {
                  cout  << "   iCry: "  << iCry
                        //<< " " << i << " " << j
                        //<< " pos[0] " << point[0] << " pos[1] " << point[1]
                        << "   col "    << idx.first << " row " << idx.second << endl;
               }
            }
         }
      }
   }

}

//_____________________________________________________________________________
//! Get line (row)
//
//! \param[in] iCry crystal ID
//! \return line (row) position
Int_t TACAparGeo::GetCrystalLine(Int_t iCry)
{
   pair<int, int> idx = fMapIndexes[iCry];

   return idx.first;
}

//_____________________________________________________________________________
//! Get column (row)
//
//! \param[in] iCry crystal ID
//! \return column position
Int_t TACAparGeo::GetCrystalCol(Int_t iCry)
{
   pair<int, int> idx = fMapIndexes[iCry];

   return idx.second;
}


//_____________________________________________________________________________
//! build the Calorimeter
//!
//! \param[in] caName name of the detector
//! \return the root representation of the Calorimeter
TGeoVolume*  TACAparGeo::BuildCalorimeter(const char *caName)
{
   if (FootDebugLevel(1)) {
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

   int iimod = -1;
   for (Int_t iCry = 0; iCry < fCrystalsN; ++iCry) {
      if ( (iCry)%9 == 0 )  iimod++;
      // Skip  crystal in dummy modules
      if ( fDummyModulesN > 0 && 
           std::find(std::begin(fListDummyModules), std::end(fListDummyModules), iimod) != std::end(fListDummyModules))
           continue;

      TGeoCombiTrans* hm = GetCombiTransfo(iCry);
      if (hm) {
         TGeoVolume *caloCristal = gGeoManager->MakeTrd2(GetDefaultCrysName(iCry), medBGO, xdim1, xdim2, ydim1, ydim2, zdim);
         caloCristal->SetLineColor(fgkDefaultCryCol);
         caloCristal->SetFillColor(fgkDefaultCryCol);
         caloCristal->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
         detector->AddNode(caloCristal, iCry, hm);
      }
   }

   for (Int_t imod = 0; imod < fModulesN; ++imod) {
      // Skip dummy modules
      if ( fDummyModulesN > 0 && 
           std::find(std::begin(fListDummyModules), std::end(fListDummyModules), imod) != std::end(fListDummyModules))
           continue;
      TGeoCombiTrans* hm = GetCombiTransfo(fCrystalsN+imod);
      if (hm) {
         TGeoVolume *support = gGeoManager->MakeTrd2("modSup", medSup, xdim1s, xdim2s, ydim1s, ydim2s, zdims);
         support->SetLineColor(fgkDefaultModCol);
         support->SetFillColor(fgkDefaultModCol);
         // Add Z shift to the support
         *hm *=  TGeoTranslation(0, 0, fSupportPositionZ);
         detector->AddNode(support, imod, hm);
      }
   }

   return detector;
}

//_____________________________________________________________________________
//! Get sensor central position on detector framework
//!
//! \param[in] idx sensor index [0 - (fCrystalsN-1)]
//! \return position in detector framework
TVector3 TACAparGeo::GetCrystalPosition(Int_t idx)
{
   TGeoHMatrix* hm = GetTransfo(idx);
   if (hm) {
      TVector3 local(0,0,0);
      fCurrentPosition = Crystal2Detector(idx, local);
   }
   return fCurrentPosition;
}

//_____________________________________________________________________________
//! Get sensor angle on detector framework
//!
//! \param[in] idx sensor index [0 - (fCrystalsN-1)]
//! \return position in detector framework
TVector3 TACAparGeo::GetCrystalAngle(Int_t idx)
{
   if (idx < 0 || idx >= fCrystalsN) {
      Warning("GetCrystalAngle()","Wrong crystal id number: %d ", idx);
      return TVector3(0,0,0);
   }

   return fListOfCrysAng[idx];
}

//_____________________________________________________________________________
//! Get module central position on detector framework
//!
//! \param[in] idx module index [0 - (fModulesN-1)]
//! \return position in detector framework
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
//! Get module angle on detector framework
//!
//! \param[in] idx module index [0 - (fModulesN-1)]
//! \return position in detector framework
TVector3 TACAparGeo::GetModuleAngle(Int_t idx)
{
   if (idx < 0 || idx > fModulesN) {
      Warning("GetModuleAngle()", "Wrong module id number: %d ", idx);
      return TVector3(0,0,0);
   }

   return fListOfModAng[idx];
}

//_____________________________________________________________________________
//! Transformation from sensor to detector framework
//!
//! \param[in] idx sensor index [0 - (fCrystalsN-1)]
//! \param[in] loc position in sensor framework
//! \return position in detector framework
TVector3 TACAparGeo::Crystal2Detector(Int_t idx, TVector3& loc) const
{
   if (idx < 0 || idx >= fCrystalsN) {
      Warning("Crystal2Detector()","Wrong crystal id number: %d ", idx);
      return TVector3(0,0,0);
   }

   return LocalToMaster(idx, loc);
}


//_____________________________________________________________________________
//! Transformation sensor to detector framework for vectors (no translation)
//!
//! \param[in] idx sensor index [0 - (fCrystalsN-1)]
//! \param[in] loc position in sensor framework
//! \return position in detector framework
TVector3 TACAparGeo::Crystal2DetectorVect(Int_t idx, TVector3& loc) const
{
   if (idx < 0 || idx >= fCrystalsN) {
      Warning("Crystal2DetectorVect()","Wrong crystal id number: %d ", idx);
      TVector3(0,0,0);
   }

   return LocalToMasterVect(idx, loc);
}

//_____________________________________________________________________________
//! Transformation from sensor to detector framework
//!
//! \param[in] idx sensor index [0 - (fCrystalsN-1)]
//! \param[in] glob position in detector framework
//! \return position in sensor framework
TVector3 TACAparGeo::Detector2Crystal(Int_t idx, TVector3& glob) const
{
   if (idx < 0 || idx >= fCrystalsN) {
      Warning("Detector2Sensor()", "Wrong crystal id number: %d ", idx);
      return TVector3(0,0,0);
   }

   return MasterToLocal(idx, glob);
}

//_____________________________________________________________________________
//! Transformation from sensor to detector framework for vectors (no translation)
//!
//! \param[in] idx sensor index [0 - (fCrystalsN-1)]
//! \param[in] glob position in detector framework
//! \return position in sensor framework
TVector3 TACAparGeo::Detector2CrystalVect(Int_t idx, TVector3& glob) const
{
   if (idx < 0 || idx >= fCrystalsN) {
      Warning("Detector2SensorVect()","Wrong crystal id number: %d ", idx);
      return TVector3(0,0,0);
   }

   return MasterToLocalVect(idx, glob);
}

//_____________________________________________________________________________
//! Define the Calorimeter material
void TACAparGeo::DefineMaterial()
{

   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }

   // CA material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fCrystalMat, fCrystalDensity);
   if (FootDebugLevel(1)) {
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
//!
//! \param[in] idx crystal id
void TACAparGeo::SetCrystalColorOn(Int_t idx)
{
   if (!gGeoManager) {
      Error("SetCrystalColorOn()", "No Geo manager defined");
      return;
   }

   TString name = GetDefaultCrysName(idx);

   TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
   if (vol)
      vol->SetLineColor(GetDefaultCryColOn());
}

//_____________________________________________________________________________
//! reset color for unfired bars
//!
//! \param[in] idx crystal id
void TACAparGeo::SetCrystalColorOff(Int_t idx)
{
   if (!gGeoManager) {
      Error("SetCrystalColorOff()", "No Geo manager defined");
      return;
   }

   TString name = GetDefaultCrysName(idx);

   TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
   if (vol)
      vol->SetLineColor(GetDefaultCryCol());
}

//_____________________________________________________________________________
//! Print the Calorimeter rotations for the FLUKA geometry file
string TACAparGeo::PrintRotations()
{
   // Defines rotations and translations to be applied
   stringstream ss;

   if (TAGrecoManager::GetPar()->IncludeCA()) {

      TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

      TVector3 center = fpFootGeo->GetCACenter();
      TVector3  angle = fpFootGeo->GetCAAngles();
      angle *= -1;  //invert the angles to take into account the FLUKA convention;

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
//! Print the Calorimeter bodies for the FLUKA geometry file
string TACAparGeo::PrintBodies()
{

   stringstream outstr;
   outstr << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

   if ( !TAGrecoManager::GetPar()->IncludeCA())
      return outstr.str();

   outstr << "* ***Calorimeter, setup: " << fConfigTypeGeo.Data() << endl;

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

   // FIVE Modules or SEVEN Modules: calo geometry for the HIT test beam
   // Divide air_cal in several parts thourgh one vertical plane in order to have
   //   2 and 3 modules in each region (FIVE_MOD)
   //   3, 1, 3 modules in each region  (SEVEN_MOD), (SEVEN_MOD_HIT22)
   if (fConfigTypeGeo.CompareTo("FIVE_MOD") == 0 ||
       fConfigTypeGeo.CompareTo("SEVEN_MOD") == 0 ||
       fConfigTypeGeo.CompareTo("SEVEN_MOD_HIT22") == 0) {
      TString plaName = "MP";
      int dir[2];
      // Vertical
      dir[0] = -1; dir[1] = 0; //dir[0] =-1 -> left plane
      int imod = 2;
      TGeoCombiTrans* hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      if (fConfigTypeGeo.CompareTo("SEVEN_MOD") == 0 ||
          fConfigTypeGeo.CompareTo("SEVEN_MOD_HIT22") == 0) {
         imod = 0;
         hm = GetCombiTransfo(fCrystalsN + imod);
         outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      }
   }

   if (fConfigTypeGeo.CompareTo("TWELVE_MOD_CNAO22") == 0 ) {
      TString plaName = "MP";
      int dir[2];
      // Vertical
      dir[0] = -1; dir[1] = 0; //dir[0] =-1 -> left plane
      int imod = 4;
      TGeoCombiTrans* hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 2;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
   }

   if ( fConfigTypeGeo.CompareTo("FULL_DET_V1") == 0 ||
        fConfigTypeGeo.CompareTo("CNAO_2023") == 0) {
      // 
      TString plaName = "MP";
      int dir[2];

      // horizontal planes
      dir[0] = 0; dir[1] = 1; //dir[1] =1 -> top plane
      int imod = 11;
      TGeoCombiTrans* hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 8;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 14;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 18;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 25;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod =  29;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod =  31;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod =  33;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod =  35;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );

      dir[1] = -1; // bottom plane
      imod = 1;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 3;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 5;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 7;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 22;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 28;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );

      // Vertical planes
      dir[0] = -1; dir[1] = 0; 
      imod = 0;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 16;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 17;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 20; 
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 21; 
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 34; 
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );

      dir[0] = 1;
      imod = 2; 
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 36; 
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );

   }

   // FULL Detector:
   // Divide air_cal with planes in order to have
   // only few modules in each region
   if (fConfigTypeGeo.CompareTo("FULL_DET") == 0) {
      TString plaName = "MP";
      int dir[2];
      // Vertical planes
      dir[0] = 1; dir[1] = 0; //dir[0] =1 -> right plane
      int imod = 16;
      TGeoCombiTrans* hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 4;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 0;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 6; dir[0] = -1; //dir[0] =-1 -> left plane
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 18;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      // horizontal planes
      dir[0] = 0; dir[1] = 1; //dir[1] =1 -> top plane
      imod = 9;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 11;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      dir[1] = -1;  //dir[1] =1 -> botton plane
      imod = 8;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
      imod = 10;
      hm = GetCombiTransfo(fCrystalsN + imod);
      outstr << SPrintParallelPla( imod, hm, plaName, fModAirFlukaSize, dir );
   }

   return outstr.str();
}

//-----------------------------------------------------------------------------
//! Print the planes parallel to a module
//!
//! \param[in] id crystal id
//! \param[in] hm transformation matrix
//! \param[in] bodyName body name
//! \param[in] trd2Size truncate shape vector
//! \param[in] dir direction vector
TString TACAparGeo::SPrintParallelPla( int id, TGeoCombiTrans* hm, TString bodyName,
                                       Double_t* trd2Size, int* dir )
{
   // dir[0] X direction could be (-1, 0, 1)
   // dir[1] Y direction could be (-1, 0, 1)

   if ( dir[0] != 0 && dir[1] != 0 ) {
      cout << "Error: TACAparGeo::SPrintParallelPla: Incompatible \"dir\" parameter\n" << endl;
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
      angle  = fpFootGeo->GetCAAngles(); //no need to invert the sing since here the angle is used to build the root object
   }

   TGeoTranslation detTrasl(center.X(), center.Y(), center.Z());
   TGeoRotation detRot;
   detRot.RotateX(angle.X());
   detRot.RotateY(angle.Y());
   detRot.RotateZ(angle.Z());
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
//! Print the body definition in FLUKA FREE format
//!
//! \param[in] id crystal id
//! \param[in] hm transformation matrix
//! \param[in] bodyName body name
//! \param[in] trd2Size truncate shape vector
TString TACAparGeo::SPrintCrystalBody( int id, TGeoCombiTrans* hm, TString bodyName, Double_t *trd2Size )
{
   // There is no truncate pyramid in FLUKA, so we need to define
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
      angle  = fpFootGeo->GetCAAngles(); //no need to invert the sing since here the angle is used to build the root object
   }

   TGeoTranslation detTrasl(center.X(), center.Y(), center.Z());
   TGeoRotation detRot;
   detRot.RotateX(angle.X());
   detRot.RotateY(angle.Y());
   detRot.RotateZ(angle.Z());
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
//! Print the Calorimeter regions for the FLUKA geometry file
string TACAparGeo::PrintRegions()
{
   stringstream outstr;

   if ( !TAGrecoManager::GetPar()->IncludeCA())
      return outstr.str();

   outstr << "* ***Calorimeter, setup: " << fConfigTypeGeo.Data() << endl;

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
//! Print the Calorimeter air regions around a module for the FLUKA geometry file
TString TACAparGeo::PrintModuleAirRegions()
{
   TString modRegion = "";

   if (fModulesN <= 0) return modRegion;

   TString line;
   for( int im=0; im<fModulesN; im++ ) {

      line.Form("ACAL_%02d       5 ", im);
      modRegion += line.Data();

      // create     pyramid      in front and back of each crystal
      Int_t iCry = im * fgkCrystalsNperModule;
      for (Int_t id=0; id<fgkCrystalsNperModule; id++) {
         line.Form(" | +AP%03d_1 -P%03d_1 +P%03d_3 +P%03d_4 +P%03d_5 +P%03d_6\n",
                  im, iCry+id, iCry+id, iCry+id, iCry+id, iCry+id);
         modRegion += line.Data();
         line.Form(" | +AP%03d_2 -P%03d_2 +P%03d_3 +P%03d_4 +P%03d_5 +P%03d_6\n",
                  im, iCry+id, iCry+id, iCry+id, iCry+id, iCry+id);
         modRegion += line.Data();
      }

      // create regions between crytals in a ROW
      Int_t nRows = 3;
      iCry = im * fgkCrystalsNperModule;

      if (fConfigTypeGeo.CompareTo("TWELVE_MOD_CNAO22") == 0 ||
          fConfigTypeGeo.CompareTo("FULL_DET_V1") == 0  ||
          fConfigTypeGeo.CompareTo("CNAO_2023") == 0 ) {
         for (Int_t ir=0; ir<nRows; ir++) {
            line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_3 -P%03d_4 +P%03d_5 +P%03d_6\n",
                     im, im, iCry+1, iCry+2, iCry+2, iCry+2);
            modRegion += line.Data();
            line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_4 -P%03d_3 +P%03d_5 +P%03d_6\n",
                     im, im, iCry+1, iCry, iCry, iCry);
            modRegion += line.Data();
            iCry += 3;
         }
      } else {
         for (Int_t ir=0; ir<nRows; ir++) {
            line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_3 -P%03d_4 +P%03d_5 +P%03d_6\n",
                     im, im, iCry, iCry+1, iCry+1, iCry+1);
            modRegion += line.Data();
            line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_4 -P%03d_3 +P%03d_5 +P%03d_6\n",
                     im, im, iCry, iCry+2, iCry+2, iCry+2);
            modRegion += line.Data();
            iCry += 3;
         }
      }

      // create regions between ROWs in a module

      iCry = im * fgkCrystalsNperModule;
      if (fConfigTypeGeo.CompareTo("TWELVE_MOD_CNAO22") == 0 ||
          fConfigTypeGeo.CompareTo("FULL_DET_V1") == 0 ||
          fConfigTypeGeo.CompareTo("CNAO_2023") == 0  ) {
         // central
         line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_6 -P%03d_5 +P%03d_4 +P%03d_3\n",
                  im, im, iCry+1, iCry+4, iCry+1, iCry+1);
         modRegion += line.Data();
         // left
         line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_6 -P%03d_5 +P%03d_4 -P%03d_4 -(P%03d_4 +P%03d_5)\n",
                  im, im, iCry, iCry+3, iCry, iCry+1, iCry+4, iCry+4);
         modRegion += line.Data();
         // right
         line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_6 -P%03d_5 +P%03d_3 -P%03d_3 -(P%03d_3 +P%03d_5)\n",
                  im, im, iCry+2, iCry+5, iCry+2, iCry+1, iCry+4, iCry+4);
         modRegion += line.Data();

         // central
         line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_5 -P%03d_6 +P%03d_4 +P%03d_3\n",
                  im, im, iCry+7, iCry+4, iCry+7, iCry+7);
         modRegion += line.Data();
         // left
         line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_5 -P%03d_6 +P%03d_4 -P%03d_4 -(P%03d_4 +P%03d_6)\n",
                  im, im, iCry+6, iCry+3, iCry+6, iCry+7, iCry+4, iCry+4);
         modRegion += line.Data();
         // right
         line.Form(" | +AP%03d_1 +AP%03d_2 -P%03d_5 -P%03d_6 +P%03d_3 -P%03d_3 -(P%03d_3 +P%03d_6)\n",
                  im, im, iCry+8, iCry+5, iCry+8, iCry+7, iCry+4, iCry+4);
         modRegion += line.Data();
      } else {
         // old setup of crystal in a module (5,3,4, 2,0,1, 8,6,7)
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
      }


      //////////////////////////////////////////////////////////
      // Fill everything outside crystals
      iCry = im * fgkCrystalsNperModule;
      if (fConfigTypeGeo.CompareTo("TWELVE_MOD_CNAO22") == 0 ||
          fConfigTypeGeo.CompareTo("FULL_DET_V1") == 0 ||
          fConfigTypeGeo.CompareTo("CNAO_2023") == 0) {
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_3 -P%03d_3 +P%03d_5 +P%03d_6\n",
                  im, im, im, iCry+5, iCry+5, iCry+5);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_4 -P%03d_4 +P%03d_5 +P%03d_6\n",
                  im, im, im, iCry+3, iCry+3, iCry+3);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_5 -P%03d_5 +P%03d_4 +P%03d_3\n",
                  im, im, im, iCry+1, iCry+1, iCry+1);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_6 -P%03d_6 +P%03d_4 +P%03d_3\n",
                  im, im, im, iCry+7, iCry+7, iCry+7);
         modRegion += line.Data();

         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_3 +AP%03d_5 -P%03d_3 -P%03d_5\n",
                  im, im, im, im,  iCry+2, iCry+5);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_3 +AP%03d_6 -P%03d_3 -P%03d_6\n",
                  im, im, im, im,  iCry+8, iCry+5);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_4 +AP%03d_5 -P%03d_4 -P%03d_5\n",
                  im, im, im, im,  iCry, iCry+3);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_4 +AP%03d_6 -P%03d_4 -P%03d_6\n",
                  im, im, im, im,  iCry+6, iCry+3);
         modRegion += line.Data();

         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_5 -P%03d_5 +P%03d_4 -P%03d_4\n",
                  im, im, im, iCry, iCry, iCry+1);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_5 -P%03d_5 +P%03d_3 -P%03d_3\n",
                  im, im, im, iCry+2, iCry+2, iCry+1);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_6 -P%03d_6 +P%03d_3 -P%03d_3\n",
                  im, im, im, iCry+8, iCry+8, iCry+7);
         modRegion += line.Data();
         line.Form(" | +AP%03d_1 +AP%03d_2 +AP%03d_6 -P%03d_6 +P%03d_4 -P%03d_4\n",
                  im, im, im, iCry+6, iCry+6, iCry+7);
         modRegion += line.Data();
      } else {
         // old setup of crystal in a module (5,3,4, 2,0,1, 8,6,7)

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

   }

   return modRegion;
}

//_____________________________________________________________________________
//! Get the FLUKA region index of the Calorimeter crystal
//!
//! \param[in] icry cellid index [0-35]
//! \return region index
Int_t TACAparGeo::GetRegCrystal(Int_t icry)
{
   TString regname;
   regname.Form("CAL%03d", icry);
   return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Print the Calorimeter regions that need to be subtracted from the air for the FLUKA geometry file
string TACAparGeo::PrintSubtractBodiesFromAir()
{

   stringstream outstr;

   if ( !TAGrecoManager::GetPar()->IncludeCA())
      return outstr.str();

   outstr << TString::Format(" - air_cal\n");
   outstr << TString::Format("AIRCAL0     5");

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
   if ( fConfigTypeGeo.CompareTo("CENTRAL_DET") == 0 ) {
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

   //FIVE MOD in a row: geometry for the HIT test beam
   if (fConfigTypeGeo.CompareTo("FIVE_MOD") == 0) {
      // first 3 modules
      int id = 3; //module id
      line.Form(" +air_cal -MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 0;
      line.Form(" +air_cal -MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 1;
      line.Form("+air_cal  -MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();

      // last 2 modules
      id = 2;
      line.Form("AIRCAL1    5 | +air_cal  +MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 4;
      line.Form("+air_cal  +MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
   }

   //SEVEN MOD in a row: geometry for the HIT test beam
   if (fConfigTypeGeo.CompareTo("SEVEN_MOD") == 0 ||
       fConfigTypeGeo.CompareTo("SEVEN_MOD_HIT22") == 0)  {
      // first 3 modules
      int id = 5;
      line.Form(" +air_cal -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 3;
      line.Form(" +air_cal -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 1;
      line.Form(" +air_cal -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();

      // central module
      id = 0;
      line.Form("AIRCAL1    5 | +air_cal +MP000 -MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();

      // last 3 modules
      id = 2;
      line.Form("AIRCAL2    5 | +air_cal  +MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 4;
      line.Form("+air_cal  +MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 6;
      line.Form("+air_cal  +MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
   }

   if (fConfigTypeGeo.CompareTo("TWELVE_MOD_CNAO22") == 0 )  {
      // first 4 modules
      int id = 7;
      line.Form(" +air_cal -MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();      
      id = 6;
      line.Form(" +air_cal -MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 0;
      line.Form(" +air_cal -MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 1;
      line.Form(" +air_cal -MP002 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();

      // central 4 modules
      id = 2;
      line.Form("AIRCAL1    5 | +air_cal +MP002 -MP004 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 8;
      line.Form("+air_cal +MP002 -MP004 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 3;
      line.Form("+air_cal +MP002 -MP004 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 9;
      line.Form("+air_cal +MP002 -MP004 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();

      // last 4 modules
      id = 4;
      line.Form("AIRCAL2    5 | +air_cal  +MP004 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 5;
      line.Form("+air_cal  +MP004 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 10;
      line.Form("+air_cal  +MP004 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 11;
      line.Form("+air_cal  +MP004 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();   
   }

   // FULL Detector (New version)
   if ( fConfigTypeGeo.CompareTo("FULL_DET_V1") == 0 ||
        fConfigTypeGeo.CompareTo("CNAO_2023") == 0) {
      // Here we do not use parenteses, otherwise FLUKA complain
      // *** Too many terms in parenthesis expansion ***

      // Central rows of 3 modules 
      // AIRCAL0
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP001 +MP000 +MP002 -AP000_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP001 +MP000 +MP002 -AP000_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 +MP001 +MP000 +MP002 -AP001_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP001 +MP000 +MP002 -AP002_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }

      outstr << "AIRCAL1     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal -MP001 +MP005 +MP000 +MP002 -AP004_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal -MP001 +MP005 +MP000 +MP002 -AP004_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 -MP001 +MP005 +MP000 +MP002 -AP005_%d\n", k);
               outstr << line.Data();
               line.Form("                 -MP001 +MP005 +MP000 +MP002 -AP006_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }

      outstr << "AIRCAL2     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal -MP005 -MP018 +MP017 -MP020 -AP010_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal -MP005 -MP018 +MP017 -MP020 -AP010_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 -MP005 -MP018 +MP017 -MP020 -AP011_%d\n", k);
               outstr << line.Data();
               line.Form("                 -MP005 -MP018 +MP017 -MP020 -AP012_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }

      outstr << "AIRCAL3     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP018 -MP025 +MP017 -MP020 -AP017_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP018 -MP025 +MP017 -MP020 -AP017_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 +MP018 -MP025 +MP017 -MP020 -AP018_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP018 -MP025 +MP017 -MP020 -AP019_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }

      outstr << "AIRCAL4     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP025 -MP031 +MP017 -MP020 -AP024_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP025 -MP031 +MP017 -MP020 -AP024_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 +MP025 -MP031 +MP017 -MP020 -AP025_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP025 -MP031 +MP017 -MP020 -AP026_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
 
      outstr << "AIRCAL5     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal -MP035 +MP031 +MP034 +MP036 -AP030_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal -MP035 +MP031 +MP034 +MP036 -AP030_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 -MP035 +MP031 +MP034 +MP036 -AP031_%d\n", k);
               outstr << line.Data();
               line.Form("                 -MP035 +MP031 +MP034 +MP036 -AP032_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
 
      outstr << "AIRCAL6     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP035 +MP034 +MP036 -AP034_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP035 +MP034 +MP036 -AP034_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 +MP035 +MP034 +MP036 -AP035_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP035 +MP034 +MP036 -AP036_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
 
      outstr << "AIRCAL7     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal -MP016 +MP008 -MP029 -AP008_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal -MP016 +MP008 -MP029 -AP008_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 -MP016 +MP008 -MP029 -AP015_%d\n", k);
               outstr << line.Data();
               line.Form("                 -MP016 +MP008 -MP029 -AP022_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal -MP016 -MP003 -MP029 -AP008_%d\n", i);
               outstr << line.Data();
               line.Form("                 -MP016 -MP003 -MP029 -AP015_%d\n", k);
               outstr << line.Data();
               line.Form("                 -MP016 -MP003 -MP029 -AP022_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal -MP016 +MP008 +MP022 -AP008_%d\n", i);
               outstr << line.Data();
               line.Form("                 -MP016 +MP008 +MP022 -AP015_%d\n", k);
               outstr << line.Data();
               line.Form("                 -MP016 +MP008 +MP022 -AP022_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal -MP016 -MP003 +MP022 -AP008_%d\n", i);
               outstr << line.Data();
               line.Form("                 -MP016 -MP003 +MP022 -AP015_%d\n", k);
               outstr << line.Data();
               line.Form("                 -MP016 -MP003 +MP022 -AP022_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }

      outstr << "AIRCAL8     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP016 -MP017 -MP003 -MP029 -MP005 -MP031 -AP009_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP016 -MP017 -MP003 -MP029 -MP005 -MP031 -AP009_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 +MP016 -MP017 -MP003 -MP029 -MP005 -MP031 -AP016_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP016 -MP017 -MP003 -MP029 -MP005 -MP031 -AP023_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal +MP016 -MP017 -MP003 -MP029 -MP000 -MP031 -AP009_%d\n", i);
               outstr << line.Data();
               line.Form("                 +MP016 -MP017 -MP003 -MP029 -MP000 -MP031 -AP016_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP016 -MP017 -MP003 -MP029 -MP000 -MP031 -AP023_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal +MP016 -MP017 -MP003 -MP029 -MP005 -MP034 -AP009_%d\n", i);
               outstr << line.Data();
               line.Form("                 +MP016 -MP017 -MP003 -MP029 -MP005 -MP034 -AP016_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP016 -MP017 -MP003 -MP029 -MP005 -MP034 -AP023_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal +MP016 -MP017 -MP003 -MP029 -MP000 -MP034 -AP009_%d\n", i);
               outstr << line.Data();
               line.Form("                 +MP016 -MP017 -MP003 -MP029 -MP000 -MP034 -AP016_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP016 -MP017 -MP003 -MP029 -MP000 -MP034 -AP023_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }

      outstr << "AIRCAL9     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP020 -MP021 -MP007 -MP033 -MP005 -MP031 -AP013_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP020 -MP021 -MP007 -MP033 -MP005 -MP031 -AP013_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 +MP020 -MP021 -MP007 -MP033 -MP005 -MP031 -AP020_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP020 -MP021 -MP007 -MP033 -MP005 -MP031 -AP027_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal +MP020 -MP021 -MP007 -MP033 -MP002 -MP031 -AP013_%d\n", i);
               outstr << line.Data();
               line.Form("                 +MP020 -MP021 -MP007 -MP033 -MP002 -MP031 -AP020_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP020 -MP021 -MP007 -MP033 -MP002 -MP031 -AP027_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal +MP020 -MP021 -MP007 -MP033 -MP005 -MP036 -AP013_%d\n", i);
               outstr << line.Data();
               line.Form("                 +MP020 -MP021 -MP007 -MP033 -MP005 -MP036 -AP020_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP020 -MP021 -MP007 -MP033 -MP005 -MP036 -AP027_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal +MP020 -MP021 -MP007 -MP033 -MP002 -MP036 -AP013_%d\n", i);
               outstr << line.Data();
               line.Form("                 +MP020 -MP021 -MP007 -MP033 -MP002 -MP036 -AP020_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP020 -MP021 -MP007 -MP033 -MP002 -MP036 -AP027_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }

      outstr << "AIRCAL10     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP021 +MP014 -MP033 -AP014_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP021 +MP014 -MP033 -AP014_%d\n", i);
                  outstr << line.Data(); 
               }
               line.Form("                 +MP021 +MP014 -MP033 -AP021_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP021 +MP014 -MP033 -AP028_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal +MP021 -MP007 -MP033 -AP014_%d\n", i);
               outstr << line.Data();
               line.Form("                 +MP021 -MP007 -MP033 -AP021_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP021 -MP007 -MP033 -AP028_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal +MP021 +MP014 +MP028 -AP014_%d\n", i);
               outstr << line.Data();
               line.Form("                 +MP021 +MP014 +MP028 -AP021_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP021 +MP014 +MP028 -AP028_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               line.Form("              |  +air_cal +MP021 -MP007 +MP028 -AP014_%d\n", i);
               outstr << line.Data();
               line.Form("                 +MP021 -MP007 +MP028 -AP021_%d\n", k);
               outstr << line.Data();
               line.Form("                 +MP021 -MP007 +MP028 -AP028_%d\n", l);
               outstr << line.Data();
            }
            outstr << "*" << endl;
         }
      }

      // 4 digonal modules
      outstr << "AIRCAL11     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP003 -MP000 -MP008 -AP003_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP003 -MP000 -MP008 -AP003_%d\n", i);
                  outstr << line.Data(); 
               }
            }
            outstr << "*" << endl;
         }
      }

      outstr << "AIRCAL12     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP007 -MP002 -MP014 -AP007_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP007 -MP002 -MP014 -AP007_%d\n", i);
                  outstr << line.Data(); 
               }
            }
            outstr << "*" << endl;
         }
      }
      

      outstr << "AIRCAL13     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP029 -MP034 -MP022 -AP029_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP029 -MP034 -MP022 -AP029_%d\n", i);
                  outstr << line.Data(); 
               }
            }
            outstr << "*" << endl;
         }
      }

      outstr << "AIRCAL14     5";
      for (int l=1; l<7; l++) {
         for (int k=1; k<7; k++) {
            for (int i=1; i<7; i++) {
               if (i==1 && k==1 && l==1) {
                  line.Form(" |  +air_cal +MP033 -MP036 -MP028 -AP033_%d\n", i);
                  outstr << line.Data(); 
               } else {
                  line.Form("              |  +air_cal +MP033 -MP036 -MP028 -AP033_%d\n", i);
                  outstr << line.Data(); 
               }
            }
            outstr << "*" << endl;
         }
      }

   }

   // FULL Detector (old version, never builded)
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
      line.Form("AIRCAL1    5 | +air_cal -MP011 -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 31;
      line.Form(" +air_cal -MP011 -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();

      // 4 botton modules
      id = 28;
      line.Form("AIRCAL2    5 | +air_cal -MP008 +MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 20;
      line.Form(" +air_cal -MP008 +MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 22;
      line.Form("AIRCAL3    5 | +air_cal -MP010 -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();
      id = 30;
      line.Form(" +air_cal -MP010 -MP000 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
                id, id, id, id, id, id);
      outstr << line.Data();

      // 6 blocks of 4 modules from left to right
      id = 25;
      line.Form("AIRCAL4    5 | +air_cal +MP016 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
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
      line.Form("AIRCAL5    5 | +air_cal -MP016 +MP004 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
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
      line.Form("AIRCAL6    5 | +air_cal -MP004 +MP000 +MP008 +MP009 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
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
      line.Form("AIRCAL7    5 | +air_cal -MP000 -MP006 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
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
      line.Form("AIRCAL8    5 | +air_cal +MP006 -MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
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
      line.Form("AIRCAL9    5 | +air_cal +MP018 +MP010 +MP011 -(AP%03d_1 + AP%03d_2 +AP%03d_3 +AP%03d_4 +AP%03d_5 +AP%03d_6)\n",
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
//! Print the Calorimeter parameters
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
//! Print the Calorimeter material for the FLUKA geometry file
//!
//! \param[in] Material input material
string TACAparGeo::PrintAssignMaterial(TAGmaterials* Material)
{
   stringstream outstr;

   if ( !TAGrecoManager::GetPar()->IncludeCA())
      return outstr.str();

   TString flkmat;

   if (Material == NULL) {
      TAGmaterials::Instance()->PrintMaterialFluka();
      flkmat = TAGmaterials::Instance()->GetFlukaMatName(fCrystalMat.Data());
   } else
      flkmat = Material->GetFlukaMatName(fCrystalMat.Data());

   bool magnetic = false;
   if (TAGrecoManager::GetPar()->IncludeDI())
      magnetic = true;


   if ( fConfigTypeGeo.CompareTo("FULL_DET_V1") == 0 ||
        fConfigTypeGeo.CompareTo("CNAO_2023") == 0) {
      outstr << PrintCard("ASSIGNMA", "AIR", "AIRCAL0", "AIRCAL14",
                          "1.", "", "", "") << endl;
   } else if (fConfigTypeGeo.CompareTo("FULL_DET") == 0) {
      outstr << PrintCard("ASSIGNMA", "AIR", "AIRCAL0", "AIRCAL9",
                          "1.", "", "", "") << endl;
   } else if (fConfigTypeGeo.CompareTo("FIVE_MOD") == 0) {
      outstr << PrintCard("ASSIGNMA", "AIR", "AIRCAL0", "AIRCAL1",
                          "1.", "", "", "") << endl;
   } else if (fConfigTypeGeo.CompareTo("SEVEN_MOD") == 0 ||
              fConfigTypeGeo.CompareTo("SEVEN_MOD_HIT22") == 0 ||
              fConfigTypeGeo.CompareTo("TWELVE_MOD_CNAO22") == 0 ) {
      outstr << PrintCard("ASSIGNMA", "AIR", "AIRCAL0", "AIRCAL2",
                          "1.", "", "", "") << endl;
   } else
      outstr << PrintCard("ASSIGNMA", "AIR", "AIRCAL0",
                          "", "", "", "", "") << endl;

   if ( fDummyModulesN > 0 ) { // this option is present only for campaing after
      int iimod = -1;          // CNAO_2023
      for (Int_t iCry = 0; iCry < fCrystalsN; ++iCry) {
         if ( (iCry)%9 == 0 )  iimod++;
         // fill crystal with AIR in dummy modules
         if ( std::find(std::begin(fListDummyModules), std::end(fListDummyModules), iimod) != std::end(fListDummyModules))
            outstr << PrintCard("ASSIGNMA", "AIR", Form("CAL%03d", iCry), "",
                          "", "", "", "") << endl;
         else
            outstr << PrintCard("ASSIGNMA", flkmat, Form("CAL%03d", iCry), "",
                          "", "", "", "") << endl;
      }
   } else {
      outstr << PrintCard("ASSIGNMA", flkmat, "CAL000", Form("CAL%03d", fCrystalsN-1),
                          "1.", "", "", "") << endl;
   }

   if (fModulesN == 1) {
      outstr << PrintCard("ASSIGNMA", "AIR", "ACAL_00",
                          "", "", "", "", "") << endl;
   }
   if (fModulesN > 1) {
      outstr << PrintCard("ASSIGNMA", "AIR", "ACAL_00", Form("ACAL_%02d", fModulesN-1),
                          "1.", "", "", "") << endl;
   }

   return outstr.str();
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TACAparGeo::Clear(Option_t*)
{
   return;
}

//______________________________________________________________________________
//! ostream insertion.
//!
//! \param[in] os output stream
//! \param[in] option option for printout
void TACAparGeo::ToStream(ostream& os, Option_t*) const
{
   os << "TACAparGeo " << GetName() << endl
      << " Setup: " << fConfigTypeGeo.Data()  << endl
      << " Number of crystals: " <<  fCrystalsN << endl;
   //  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
   //     << "   ver_x   ver_y   ver_z  width" << endl;

   return;
}
