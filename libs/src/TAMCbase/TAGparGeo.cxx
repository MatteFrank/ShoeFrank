

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"

#include "TAGgeoTrafo.hxx"
#include "TAGmaterials.hxx"

#include "TAGparGeo.hxx"

//##############################################################################

/*!
 \class TAGparGeo TAGparGeo.hxx "TAGparGeo.hxx"
 \brief Map and Geometry parameters for vertex. **
 */

ClassImp(TAGparGeo);

const TString TAGparGeo::fgkBaseName    = "TG";
const TString TAGparGeo::fgkDefParaName = "tgGeo";


//______________________________________________________________________________
TAGparGeo::TAGparGeo()
: TAGparTools()
{
   // Standard constructor
   fgDefaultGeoName = "./geomaps/TAGdetector.map";
}

//______________________________________________________________________________
TAGparGeo::~TAGparGeo()
{
   // Destructor
}

//_____________________________________________________________________________
void TAGparGeo::DefineMaterial()
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // Epitaxial material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fTargetParameter.Material, fTargetParameter.Density);
   if (fDebugLevel) {
      printf("Target material:\n");
      mat->Print();
   }
}

//______________________________________________________________________________
Bool_t TAGparGeo::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fgDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   if(fDebugLevel)
      cout << endl << "Reading Beam Parameters " << endl;
   
   ReadItem(fBeamParameter.Size);
   if(fDebugLevel)
      cout << "  Beam size:  "<< fBeamParameter.Size << endl;
   
   ReadStrings(fBeamParameter.Shape);
   if(fDebugLevel)
      cout  << "  Beam shape:  "<< fBeamParameter.Shape.Data() << endl;
   
   ReadItem(fBeamParameter.Energy);
   if(fDebugLevel)
      cout  << "  Beam energy:  "<< fBeamParameter.Energy << endl;
   
   ReadItem(fBeamParameter.AtomicMass);
   if(fDebugLevel)
      cout  << "  Beam atomic mass:  "<< fBeamParameter.AtomicMass << endl;
   
   ReadItem(fBeamParameter.AtomicNumber);
   if(fDebugLevel)
      cout  << "  Beam atomic number:  "<< fBeamParameter.AtomicNumber << endl;
   
   ReadItem(fBeamParameter.PartNumber);
   if(fDebugLevel)
      cout  << "   Number of particles:  "<< fBeamParameter.PartNumber << endl;
	  
   ReadVector3(fBeamParameter.Position);
   if(fDebugLevel)
      cout  << "  Position: "
      << Form("%f %f %f", fBeamParameter.Position[0], fBeamParameter.Position[1], fBeamParameter.Position[2]) << endl;
   
   ReadVector3(fBeamParameter.AngSpread);
   if(fDebugLevel)
      cout  << "  Angular spread: "
      << Form("%f %f %f", fBeamParameter.AngSpread[0], fBeamParameter.AngSpread[1], fBeamParameter.AngSpread[2]) << endl;
   
   ReadItem(fBeamParameter.AngDiv);
   if(fDebugLevel)
      cout  << "   Angular divergence:  "<< fBeamParameter.AngDiv << endl;

   
   if(fDebugLevel)
      cout << endl << "Reading target Parameters " << endl;
   
   ReadStrings(fTargetParameter.Shape);
   if(fDebugLevel)
      cout  << "  Target shape:  "<< fTargetParameter.Shape.Data() << endl;
	  
   ReadVector3(fTargetParameter.Size);
   if(fDebugLevel)
      cout  << "  Size: "
      << Form("%f %f %f", fTargetParameter.Size[0], fTargetParameter.Size[1], fTargetParameter.Size[2]) << endl;
	  
   ReadStrings(fTargetParameter.Material);
   if(fDebugLevel)
      cout  << "  Target material:  "<< fTargetParameter.Material.Data() << endl;
   
   ReadItem(fTargetParameter.Density);
   if(fDebugLevel)
      cout  << "  Target density:  "<< fTargetParameter.Density << endl;
   
   
   ReadItem(fInsertsN);
   if(fDebugLevel)
      cout  << "Number of inserts:  "<< fInsertsN << endl;
   
   for (Int_t p = 0; p < fInsertsN; p++) { // Loop on each plane
      
      // read sensor index
      ReadItem(fInsertParameter[p].InsertIdx);
      if(fDebugLevel)
         cout << endl << " - Parameters of Sensor " <<  fInsertParameter[p].InsertIdx << endl;
      
      ReadStrings(fInsertParameter[p].Material);
      if(fDebugLevel)
         cout  << "  Insert material:  "<< fInsertParameter[p].Material.Data() << endl;
      
      ReadStrings(fInsertParameter[p].Shape);
      if(fDebugLevel)
         cout  << "  Insert shape:  "<< fInsertParameter[p].Shape.Data() << endl;
      
      ReadVector3(fInsertParameter[p].Size);
      if(fDebugLevel)
         cout  << "  Size: "
         << Form("%f %f %f", fInsertParameter[p].Size[0], fInsertParameter[p].Size[1], fInsertParameter[p].Size[2]) << endl;
      
      // read sensor position
      ReadVector3(fInsertParameter[p].Position);
      if(fDebugLevel)
         cout << "   Position: "
         << Form("%f %f %f", fInsertParameter[p].Position[0], fInsertParameter[p].Position[1], fInsertParameter[p].Position[2]) << endl;
   }
   
   // Close file
   Close();
   
   // Define materials
   DefineMaterial();

   return true;
}

//_____________________________________________________________________________
TGeoVolume* TAGparGeo::AddTarget(const char *targetName)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   TGeoVolume* target = 0x0;
   fTargetParameter.Shape.ToLower();
   if (fTargetParameter.Shape.Contains("cubic")) 
      target = AddCubicTarget(targetName);
   else if (fTargetParameter.Shape.Contains("cyl"))
      target =  AddCylindricTarget(targetName);
   else
      Warning("AddTarget", "No target in geometry");
   
   return target;
}

//_____________________________________________________________________________
TGeoVolume* TAGparGeo::BuildTarget(const char *targetName)
{
   return AddTarget(targetName);
}


//_____________________________________________________________________________
TGeoVolume* TAGparGeo::AddCubicTarget(const char *targetName)
{
   // get size
   Float_t dx = fTargetParameter.Size[0]/2.;
   Float_t dy = fTargetParameter.Size[1]/2.;
   Float_t dz = fTargetParameter.Size[2]/2.;
   
   // create module
   const Char_t* matName = fTargetParameter.Material.Data();
   TGeoMedium*   medTarget = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   TGeoVolume* target = gGeoManager->MakeBox(targetName, medTarget, dx, dy, dz);
   target->SetVisibility(true);
   target->SetTransparency( TAGgeoTrafo::GetDefaultTransp());
   
   return target;
}

//_____________________________________________________________________________
TGeoVolume* TAGparGeo::AddCylindricTarget(const char *targetName)
{
   // get size
   Float_t height  = fTargetParameter.Size[0]/2.;
   Float_t degrees = fTargetParameter.Size[1];
   Float_t radius  = fTargetParameter.Size[2]/2.;
   
   // create module
   const Char_t* matName = fTargetParameter.Material.Data();
   TGeoMedium*   medTarget = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);

   TGeoVolume* tubs = 0x0;
   if (TMath::Abs(degrees-360.) < 0.1)
      tubs = gGeoManager->MakeTube(Form("%s_tubs",targetName), medTarget, 0, radius, height);
   else
      tubs = gGeoManager->MakeTubs(Form("%s_tubs",targetName), medTarget, 0, radius, height, 0, degrees);
   
   TGeoVolume* target = gGeoManager->MakeBox(targetName,medTarget,radius,height,radius); // volume corresponding to vertex
   
   TGeoHMatrix hm;
   hm.RotateX(90);
   target->AddNode(tubs, 1, new TGeoHMatrix(hm));
   
   target->SetVisibility(true);
   target->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   return target;
}

//_____________________________________________________________________________
void TAGparGeo::Print(Option_t* /*opt*/) const
{
   cout << endl << "Reading Beam Parameters " << endl;
   
   cout << "  Beam size:  "<< fBeamParameter.Size << " cm (FWHM)" << endl;
   
   cout  << "  Beam shape:  "<< fBeamParameter.Shape.Data() << endl;
   
   cout  << "  Beam energy:  "<< fBeamParameter.Energy << " MeV" <<endl;
   
   cout  << "  Beam atomic mass:  "<< fBeamParameter.AtomicMass << endl;
   
   cout  << "  Beam atomic number:  "<< fBeamParameter.AtomicNumber << endl;
   
   cout  << "  Number of particles:  "<< fBeamParameter.PartNumber << endl;
	  
   cout  << "  Position: "
   << Form("%f %f %f", fBeamParameter.Position[0], fBeamParameter.Position[1], fBeamParameter.Position[2]) << " cm" << endl;
   
   cout  << "  Angular spread: "
   << Form("%f %f %f", fBeamParameter.AngSpread[0], fBeamParameter.AngSpread[1], fBeamParameter.AngSpread[2]) << " degrees" <<  endl;
   
   cout << endl << "Reading target Parameters " << endl;
   
   cout  << "  Target shape:  "<< fTargetParameter.Shape.Data() << endl;
	  
   cout  << "  Size: "
   << Form("%f %f %f", fTargetParameter.Size[0], fTargetParameter.Size[1], fTargetParameter.Size[2]) << " cm" << endl;
	  
   cout  << "  Target material:  "<< fTargetParameter.Material.Data() << endl;
   
   cout << endl;
   cout  << "Number of inserts:  "<< fInsertsN << endl;
   
   for (Int_t p = 0; p < fInsertsN; p++) { // Loop on each plane
      
      cout << endl << " - Parameters of Sensor " <<  fInsertParameter[p].InsertIdx << endl;
      
      cout  << "  Insert material:  "<< fInsertParameter[p].Material.Data() << endl;
      
      cout  << "  Insert shape:  "<< fInsertParameter[p].Shape.Data() << endl;
      
      cout  << "  Size: "
      << Form("%f %f %f", fInsertParameter[p].Size[0], fInsertParameter[p].Size[1], fInsertParameter[p].Size[2]) << " cm" <<  endl;
      
      cout << "   Position: "
      << Form("%f %f %f", fInsertParameter[p].Position[0], fInsertParameter[p].Position[1], fInsertParameter[p].Position[2]) << " cm" << endl;
   }
   cout << endl;

}

