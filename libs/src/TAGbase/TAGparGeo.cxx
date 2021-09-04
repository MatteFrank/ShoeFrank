

#include <Riostream.h>
#include <fstream>
#include <sstream>


#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"

#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGmaterials.hxx"
#include "TAGionisMaterials.hxx"

#include "TAGparGeo.hxx"
#include "TAGroot.hxx"


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
: TAGparTools(),
  fIonisation(new TAGionisMaterials()),
  fFileName("")
{
   // Standard constructor
   fDefaultGeoName = "./geomaps/TAGdetector.geo";
}

//______________________________________________________________________________
TAGparGeo::~TAGparGeo()
{
   // Destructor
   delete fIonisation;
}

//_____________________________________________________________________________
void TAGparGeo::DefineMaterial()
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }

   // target material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fTargetParameter.Material, fTargetParameter.Density);
   if(FootDebugLevel(1)) {
      printf("Target material:\n");
      mat->Print();
   }

   TGeoElementTable tab(0);
   TGeoElement*  elem = tab.GetElement(fBeamParameter.AtomicNumber);
   TGeoMaterial* beam = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject(elem->GetName());

   if (beam == 0x0)
      beam =  new TGeoMaterial(elem->GetName(), elem->A(), elem->Z(), 0.1); // add beam materials for scattering angle computing purpose

   fBeamParameter.Material = elem->GetName();

   if(FootDebugLevel(1)) {
      printf("Beam material:\n");
      beam->Print();
   }

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
   fIonisation->SetMaterial(mat);
   fIonisation->AddMeanExcitationEnergy(fTargetParameter.ExcEnergy);

#else
   fIonisation->SetMeanExcitationEnergy(fTargetParameter.ExcEnergy);
   // put it under Cerenkov since only this EM property is available
   mat->SetCerenkovProperties(fIonisation);

#endif

}

//______________________________________________________________________________
Bool_t TAGparGeo::FromFile(const TString& name)
{
   TString nameExp;

   if (name.IsNull())
      nameExp = fDefaultGeoName;
   else
      nameExp = name;

   fFileName = nameExp;

   if (!Open(nameExp)) return false;

   if(FootDebugLevel(1))
      cout << endl << "Reading Beam Parameters " << endl;

   ReadItem(fBeamParameter.Size);
   if(FootDebugLevel(1))
      cout << "  Beam size:  "<< fBeamParameter.Size << endl;

   ReadStrings(fBeamParameter.Shape);
   if(FootDebugLevel(1))
      cout  << "  Beam shape:  "<< fBeamParameter.Shape.Data() << endl;

   ReadItem(fBeamParameter.Energy);
   if(FootDebugLevel(1))
      cout  << "  Beam energy:  "<< fBeamParameter.Energy << endl;

   ReadItem(fBeamParameter.AtomicMass);
   if(FootDebugLevel(1))
      cout  << "  Beam atomic mass:  "<< fBeamParameter.AtomicMass << endl;

   ReadItem(fBeamParameter.AtomicNumber);
   if(FootDebugLevel(1))
      cout  << "  Beam atomic number:  "<< fBeamParameter.AtomicNumber << endl;

   ReadStrings(fBeamParameter.Material);
   if(FootDebugLevel(1))
      cout  << "  Beam material:  "<< fBeamParameter.Material.Data() << endl;

   ReadItem(fBeamParameter.PartNumber);
   if(FootDebugLevel(1))
      cout  << "   Number of particles:  "<< fBeamParameter.PartNumber << endl;

   ReadVector3(fBeamParameter.Position);
   if(FootDebugLevel(1))
      cout  << "  Position: "
      << Form("%f %f %f", fBeamParameter.Position[0], fBeamParameter.Position[1], fBeamParameter.Position[2]) << endl;

   ReadVector3(fBeamParameter.AngSpread);
   if(FootDebugLevel(1))
      cout  << "  Angular spread: "
      << Form("%f %f %f", fBeamParameter.AngSpread[0], fBeamParameter.AngSpread[1], fBeamParameter.AngSpread[2]) << endl;

   ReadItem(fBeamParameter.AngDiv);
   if(FootDebugLevel(1))
      cout  << "   Angular divergence:  "<< fBeamParameter.AngDiv << endl;


   if(FootDebugLevel(1))
      cout << endl << "Reading target Parameters " << endl;

   ReadStrings(fTargetParameter.Shape);
   if(FootDebugLevel(1))
      cout  << "  Target shape:  "<< fTargetParameter.Shape.Data() << endl;

   ReadVector3(fTargetParameter.Size);
   if(FootDebugLevel(1))
      cout  << "  Size: "
      << Form("%f %f %f", fTargetParameter.Size[0], fTargetParameter.Size[1], fTargetParameter.Size[2]) << endl;

   ReadStrings(fTargetParameter.Material);
   if(FootDebugLevel(1))
      cout  << "  Target material:  "<< fTargetParameter.Material.Data() << endl;

   ReadItem(fTargetParameter.AtomicMass);
   if(FootDebugLevel(1))
      cout  << "  Target atomic mass:  "<< fTargetParameter.AtomicMass << endl;

   ReadItem(fTargetParameter.Density);
   if(FootDebugLevel(1))
      cout  << "  Target density:  "<< fTargetParameter.Density << endl;

   ReadItem(fTargetParameter.ExcEnergy);
   if(FootDebugLevel(1))
      cout  << "  Target material mean excitation energy:  "<< fTargetParameter.ExcEnergy << endl;

   ReadItem(fInsertsN);
   if(FootDebugLevel(1))
      cout  << "Number of inserts:  "<< fInsertsN << endl;

   for (Int_t p = 0; p < fInsertsN; p++) { // Loop on each plane

      // read sensor index
      ReadItem(fInsertParameter[p].InsertIdx);
      if(FootDebugLevel(1))
         cout << endl << " - Parameters of Sensor " <<  fInsertParameter[p].InsertIdx << endl;

      ReadStrings(fInsertParameter[p].Material);
      if(FootDebugLevel(1))
         cout  << "  Insert material:  "<< fInsertParameter[p].Material.Data() << endl;

      ReadItem(fInsertParameter[p].AtomicMass);
      if(FootDebugLevel(1))
         cout  << "  Insert atomic mass:  "<< fInsertParameter[p].AtomicMass << endl;

      ReadStrings(fInsertParameter[p].Shape);
      if(FootDebugLevel(1))
         cout  << "  Insert shape:  "<< fInsertParameter[p].Shape.Data() << endl;

      ReadVector3(fInsertParameter[p].Size);
      if(FootDebugLevel(1))
         cout  << "  Size: "
         << Form("%f %f %f", fInsertParameter[p].Size[0], fInsertParameter[p].Size[1], fInsertParameter[p].Size[2]) << endl;

      // read sensor position
      ReadVector3(fInsertParameter[p].Position);
      if(FootDebugLevel(1))
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
void TAGparGeo::Print(Option_t* option) const
{
   TString opt(option);

   if (opt.Contains("all")) {

      cout << endl << "Reading Beam Parameters " << endl;

      cout << "  Beam size:  "<< fBeamParameter.Size << " cm (FWHM)" << endl;

      cout  << "  Beam shape:  "<< fBeamParameter.Shape.Data() << endl;

      cout  << "  Beam energy:  "<< fBeamParameter.Energy << " GeV/u" <<endl;

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
   } else {
      printf("\nIn file %s the following beam parameters for a %.0f%s beam impinging on %s target have been set:\n",
	     fFileName.Data(), fBeamParameter.AtomicMass,
	     fBeamParameter.Material.Data(),
             fTargetParameter.Material.Data());

      printf("BeamEnergy:          %.3f GeV/u\n", fBeamParameter.Energy);
      printf("BeamAtomicMass:      %.0f\n",       fBeamParameter.AtomicMass);
      printf("BeamAtomicNumber:    %d\n",         fBeamParameter.AtomicNumber);
      printf("BeamMaterial:       \"%s\"\n\n",    fBeamParameter.Material.Data());

      printf("TargetMaterial:     \"%s\"\n",      fTargetParameter.Material.Data());
      printf("TargetThickness:     %.3f cm\n\n",  fTargetParameter.Size[2]);

   }
}


//_____________________________________________________________________________
string TAGparGeo::PrintStandardBodies( ) {

  stringstream ss;

  ss << "* ***Black Body" << endl;
  ss << "RPP blk        -1000. 1000. -1000. 1000. -1000. 1000." << endl;
  ss << "* ***Air" << endl;
  ss << "RPP air        -900.0 900.0 -900.0 900.0 -900.0 900.0" << endl;

  double zplane;

  TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

  TVector3  fCenterTW = fpFootGeo->GetTWCenter();
  TVector3  fCenterMSD = fpFootGeo->GetMSDCenter();
  zplane =  fCenterMSD.Z() + ( fCenterTW.Z()-fCenterMSD.Z() ) /2.;
  //needed to subdivide air in two because when the calo is present too many bodies
  //are subtracted to air and fluka complains
  if(TAGrecoManager::GetPar()->IncludeCA())
    ss << "XYP airpla     " << zplane << endl;

  return ss.str();

}


//_____________________________________________________________________________
string TAGparGeo::PrintTargRotations()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeTG()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

    TVector3 fCenter = fpFootGeo->GetTGCenter();
    TVector3  fAngle = fpFootGeo->GetTGAngles();

    if(fAngle.X()!=0 || fAngle.Y()!=0 || fAngle.Z()!=0){

      ss << PrintCard("ROT-DEFI", "300.", "", "",
		      Form("%f",-fCenter.X()), Form("%f",-fCenter.Y()),
		      Form("%f",-fCenter.Z()), "tg") << endl;
      if(fAngle.X()!=0)
	ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",fAngle.X()),"", "", "", "tg") << endl;
      if(fAngle.Y()!=0)
	ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",fAngle.Y()),"", "", "", "tg") << endl;
      if(fAngle.Z()!=0)
	ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",fAngle.Z()),"", "", "", "tg") << endl;
      ss << PrintCard("ROT-DEFI", "300.", "", "",
		      Form("%f",fCenter.X()), Form("%f",fCenter.Y()),
		      Form("%f",fCenter.Z()), "tg") << endl;

    }
  }

  return ss.str();

}


//_____________________________________________________________________________
string TAGparGeo::PrintTargBody( ) {

  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeTG()){
    ss << "* ***Target" << endl;

    double zero = 0.;

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

    TVector3  fCenter = fpFootGeo->GetTGCenter();
    TVector3  fAngle = fpFootGeo->GetTGAngles();

    TVector3 tgSize =  GetTargetPar().Size;

    if(fAngle.Mag()!=0)
      ss << "$start_transform tg" << endl;

    ss << setiosflags(ios::fixed) << setprecision(6);
    ss << "RPP tgt     "  << fCenter[0]-tgSize.X()/2. << " " << fCenter[0]+tgSize.X()/2 << " " <<
      fCenter[1]-tgSize.Y()/2. << " " << fCenter[1]+tgSize.Y()/2. << " " <<
      fCenter[2]-tgSize.Z()/2. << " " << fCenter[2]+tgSize.Z()/2. << " " <<  endl;

    if(fAngle.Mag()!=0)
      ss << "$end_transform" << endl;

  }
  return ss.str();
}


//_____________________________________________________________________________
// for blackbody and air
string TAGparGeo::PrintStandardRegions1() {

  stringstream ss;

  ss <<"BLACK        5 blk -air\n";
  ss <<"* ***Air\n";
  if(TAGrecoManager::GetPar()->IncludeCA())
    ss <<"AIR1          5 air +airpla";
  else
    ss <<"AIR1          5 air";


  return ss.str();

}


//_____________________________________________________________________________
//for air 2 because when the calo is present too many bodies
//are subtracted to air and fluka complains so it's necessary to subdivide air in 2 parts
string TAGparGeo::PrintStandardRegions2() {

  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeCA())
    ss <<"AIR2          5 air -airpla";

  return ss.str();

}


//_____________________________________________________________________________
string TAGparGeo::PrintTargRegion() {

  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeTG()){

    ss << "* ***Target" << endl;

    ss << "TARGET      5 +tgt" << endl;

  }

  return ss.str();
}


//_____________________________________________________________________________
string TAGparGeo::PrintSubtractTargBodyFromAir() {

  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeTG()){

    ss << "-tgt " << endl;

  }

  return ss.str();

}

//_____________________________________________________________________________
string TAGparGeo::PrintTargAssignMaterial(TAGmaterials *Material) {

  stringstream outstr;

  if(TAGrecoManager::GetPar()->IncludeTG()){

    TString flkmat;

    if (Material == NULL){
      TAGmaterials::Instance()->PrintMaterialFluka();
      flkmat = TAGmaterials::Instance()->GetFlukaMatName(GetTargetPar().Material);
    }
    else
      flkmat = Material->GetFlukaMatName(GetTargetPar().Material);

    bool magnetic = false;
    if(TAGrecoManager::GetPar()->IncludeDI())
      magnetic = true;

    outstr << PrintCard("ASSIGNMA", flkmat, "TARGET", "", "", Form("%d",magnetic), "", "") << endl;

  }

  return outstr.str();
}

//_____________________________________________________________________________
string TAGparGeo::PrintStandardAssignMaterial() {

  stringstream ss;

  int magnetic = 0;
  if(TAGrecoManager::GetPar()->IncludeDI())
    magnetic = 1;

  ss << PrintCard("ASSIGNMA","BLCKHOLE", "BLACK","","","","","") << endl;
  ss << PrintCard("ASSIGNMA","AIR","AIR1","","",TString::Format("%d",magnetic),"","") << endl;

  if(TAGrecoManager::GetPar()->IncludeCA())
    ss << PrintCard("ASSIGNMA","AIR","AIR2","","",TString::Format("%d",magnetic),"","") << endl;

  return ss.str();
}

//_____________________________________________________________________________
string TAGparGeo::PrintBeam() {

  stringstream str;

  string part_type;
  if (GetBeamPar().AtomicNumber>2)
    part_type = "HEAVYION";
  else if (GetBeamPar().AtomicNumber==1 && GetBeamPar().AtomicMass==1)
    part_type = "PROTON";
  else if (GetBeamPar().AtomicNumber==2 && GetBeamPar().AtomicMass==4)
    part_type = "4-HELIUM";
  else{
    cout << "**** ATTENTION: unknown beam!!!! ****"<< endl;
    exit(0);
  }

  str << PrintCard("BEAM",TString::Format("%f",-(GetBeamPar().Energy)), "",
		   TString::Format("%f",GetBeamPar().AngDiv),
		   TString::Format("%f",-GetBeamPar().Size),
		   TString::Format("%f",-GetBeamPar().Size),
		   "1.0",part_type) << endl;
  if(part_type == "HEAVYION")
    str << PrintCard("HI-PROPE",TString::Format("%d",GetBeamPar().AtomicNumber),
		     TString::Format("%.0f",GetBeamPar().AtomicMass),"","","","","") << endl;
  str << PrintCard("BEAMPOS",TString::Format("%.3f",GetBeamPar().Position.X()),
		   TString::Format("%.3f",GetBeamPar().Position.Y()),
		   TString::Format("%.3f",GetBeamPar().Position.Z()),"","","","") << endl;

  return str.str();

}


//_____________________________________________________________________________
string TAGparGeo::PrintPhysics() {

  stringstream str;

  if ( TAGrecoManager::GetPar()->verFLUKA() ) {
    str << PrintCard("PHYSICS","1.","","","","","","COALESCE") << endl;
    str << PrintCard("PHYSICS","3.","","","","","","EVAPORAT") << endl;
    str << PrintCard("PHYSICS","1.","","","","","","ISOMERS") << endl;
    str << PrintCard("PHYSICS","1.","0.005","0.15","2.","2.","2.","IONSPLIT") << endl;
    str << PrintCard("RADDECAY","2.","","1.","","","","") << endl;
  }  else {
    str << PrintCard("PHYSICS","212001.","1.","1.","","","","COALESCE") << endl;
    str << PrintCard("PHYSICS","3.","","","","","","EVAPORAT") << endl;
    str << PrintCard("PHYSICS","1.","","","","","","ISOMERS") << endl;
    str << PrintCard("PHYSICS","1.","","","","","","INFLDCAY") << endl;
    str << PrintCard("PHYSICS","1.","","","","","","QUASI-EL") << endl;
    str << PrintCard("PHYSICS","1.","0.005","0.15","2.","2.","2.","IONSPLIT") << endl;
    str << PrintCard("RADDECAY","1.","2.","","1.","","","") << endl;
  }
  str << PrintCard("EMFCUT","-1.","1.","","BLACK","@LASTREG","1.0","") << endl;
  str << PrintCard("EMFCUT","-1.","1.","1.","BLCKHOLE","@LASTMAT","1.0","PROD-CUT") << endl;
  str << PrintCard("DELTARAY","1.","","","BLCKHOLE","@LASTMAT","1.0","") << endl;
  str << PrintCard("PAIRBREM","-3.","","","BLCKHOLE","@LASTMAT","","") << endl;

  if(TAGrecoManager::GetPar()->IncludeDI()){
    str << PrintCard("MGNFIELD","0.1","0.00001","","0.","0.","0.","") << endl;
  }

  return str.str();

}
