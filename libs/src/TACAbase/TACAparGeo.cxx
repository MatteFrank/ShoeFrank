#include <iostream>
#include <fstream>
#include <Riostream.h>
#include <stdio.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TList.h"
#include "TMath.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TSystem.h"

#include "TAGgeoTrafo.hxx"
#include "TAGmaterials.hxx"

#include "TACAparGeo.hxx"
#include "TAGroot.hxx"

#include "GlobalPar.hxx"

//##############################################################################

const TString TACAparGeo::fgkDefParaName     = "caGeo";
const TString TACAparGeo::fgkBaseName        = "CA";
const Color_t TACAparGeo::fgkDefaultModCol   = kAzure+5;
const Color_t TACAparGeo::fgkDefaultModColOn = kRed-5;
const TString TACAparGeo::fgkDefaultCrysName = "caCrys";


//_____________________________________________________________________________
TACAparGeo::TACAparGeo() 
: TAGparTools()
{
   fkDefaultGeoName = "./geomaps/TACAdetector.map";
}

//______________________________________________________________________________
TACAparGeo::~TACAparGeo()
{
}

//______________________________________________________________________________
Bool_t TACAparGeo::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   //   FootDebug(1, "FromFile()", Form("Number of crystals: %d", fCrystalsN));
   //
   //   if(FootDebugLevel(1))
   //      cout  << "Number of crystals: " <<  fCrystalsN << endl;
   
   ReadItem(fCrystalsN);
   if (fDebugLevel)
      cout  << "Number of crystals: " <<  fCrystalsN << endl;
   
   ReadStrings(fCrystalMat);
   if(fDebugLevel)
      cout  << "   Crystals material : " <<  fCrystalMat << endl;
   
   ReadItem(fCrystalDensity);
   if(fDebugLevel)
      cout  << "   Crystals density : " <<  fCrystalDensity << endl;
   
   ReadVector3(fCaloSize);
   if(fDebugLevel)
      cout << "   Calorimeter size: "
      << Form("%f %f %f", fCaloSize[0], fCaloSize[1], fCaloSize[2]) << endl;
   
   ReadVector3(fCrystalSize);
   if(fDebugLevel)
      cout << "   Crystal size: "
      << Form("%f %f %f", fCrystalSize[0], fCrystalSize[1], fCrystalSize[2]) << endl;
   
   // define material
   DefineMaterial();
   
   
   TVector3 position;
   TVector3 tilt;
   
   Int_t nCrystal = 0;
   
   SetupMatrices(fCrystalsN);
   
   // Read transformtion info
      for (Int_t iCrystal = 0; iCrystal < fCrystalsN; ++iCrystal) {
         
         ReadItem(nCrystal);
         if(fDebugLevel)
            cout  << "Crystal id "<< nCrystal << endl;
         
         // read  position
         ReadVector3(position);
         if(fDebugLevel)
            cout << "   Position: "
            << Form("%f %f %f", position[0], position[1], position[2]) << endl;
         
         ReadVector3(tilt);
         if(fDebugLevel)
            cout  << "   Tilt: "
            << Form("%f %f %f", tilt[0], tilt[1], tilt[2]) << endl;
         
         TGeoRotation rot;
         rot.RotateX(tilt[0]);
         rot.RotateY(tilt[1]);
         rot.RotateZ(tilt[2]);
         
         TGeoTranslation trans(position[0], position[1], position[2]);
         
         TGeoHMatrix  transfo;
         transfo  = trans;
         transfo *= rot;
         AddTransMatrix(new TGeoHMatrix(transfo), nCrystal);
      }

   return true;
}

//_____________________________________________________________________________
TVector3 TACAparGeo::GetCrystalPosition(Int_t idx)
{
   TGeoHMatrix* hm = GetTransfo(idx);
   if (hm) {
      TVector3 local(0,0,0);
      fCurrentPosition =  Sensor2Detector(idx, local);
   }
   return fCurrentPosition;   
}

//_____________________________________________________________________________
TVector3 TACAparGeo::Sensor2Detector(Int_t idx, TVector3& loc) const
{
   if (idx < 0 || idx > fCrystalsN) {
      Warning("Sensor2Detector()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return LocalToMaster(idx, loc);
}


//_____________________________________________________________________________
TVector3 TACAparGeo::Sensor2DetectorVect(Int_t idx, TVector3& loc) const
{
   if (idx < 0 || idx > fCrystalsN) {
      Warning("Sensor2DetectorVect()","Wrong detector id number: %d ", idx);
      TVector3(0,0,0);
   }
   
   return LocalToMasterVect(idx, loc);
}

//_____________________________________________________________________________
TVector3 TACAparGeo::Detector2Sensor(Int_t idx, TVector3& glob) const
{
   if (idx < 0 || idx > fCrystalsN) {
      Warning("Detector2Sensor()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return MasterToLocal(idx, glob);
}

//_____________________________________________________________________________
TVector3 TACAparGeo::Detector2SensorVect(Int_t idx, TVector3& glob) const
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
   
   // TW material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fCrystalMat, fCrystalDensity);
   if (fDebugLevel) {
      printf("Calorimeter material:\n");
      mat->Print();
   }
}

//_____________________________________________________________________________
TGeoVolume* TACAparGeo::BuildCalorimeter(const char *caName)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager(TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   TGeoVolume* wall = gGeoManager->FindVolumeFast(caName);
   if ( wall == 0x0 ) {
      TGeoMedium*  med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
      wall = gGeoManager->MakeBox(caName, med,  GetCrystalHeight()/2.,  GetCrystalHeight()/2., GetCrystalThick()/2.);
   }
   
   for (Int_t i = 0; i < fCrystalsN; ++i) {
      
         TGeoHMatrix* hm = GetTransfo(i);
         TGeoVolume* module = BuildModule(i);
         wall->AddNode(module, i, hm);
   }
   
   return wall;
}


/*------------------------------------------+---------------------------------*/
//! build module

TGeoVolume* TACAparGeo::BuildModule(Int_t idx)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   const char* moduleName = GetDefaultCrysName(idx);
   TGeoVolume* module     = gGeoManager->FindVolumeFast(moduleName);
   if ( module == 0x0 ) {
      const Char_t* matName = fCrystalMat.Data();
      TGeoMedium*   med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
      module = gGeoManager->MakeBox(moduleName, med,  GetCrystalWidth()/2., GetCrystalWidth()/2., GetCrystalThick()/2.);
   }
   
   module->SetLineColor(fgkDefaultModCol);
   module->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   return module;
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
      vol->SetLineColor(GetDefaultModColOn());
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
      vol->SetLineColor(GetDefaultModCol());
}

//_____________________________________________________________________________
string TACAparGeo::PrintBodies(){
   
   stringstream ss;
   
   if ( GlobalPar::GetPar()->IncludeCA()){   

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3  fCenter = fpFootGeo->GetCACenter();
   
     ss << "* ***Calorimeter" << endl;

     for (int iCal=0; iCal<GetCrystalsN(); iCal++){
       
       string bodyname, regionname;

       bodyname = Form("cal%d",iCal);
       regionname = Form("CAL%d",iCal);
       vBody.push_back(bodyname);
       vRegion.push_back(regionname);

       ss << "RPP " << bodyname <<  "     " 
	      <<  fCenter.X() + GetCrystalPosition(iCal).X() - GetCrystalSize().X()/2.<< " "	
	      <<  fCenter.X() + GetCrystalPosition(iCal).X() + GetCrystalSize().X()/2.<< " "
	      <<  fCenter.Y() + GetCrystalPosition(iCal).Y() - GetCrystalSize().Y()/2.<< " "
	      <<  fCenter.Y() + GetCrystalPosition(iCal).Y() + GetCrystalSize().Y()/2.<< " "
	      <<  fCenter.Z() + GetCrystalPosition(iCal).Z() - GetCrystalSize().Z()/2.<< " "
	      <<  fCenter.Z() + GetCrystalPosition(iCal).Z() + GetCrystalSize().Z()/2.<< endl;
     }
     
   }
   
   return ss.str();
}



//_____________________________________________________________________________
string TACAparGeo::PrintRegions(){

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeCA()){

    string name;

    ss << "* ***Calorimeter regions" << endl;

    for(int i=0; i<vRegion.size(); i++) 
      ss << setw(13) << setfill( ' ' )  << std::left << vRegion.at(i) << "5 " << vBody.at(i) << endl;
  
  }
  
  return ss.str();

}


//_____________________________________________________________________________
string TACAparGeo::PrintParameters() {
  
  stringstream ss;
  ss << setiosflags(ios::fixed) << setprecision(5);

  ss << "c     CALORIMETER PARAMETERS " << endl;
  ss << endl;    
  
  string ncrystal = "ncryCAL";
  ss << "      integer " << ncrystal << endl;
  ss << "      parameter(" << ncrystal << " = " << fCrystalsN << ")" << endl;
  ss << endl;    
  
  return ss.str();

}
//_____________________________________________________________________________
string TACAparGeo::PrintAssignMaterial() {

  stringstream ss;
  
  if(GlobalPar::GetPar()->IncludeCA()){

    const Char_t* mat = fCrystalMat.Data();
      
    bool magnetic = false;
    if(GlobalPar::GetPar()->IncludeDI())
      magnetic = true;

    if (vRegion.size()==0 )
      cout << "Error: CA regions vector not correctly filled!" << endl;

    ss << PrintCard("ASSIGNMA", mat, vRegion.at(0), vRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;

  }

  return ss.str();
  
}



//_____________________________________________________________________________
string TACAparGeo::PrintSubtractBodiesFromAir() {

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeCA()){

    for(int i=0; i<vBody.size(); i++) {
      ss << " -" << vBody.at(i);
      if ((i+1)%10==0 && (i+1)<GetCrystalsN()) ss << endl;
    }
    ss << endl;

  }

    return ss.str();
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

