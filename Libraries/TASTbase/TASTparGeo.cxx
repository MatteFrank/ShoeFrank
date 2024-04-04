/*!
  \file TASTparGeo.cxx
  \brief   Implementation of TASTparGeo.
*/

#include <string.h>

#include <fstream>
#include <sstream>
#include "TSystem.h"
#include "TString.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"

#include "TASTparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TASTparGeo 
  \brief Map and Geometry parameters for the start counter. **
*/

//! Class Imp
ClassImp(TASTparGeo);

const TString TASTparGeo::fgkBaseName    = "ST";

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTparGeo::TASTparGeo()
 : TAGparTools(),
   fSize(),
   fMaterial(""),
   fDensity(0.)
{
  fkDefaultGeoName = "./geomaps/TASTdetector.geo";
}

//------------------------------------------+-----------------------------------
//! Destructor.
TASTparGeo::~TASTparGeo()
{
}

//_____________________________________________________________________________
void TASTparGeo::DefineMaterial()
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // ST material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fMaterial, fDensity);
   if(FootDebugLevel(1)) {
      printf("Start counter material:\n");
      mat->Print();
   }
   // Frame material
   TGeoMaterial* matFrame = TAGmaterials::Instance()->CreateMaterial(fFrameMat, fFrameDensity);
   if(FootDebugLevel(1)) {
      printf("Frame material:\n");
      matFrame->Print();
   }
   // Foil material
   TGeoMaterial* matFoil = TAGmaterials::Instance()->CreateMaterial(fFoilMat, fFoilDensity);
   if(FootDebugLevel(1)) {
      printf("Foil material:\n");
      matFoil->Print();
   }
}

//______________________________________________________________________________
Bool_t TASTparGeo::FromFile(const TString& name)
{
   cout << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;

   Info("FromFile()", "Open file %s for geometry\n", name.Data());

   //The center is taken from the global setup of the experiment.
   ReadVector3(fSize);
   if(FootDebugLevel(1))
      cout  << "  Size: "
      << fSize[0] << " " << fSize[1]  << " " <<  fSize[2] << endl;
   
   ReadStrings(fMaterial);
   if(FootDebugLevel(1))
      cout  << "   ST material: " <<  fMaterial << endl;
   
   ReadItem(fDensity);
   if(FootDebugLevel(1))
      cout  << "   ST density: " <<  fDensity << endl;

   // Define material
   DefineMaterial();

   return true;
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TASTparGeo::Clear(Option_t*)
{
   return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TASTparGeo::ToStream(ostream& os, Option_t*) const
{
  os << "TASTparGeo " << GetName() << endl;
  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
     << "   ver_x   ver_y   ver_z  width" << endl;
}

//_____________________________________________________________________________
TGeoVolume* TASTparGeo::BuildStartCounter(const char *stName )
{
   // create counter
   const Char_t* matName = fMaterial.Data();
   TGeoMedium*  medST = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   TGeoBBox* box = new TGeoBBox(stName, fSize[0]/2., fSize[1]/2., fSize[2]/2.);
   TGeoVolume *start = new TGeoVolume(stName, box, medST);

   start->SetVisibility(true);
   start->SetLineColor(kRed-5);
   start->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   return start;
}

//_____________________________________________________________________________
string TASTparGeo::PrintRotations()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeST()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3 center = fpFootGeo->GetSTCenter();
    TVector3  angle = fpFootGeo->GetSTAngles(); //invert the angles to take into account the FLUKA convention
    angle *= -1;
     
    if(angle.Mag()!=0){
	  
      ss << PrintCard("ROT-DEFI", "300.", "", "",
		      Form("%f",-center.X()), Form("%f",-center.Y()),
		      Form("%f",-center.Z()), "st") << endl;
      if(angle.X()!=0)
	ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",angle.X()),"", "", "", "st") << endl;
      if(angle.Y()!=0)
	ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",angle.Y()),"", "", "", "st") << endl;
      if(angle.Z()!=0)
	ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",angle.Z()),"", "", "", "st") << endl;
      ss << PrintCard("ROT-DEFI", "300.", "", "",
		      Form("%f",center.X()), Form("%f",center.Y()),
		      Form("%f",center.Z()), "st") << endl;
      
    }
  }

  return ss.str();
}

//_____________________________________________________________________________
string TASTparGeo::PrintBodies( )
{
  stringstream outstr;
  outstr << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);
  string bodyname, regionname;

  if(TAGrecoManager::GetPar()->IncludeST()){
    outstr << "* ***Start Counter" << endl;

    stringstream ss;
    double zero = 0.;

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

    TVector3  center = fpFootGeo->GetSTCenter();
    TVector3  angle = fpFootGeo->GetSTAngles();
    fFrameSize.SetXYZ(7.,7.,1.);
    
    if(angle.Mag()!=0)
      outstr << "$start_transform st" << endl;

    outstr << setiosflags(ios::fixed) << setprecision(6);
    bodyname = "airstc";
    fvStBody.push_back(bodyname);
    outstr << "RPP " << bodyname << "   " <<
      center[0] - fSize[0]/2. - 0.2 << " " << center[0] + fSize[0]/2. + 0.2 << " " <<
      center[1] - fSize[1]/2. - 0.2 << " " << center[1] + fSize[1]/2. + 0.2 << " " <<
      center[2] - fFrameSize.Z() - 0.1 << " " << center[2] + fFrameSize.Z() + 0.1 << " " <<  endl;

    bodyname = "frame1";
    fvStBody.push_back(bodyname);
    outstr << "RPP " << bodyname << "   " <<
      center[0]-fSize[0]/2. << " " << center[0]+fSize[0]/2. << " " <<
      center[1]-fSize[1]/2. << " " << center[1]+fSize[1]/2. << " " <<
      center[2]-fSize[2]/2. - fFrameSize.Z() - 0.001 << " " <<
      center[2]-fSize[2]/2. - 0.001 << " " <<  endl;

    bodyname = "frame2";
    fvStBody.push_back(bodyname);
    outstr << "RPP " << bodyname << "   " <<
      center[0]-fSize[0]/2. << " " << center[0]+fSize[0]/2. << " " <<
      center[1]-fSize[1]/2. << " " << center[1]+fSize[1]/2. << " " <<
      center[2]+fSize[2]/2. + 0.001 << " " <<
      center[2]+fSize[2]/2. + fFrameSize.Z() + 0.001 << " " <<  endl;

    bodyname = "hole1";
    fvStBody.push_back(bodyname);
    outstr << "RPP " << bodyname << "    " <<
      center[0]-fSize[0]/2.+1. << " " << center[0]+fSize[0]/2.-1. << " " <<
      center[1]-fSize[1]/2.+1. << " " << center[1]+fSize[1]/2.-1. << " " <<
      center[2]-fSize[2]/2. - fFrameSize.Z() - 0.001 << " " <<
      center[2]-fSize[2]/2. - 0.001 << " " <<  endl;

    bodyname = "hole2";
    fvStBody.push_back(bodyname);
    outstr << "RPP " << bodyname << "    " <<
      center[0]-fSize[0]/2+1. << " " << center[0]+fSize[0]/2.-1 << " " <<
      center[1]-fSize[1]/2.+1. << " " << center[1]+fSize[1]/2.-1. << " " <<
      center[2]+fSize[2]/2. + 0.001 << " " <<
      center[2]+fSize[2]/2. + fFrameSize.Z() + 0.001 << " " <<  endl;
    
    bodyname = "stc";
    fvStBody.push_back(bodyname);
    outstr << "RPP " << bodyname << "      " <<
      center[0]-fSize[0]/2. << " " << center[0]+fSize[0]/2 << " " <<
      center[1]-fSize[1]/2. << " " << center[1]+fSize[1]/2 << " " <<
      center[2]-fSize[2]/2. - 0.001 << " " << center[2]+fSize[2]/2 + 0.001 << " " <<  endl;

    //Mylar that is 10\mum thick
    bodyname = "stcmyl1";
    fvStBody.push_back(bodyname);
    outstr << "XYP " << bodyname << "  " << center[2]-fSize[2]/2. <<  endl;
    //Mylar that is 10\mum thick
    bodyname = "stcmyl2";
    fvStBody.push_back(bodyname);
    outstr << "XYP " << bodyname << "  " << center[2]+fSize[2]/2. <<  endl;
    
    if(angle.Mag()!=0)
      outstr << "$end_transform" << endl;
  }

  return outstr.str();
}


//_____________________________________________________________________________
string TASTparGeo::PrintRegions()
{
  stringstream outstr;

  if(TAGrecoManager::GetPar()->IncludeST()){

    outstr << "* ***Start Counter" << endl;

    outstr << "STC          5 +stc -stcmyl1 +stcmyl2" << endl;
    outstr << "STCMYL1      5 +stc +stcmyl1" << endl;
    outstr << "STCMYL2      5 +stc -stcmyl2" << endl;
  }

  return outstr.str();
}

//_____________________________________________________________________________
string TASTparGeo::PrintPassiveRegions()
{
  stringstream outstr;

  if(TAGrecoManager::GetPar()->IncludeST()){

    outstr << "* ***Start Counter - Passive" << endl;

    outstr << "AIRSTC       5 +airstc -(frame1 -hole1 ) -(frame2 -hole2)  -stc" << endl;
    outstr << "FRAME1       5 +frame1 -hole1" << endl;
    outstr << "FRAME2       5 +frame2 -hole2" << endl;


  }

  return outstr.str();
}


//_____________________________________________________________________________
string TASTparGeo::PrintSubtractBodiesFromAir()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeST()){
    ss << "-airstc " << endl;;
  }
  
  return ss.str();
}

//_____________________________________________________________________________
string TASTparGeo::PrintAssignMaterial(TAGmaterials *Material)
{
  stringstream outstr;

  if(TAGrecoManager::GetPar()->IncludeST()){

    TString flkmat, flkmatFrame, flkmatFoil;  
    
    if (Material == NULL){
      TAGmaterials::Instance()->PrintMaterialFluka();
      flkmat = TAGmaterials::Instance()->GetFlukaMatName(fMaterial.Data());
      flkmatFrame = TAGmaterials::Instance()->GetFlukaMatName(fFrameMat.Data());
      flkmatFoil = TAGmaterials::Instance()->GetFlukaMatName(fFoilMat.Data());
    }
    else {
      flkmat = Material->GetFlukaMatName(fMaterial.Data());
      flkmatFrame = Material->GetFlukaMatName(fFrameMat.Data());
      flkmatFoil = Material->GetFlukaMatName(fFoilMat.Data());
    }
    bool magnetic = false;
    if(TAGrecoManager::GetPar()->IncludeDI())
      magnetic = true;
        
    //    outstr << PrintCard("ASSIGNMA", flkmat, "STC", "", "", Form("%d",magnetic), "", "") << endl;
    //    outstr << PrintCard("ASSIGNMA", "Mylar", "STCMYL1", "STCMYL2", "1.", Form("%d",magnetic), "", "") << endl;
    outstr << PrintCard("ASSIGNMA", flkmat, "STC", "", "", "", "", "") << endl;
    outstr << PrintCard("ASSIGNMA", "Mylar", "STCMYL1", "STCMYL2", "1.", "", "", "") << endl;
    outstr << PrintCard("ASSIGNMA", flkmatFrame, "FRAME1", "FRAME2", "", "", "", "") << endl;
    outstr << PrintCard("ASSIGNMA", "AIR", "AIRSTC", "", "", "", "", "") << endl;
        
  }
  
  return outstr.str();
}

