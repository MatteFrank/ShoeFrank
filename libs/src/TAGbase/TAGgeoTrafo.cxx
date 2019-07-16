#include "TAGgeoTrafo.hxx"
#include <fstream>

#include "TObjArray.h"

#include "GlobalPar.hxx"

//Class that handles the Geometrical transformations in QAPIVI RS

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
const Float_t TAGgeoTrafo::fgkSecToNs       = 1e+9;
const Float_t TAGgeoTrafo::fgkSecToPs       = 1e+12;
const Float_t TAGgeoTrafo::fgkGevToMev      = 1e3;
const Float_t TAGgeoTrafo::fgkGevToKev      = 1e6;
const Float_t TAGgeoTrafo::fgkMevToGev      = 1e-3;
const Float_t TAGgeoTrafo::fgkMassFactor    = 0.9383; // GeV



//_____________________________________________________________________________
TAGgeoTrafo::TAGgeoTrafo(const TString expName)
: TAGaction(fgDefaultActName.Data(), "TAGgeoTrafo - Geometry Transformations"),
  fFileStream(new TAGparTools()),
  fMatrixList(new TObjArray()),
  fDeviceList(new TObjArray()),
  fExpName(expName)
{
   fMatrixList->SetOwner(true);
   fDeviceList->SetOwner(true);
   TGeoIdentity* id = new TGeoIdentity();
   fIdentity        = new TGeoHMatrix(*id);
}

//_____________________________________________________________________________
TAGgeoTrafo::~TAGgeoTrafo() 
{
   delete fMatrixList;
   delete fDeviceList;
   delete fIdentity;
   delete fFileStream;
}

/*
  Transformations of points in space
*/
//_____________________________________________________________________________
const TGeoHMatrix* TAGgeoTrafo::GetTrafo(const char* nameSuf) const
{
   TString name = Form("%s%s", fgkTrafoBaseName, nameSuf);
   const TGeoHMatrix* matrix = (const TGeoHMatrix*)fMatrixList->FindObject(name);
   if (matrix)
	  return matrix;
   else {
	  Error("GetTrafo","No matrix with name %s found, reset to ID", name.Data());
	  return fIdentity;
   }
}

//_____________________________________________________________________________
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
void TAGgeoTrafo::AddTrafo(TGeoHMatrix* mat)
{
	  fMatrixList->Add(mat);  
}

//_____________________________________________________________________________
void TAGgeoTrafo::AddDevice(DeviceParameter_t* device)
{
	  fDeviceList->Add(device);
}

//_____________________________________________________________________________
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
bool TAGgeoTrafo::FromFile(TString ifile)
{
  //Initialization of Geom Parameters
   if(FootDebugLevel(1))
      Info("InitGeo()"," Init Geo ");

   if (!fFileStream->Open(ifile)) {
      Error("FromFile()", "failed to open file '%s'", ifile.Data());
      return kTRUE;
   }
  
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
      
      Double_t loc[] = {center[0], center[1], center[2]};
      Double_t glo[] = {0.,0.,0};
      rot.LocalToMaster(loc, glo);
      TGeoTranslation trans(glo[0], glo[1], glo[2]);
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
TVector3 TAGgeoTrafo::FromSTLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("ST", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromBMLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("BM", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromTGLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("TG", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromDILocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("DI", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromVTLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("VT", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromITLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("IT", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromMSDLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("MSD", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromTWLocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("TW", apoi);
   return glb_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromCALocalToGlobal(TVector3 apoi)
{
   TVector3 glb_poi = Local2Global("CA", apoi);
   return glb_poi;
}
/*
 Transformations of vectors in space:: local --> global
 */
//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromSTLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("ST", avec);
   return glb_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromBMLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("BM", avec);
   return glb_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromTGLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("TG", avec);
   return glb_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromDILocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("DI", avec);
   return glb_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromVTLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("VT", avec);
   return glb_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromITLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("IT", avec);
   return glb_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromMSDLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("MSD", avec);
   return glb_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromTWLocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("TW", avec);
   return glb_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromCALocalToGlobal(TVector3 avec)
{
   TVector3 glb_vec = Local2GlobalVect("CA", avec);
   return glb_vec;
}

/*
 Transformations of point in space:: global --> Local
 */
//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromGlobalToSTLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("ST", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromGlobalToBMLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("BM", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromGlobalToTGLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("TG", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromGlobalToDILocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("DI", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromGlobalToVTLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("VT", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromGlobalToITLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("IT", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromGlobalToMSDLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("MSD", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromGlobalToTWLocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("TW", apoi);
   return lcl_poi;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::FromGlobalToCALocal(TVector3 apoi)
{
   TVector3 lcl_poi = Global2Local("CA", apoi);
   return lcl_poi;
}

/*
 Transformations of vectors in space:: global --> Local
 */
//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromGlobalToSTLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("ST", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromGlobalToBMLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("BM", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromGlobalToTGLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("TG", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromGlobalToDILocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("DI", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromGlobalToVTLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("VT", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromGlobalToITLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("IT", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromGlobalToMSDLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("MSD", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromGlobalToTWLocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("TW", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::VecFromGlobalToCALocal(TVector3 avec)
{
   TVector3 lcl_vec = Global2LocalVect("CA", avec);
   return lcl_vec;
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetSTCenter()
{
   return GetDeviceCenter("ST");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetSTAngles()
{
   return GetDeviceAngle("ST");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetBMCenter()
{
   return GetDeviceCenter("BM");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetBMAngles()
{
   return GetDeviceAngle("BM");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetTGCenter()
{
   return GetDeviceCenter("TG");
}
//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetTGAngles()
{
   return GetDeviceAngle("TG");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetDICenter()
{
   return GetDeviceCenter("DI");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetDIAngles()
{
   return GetDeviceAngle("DI");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetVTCenter()
{
   return GetDeviceCenter("VT");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetVTAngles()
{
   return GetDeviceAngle("VT");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetITCenter()
{
   return GetDeviceCenter("IT");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetITAngles()
{
   return GetDeviceAngle("IT");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetMSDCenter()
{
   return GetDeviceCenter("MSD");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetMSDAngles()
{
   return GetDeviceAngle("MSD");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetTWCenter()
{
   return GetDeviceCenter("TW");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetTWAngles()
{
   return GetDeviceAngle("TW");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetCACenter()
{
   return GetDeviceCenter("CA");
}

//_____________________________________________________________________________
TVector3 TAGgeoTrafo::GetCAAngles()
{
   return GetDeviceAngle("CA");
}
