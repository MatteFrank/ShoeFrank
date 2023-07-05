/*!
  \file
  \version $Id: TACEparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TACEparGeo.
*/

#include <string.h>

#include <fstream>
#include <sstream>
#include "TSystem.h"
#include "TString.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TGeoTube.h"

#include "TAGmaterials.hxx"
#include "TACEparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TACEparGeo TACEparGeo.hxx "TACEparGeo.hxx"
  \brief Map and Geometry parameters for the start counter. **
*/

ClassImp(TACEparGeo);

const TString TACEparGeo::fgkBaseName    = "CE";
const TString TACEparGeo::fgkDefParaName = "pwGeo";

//------------------------------------------+-----------------------------------
//! Default constructor.
TACEparGeo::TACEparGeo()
 : TAGparTools(),
   fSize(),
   fDebugLevel(0)
{
  fkDefaultGeoName = "./geomaps/TACEdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACEparGeo::~TACEparGeo()
{
}

//______________________________________________________________________________
Bool_t TACEparGeo::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;

   Info("FromFile()", "Open file %s for geometry\n", name.Data());

   //The center is taken from the global setup of the experiment.
   ReadVector3(fSize);
   if(fDebugLevel)
      cout  << "  Size: "
      << fSize[0] << " " << fSize[1]  << " " <<  fSize[2] << endl;
   
   ReadStrings(fMaterial);
   if(fDebugLevel)
      cout  << "   TW material: " <<  fMaterial << endl;
   
   ReadItem(fDensity);
   if(fDebugLevel)
      cout  << "   TW density: " <<  fDensity << endl;

   // window entrance
   ReadVector3(fSizeWindow);
   if(fDebugLevel)
      cout  << "  Size of entrance window: "
      << fSizeWindow[0] << " " << fSizeWindow[1]  << " " <<  fSizeWindow[2] << endl;
   
   ReadStrings(fMatWindow);
   if(fDebugLevel)
      cout  << "   TW entrance window material: " <<  fMatWindow << endl;
   
   ReadItem(fDensityWindow);
   if(fDebugLevel)
      cout  << "   TW entrance window density: " <<  fDensityWindow << endl;
   
   // Define material
   DefineMaterial();
   
   return true;
}

//_____________________________________________________________________________
void TACEparGeo::DefineMaterial()
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // TW material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fMaterial, fDensity);
   if(fDebugLevel) {
      printf("Time Of flight material:\n");
      mat->Print();
   }
   
   // TW material
   TGeoMaterial* matW = TAGmaterials::Instance()->CreateMaterial(fMatWindow, fDensityWindow);
   if(fDebugLevel) {
      printf("Time Of flight window material:\n");
      matW->Print();
   }
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.

void TACEparGeo::Clear(Option_t*)
{
   return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TACEparGeo::ToStream(ostream& os, Option_t*) const
{
  os << "TACEparGeo " << GetName() << endl;

  return;
}
//_____________________________________________________________________________
TGeoVolume* TACEparGeo::BuildTimeOfFlight(const char *twName )
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // create PW
   TGeoMaterial* mat;
   TGeoMedium*   med;
   
   if ( (mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject("Vacuum")) == 0x0 )
      mat = new TGeoMaterial("Vacuum", 0., 0., 0.);;
   if ( (med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("Vacuum")) == 0x0 )
      med = new TGeoMedium("Vacuum",2,mat);
   
   // create TW geometry
   const Char_t* matName = fMaterial.Data();
   TGeoMedium*  medTW = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   TGeoTube* boxTW = new TGeoTube(twName, 0, fSize[0]/2., fSize[2]/2.);
   TGeoVolume *tw = new TGeoVolume(twName, boxTW, medTW);
   tw->SetVisibility(true);
   tw->SetLineColor(kBlue-2);
   tw->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   // create TW window entrance geometry
   const Char_t* matNameW = fMatWindow.Data();
   TGeoMedium*  medW = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matNameW);
   
   TGeoTube* boxW = new TGeoTube(Form("%s_W",twName), 0, fSizeWindow[0]/2., fSizeWindow[2]/2.);
   TGeoVolume *twW = new TGeoVolume(Form("%s_W",twName), boxW, medW);
   twW->SetVisibility(true);
   twW->SetLineColor(kBlue-4);
   twW->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   
   TGeoBBox* box = new TGeoBBox(twName, fSize[0]/2., fSize[1]/2., (fSize[2]+fSizeWindow[2])/2.);
   TGeoVolume *det = new TGeoVolume(twName, box, med);


   TGeoTranslation trans1(0, 0, -(fSize[2]+fSizeWindow[2])/2. + fSizeWindow[2]/2.);
   TGeoHMatrix* trafo1 = new TGeoHMatrix(trans1);
   det->AddNode(twW, 1, trafo1);
   TGeoTranslation trans2(0, 0, (fSize[2]+fSizeWindow[2])/2. - fSize[2]/2.);
   TGeoHMatrix* trafo2 = new TGeoHMatrix(trans2);
   det->AddNode(tw, 2, trafo2);
   
   return det;
}
