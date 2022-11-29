/*!
  \file
  \version $Id: TANEparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TANEparGeo.
*/
#include "TGeoMatrix.h"
#include <string.h>

#include <fstream>
#include <sstream>
#include "TSystem.h"
#include "TString.h"
#include "TAGrecoManager.hxx"

#include "TANEparGeo.hxx"
#include "TAGroot.hxx"


//##############################################################################

/*!
  \class TANEparGeo TANEparGeo.hxx "TANEparGeo.hxx"
  \brief Map and Geometry parameters for the start counter. **
*/

ClassImp(TANEparGeo);

const TString TANEparGeo::fgkBaseName    = "NE";
const TString TANEparGeo::fgkDefParaName = "neGeo";

//------------------------------------------+-----------------------------------
//! Default constructor.
TANEparGeo::TANEparGeo()
 : TAGparTools(),
   fModulesN(0),
   fSize(0,0,0),
   fMaterial(""),
   fDensity(0.),
   fIonisMat(0.)
{
  fkDefaultGeoName = "./geomaps/TANEdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.

TANEparGeo::~TANEparGeo()
{
}

//______________________________________________________________________________
Bool_t TANEparGeo::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;

   Info("FromFile()", "Open file %s for geometry\n", name.Data());

   ReadItem(fModulesN);
   if(FootDebugLevel(1))
      cout << endl << "Modules number "<< fModulesN << endl;
   
   //The center is taken from the global setup of the experiment.
   ReadStrings(fMaterial);
   if(FootDebugLevel(1))
      cout  << "   Module material: " <<  fMaterial << endl;
   
   ReadItem(fDensity);
   if(FootDebugLevel(1))
      cout  << "   Module density: " <<  fDensity << endl;
   
   ReadItem(fIonisMat);
   if(FootDebugLevel(1))
      cout << "   Module Mean excitation energy : " <<  fIonisMat << endl;

   ReadVector3(fSize);
   if(FootDebugLevel(1))
      cout  << "   Size of module:     "<< fSize.X() << " " <<  fSize.Y() << " " <<  fSize.Z()  << endl;
   
   SetupMatrices(fModulesN);

   for (Int_t p = 0; p < fModulesN; p++) { // Loop on each plane
      
      // read Module index
      ReadItem(fModuleParameter[p].ModuleIdx);
      if(FootDebugLevel(1))
         cout << endl << " - Parameters of Module " <<  fModuleParameter[p].ModuleIdx << endl;
      
      // read Module position
      ReadVector3(fModuleParameter[p].Position);
      if(FootDebugLevel(1))
         cout << "   Position: " << fModuleParameter[p].Position[0] << " " << fModuleParameter[p].Position[1] << " " << fModuleParameter[p].Position[2] << endl;
      
      // read Module angles
      ReadVector3(fModuleParameter[p].Tilt);
      if(FootDebugLevel(1))
         cout  << "   Tilt: "
         << fModuleParameter[p].Tilt[0] << " " <<  fModuleParameter[p].Tilt[1] << " " << fModuleParameter[p].Tilt[2] << endl;
      
      Float_t thetaX = fModuleParameter[p].Tilt[0];
      Float_t thetaY = fModuleParameter[p].Tilt[1];
      Float_t thetaZ = fModuleParameter[p].Tilt[2];
      TGeoRotation rot;
      rot.RotateX(thetaX);
      rot.RotateY(thetaY);
      rot.RotateZ(thetaZ);
      
      Float_t transX = fModuleParameter[p].Position[0];
      Float_t transY = fModuleParameter[p].Position[1];
      Float_t transZ = fModuleParameter[p].Position[2];
      
      TGeoTranslation trans(transX, transY, transZ);
      
      TGeoHMatrix  transfo;
      transfo  = trans;
      transfo *= rot;
      AddTransMatrix(new TGeoHMatrix(transfo), fModuleParameter[p].ModuleIdx-1);
      
      // change to rad
      fModuleParameter[p].Tilt[0] = fModuleParameter[p].Tilt[0]*TMath::DegToRad();
      fModuleParameter[p].Tilt[1] = fModuleParameter[p].Tilt[1]*TMath::DegToRad();
      fModuleParameter[p].Tilt[2] = fModuleParameter[p].Tilt[2]*TMath::DegToRad();
   }
      
   // Close file
   Close();
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.

void TANEparGeo::Clear(Option_t*)
{
   return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TANEparGeo::ToStream(ostream& os, Option_t*) const
{
  os << "TANEparGeo " << GetName() << endl;

  return;
}
