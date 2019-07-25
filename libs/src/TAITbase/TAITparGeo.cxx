/*!
  \file
  \version $Id: TAITparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TAITparGeo.
*/

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"

#include "GlobalPar.hxx"
#include "TAGmaterials.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAITparGeo.hxx"
#include "TAGroot.hxx"

//##############################################################################

const TString TAITparGeo::fgkBaseNameIt      = "IT";
const TString TAITparGeo::fgkDefParaNameIt   = "itGeo";

//_____________________________________________________________________________
TAITparGeo::TAITparGeo()
: TAVTparGeo()
{
   // Constructor
   fFlagIt = true;
   fLayersN = 2;
   fkDefaultGeoName = "./geomaps/TAITdetector.map";
}

//______________________________________________________________________________
TAITparGeo::~TAITparGeo()
{
   // Destructor
}

//_____________________________________________________________________________
void TAITparGeo::DefineMaterial()
{
   // material for M28
   TAVTparGeo::DefineMaterial();
   
   TGeoMaterial* mat = 0x0;
   TGeoMixture*  mix = 0x0;
   
   // Foam SiC+Air
   mix = TAGmaterials::Instance()->CreateMixture(fFoamMat, fFoamMatDensities, fFoamMatProp, fFoamMatDensity);
   if(FootDebugLevel(1)) {
      printf("Foam material:\n");
      mix->Print();
   }

   // Kapton C22_H10_N2_O5
   mat = TAGmaterials::Instance()->CreateMaterial(fKaptonMat, fKaptonMatDensity);
   if(FootDebugLevel(1)) {
      printf("Kapton material:\n");
      mat->Print();
   }

   // Epoxy C18_H19_O3
   mat = TAGmaterials::Instance()->CreateMaterial(fEpoxyMat, fEpoxyMatDensity);
   if(FootDebugLevel(1)) {
      printf("Epoxy material:\n");
      mat->Print();
   }
   
   // Aluminum
   mat = TAGmaterials::Instance()->CreateMaterial(fAlMat, fAlMatDensity);
   if(FootDebugLevel(1)) {
      printf("Aluminum material:\n");
      mat->Print();
   }
}

//_____________________________________________________________________________
void TAITparGeo::ReadSupportInfo()
{
   ReadVector3(fFoamSize);
   if(FootDebugLevel(1))
      cout  << endl << "  Foam size of support: "<< fFoamSize.X() << " " <<  fFoamSize.Y() << " "
      <<  fFoamSize.Z()  << endl;
   
   ReadStrings(fFoamMat);
   if(FootDebugLevel(1))
      cout   << "  Foam material: "<< fFoamMat.Data() << endl;
   
   ReadStrings(fFoamMatDensities);
   if(FootDebugLevel(1))
      cout  << "  Foam material component densities: "<< fFoamMatDensities.Data() << endl;
   
   ReadStrings(fFoamMatProp);
   if(FootDebugLevel(1))
      cout  << "  Foam material proportion: "<< fFoamMatProp.Data() << endl;
   
   ReadItem(fFoamMatDensity);
   if(FootDebugLevel(1))
      cout  << "  Foam material density:  "<< fFoamMatDensity << endl;

   
   ReadItem(fKaptonThickness);
   if(FootDebugLevel(1))
      cout  << endl << "  Kapton thickness: "<< fKaptonThickness << endl;
   
   ReadStrings(fKaptonMat);
   if(FootDebugLevel(1))
      cout   << "  Kapton material: "<< fKaptonMat.Data() << endl;
   
   ReadItem(fKaptonMatDensity);
   if(FootDebugLevel(1))
      cout  << "  Kapton material density:  "<< fKaptonMatDensity << endl;
   
   
   ReadItem(fEpoxyThickness);
   if(FootDebugLevel(1))
      cout  << endl << "  Epoxy thickness: "<< fEpoxyThickness << endl;
   
   ReadStrings(fEpoxyMat);
   if(FootDebugLevel(1))
      cout   << "  Epoxy material: "<< fEpoxyMat.Data() << endl;
   
   ReadItem(fEpoxyMatDensity);
   if(FootDebugLevel(1))
      cout  << "  Epoxy material density:  "<< fEpoxyMatDensity << endl;
   
   
   ReadItem(fAlThickness);
   if(FootDebugLevel(1))
      cout  << endl << "  Alunimum thickness: "<< fAlThickness << endl;
   
   ReadStrings(fAlMat);
   if(FootDebugLevel(1))
      cout   << "  Alunimum material: "<< fAlMat.Data() << endl;
   
   ReadItem(fAlMatDensity);
   if(FootDebugLevel(1))
      cout  << "  Alunimum material density:  "<< fAlMatDensity << endl;

}

//_____________________________________________________________________________
TGeoVolume* TAITparGeo::BuildInnerTracker(const char *itName, const char* basemoduleName)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // define box
   DefineMaxMinDimension();
   
   TGeoVolume* it = gGeoManager->FindVolumeFast(itName);
   if ( it == 0x0 ) {
      TGeoMaterial* mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject("AIR");
      TGeoMedium*   med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
      it = gGeoManager->MakeBox(itName,med,fSizeBox.X()/2.,fSizeBox.Y()/2.,fSizeBox.Z()/2.); // volume corresponding to vertex
   }
   
   TGeoVolume* itMod = 0x0;
   
   for(Int_t iSensor = 0; iSensor < GetNSensors(); iSensor++) {
      TGeoHMatrix* hm = GetTransfo(iSensor);
      itMod = AddModule(Form("%s%d",basemoduleName, iSensor), itName);
      
      TGeoHMatrix* transf = (TGeoHMatrix*)hm->Clone();
      double* trans = transf->GetTranslation();
      
      it->AddNode(itMod, iSensor, transf);
   }
   
   return it;
}
//_____________________________________________________________________________
/*
void TAITparGeo::PrintFluka()
{
   static Int_t count = 0;
   const Char_t* matName = fEpiMat.Data();
   
   TVector3 pos = GetSensorPosition(count);
   stringstream ss;
   ss << setiosflags(ios::fixed) << setprecision(6);
   ss <<  "RPP " << Form("ITRP%d", count) <<  "     "
   << pos.x() - fTotalSize.X()/2.  << " " << pos.x() +  fTotalSize.X()/2. << " "
   << pos.y() - fTotalSize.Y()/2.  << " " << pos.y() +  fTotalSize.Y()/2. << " "
   << pos.z() - fTotalSize.Z()/2.  << " " << pos.z() +  fTotalSize.Z()/2. << endl;
   
   m_bodyPrintOut[matName].push_back( ss.str() );
   
   m_regionName[matName].push_back(Form("itrp%d", count));
   m_bodyName[matName].push_back(Form("ITRP%d", count));
   m_bodyPrintOut[matName].push_back(Form("ITRP%d", count));
   m_bodyName[matName].push_back(Form("ITRP%d", count));
}
*/

//_____________________________________________________________________________
string TAITparGeo::PrintParameters()
{
  stringstream outstr;
  // outstr << setiosflags(ios::fixed) << setprecision(5);

  if(GlobalPar::GetPar()->IncludeInnerTracker()){
   
    string precision = "D+00";
   
    outstr << "c     INNER TRACKER PARAMETERS " << endl;
    outstr << endl;
   
    map<string, int> intp;
    intp["nsensITR"] = fSensorsN;
    for (auto i : intp){
      outstr << "      integer " << i.first << endl;
      outstr << "      parameter (" << i.first << " = " << i.second << ")" << endl;
      // outstr << endl;
    }
        
    outstr << endl;
  }
   
   return outstr.str();
}

//_____________________________________________________________________________
string TAITparGeo::PrintRotations()
{
  stringstream ss;

  if(GlobalPar::GetPar()->IncludeVertex()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    TVector3  fCenter = fpFootGeo->GetITCenter();
    TVector3  fAngle = fpFootGeo->GetITAngles();
    
    for(int iSens=0; iSens<GetNSensors(); iSens++) {

   
      //check if sensor or detector have a tilt
      if (fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0){

	//put the sensor in local coord before the rotation
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",-fCenter.X()),
			Form("%f",-fCenter.Y()),
			Form("%f",-fCenter.Z()),
			Form("it_%d",iSens) ) << endl;

	//check if sensor has a tilt
	if (fSensorParameter[iSens].Tilt.Mag()!=0){
	  
	  // put the sensor in 0,0,0 before the sensor's rot
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",-GetSensorPosition(iSens).X()),
			  Form("%f",-GetSensorPosition(iSens).Y()),
			  Form("%f",-GetSensorPosition(iSens).Z()),
			  Form("it_%d",iSens) ) << endl;
	  //rot around x
	  if(fSensorParameter[iSens].Tilt[0]!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "",
			    Form("%f",fSensorParameter[iSens].Tilt[0]*TMath::RadToDeg()),
			    "", "", "", Form("it_%d",iSens) ) << endl;
	  }
	  //rot around y      
	  if(fSensorParameter[iSens].Tilt[1]!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "",
			    Form("%f",fSensorParameter[iSens].Tilt[1]*TMath::RadToDeg()),
			    "", "", "", Form("it_%d",iSens) ) << endl;
	  }
	  //rot around z
	  if(fSensorParameter[iSens].Tilt[2]!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "",
			    Form("%f",fSensorParameter[iSens].Tilt[2]*TMath::RadToDeg()),
			    "", "", "", Form("it_%d",iSens) ) << endl;
	  }
	  
	  //put back the sensor into its position in local coord
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",GetSensorPosition(iSens).X()),
			  Form("%f",GetSensorPosition(iSens).Y()),
			  Form("%f",GetSensorPosition(iSens).Z()),
			  Form("it_%d",iSens) ) << endl;
	}
      }

      //check if detector has a tilt and then apply rot
      if(fAngle.Mag()!=0){
	  
	if(fAngle.X()!=0){
	  ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",fAngle.X()),"", "", 
	  		  "", Form("it_%d",iSens)) << endl;
	}
	if(fAngle.Y()!=0){
	  ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",fAngle.Y()),"", "", 
	  		  "", Form("it_%d",iSens)) << endl;
	}
	if(fAngle.Z()!=0){
	  ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",fAngle.Z()),"", "", 
	  		  "", Form("it_%d",iSens)) << endl;
	}
      }
      
      //put back the detector in global coord
      ss << PrintCard("ROT-DEFI", "", "", "",
		      Form("%f",fCenter.X()), Form("%f",fCenter.Y()),
		      Form("%f",fCenter.Z()), Form("it_%d",iSens)) << endl;

    }
  }
  
  return ss.str();

}

//_____________________________________________________________________________
string TAITparGeo::PrintBodies()
{

  stringstream ss;
  ss << setiosflags(ios::fixed) << setprecision(10);

  if(GlobalPar::GetPar()->IncludeInnerTracker()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3  fCenter = fpFootGeo->GetITCenter();
    TVector3  fAngle = fpFootGeo->GetITAngles();
    
    TVector3 posEpi, posPix, posMod;
    string bodyname, regionname;
    
    ss << "* ***Inner tracker bodies" << endl;  
    
    for(int iSens=0; iSens<GetNSensors(); iSens++) {

          
      if(fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0)
	ss << "$start_transform " << Form("it_%d",iSens) << endl;
              
      //epitaxial layer
      bodyname = Form("itre%d",iSens);
      regionname = Form("ITRE%02d",iSens);
      posEpi.SetXYZ( fCenter.X() + GetSensorPosition(iSens).X(),
		     fCenter.Y() + GetSensorPosition(iSens).Y(),
		     fCenter.Z() + GetSensorPosition(iSens).Z() - fTotalSize.Z()/2.
		     + fPixThickness + fEpiSize.Z()/2. );
      ss <<  "RPP " << bodyname <<  "     "
	 << posEpi.x() - fEpiSize.X()/2. << " "
	 << posEpi.x() + fEpiSize.X()/2. << " "
	 << posEpi.y() - fEpiSize.Y()/2. << " "
	 << posEpi.y() + fEpiSize.Y()/2. << " "
	 << posEpi.z() - fEpiSize.Z()/2. << " "
	 << posEpi.z() + fEpiSize.Z()/2. << endl;
      vEpiBody.push_back(bodyname);
      vEpiRegion.push_back(regionname);
    
      //module
      bodyname = Form("itrm%d",iSens);
      regionname = Form("ITRM%d",iSens);
      posMod.SetXYZ( posEpi.X() + fEpiSize.X()/2. + fEpiOffset.X() - fTotalSize.X()/2.,
		     posEpi.Y() - fEpiSize.Y()/2. - fEpiOffset.Y() + fTotalSize.Y()/2.,
		     fCenter.Z() + GetSensorPosition(iSens).Z() );
      ss <<  "RPP " << bodyname <<  "     "
	 << posMod.x() - fTotalSize.X()/2. << " "
	 << posMod.x() + fTotalSize.X()/2. << " "
	 << posMod.y() - fTotalSize.Y()/2. << " "
	 << posMod.y() + fTotalSize.Y()/2. << " "
	 << posMod.z() - fTotalSize.Z()/2. << " "
	 << posMod.z() + fTotalSize.Z()/2. << endl;
      vModBody.push_back(bodyname);
      vModRegion.push_back(regionname);
    
      //pixel layer
      bodyname = Form("itrp%d",iSens);
      regionname = Form("ITRP%d",iSens);
      posPix.SetXYZ( posEpi.X(), posEpi.Y(), posEpi.Z() - fEpiSize.Z()/2. - fPixThickness/2.);   
      ss <<  "RPP " << bodyname <<  "     "
	 << posPix.x() - fEpiSize.X()/2. << " "
	 << posPix.x() + fEpiSize.X()/2. << " "
	 << posPix.y() - fEpiSize.Y()/2. << " "
	 << posPix.y() + fEpiSize.Y()/2. << " "
	 << posPix.z() - fPixThickness/2. << " "
	 << posPix.z() + fPixThickness/2. << endl;
      vPixBody.push_back(bodyname);
      vPixRegion.push_back(regionname);
      
      if(fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0)
	ss << "$end_transform " << endl;
	  
    }
  }
  
    return ss.str();
}


//_____________________________________________________________________________
string TAITparGeo::PrintRegions()
{

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeInnerTracker()){

    string name;

    ss << "* ***Vertex regions" << endl;

    for(int i=0; i<vEpiRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << vEpiRegion.at(i)
    	 << "5 " << vEpiBody.at(i) <<endl;
    }

    for(int i=0; i<vModRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << vModRegion.at(i)
	 << "5 " << vModBody.at(i)
	 << " -" << vEpiBody.at(i) << " -" << vPixBody.at(i) <<endl;
    }

    for(int i=0; i<vPixRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << vPixRegion.at(i)
    	 << "5 " << vPixBody.at(i) <<endl;
    }

  }
  
  return ss.str();
  
}

//_____________________________________________________________________________
string TAITparGeo::PrintSubtractBodiesFromAir()
{
  
  stringstream ss;

  if(GlobalPar::GetPar()->IncludeInnerTracker()){

    for(int i=0; i<vModBody.size(); i++) {
      ss << " -" << vModBody.at(i);
      if ((i+1)%10==0) ss << endl;
    }
    ss << endl;

  }

   return ss.str();   
}

//_____________________________________________________________________________
string TAITparGeo::PrintAssignMaterial()
{

  stringstream ss;
  
  if(GlobalPar::GetPar()->IncludeInnerTracker()){

    string matMod = "SILICON";//fEpiMat.Data();
    if (fEpiMat.CompareTo("Si")!=0)
      cout << "ATTENTION in TAITparGeo PrintAssignMaterial: check the IT material"<<endl;
    // matMod[1] =toupper(matMod[1]);
    // const Char_t* matMod = fEpiMat.Data();
    const Char_t* matPix = fPixMat.Data();
    bool magnetic = false;
    if(GlobalPar::GetPar()->IncludeDI())
      magnetic = true;
    
    if (vEpiRegion.size()==0 || vModRegion.size()==0 || vPixRegion.size()==0 )
      cout << "Error: IT regions vector not correctly filled!"<<endl;
    
    ss << PrintCard("ASSIGNMA", matMod, vEpiRegion.at(0), vEpiRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", matMod, vModRegion.at(0), vModRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", matPix, vPixRegion.at(0), vPixRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;

  }

  return ss.str();

}

