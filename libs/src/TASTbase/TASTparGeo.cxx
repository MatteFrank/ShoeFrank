/*!
  \file
  \version $Id: TASTparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
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
#include "TAGmaterials.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TASTparGeo TASTparGeo.hxx "TASTparGeo.hxx"
  \brief Map and Geometry parameters for the start counter. **
*/

ClassImp(TASTparGeo);

const TString TASTparGeo::fgkBaseName    = "ST";
const TString TASTparGeo::fgkDefParaName = "trGeo";

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTparGeo::TASTparGeo()
 : TAGparTools(),
   fSize(),
   fMaterial(""),
   fDensity(0.)
{
  fkDefaultGeoName = "./geomaps/TASTdetector.map";
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
  if (fDebugLevel) {
      printf("Start counter material:\n");
      mat->Print();
   }
}

//______________________________________________________________________________
Bool_t TASTparGeo::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;

   //The center is taken from the global setup of the experiment.
   ReadVector3(fSize);
   if(fDebugLevel)
      cout  << "  Size: "
      << Form("%f %f %f", fSize[0], fSize[1], fSize[2]) << endl;
   
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

  return;
}
//_____________________________________________________________________________
TGeoVolume* TASTparGeo::BuildStartCounter(const char *stName )
{
   // create counter
   const Char_t* matName = fMaterial.Data();
   TGeoMedium*  medST = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   TGeoBBox* box = new TGeoBBox(stName, fSize[0]/2., fSize[1]/2., fSize[2]/2.);
   TGeoVolume *start = new TGeoVolume(Form("%s_Mod",stName), box, medST);

   start->SetVisibility(true);
   start->SetLineColor(kRed-5);
   start->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   return start;
}


//_____________________________________________________________________________
string TASTparGeo::PrintRotations()
{
  stringstream ss;

  if(GlobalPar::GetPar()->IncludeST()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3 fCenter = fpFootGeo->GetSTCenter();
    TVector3  fAngle = fpFootGeo->GetSTAngles();
    
    if(fAngle.Mag()!=0){
	  
      ss << PrintCard("ROT-DEFI", "300.", "", "",
		      Form("%f",-fCenter.X()), Form("%f",-fCenter.Y()),
		      Form("%f",-fCenter.Z()), "st") << endl;
      if(fAngle.X()!=0)
	ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",fAngle.X()),"", "", "", "st") << endl;
      if(fAngle.Y()!=0)
	ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",fAngle.Y()),"", "", "", "st") << endl;
      if(fAngle.Z()!=0)
	ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",fAngle.Z()),"", "", "", "st") << endl;
      ss << PrintCard("ROT-DEFI", "300.", "", "",
		      Form("%f",fCenter.X()), Form("%f",fCenter.Y()),
		      Form("%f",fCenter.Z()), "st") << endl;
      
    }
  }

  return ss.str();

}


//_____________________________________________________________________________
string TASTparGeo::PrintBodies( ) {
  
  stringstream outstr;

  if(GlobalPar::GetPar()->IncludeST()){
    outstr << "* ***Start Counter" << endl;

    stringstream ss;
    double zero = 0.;

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

    TVector3  fCenter = fpFootGeo->GetSTCenter();
    TVector3  fAngle = fpFootGeo->GetSTAngles();

    
    if(fAngle.Mag()!=0)
      outstr << "$start_transform st" << endl;

    outstr << setiosflags(ios::fixed) << setprecision(6);
    outstr << "RPP stc     "  << fCenter[0]-fSize[0]/2. << " " << fCenter[0]+fSize[0]/2 << " " <<
      fCenter[1]-fSize[1]/2. << " " << fCenter[1]+fSize[1]/2 << " " <<
      fCenter[2]-fSize[2]/2. << " " << fCenter[2]+fSize[2]/2 << " " <<  endl;

    //Mylar that is 10\mum thick
    outstr << "XYP stcmyl1    "  << fCenter[2]-fSize[2]/2. - 0.001<<  endl;
    //Mylar that is 10\mum thick
    outstr << "XYP stcmyl2    "  << fCenter[2]+fSize[2]/2. + 0.001<<  endl;
    
    if(fAngle.Mag()!=0)
      outstr << "$end_transform" << endl;
  }

  return outstr.str();
}


//_____________________________________________________________________________
string TASTparGeo::PrintRegions() {
  
  stringstream outstr;

  if(GlobalPar::GetPar()->IncludeST()){

    outstr << "* ***Start Counter" << endl;

    outstr << "STC          5 +stc -stcmyl1 +stcmyl2" << endl;
    outstr << "STCMYL1      5 +stc +stcmyl1" << endl;
    outstr << "STCMYL2      5 +stc -stcmyl2" << endl;

  }

  return outstr.str();
}


//_____________________________________________________________________________
string TASTparGeo::PrintSubtractBodiesFromAir() {

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeST()){

    ss << "-stc " << endl;;

  }
  
  return ss.str();

}

//_____________________________________________________________________________
string TASTparGeo::PrintAssignMaterial() {

  stringstream outstr;

  if(GlobalPar::GetPar()->IncludeST()){

    bool magnetic = false;
    if(GlobalPar::GetPar()->IncludeDI())
      magnetic = true;
  
    const Char_t* matName = fMaterial.Data();

        
    outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	   << setw(10) << setfill(' ') << std::right << matName
	   << setw(10) << setfill(' ') << std::right << "STC"
	   << setw(10) << setfill(' ') << std::right << ""
	   << setw(10) << setfill(' ') << std::right << ""
	   << setw(10) << setfill(' ') << std::right << magnetic
	   << endl;
        
    outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	   << setw(10) << setfill(' ') << std::right << "Mylar"
	   << setw(10) << setfill(' ') << std::right << "STCMYL1"
	   << setw(10) << setfill(' ') << std::right << "STCMYL2"
	   << setw(10) << setfill(' ') << std::right << "1."
	   << setw(10) << setfill(' ') << std::right << magnetic
    	   << endl;
    
  }
  
  return outstr.str();
}

