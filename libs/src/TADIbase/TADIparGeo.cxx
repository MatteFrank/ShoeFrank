
#include <Riostream.h>

#include "TGeoBBox.h"
#include "TGeoTube.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TList.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGmaterials.hxx"
#include "TADIparGeo.hxx"

//##############################################################################

/*!
  \class TADIparGeo TADIparGeo.hxx "TADIparGeo.hxx"
  \brief Map and Geometry parameters for magnets. **
*/

ClassImp(TADIparGeo);

const TString TADIparGeo::fgkDefParaName     = "diGeo";
      TString TADIparGeo::fgDefaultGeoName = "./geomaps/TADIdetector.map";
const TString TADIparGeo::fgkDevBaseName   = "DI";
const Int_t   TADIparGeo::fgkDefMagnetsN   = 2;

//______________________________________________________________________________
TADIparGeo::TADIparGeo()
: TAGparTools(),
  fMagnetsN(0),
  fType(-1),
  fMapName("")
{
   // Standard constructor
}

//______________________________________________________________________________
TADIparGeo::~TADIparGeo()
{
   // Destructor
}

//______________________________________________________________________________
Bool_t TADIparGeo::FromFile(const TString& name) 
{
   // simple file reading, waiting for real config file
   TString nameExp;
   
   if (name.IsNull())
	  nameExp = fgDefaultGeoName;
   else 
	  nameExp = name;

   if (!Open(nameExp)) return false;
   
   ReadItem(fMagnetsN);  
   if(FootDebugLevel(1))
	  cout << endl << "Magnets number "<< fMagnetsN << endl;
   
   // read Magnet index
	  ReadItem(fType);
	  if(FootDebugLevel(1))
        cout  << "   Type of Magnet: " <<  fType << endl;
   
   if (fType == 2) {
      ReadStrings(fMapName);
      if(FootDebugLevel(1))
         cout  << "   Map File Name:  "<< fMapName.Data() << endl;
   }
   
   // Read cover thickness
   ReadItem(fShieldThick);
   if(FootDebugLevel(1))
      cout << endl << "   Shield thickness: "<< fShieldThick << endl;
   
   // Read cover material
   ReadStrings(fShieldMat);
   if(FootDebugLevel(1))
      cout  << "   Shield material:  "<< fShieldMat.Data() << endl;

   // read cover material density
	  ReadItem(fShieldDensity);
	  if(FootDebugLevel(1))
        cout  << "   Shield material density: " <<  fShieldDensity << endl;

   // Read magnet material
   ReadStrings(fMagMat);
   if(FootDebugLevel(1))
      cout  << "   Magnet material:  "<< fMagMat.Data() << endl;
   
   // read magnet material density
	  ReadItem(fMagDensity);
	  if(FootDebugLevel(1))
        cout  << "   Magnet material density: " <<  fMagDensity << endl;


   if(FootDebugLevel(1))
      cout << endl << "Reading Magnet Parameters " << endl;
   
   SetupMatrices(fMagnetsN);

   for (Int_t p = 0; p < fMagnetsN; p++) { // Loop on each plane
	  
	  // read Magnet index
	  ReadItem(fMagnetParameter[p].MagnetIdx);
	  if(FootDebugLevel(1))
		 cout << endl << " - Parameters of Magnet " <<  fMagnetParameter[p].MagnetIdx << endl;
      
      // read Magnet size
      ReadVector3(fMagnetParameter[p].Size);
      if(FootDebugLevel(1))
         cout << "   Size: "
         << Form("%f %f %f", fMagnetParameter[p].Size[0], fMagnetParameter[p].Size[1], fMagnetParameter[p].Size[2]) << endl;

      fMagnetParameter[p].ShieldSize[0] = fMagnetParameter[p].Size[0] - fShieldThick;
      fMagnetParameter[p].ShieldSize[1] = fMagnetParameter[p].Size[1] + fShieldThick;
      fMagnetParameter[p].ShieldSize[2] = fMagnetParameter[p].Size[2] + 2*fShieldThick;
      
      if(FootDebugLevel(1))
         cout << "   Shield Size: "
         << Form("%f %f %f", fMagnetParameter[p].ShieldSize[0], fMagnetParameter[p].ShieldSize[1], fMagnetParameter[p].ShieldSize[2]) << endl;
      

	  // read Magnet position
	  ReadVector3(fMagnetParameter[p].Position);
	  if(FootDebugLevel(1))
		 cout << "   Position: "
	         << Form("%f %f %f", fMagnetParameter[p].Position[0], fMagnetParameter[p].Position[1], fMagnetParameter[p].Position[2]) << endl;
	  
	  // read Magnet angles
	  ReadVector3(fMagnetParameter[p].Tilt);
	  if(FootDebugLevel(1))
		 cout  << "   Tilt: "
		       << Form("%f %f %f", fMagnetParameter[p].Tilt[0], fMagnetParameter[p].Tilt[1], fMagnetParameter[p].Tilt[2]) << endl;
	  
      Float_t thetaX = fMagnetParameter[p].Tilt[0];
      Float_t thetaY = fMagnetParameter[p].Tilt[1];
      Float_t thetaZ = fMagnetParameter[p].Tilt[2];

      TGeoRotation rot;
	  rot.RotateX(thetaX);   
	  rot.RotateY(thetaY);   
	  rot.RotateZ(thetaZ);
      
     Float_t transX = fMagnetParameter[p].Position[0];
     Float_t transY = fMagnetParameter[p].Position[1];
     Float_t transZ = fMagnetParameter[p].Position[2];
      
     TGeoTranslation trans(transX, transY, transZ);
	  
	  TGeoHMatrix  transfo;
	  transfo  = trans;
	  transfo *= rot;
	  AddTransMatrix(new TGeoHMatrix(transfo), fMagnetParameter[p].MagnetIdx-1);
	 
	 // change to rad
	 fMagnetParameter[p].Tilt[0] = fMagnetParameter[p].Tilt[0]*TMath::DegToRad();
	 fMagnetParameter[p].Tilt[1] = fMagnetParameter[p].Tilt[1]*TMath::DegToRad();
	 fMagnetParameter[p].Tilt[2] = fMagnetParameter[p].Tilt[2]*TMath::DegToRad();
   }	  

   // define material
   DefineMaterial();

   // Close file
   Close();
   
   return true;
}

//_____________________________________________________________________________
void TADIparGeo::DefineMaterial()
{
   
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // Shield material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fShieldMat, fShieldDensity);
   if (fDebugLevel) {
      printf("Shield material:\n");
      mat->Print();
   }

   
   // Magnet material
   mat = TAGmaterials::Instance()->CreateMaterial(fMagMat, fMagDensity);
   if (fDebugLevel) {
      printf("Magnet material:\n");
      mat->Print();
   }
}

//_____________________________________________________________________________
TVector3 TADIparGeo::GetPosition(Int_t iMagnet)
{
   TGeoHMatrix* hm = GetTransfo(iMagnet);
   if (hm) {
	  TVector3 local(0,0,0);
	  fCurrentPosition =  Local2Global(iMagnet,local);
   }
   return fCurrentPosition;
}

//_____________________________________________________________________________
void TADIparGeo::Global2Local(Int_t detID,
									Double_t xg, Double_t yg, Double_t zg, 
									Double_t& xl, Double_t& yl, Double_t& zl) const
{  
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Global2Local()","Wrong detector id number: %d ", detID); 
	  return ;
   }
   
   MasterToLocal(detID, xg, yg, zg, xl, yl, zl);
}   

//_____________________________________________________________________________
TVector3 TADIparGeo::Global2Local(Int_t detID, TVector3& glob) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Global2Local()","Wrong detector id number: %d ", detID); 
	  return TVector3(0,0,0);
   }
   
   return MasterToLocal(detID, glob);
}

//_____________________________________________________________________________
TVector3 TADIparGeo::Global2LocalVect(Int_t detID, TVector3& glob) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Global2LocalVect()","Wrong detector id number: %d ", detID); 
	  return TVector3(0,0,0);
   }
   
   return MasterToLocalVect(detID, glob);
}   

//_____________________________________________________________________________
void TADIparGeo::Local2Global(Int_t detID,
									Double_t xl, Double_t yl, Double_t zl, 
									Double_t& xg, Double_t& yg, Double_t& zg) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Local2Global()","Wrong detector id number: %d ", detID); 
	  return;
   }
   
   LocalToMaster(detID, xl, yl, zl, xg, yg, zg);
}

//_____________________________________________________________________________
TVector3 TADIparGeo::Local2Global(Int_t detID, TVector3& loc) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Local2Global()","Wrong detector id number: %d ", detID); 
	  TVector3(0,0,0);
   }
   
   return LocalToMaster(detID, loc);
}


//_____________________________________________________________________________
TVector3 TADIparGeo::Local2GlobalVect(Int_t detID, TVector3& loc) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Local2GlobalVect()","Wrong detector id number: %d ", detID); 
	  TVector3(0,0,0);
   }

   return LocalToMasterVect(detID, loc);
}

//_____________________________________________________________________________
TGeoVolume* TADIparGeo::BuildMagnet(const char* basemoduleName, const char *magnetName)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
	  new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   TGeoVolume* magnet = gGeoManager->FindVolumeFast(magnetName);
   if ( magnet == 0x0 ) {
      TGeoMedium*  med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
      magnet = gGeoManager->MakeBox(magnetName,med,fSizeBox.X()/2.,fSizeBox.Y()/2.,fSizeBox.Z()/2.); // volume corresponding to vertex
   }
   
   // create module
   const Char_t* matName = fMagMat.Data();
   TGeoMedium*  medMod = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);

   TGeoVolume* magnetMod = 0x0;
   for(Int_t iMag = 0; iMag < GetMagnetsN(); ++iMag) {
      TVector3 size = GetMagnetPar(iMag).Size;

      TGeoHMatrix* transfo = GetTransfo(iMag);
      TGeoHMatrix* transf = (TGeoHMatrix*)transfo->Clone();
      
      TGeoTube* tube = new TGeoTube(Form("TubeMagnet%d", iMag+1), size[0], size[1], size[2]/2.);
      
      TGeoVolume *magnetMod = new TGeoVolume(Form("%s_Magnet",basemoduleName), tube, medMod);
      magnetMod->SetLineColor(kBlue-3);
      magnetMod->SetTransparency( TAGgeoTrafo::GetDefaultTransp());
      
      magnet->AddNode(magnetMod, iMag, transf);
   }

   return magnet;
}

//_____________________________________________________________________________
void TADIparGeo::DefineMaxMinDimension()
{
   TVector3 size(0, 0, 0);
   TVector3 posAct(0, 0, 0);
   
   Int_t nSens = GetMagnetsN();
   
   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   for (Int_t iS = 0; iS < nSens; iS++) {
      posAct = GetMagnetPar(iS).Position;
      size  = GetMagnetPar(iS).Size;
      
      for(Int_t i = 0; i < 3; i++) {
         minPosition[i] = (minPosition[i] <= posAct[i] ) ? minPosition[i] : posAct[i];
         maxPosition[i] = (maxPosition[i] >= posAct[i] ) ? maxPosition[i] : posAct[i];
      }
   }
   
   minPosition[0] -= size[1];    maxPosition[0] += size[1];
   minPosition[1] -= size[1];    maxPosition[1] += size[1];
   minPosition[2] -= size[2]/2.; maxPosition[2] += size[2]/2.;
   
   fMinPosition = minPosition;
   fMaxPosition = maxPosition;
   
   for(Int_t i = 0; i< 3; ++i)
      fSizeBox[i] = (fMaxPosition[i] - fMinPosition[i]);
}
