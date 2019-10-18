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
   ReadVector3(fSupportSize);
   if(FootDebugLevel(1))
      cout  << endl << "  Foam size of support: "<< fSupportSize.X() << " " <<  fSupportSize.Y() << " "
      <<  fSupportSize.Z()  << endl;
   
   ReadItem(fFoamThickness);
   if(FootDebugLevel(1))
      cout  << endl << "  Foam thickness: "<< fFoamThickness << endl;
   
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
TGeoVolume* TAITparGeo::BuildInnerTracker(const char *itName, const char* basemoduleName, Bool_t board, Bool_t suport)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // define box
   DefineMaxMinDimension();
   
   TGeoVolume* it = gGeoManager->FindVolumeFast(itName);
   if ( it == 0x0 ) {
      TGeoMedium*   med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
      it = gGeoManager->MakeBox(itName,med,fSizeBox.X()/2.,fSizeBox.Y()/2.,fSizeBox.Z()/2.); // volume corresponding to IT
   }
   
   TGeoVolume* itMod = 0x0;
   TGeoVolume* itBoard = 0x0;
   
   if (board)
      itBoard = BuildBoard();

   for(Int_t iSensor = 0; iSensor < GetSensorsN(); iSensor++) {

      TGeoCombiTrans* hm = GetCombiTransfo(iSensor);
      itMod = AddModule(Form("%s%d",basemoduleName, iSensor), itName);
            
      it->AddNode(itMod, iSensor, hm);
      
      if (board) {
         Float_t signY = fSensorParameter[iSensor].IsReverseX ? +1. : -1.;
         Float_t signX = fSensorParameter[iSensor].IsReverseY ? +1. : -1.;
         
         const Double_t* mat = hm->GetRotationMatrix();
         const Double_t* dis = hm->GetTranslation();
         
         TGeoRotation rot;
         rot.SetMatrix(mat);
         
         TGeoTranslation trans;
         trans.SetTranslation(dis[0] + signX*fEpiOffset[0]/2., dis[1] + signY*fEpiOffset[1]/2., dis[2] - fEpiOffset[2]);
         
         it->AddNode(itBoard, iSensor+GetSensorsN(), new TGeoCombiTrans(trans, rot));
      }
   }
   
   if (suport) {
      TGeoVolume* itSupoort = BuildPlumeSupport("Support", itName);
   
      for(Int_t iSup = 0; iSup < GetSensorsN()/2; iSup+=4) {
         TGeoCombiTrans* hm1 = GetCombiTransfo(iSup);
         TGeoCombiTrans* hm2 = GetCombiTransfo(iSup+16);
         TGeoRotation rot;
         Float_t x = 0.;
         Float_t y = (hm1->GetTranslation()[1] + hm2->GetTranslation()[1])/2.;
         Float_t z = (hm1->GetTranslation()[2] + hm2->GetTranslation()[2])/2.;
         TGeoTranslation trans(x, y, z);
         
         it->AddNode(itSupoort, iSup+100, new TGeoCombiTrans(trans, rot));
      }
   }
   
   return it;
}

//_____________________________________________________________________________
TGeoVolume* TAITparGeo::BuildPlumeSupport(const char* basemoduleName, const char *vertexName)
{
   // create media
   const Char_t* matName = fFoamMat.Data();
   TGeoMedium*   medFoam = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   matName = fKaptonMat.Data();
   TGeoMedium*  medKapton = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   matName = fAlMat.Data();
   TGeoMedium*  medAl = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   matName = fEpoxyMat.Data();
   TGeoMedium*  medEpoxy = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   TGeoMedium* medAir = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");

   // Support volume
   TGeoBBox *box = new TGeoBBox(Form("%s_Box",basemoduleName), fSupportSize.X()/2., fSupportSize.Y()/2., fSupportSize.Z()/2.);
   TGeoVolume *supportMod = new TGeoVolume(Form("%s_Support",basemoduleName),box, medAir);
   supportMod->SetVisibility(0);
   supportMod->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   // Foam volume
   TGeoBBox *foamBox = new TGeoBBox("Foam", fSupportSize.X()/2., fSupportSize.Y()/2.,  fFoamThickness/2.);
   TGeoVolume *foam = new TGeoVolume("Foam",foamBox, medFoam);
   foam->SetLineColor(kAzure-2);
   foam->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   // Kapton1 volume
   TGeoBBox *kapton1Box = new TGeoBBox("Kapton1", fSupportSize.X()/2., fSupportSize.Y()/2.,  fKaptonThickness/2.);
   TGeoVolume *kapton1 = new TGeoVolume("Kapton1",kapton1Box, medKapton);
   kapton1->SetLineColor(kOrange-5);
   kapton1->SetTransparency(TAGgeoTrafo::GetDefaultTransp());

   // Kapton2 volume
   TGeoBBox *kapton2Box = new TGeoBBox("Kapton2", fSupportSize.X()/2., fSupportSize.Y()/2.,  fKaptonThickness); // double thickness
   TGeoVolume *kapton2 = new TGeoVolume("Kapton2",kapton2Box, medKapton);
   kapton2->SetLineColor(kOrange-5);
   kapton2->SetTransparency(TAGgeoTrafo::GetDefaultTransp());

   // Al volume
   TGeoBBox *alBox = new TGeoBBox("Al", fSupportSize.X()/2., fSupportSize.Y()/2.,  fAlThickness/2.);
   TGeoVolume *al = new TGeoVolume("Al",alBox, medAl);
   al->SetLineColor(kBlue-5);
   al->SetTransparency(TAGgeoTrafo::GetDefaultTransp());

   // Epoxy volume
   TGeoBBox *epoxyBox = new TGeoBBox("Epoxy", fSupportSize.X()/2., fSupportSize.Y()/2.,  fEpoxyThickness/2.);
   TGeoVolume *epoxy = new TGeoVolume("Epoxy", epoxyBox, medEpoxy);
   epoxy->SetLineColor(kViolet-5);
   epoxy->SetTransparency(TAGgeoTrafo::GetDefaultTransp());

   // placement
   Int_t count = 0;
   
   // foam
   TGeoTranslation trans(0, 0, 0);
   supportMod->AddNode(foam, count++, new TGeoTranslation(trans));

   Float_t posZ = fFoamThickness/2. + fKaptonThickness/2.;
   // first kapton layer
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(kapton1, count++, new TGeoTranslation(trans));
   
   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(kapton1, count++, new TGeoTranslation(trans));

   // first layer aluminium
   posZ += fKaptonThickness/2. + fAlThickness/2.;
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(al, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(al, count++, new TGeoTranslation(trans));

   // Second kapton layer
   posZ += fAlThickness/2. + fKaptonThickness;
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(kapton2, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(kapton2, count++, new TGeoTranslation(trans));

   // second layer aluminium
   posZ += fKaptonThickness +  fAlThickness/2.;
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(al, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(al, count++, new TGeoTranslation(trans));

   // Third kapton layer
   posZ += fAlThickness/2. + fKaptonThickness/2.;
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(kapton1, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(kapton1, count++, new TGeoTranslation(trans));

   // layer of epoxy
   posZ += fKaptonThickness/2. + fEpoxyThickness/2.;
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(epoxy, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(epoxy, count++, new TGeoTranslation(trans));

   return supportMod;
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

  if(GlobalPar::GetPar()->IncludeInnerTracker()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    TVector3  fCenter = fpFootGeo->GetITCenter();
    TVector3  fAngle = fpFootGeo->GetITAngles();
    
    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

   
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
    
    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

          
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
string TAITparGeo::PrintAssignMaterial(TAGmaterials *Material)
{

  stringstream ss;
  
  if(GlobalPar::GetPar()->IncludeInnerTracker()){

    TString flkmatMod, flkmatPix;  
    
    if (Material == NULL){
      TAGmaterials::Instance()->PrintMaterialFluka();
      flkmatMod = TAGmaterials::Instance()->GetFlukaMatName(fEpiMat.Data());
      flkmatPix = TAGmaterials::Instance()->GetFlukaMatName(fPixMat.Data());
    }else{
      flkmatMod = Material->GetFlukaMatName(fEpiMat.Data());
      flkmatPix = Material->GetFlukaMatName(fPixMat.Data());
    }

    bool magnetic = false;
    if(GlobalPar::GetPar()->IncludeDI())
      magnetic = true;
    
    if (vEpiRegion.size()==0 || vModRegion.size()==0 || vPixRegion.size()==0 )
      cout << "Error: IT regions vector not correctly filled!"<<endl;
    
    ss << PrintCard("ASSIGNMA", flkmatMod, vEpiRegion.at(0), vEpiRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatMod, vModRegion.at(0), vModRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatPix, vPixRegion.at(0), vPixRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;

  }

  return ss.str();

}

