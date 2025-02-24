/*!
 \file TAGgeoTrafo.cxx
 \brief   Implementation of TAGgeoTrafo.
 */

#include <fstream>
#include "TObjArray.h"

#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TAGgeoTrafo
 \brief/Class that handles the Geometrical transformations in FOOT. **
 */

//! Class Imp
ClassImp(TAGgeoTrafo);

      TString TAGgeoTrafo::fgDefaultActName = "actGeoTrafo";
const Char_t* TAGgeoTrafo::fgkGeomName      = "FOOT";
const Char_t* TAGgeoTrafo::fgkGeomTitle     = "FOOT Geometry";
const Char_t* TAGgeoTrafo::fgkTrafoBaseName = "trafo";
      Char_t  TAGgeoTrafo::fgDefaultTransp  = 60;
const Float_t TAGgeoTrafo::fgkCmToMu        = 10000;
const Float_t TAGgeoTrafo::fgkMuToCm        = 1e-4;
const Float_t TAGgeoTrafo::fgkMmToMu        = 1000;
const Float_t TAGgeoTrafo::fgkMuToMm        = 1e-3;
const Float_t TAGgeoTrafo::fgkCmToMm        = 10;
const Float_t TAGgeoTrafo::fgkMmToCm        = 1e-1;
const Float_t TAGgeoTrafo::fgkMToCm         = 100;
const Float_t TAGgeoTrafo::fgkCmToM         = 1e-2;
const Float_t TAGgeoTrafo::fgkNsToSec       = 1e-9;
const Float_t TAGgeoTrafo::fgkPsToSec       = 1e-12;
const Float_t TAGgeoTrafo::fgkPsToNs        = 1e-3;
const Float_t TAGgeoTrafo::fgkNsToPs        = 1e+3;
const Float_t TAGgeoTrafo::fgkSecToNs       = 1e+9;
const Float_t TAGgeoTrafo::fgkSecToPs       = 1e+12;
const Float_t TAGgeoTrafo::fgkGevToMev      = 1e3;
const Float_t TAGgeoTrafo::fgkGevToKev      = 1e6;
const Float_t TAGgeoTrafo::fgkMevToGev      = 1e-3;
const Float_t TAGgeoTrafo::fgkGausToTesla   = 1e-4;
const Float_t TAGgeoTrafo::fgkAmuToGev      = 0.9315;
const Float_t TAGgeoTrafo::fgkGausToKGaus   = 1e-3;
const Float_t TAGgeoTrafo::fgkMassFactor    = 0.9383; // GeV
const Float_t TAGgeoTrafo::fgkElectronMass  = 0.5109; // MeV
const Float_t TAGgeoTrafo::fgkLightVelocity = 29.98; // cm/ns

map<TString, Int_t> TAGgeoTrafo::fgkDeviceType = {{"ST", 0}, {"BM", 10},  {"DI", 20}, {"TG", 30},  {"VT", 40},
                                                  {"IT", 50}, {"MSD", 60}, {"TW", 70}, {"CA", 80}};

//------------------------------------------+-----------------------------------
//! Return name of device from type number
//!
//! \param[in] devType device type number
const Char_t* TAGgeoTrafo::GetDeviceName(Int_t devType)
{
   for (const auto &item : fgkDeviceType) {
     if(item.second == devType)
        return item.first.Data();
   }

   return TString().Data();
}

//_____________________________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
TAGgeoTrafo::TAGgeoTrafo(const TString expName)
: TAGaction(fgDefaultActName.Data(), "TAGgeoTrafo - Geometry Transformations"),
  fFileStream(new TAGparTools()),
  fMatrixList(new TObjArray()),
  fDeviceList(new TObjArray()),
  fExpName(expName)
{
   fMatrixList->SetOwner(true);
   fDeviceList->SetOwner(true);
}

//_____________________________________________________________________________
//! Destructor
TAGgeoTrafo::~TAGgeoTrafo()
{
   delete fMatrixList;
   delete fDeviceList;
   delete fFileStream;
}

/*
  Transformations of points in space
*/
//_____________________________________________________________________________
//! Return H-matrix from device name
//!
//! \param[in] nameSuf device name
const TGeoHMatrix* TAGgeoTrafo::GetTrafo(const char* nameSuf) const
{
   TString name = Form("%s%s", fgkTrafoBaseName, nameSuf);
   const TGeoHMatrix* matrix = (const TGeoHMatrix*)fMatrixList->FindObject(name);
   if (matrix)
	  return matrix;
   else {
	  Error("GetTrafo","No matrix with name %s found, reset to ID", name.Data());
	  return new TGeoHMatrix();
   }
}

//_____________________________________________________________________________
//! Return combi-matrix from device name
//!
//! \param[in] nameSuf device name
TGeoCombiTrans* TAGgeoTrafo::GetCombiTrafo(const char* nameSuf) const
{
   TString name = Form("%s%s", fgkTrafoBaseName, nameSuf);
   const TGeoHMatrix* matrix = (const TGeoHMatrix*)fMatrixList->FindObject(name);

   if (matrix) {
      const Double_t* mat = matrix->GetRotationMatrix();
      const Double_t* dis = matrix->GetTranslation();

      TGeoRotation rot;
      rot.SetMatrix(mat);

      TGeoTranslation trans;
      trans.SetTranslation(dis[0], dis[1], dis[2]);

      return  new TGeoCombiTrans(trans, rot);

   } else {
      Error("GetTrafo","No matrix with name %s found, reset to ID", name.Data());
      return new TGeoCombiTrans();
   }
}

//_____________________________________________________________________________
//! Get device center position from device name
//!
//! \param[in] nameSuf device name
TVector3 TAGgeoTrafo::GetDeviceCenter(const char* nameSuf) const
{
   TString name = Form("%s%s", fgkTrafoBaseName, nameSuf);
   const DeviceParameter_t* device = (const DeviceParameter_t*)fDeviceList->FindObject(name);
   if (device)
      return device->Center;
   else {
      Error("GetTrafo","No Device Parameter with name %s found, reset to ID", name.Data());
      return TVector3(0,0,0);
   }
}

//_____________________________________________________________________________
//! Get device center angle from device name
//!
//! \param[in] nameSuf device name
TVector3 TAGgeoTrafo::GetDeviceAngle(const char* nameSuf) const
{
   TString name = Form("%s%s", fgkTrafoBaseName, nameSuf);
   const DeviceParameter_t* device = (const DeviceParameter_t*)fDeviceList->FindObject(name);
   if (device)
      return device->Angle;
   else {
      Error("GetTrafo","No Device Parameter with name %s found, reset to ID", name.Data());
      return TVector3(0,0,0);
   }
}

//_____________________________________________________________________________
//! Add H-matrix
//!
//! \param[in] mat a given matrix
void TAGgeoTrafo::AddTrafo(TGeoHMatrix* mat)
{
	  fMatrixList->Add(mat);
}

//_____________________________________________________________________________
//! Add device parameters
//!
//! \param[in] device device parameters
void TAGgeoTrafo::AddDevice(DeviceParameter_t* device)
{
	  fDeviceList->Add(device);
}

//_____________________________________________________________________________
//! Transformation from global FOOT to local detector framework
//!
//! \param[in] name name of device
//! \param[in] glob position in detector framework
//! \return position in detector framework
TVector3 TAGgeoTrafo::Global2Local(const char* name, TVector3& glob) const
{
   const TGeoHMatrix* mat = GetTrafo(name);

   Double_t local[3]  = {0., 0., 0.};
   Double_t global[3] = {glob.X(), glob.Y(), glob.Z()};

   mat->MasterToLocal(global, local);
   TVector3 pos(local[0], local[1], local[2]);

   return pos;
}

//_____________________________________________________________________________
//! Transformation from global FOOT to local detector framework for vector (no translation)
//!
//! \param[in] name name of device
//! \param[in] glob position in FOOT framework
//! \return position in detector framework
TVector3 TAGgeoTrafo::Global2LocalVect(const char* name, TVector3& glob) const
{
   const TGeoHMatrix* mat = GetTrafo(name);

   Double_t local[3]  = {0., 0., 0.};
   Double_t global[3] = {glob.X(), glob.Y(), glob.Z()};

   mat->MasterToLocalVect(global, local);
   TVector3 pos(local[0], local[1], local[2]);

   return pos;
}

//_____________________________________________________________________________
//! Transformation from local detector to global FOOT framework
//!
//! \param[in] name name of device
//! \param[in] loc position in detector framework
//! \return position in FOOT framework
TVector3 TAGgeoTrafo::Local2Global(const char* name, TVector3& loc) const
{
   const TGeoHMatrix* mat = GetTrafo(name);

   Double_t local[3]  = {loc.X(), loc.Y(), loc.Z()};
   Double_t global[3] = {0., 0., 0.};

   mat->LocalToMaster(local, global);
   TVector3 pos(global[0], global[1], global[2]);

   return pos;
}

//_____________________________________________________________________________
//! Transformation from local detector to global FOOT framework for vector (no translation)
//!
//! \param[in] name name of device
//! \param[in] loc position in detector framework
//! \return position in FOOT framework
TVector3 TAGgeoTrafo::Local2GlobalVect(const char* name, TVector3& loc) const
{
   const TGeoHMatrix* mat = GetTrafo(name);

   Double_t local[3]  = {loc.X(), loc.Y(), loc.Z()};
   Double_t global[3] = {0., 0., 0.};

   mat->LocalToMasterVect(local, global);
   TVector3 pos(global[0], global[1], global[2]);

   return pos;
}


//_____________________________________________________________________________
//! Read FOOT geometry file
//!
//! \param[in] ifile file name
bool TAGgeoTrafo::FromFile(TString ifile)
{
  //Initialization of Geom Parameters
   if(FootDebugLevel(1))
      Info("InitGeo()"," Init Geo ");

   if (!fFileStream->Open(ifile)) {
      Error("FromFile()", "failed to open file '%s'", ifile.Data());
      return kTRUE;
   }

   Info("FromFile()", "Open file %s for geometry", ifile.Data());

   while(!fFileStream->Eof()) {
      TString name;
      TString baseName = "";
      TVector3 center;
      TVector3 angle;

      fFileStream->ReadStrings(baseName);
      fFileStream->ReadVector3(center);
      fFileStream->ReadVector3(angle);

      name = Form("%s%s", fgkTrafoBaseName, baseName.Data());
      if(FootDebugLevel(1))
         printf("%s\n", name.Data());
      DeviceParameter_t* device = new DeviceParameter_t;
      device->Center = center;
      device->Angle  = angle;
      device->SetName(name.Data());
      AddDevice(device);

      TGeoRotation rot;
      rot.RotateX(angle[0]); rot.RotateY(angle[1]); rot.RotateZ(angle[2]);
      TGeoTranslation trans(center[0], center[1], center[2]);
      TGeoHMatrix  transfo;
      transfo  = trans;
      transfo *= rot;
      TGeoHMatrix* trafo = new TGeoHMatrix(transfo);
      trafo->SetName(name.Data());
      AddTrafo(trafo);

   }

   fFileStream->Close();

   return kTRUE;
}

//_____________________________________________________________________________
//! Transformation from local target to BM local framework
//!
//! \param[in] apoi position in target framework
TVector3 TAGgeoTrafo::FromTGLocalToBMLocal(TVector3 apoi)
{
  TVector3 posTg = FromTGLocalToGlobal(apoi);
  return FromGlobalToBMLocal(posTg);
}

//_____________________________________________________________________________
//! Transformation from local target to VTX local framework
//!
//! \param[in] apoi position in target framework
TVector3 TAGgeoTrafo::FromTGLocalToVTLocal(TVector3 apoi)
{
  TVector3 posTg = FromTGLocalToGlobal(apoi);
  return FromGlobalToVTLocal(posTg);
}

//_____________________________________________________________________________
//! Transformation from local target to ITR local framework
//!
//! \param[in] apoi position in target framework
TVector3 TAGgeoTrafo::FromTGLocalToITLocal(TVector3 apoi)
{
  TVector3 posTg = FromTGLocalToGlobal(apoi);
  return FromGlobalToITLocal(posTg);
}

//_____________________________________________________________________________
//! Transformation from local ITR to VTX local framework
//!
//! \param[in] apoi position in ITR framework
TVector3 TAGgeoTrafo::FromITLocalToVTLocal(TVector3 apoi)
{
  TVector3 posIt = FromITLocalToGlobal(apoi);
  return FromGlobalToVTLocal(posIt);
}

//_____________________________________________________________________________
//! Transformation from local TW to VTX local framework
//!
//! \param[in] apoi position in TW framework
TVector3 TAGgeoTrafo::FromTWLocalToVTLocal(TVector3 apoi)
{
  TVector3 posTw = FromTWLocalToGlobal(apoi);
  return FromGlobalToVTLocal(posTw);
}

//_____________________________________________________________________________
//! Transformation from local STC to FOOT global framework
//!
//! \param[in] apoi position in STC framework
TVector3 TAGgeoTrafo::FromSTLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("ST", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
//! Transformation from local BM to FOOT global framework
//!
//! \param[in] apoi position in BM framework
TVector3 TAGgeoTrafo::FromBMLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("BM", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
//! Transformation from local target to FOOT global framework
//!
//! \param[in] apoi position in target framework
TVector3 TAGgeoTrafo::FromTGLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("TG", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
//! Transformation from local dipole to FOOT global framework
//!
//! \param[in] apoi position in dipole framework
TVector3 TAGgeoTrafo::FromDILocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("DI", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
//! Transformation from local VTX to FOOT global framework
//!
//! \param[in] apoi position in VTX framework
TVector3 TAGgeoTrafo::FromVTLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("VT", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
//! Transformation from local ITR to FOOT global framework
//!
//! \param[in] apoi position in ITR framework
TVector3 TAGgeoTrafo::FromITLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("IT", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
//! Transformation from local MSD to FOOT global framework
//!
//! \param[in] apoi position in MSD framework
TVector3 TAGgeoTrafo::FromMSDLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("MSD", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
//! Transformation from local TW to FOOT global framework
//!
//! \param[in] apoi position in TW framework
TVector3 TAGgeoTrafo::FromTWLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("TW", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
//! Transformation from local CAL to FOOT global framework
//!
//! \param[in] apoi position in CAL framework
TVector3 TAGgeoTrafo::FromCALocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("CA", apoi);
   return glb_poi;
}
/*
 Transformations of vectors in space:: local --> global
 */
//_____________________________________________________________________________
//! Transformation from local STC to FOOT global framework for vector (no translation)
//!
//! \param[in] avec position in STC framework
TVector3 TAGgeoTrafo::VecFromSTLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("ST", avec);
   return glb_vec;
}

//_____________________________________________________________________________
//! Transformation from local BM to FOOT global framework for vector (no translation)
//!
//! \param[in] avec position in BM framework
TVector3 TAGgeoTrafo::VecFromBMLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("BM", avec);
   return glb_vec;
}

//_____________________________________________________________________________
//! Transformation from local target to FOOT global framework for vector (no translation)
//!
//! \param[in] avec position in target framework
TVector3 TAGgeoTrafo::VecFromTGLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("TG", avec);
   return glb_vec;
}

//_____________________________________________________________________________
//! Transformation from local dipole to FOOT global framework for vector (no translation)
//!
//! \param[in] avec position in dipole framework
TVector3 TAGgeoTrafo::VecFromDILocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("DI", avec);
   return glb_vec;
}

//_____________________________________________________________________________
//! Transformation from local VTX to FOOT global framework for vector (no translation)
//!
//! \param[in] avec position in VTX framework
TVector3 TAGgeoTrafo::VecFromVTLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("VT", avec);
   return glb_vec;
}

//_____________________________________________________________________________
//! Transformation from local ITR to FOOT global framework for vector (no translation)
//!
//! \param[in] avec position in ITR framework
TVector3 TAGgeoTrafo::VecFromITLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("IT", avec);
   return glb_vec;
}

//_____________________________________________________________________________
//! Transformation from local MSD to FOOT global framework for vector (no translation)
//!
//! \param[in] avec position in MSD framework
TVector3 TAGgeoTrafo::VecFromMSDLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("MSD", avec);
   return glb_vec;
}

//_____________________________________________________________________________
//! Transformation from local TW to FOOT global framework for vector (no translation)
//!
//! \param[in] avec position in TW framework
TVector3 TAGgeoTrafo::VecFromTWLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("TW", avec);
   return glb_vec;
}

//_____________________________________________________________________________
//! Transformation from local CAL to FOOT global framework for vector (no translation)
//!
//! \param[in] avec position in CAL framework
TVector3 TAGgeoTrafo::VecFromCALocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("CA", avec);
   return glb_vec;
}

/*
 Transformations of point in space:: global --> Local
 */
//_____________________________________________________________________________
//! Transformation from FOOT global to local STC framework
//!
//! \param[in] apoi position in FOOT global framework
TVector3 TAGgeoTrafo::FromGlobalToSTLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("ST", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local BM framework
//!
//! \param[in] apoi position in FOOT global framework
TVector3 TAGgeoTrafo::FromGlobalToBMLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("BM", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local target framework
//!
//! \param[in] apoi position in FOOT global framework
TVector3 TAGgeoTrafo::FromGlobalToTGLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("TG", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local dipole framework
//!
//! \param[in] apoi position in FOOT global framework
TVector3 TAGgeoTrafo::FromGlobalToDILocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("DI", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local VTX framework
//!
//! \param[in] apoi position in FOOT global framework
TVector3 TAGgeoTrafo::FromGlobalToVTLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("VT", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local ITR framework
//!
//! \param[in] apoi position in FOOT global framework
TVector3 TAGgeoTrafo::FromGlobalToITLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("IT", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local MSD framework
//!
//! \param[in] apoi position in FOOT global framework
TVector3 TAGgeoTrafo::FromGlobalToMSDLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("MSD", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local TW framework
//!
//! \param[in] apoi position in FOOT global framework
TVector3 TAGgeoTrafo::FromGlobalToTWLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("TW", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local CAL framework
//!
//! \param[in] apoi position in FOOT global framework
TVector3 TAGgeoTrafo::FromGlobalToCALocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("CA", apoi);
   return lcl_poi;
}

/*
 Transformations of vectors in space:: global --> Local
 */
//_____________________________________________________________________________
//! Transformation from FOOT global to local STC framework for vector (no translation)
//!
//! \param[in] avec position in FOOT global framework
TVector3 TAGgeoTrafo::VecFromGlobalToSTLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("ST", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local BM framework for vector (no translation)
//!
//! \param[in] avec position in FOOT global framework
TVector3 TAGgeoTrafo::VecFromGlobalToBMLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("BM", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local target framework for vector (no translation)
//!
//! \param[in] avec position in FOOT global framework
TVector3 TAGgeoTrafo::VecFromGlobalToTGLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("TG", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local dipole framework for vector (no translation)
//!
//! \param[in] avec position in FOOT global framework
TVector3 TAGgeoTrafo::VecFromGlobalToDILocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("DI", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local VTX framework for vector (no translation)
//!
//! \param[in] avec position in FOOT global framework
TVector3 TAGgeoTrafo::VecFromGlobalToVTLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("VT", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local ITR framework for vector (no translation)
//!
//! \param[in] avec position in FOOT global framework
TVector3 TAGgeoTrafo::VecFromGlobalToITLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("IT", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local MSD framework for vector (no translation)
//!
//! \param[in] avec position in FOOT global framework
TVector3 TAGgeoTrafo::VecFromGlobalToMSDLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("MSD", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local TW framework for vector (no translation)
//!
//! \param[in] avec position in FOOT global framework
TVector3 TAGgeoTrafo::VecFromGlobalToTWLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("TW", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
//! Transformation from FOOT global to local CAL framework for vector (no translation)
//!
//! \param[in] avec position in FOOT global framework
TVector3 TAGgeoTrafo::VecFromGlobalToCALocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("CA", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
//! Get STC center position
TVector3 TAGgeoTrafo::GetSTCenter()
{
   return GetDeviceCenter("ST");
}

//_____________________________________________________________________________
//! Get STC angles
TVector3 TAGgeoTrafo::GetSTAngles()
{
   return GetDeviceAngle("ST");
}

//_____________________________________________________________________________
//! Get BM center position
TVector3 TAGgeoTrafo::GetBMCenter()
{
   return GetDeviceCenter("BM");
}

//_____________________________________________________________________________
//! Get BM angles
TVector3 TAGgeoTrafo::GetBMAngles()
{
   return GetDeviceAngle("BM");
}

//_____________________________________________________________________________
//! Get target center position
TVector3 TAGgeoTrafo::GetTGCenter()
{
   return GetDeviceCenter("TG");
}
//_____________________________________________________________________________
//! Get target angles
TVector3 TAGgeoTrafo::GetTGAngles()
{
   return GetDeviceAngle("TG");
}

//_____________________________________________________________________________
//! Get dipole center position
TVector3 TAGgeoTrafo::GetDICenter()
{
   return GetDeviceCenter("DI");
}

//_____________________________________________________________________________
//! Get dipole angles
TVector3 TAGgeoTrafo::GetDIAngles()
{
   return GetDeviceAngle("DI");
}

//_____________________________________________________________________________
//! Get VTX center position
TVector3 TAGgeoTrafo::GetVTCenter()
{
   return GetDeviceCenter("VT");
}

//_____________________________________________________________________________
//! Get VTX angles
TVector3 TAGgeoTrafo::GetVTAngles()
{
   return GetDeviceAngle("VT");
}

//_____________________________________________________________________________
//! Get ITR center position
TVector3 TAGgeoTrafo::GetITCenter()
{
   return GetDeviceCenter("IT");
}

//_____________________________________________________________________________
//! Get ITR angles
TVector3 TAGgeoTrafo::GetITAngles()
{
   return GetDeviceAngle("IT");
}

//_____________________________________________________________________________
//! Get MSD center position
TVector3 TAGgeoTrafo::GetMSDCenter()
{
   return GetDeviceCenter("MSD");
}

//_____________________________________________________________________________
//! Get MSD angles
TVector3 TAGgeoTrafo::GetMSDAngles()
{
   return GetDeviceAngle("MSD");
}

//_____________________________________________________________________________
//! Get TW center position
TVector3 TAGgeoTrafo::GetTWCenter()
{
   return GetDeviceCenter("TW");
}

//_____________________________________________________________________________
//! Get TW angles
TVector3 TAGgeoTrafo::GetTWAngles()
{
   return GetDeviceAngle("TW");
}

//_____________________________________________________________________________
//! Get CAL center position
TVector3 TAGgeoTrafo::GetCACenter()
{
   return GetDeviceCenter("CA");
}

//_____________________________________________________________________________
//! Get CAL angles
TVector3 TAGgeoTrafo::GetCAAngles()
{
   return GetDeviceAngle("CA");
}

//_____________________________________________________________________________
//! Get Projection
TVector3 TAGgeoTrafo::Intersection(const TVector3 slope, const TVector3 origin, const Double_t finalZ){
  return origin+slope*((finalZ-origin[2])/slope[2]);
}
