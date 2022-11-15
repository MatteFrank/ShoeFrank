/*!
  \file
  \version $Id: TAPLparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TAPLparGeo.
*/

#include <string.h>

#include <fstream>
#include <sstream>
#include "TSystem.h"
#include "TString.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"

#include "TAGmaterials.hxx"
#include "TAPLparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TAPLparGeo TAPLparGeo.hxx "TAPLparGeo.hxx"
  \brief Map and Geometry parameters for the start counter. **
*/

ClassImp(TAPLparGeo);

const TString TAPLparGeo::fgkBaseName    = "ST";
const TString TAPLparGeo::fgkDefParaName = "stGeo";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAPLparGeo::TAPLparGeo()
 : TAGparTools(),
   fSize(),
   fDebugLevel(0)
{
  fkDefaultGeoName = "./geomaps/TAPLdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAPLparGeo::~TAPLparGeo()
{
}

//______________________________________________________________________________
Bool_t TAPLparGeo::FromFile(const TString& name)
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
      cout  << "   ST material: " <<  fMaterial << endl;
   
   ReadItem(fDensity);
   if(fDebugLevel)
      cout  << "   ST density: " <<  fDensity << endl;
   
   // Define material
   DefineMaterial();

   return true;
}

//_____________________________________________________________________________
void TAPLparGeo::DefineMaterial()
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // ST material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fMaterial, fDensity);
   if(fDebugLevel) {
      printf("Start counter material:\n");
      mat->Print();
   }
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.

void TAPLparGeo::Clear(Option_t*)
{
   return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TAPLparGeo::ToStream(ostream& os, Option_t*) const
{
  os << "TAPLparGeo " << GetName() << endl;

  return;
}
//_____________________________________________________________________________
TGeoVolume* TAPLparGeo::BuildStartCounter(const char *stName )
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // create counter
   const Char_t* matName = fMaterial.Data();
   TGeoMedium*  medST = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   TGeoBBox* box = new TGeoBBox(stName, fSize[0]/2., fSize[1]/2., fSize[2]/2.);
   TGeoVolume *start = new TGeoVolume(stName, box, medST);

   start->SetVisibility(true);
   start->SetLineColor(kBlue-5);
   start->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   return start;
}
