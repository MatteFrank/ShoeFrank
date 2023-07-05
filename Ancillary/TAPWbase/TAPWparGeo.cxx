/*!
  \file
  \version $Id: TAPWparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TAPWparGeo.
*/

#include <string.h>

#include <fstream>
#include <sstream>
#include "TSystem.h"
#include "TString.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"

#include "TAGmaterials.hxx"
#include "TAPWparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TAPWparGeo TAPWparGeo.hxx "TAPWparGeo.hxx"
  \brief Map and Geometry parameters for the start counter. **
*/

ClassImp(TAPWparGeo);

const TString TAPWparGeo::fgkBaseName    = "PW";
const TString TAPWparGeo::fgkDefParaName = "pwGeo";

//------------------------------------------+-----------------------------------
//! Default constructor.
TAPWparGeo::TAPWparGeo()
 : TAGparTools(),
   fSize1(),
   fMaterial1(""),
   fDensity1(0.),
   fSize2(),
   fMaterial2(""),
   fDensity2(0.),
   fDebugLevel(0)
{
  fkDefaultGeoName = "./geomaps/TAPWdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAPWparGeo::~TAPWparGeo()
{
}

//______________________________________________________________________________
Bool_t TAPWparGeo::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;

   Info("FromFile()", "Open file %s for geometry\n", name.Data());

   //The center is taken from the global setup of the experiment.
   ReadVector3(fSize1);
   if(fDebugLevel)
      cout  << "  Size: "
      << fSize1[0] << " " << fSize1[1]  << " " <<  fSize1[2] << endl;
   
   ReadStrings(fMaterial1);
   if(fDebugLevel)
      cout  << "   PW material1: " <<  fMaterial1 << endl;
   
   ReadItem(fDensity1);
   if(fDebugLevel)
      cout  << "   PW density1: " <<  fDensity1 << endl;
   
   ReadVector3(fSize2);
   if(fDebugLevel)
      cout  << "  Size: "
      << fSize2[0] << " " << fSize2[1]  << " " <<  fSize2[2] << endl;
   
   ReadStrings(fMaterial2);
   if(fDebugLevel)
      cout  << "   PW material2: " <<  fMaterial2 << endl;
   
   ReadItem(fDensity2);
   if(fDebugLevel)
      cout  << "   PW density2: " <<  fDensity2 << endl;

   // Define material
   DefineMaterial();
   
   return true;
}

//_____________________________________________________________________________
void TAPWparGeo::DefineMaterial()
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // stage1 material
   TGeoMaterial* mat1 = TAGmaterials::Instance()->CreateMaterial(fMaterial1, fDensity1);
   if(fDebugLevel) {
      printf("PW layer1 material:\n");
      mat1->Print();
   }
   
   // stage2 material
   TGeoMaterial* mat2 = TAGmaterials::Instance()->CreateMaterial(fMaterial2, fDensity2);
   if(fDebugLevel) {
      printf("PW layer2 material:\n");
      mat2->Print();
   }
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.

void TAPWparGeo::Clear(Option_t*)
{
   return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TAPWparGeo::ToStream(ostream& os, Option_t*) const
{
  os << "TAPWparGeo " << GetName() << endl;

  return;
}
//_____________________________________________________________________________
TGeoVolume* TAPWparGeo::BuildPhoswich(const char *pwName )
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // create PW
   TGeoMaterial* matPW;
   TGeoMedium*   medPW;
   
   if ( (matPW = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject("Vacuum")) == 0x0 )
      matPW = new TGeoMaterial("Vacuum", 0., 0., 0.);;
   if ( (medPW = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("Vacuum")) == 0x0 )
      medPW = new TGeoMedium("Vacuum",2,matPW);
   
   // create PW
   const Char_t* matName1 = fMaterial1.Data();
   TGeoMedium*  med1 = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName1);
   
   TGeoBBox* box1 = new TGeoBBox(pwName, fSize1[0]/2., fSize1[1]/2., fSize1[2]/2.);
   TGeoVolume *PW1 = new TGeoVolume(pwName, box1, med1);
   PW1->SetVisibility(true);
   PW1->SetLineColor(kBlue-2);
   PW1->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   const Char_t* matName2 = fMaterial2.Data();
   TGeoMedium*  med2 = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName2);

   TGeoBBox* box2 = new TGeoBBox(pwName, fSize2[0]/2., fSize2[1]/2., fSize2[2]/2.);
   TGeoVolume *PW2 = new TGeoVolume(pwName, box2, med2);
   PW2->SetVisibility(true);
   PW2->SetLineColor(kBlue-3);
   PW2->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   TGeoBBox* box = new TGeoBBox(pwName, fSize1[0]/2., fSize1[1]/2., (fSize1[2]+fSize2[2])/2.);
   TGeoVolume *PW = new TGeoVolume(pwName, box, medPW);

   TGeoTranslation trans1(0, 0, -(fSize1[2]+fSize2[2])/2. + fSize1[2]/2.);
   TGeoHMatrix* trafo1 = new TGeoHMatrix(trans1);
   PW->AddNode(PW1, 1, trafo1);
   TGeoTranslation trans2(0, 0, (fSize1[2]+fSize2[2])/2. - fSize2[2]/2.);
   TGeoHMatrix* trafo2 = new TGeoHMatrix(trans2);
   PW->AddNode(PW2, 2, trafo2);

   PW->SetVisibility(false);
   
   return PW;
}
