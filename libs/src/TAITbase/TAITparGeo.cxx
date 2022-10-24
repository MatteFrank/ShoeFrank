/*!
  \file TAITparGeo.cxx
  \brief    Geometry parameters for ITR
*/

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"

#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAITparGeo.hxx"
#include "TAGroot.hxx"

/*!
 \class TAITparGeo
 \brief Geometry parameters for ITR
 */

//! Class Imp
ClassImp(TAITparGeo);

//##############################################################################

const TString TAITparGeo::fgkBaseNameIt      = "IT";
const TString TAITparGeo::fgkDefParaNameIt   = "itGeo";
const Int_t   TAITparGeo::fgkDefSensPerLayer = 8;
const Int_t   TAITparGeo::fgkDefSensPerBoard = 4;
const Int_t   TAITparGeo::fgkDefLayersN      = 2;

//_____________________________________________________________________________
//! Constructor
TAITparGeo::TAITparGeo()
: TAVTparGeo()
{
   fFlagIt          = true;
   fLayersN         = fgkDefLayersN;
   fSubLayersN      = fLayersN*2;
   fSensPerLayer    = fgkDefSensPerLayer;
   fSensPerBoard    = fgkDefSensPerBoard;
   fkDefaultGeoName = "./geomaps/TAITdetector.geo";
}

//______________________________________________________________________________
//! Destructor
TAITparGeo::~TAITparGeo()
{
}

//_____________________________________________________________________________
//! Define materials
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
//! Read support informations
void TAITparGeo::ReadSupportInfo()
{
   ReadVector3(fSupportSize);
   if(FootDebugLevel(1))
      cout  << endl << "  Size of support: "<< fSupportSize.X() << " " <<  fSupportSize.Y() << " "
      <<  fSupportSize.Z()  << endl;

   ReadVector3(fSupportOffset);
   if(FootDebugLevel(1))
      cout  << endl << "  Offset of support: "<< fSupportOffset.X() << " " <<  fSupportOffset.Y() << " "
      <<  fSupportOffset.Z()  << endl;

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
//! Build inner tracker in Root geometry
//!
//! \param[in] itName  inner tracker volume name
//! \param[in] basemoduleName module volume base name
//! \param[in] board flag for implemented passive mother board
//! \param[in] suport flag for implemented support
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

         Float_t sign = (iSup > 7) ? +1 : -1;
         Float_t x = sign*fSupportOffset[0]/2.;
         Float_t y = (hm1->GetTranslation()[1] + hm2->GetTranslation()[1])/2. + sign*fSupportOffset[1]/2.;
         Float_t z = (hm1->GetTranslation()[2] + hm2->GetTranslation()[2])/2.;
         TGeoTranslation trans(x, y, z);

         it->AddNode(itSupoort, iSup+100, new TGeoCombiTrans(trans, rot));
      }
   }

   return it;
}

//_____________________________________________________________________________
//! Build plume support in Root geometry
//!
//! \param[in] basemoduleName module volume base name
//! \param[in] vertexName vertex volume name
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

   // first kapton layer
   Float_t posZ = Get1stKaptonLayer();
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(kapton1, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(kapton1, count++, new TGeoTranslation(trans));

   // first layer aluminium
   posZ = Get1stAlLayer();
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(al, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(al, count++, new TGeoTranslation(trans));

   // Second kapton layer
   posZ = Get2ndKaptonLayer();
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(kapton2, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(kapton2, count++, new TGeoTranslation(trans));

   // second layer aluminium
   posZ = Get2ndAlLayer();
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(al, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(al, count++, new TGeoTranslation(trans));

   // Third kapton layer
   posZ = Get3rdKaptonLayer();
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(kapton1, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(kapton1, count++, new TGeoTranslation(trans));

   // layer of epoxy
   posZ = GetEpoxyLayer();
   trans.SetTranslation(0, 0, posZ);
   supportMod->AddNode(epoxy, count++, new TGeoTranslation(trans));

   trans.SetTranslation(0, 0, -posZ);
   supportMod->AddNode(epoxy, count++, new TGeoTranslation(trans));

   return supportMod;
}

//_____________________________________________________________________________
//! Get Z-position for a given layer
//!
//! \param[in] layer a given layer
Float_t TAITparGeo::GetPassiveLayerPosZ(Int_t layer)
{
   Float_t posZ = 0;

   // first kapton layer
   if (layer >= 1)
      posZ = fFoamThickness/2. + fKaptonThickness/2.;

   // first layer aluminium
   if (layer >= 2)
      posZ += fKaptonThickness/2. + fAlThickness/2.;

   // Second kapton layer
   if (layer >= 3)
      posZ += fAlThickness/2. + fKaptonThickness;

   // second layer aluminium
   if (layer >= 4)
      posZ += fKaptonThickness +  fAlThickness/2.;

   // Third kapton layer
   if (layer >= 5)
      posZ += fAlThickness/2. + fKaptonThickness/2.;

   // layer of epoxy
   if (layer == 6)
   posZ += fKaptonThickness/2. + fEpoxyThickness/2.;

   return posZ;
}

//_____________________________________________________________________________
//! Get Foam Layer position in Z-direction
Float_t TAITparGeo::GetFoamLayer()
{
   return GetPassiveLayerPosZ(0);
}

//_____________________________________________________________________________
//! Get 1st Kapton Layer position in Z-direction
Float_t TAITparGeo::Get1stKaptonLayer()
{
   return GetPassiveLayerPosZ(1);
}

//_____________________________________________________________________________
//! Get 1st Al Layer position in Z-direction
Float_t TAITparGeo::Get1stAlLayer()
{
   return GetPassiveLayerPosZ(2);
}

//_____________________________________________________________________________
//! Get 2nd Kapton Layer position in Z-direction
Float_t TAITparGeo::Get2ndKaptonLayer()
{
   return GetPassiveLayerPosZ(3);
}

//_____________________________________________________________________________
//! Get 2nd Al Layer position in Z-direction
Float_t TAITparGeo::Get2ndAlLayer()
{
   return GetPassiveLayerPosZ(4);
}

//_____________________________________________________________________________
//! Get 3rd Kapton Layer position in Z-direction
Float_t TAITparGeo::Get3rdKaptonLayer()
{
   return GetPassiveLayerPosZ(5);
}

//_____________________________________________________________________________
//! Get Epoxy Layer position in Z-direction
Float_t TAITparGeo::GetEpoxyLayer()
{
   return GetPassiveLayerPosZ(6);
}

//_____________________________________________________________________________
//! Print Fluka parameters
string TAITparGeo::PrintParameters()
{
  stringstream outstr;
  // outstr << setiosflags(ios::fixed) << setprecision(5);

  if(TAGrecoManager::GetPar()->IncludeIT()){

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

     // add something for support ??
  }

   return outstr.str();
}

//_____________________________________________________________________________
//! Print Fluka rotations
string TAITparGeo::PrintRotations()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeIT()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    TVector3  fCenter = fpFootGeo->GetITCenter();
    TVector3  fAngle = fpFootGeo->GetITAngles()*(-1.,-1.,-1.); //invert the angles to take into account the FLUKA convention;

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
    // Only global detector rotations for support
    if(fAngle.Mag()!=0){
    //put the passive layers in local coord before the rotation
  	ss << PrintCard("ROT-DEFI", "", "", "",
  			Form("%f",-fCenter.X()),
  			Form("%f",-fCenter.Y()),
  			Form("%f",-fCenter.Z()),
  			"itpass" ) << endl;

    if(fAngle.X()!=0){
      ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",fAngle.X()),"", "",
      "", "itpass") << endl;
    }
    if(fAngle.Y()!=0){
      ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",fAngle.Y()),"", "",
      "", "itpass") << endl;
    }
    if(fAngle.Z()!=0){
      ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",fAngle.Z()),"", "",
      "", "itpass") << endl;
    }
    //put back the detector in global coord
    ss << PrintCard("ROT-DEFI", "", "", "",
  			Form("%f",fCenter.X()), Form("%f",fCenter.Y()),
  			Form("%f",fCenter.Z()), "itpass") << endl;
    }
  }


  return ss.str();

}

//_____________________________________________________________________________
//! Print Fluka bodies
string TAITparGeo::PrintBodies()
{

   stringstream ss;
   ss << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

   if(TAGrecoManager::GetPar()->IncludeIT()){

      TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

      TVector3  fCenter = fpFootGeo->GetITCenter();
      TVector3  fAngle = fpFootGeo->GetITAngles();

      TVector3 posEpi, posPix, posMod;
      TVector3 posFoam, posKapton, posEpoxy, posAl;
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
         fvEpiBody.push_back(bodyname);
         fvEpiRegion.push_back(regionname);

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
         fvModBody.push_back(bodyname);
         fvModRegion.push_back(regionname);

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
         fvPixBody.push_back(bodyname);
         fvPixRegion.push_back(regionname);

         if(fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0)
            ss << "$end_transform " << endl;

      }
      ss << "* End of IT bodies" << endl;
       if(FootDebugLevel(2))
          cout << ss.str() << endl;
      // support
      if(fAngle.Mag()!=0)
        ss << "$start_transform " << "itpass" << endl;
      for(Int_t iSup = 0; iSup < GetSensorsN()/2; iSup+=4) {

	 //         string bodyname, regionname;

         Float_t sign = (iSup > 7) ? +1 : -1;

         // foam
         bodyname = Form("itrf%d",iSup);
         regionname = Form("ITRF%02d",iSup);
         posFoam.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                        fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                        fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2.);
         ss <<  "RPP " << bodyname <<  "     "
         << posFoam.x() - fSupportSize.X()/2. << " "
         << posFoam.x() + fSupportSize.X()/2. << " "
         << posFoam.y() - fSupportSize.Y()/2. << " "
         << posFoam.y() + fSupportSize.Y()/2. << " "
         << posFoam.z() - fFoamThickness/2. << " "
         << posFoam.z() + fFoamThickness/2. << endl;
         fvFoamBody.push_back(bodyname);
         fvFoamRegion.push_back(regionname);

         // Kapton
         Float_t posZ = Get1stKaptonLayer();

         bodyname = Form("itrk%d",iSup);
         regionname = Form("ITRK%02d",iSup);
         posKapton.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                        fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                        fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. + posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posKapton.x() - fSupportSize.X()/2. << " "
         << posKapton.x() + fSupportSize.X()/2. << " "
         << posKapton.y() - fSupportSize.Y()/2. << " "
         << posKapton.y() + fSupportSize.Y()/2. << " "
         << posKapton.z() - fKaptonThickness/2. << " "
         << posKapton.z() + fKaptonThickness/2. << endl;
         fvKaptonBody.push_back(bodyname);
         fvKaptonRegion.push_back(regionname);

         // negative side
         bodyname = Form("itrk%d",iSup+100);
         regionname = Form("ITRK%02d",iSup+100);
         posKapton.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                          fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                          fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. - posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posKapton.x() - fSupportSize.X()/2. << " "
         << posKapton.x() + fSupportSize.X()/2. << " "
         << posKapton.y() - fSupportSize.Y()/2. << " "
         << posKapton.y() + fSupportSize.Y()/2. << " "
         << posKapton.z() - fKaptonThickness/2. << " "
         << posKapton.z() + fKaptonThickness/2. << endl;
         fvKaptonBody.push_back(bodyname);
         fvKaptonRegion.push_back(regionname);

         // Al
         posZ = Get1stAlLayer();
         bodyname = Form("itra%d",iSup);
         regionname = Form("ITRA%02d",iSup);
         posAl.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                        fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                        fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. + posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posAl.x() - fSupportSize.X()/2. << " "
         << posAl.x() + fSupportSize.X()/2. << " "
         << posAl.y() - fSupportSize.Y()/2. << " "
         << posAl.y() + fSupportSize.Y()/2. << " "
         << posAl.z() - fAlThickness/2. << " "
         << posAl.z() + fAlThickness/2. << endl;
         fvAlBody.push_back(bodyname);
         fvAlRegion.push_back(regionname);

         // negative side
         bodyname = Form("itra%d",iSup+100);
         regionname = Form("ITRA%02d",iSup+100);
         posAl.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                      fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                      fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. - posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posAl.x() - fSupportSize.X()/2. << " "
         << posAl.x() + fSupportSize.X()/2. << " "
         << posAl.y() - fSupportSize.Y()/2. << " "
         << posAl.y() + fSupportSize.Y()/2. << " "
         << posAl.z() - fAlThickness/2. << " "
         << posAl.z() + fAlThickness/2. << endl;
         fvAlBody.push_back(bodyname);
         fvAlRegion.push_back(regionname);

         // Kapton 2nd (twice thickness)
         posZ = Get2ndKaptonLayer();
         bodyname = Form("itrk%d",iSup+2);
         regionname = Form("ITRK%02d",iSup+2);
         posKapton.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                          fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                          fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. + posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posKapton.x() - fSupportSize.X()/2. << " "
         << posKapton.x() + fSupportSize.X()/2. << " "
         << posKapton.y() - fSupportSize.Y()/2. << " "
         << posKapton.y() + fSupportSize.Y()/2. << " "
         << posKapton.z() - fKaptonThickness << " "
         << posKapton.z() + fKaptonThickness << endl;
         fvKaptonBody.push_back(bodyname);
         fvKaptonRegion.push_back(regionname);

         // negative side
         bodyname = Form("itrk%d",iSup+102);
         regionname = Form("ITRK%02d",iSup+102);
         posKapton.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                          fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                          fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. - posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posKapton.x() - fSupportSize.X()/2. << " "
         << posKapton.x() + fSupportSize.X()/2. << " "
         << posKapton.y() - fSupportSize.Y()/2. << " "
         << posKapton.y() + fSupportSize.Y()/2. << " "
         << posKapton.z() - fKaptonThickness << " "
         << posKapton.z() + fKaptonThickness << endl;
         fvKaptonBody.push_back(bodyname);
         fvKaptonRegion.push_back(regionname);


         // Al 2nd
         posZ = Get2ndAlLayer();
         bodyname = Form("itra%d",iSup+2);
         regionname = Form("ITRA%02d",iSup+2);
         posAl.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                      fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                      fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. + posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posAl.x() - fSupportSize.X()/2. << " "
         << posAl.x() + fSupportSize.X()/2. << " "
         << posAl.y() - fSupportSize.Y()/2. << " "
         << posAl.y() + fSupportSize.Y()/2. << " "
         << posAl.z() - fAlThickness/2. << " "
         << posAl.z() + fAlThickness/2. << endl;
         fvAlBody.push_back(bodyname);
         fvAlRegion.push_back(regionname);

         // negative side
         bodyname = Form("itra%d",iSup+102);
         regionname = Form("ITRA%02d",iSup+102);
         posAl.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                      fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                      fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. - posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posAl.x() - fSupportSize.X()/2. << " "
         << posAl.x() + fSupportSize.X()/2. << " "
         << posAl.y() - fSupportSize.Y()/2. << " "
         << posAl.y() + fSupportSize.Y()/2. << " "
         << posAl.z() - fAlThickness/2. << " "
         << posAl.z() + fAlThickness/2. << endl;
         fvAlBody.push_back(bodyname);
         fvAlRegion.push_back(regionname);

         // Kapton 3rd
         posZ = Get3rdKaptonLayer();
         bodyname = Form("itrk%d",iSup+3);
         regionname = Form("ITRK%02d",iSup+3);
         posKapton.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                          fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                          fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. + posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posKapton.x() - fSupportSize.X()/2. << " "
         << posKapton.x() + fSupportSize.X()/2. << " "
         << posKapton.y() - fSupportSize.Y()/2. << " "
         << posKapton.y() + fSupportSize.Y()/2. << " "
         << posKapton.z() - fKaptonThickness/2. << " "
         << posKapton.z() + fKaptonThickness/2. << endl;
         fvKaptonBody.push_back(bodyname);
         fvKaptonRegion.push_back(regionname);

         // negative side
         bodyname = Form("itrk%d",iSup+103);
         regionname = Form("ITRK%02d",iSup+103);
         posKapton.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                          fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                          fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. - posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posKapton.x() - fSupportSize.X()/2. << " "
         << posKapton.x() + fSupportSize.X()/2. << " "
         << posKapton.y() - fSupportSize.Y()/2. << " "
         << posKapton.y() + fSupportSize.Y()/2. << " "
         << posKapton.z() - fKaptonThickness/2. << " "
         << posKapton.z() + fKaptonThickness/2. << endl;
         fvKaptonBody.push_back(bodyname);
         fvKaptonRegion.push_back(regionname);

         // Epoxy
         posZ = GetEpoxyLayer();
         bodyname = Form("itry%d",iSup);
         regionname = Form("ITRY%02d",iSup);
         posEpoxy.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                          fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                          fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. + posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posEpoxy.x() - fSupportSize.X()/2. << " "
         << posEpoxy.x() + fSupportSize.X()/2. << " "
         << posEpoxy.y() - fSupportSize.Y()/2. << " "
         << posEpoxy.y() + fSupportSize.Y()/2. << " "
         << posEpoxy.z() - fEpoxyThickness/2. << " "
         << posEpoxy.z() + fEpoxyThickness/2. << endl;
         fvEpoxyBody.push_back(bodyname);
         fvEpoxyRegion.push_back(regionname);

         // negative side
         bodyname = Form("itry%d",iSup+100);
         regionname = Form("ITRY%02d",iSup+100);
         posEpoxy.SetXYZ(fCenter.X() + sign*fSupportOffset[0]/2.,
                          fCenter.Y() + (GetSensorPosition(iSup).Y() + GetSensorPosition(iSup+16).Y())/2. + sign*fSupportOffset[1]/2.,
                          fCenter.Z() + (GetSensorPosition(iSup).Z() + GetSensorPosition(iSup+16).Z())/2. - posZ);
         ss <<  "RPP " << bodyname <<  "     "
         << posEpoxy.x() - fSupportSize.X()/2. << " "
         << posEpoxy.x() + fSupportSize.X()/2. << " "
         << posEpoxy.y() - fSupportSize.Y()/2. << " "
         << posEpoxy.y() + fSupportSize.Y()/2. << " "
         << posEpoxy.z() - fEpoxyThickness/2. << " "
         << posEpoxy.z() + fEpoxyThickness/2. << endl;
         fvEpoxyBody.push_back(bodyname);
         fvEpoxyRegion.push_back(regionname);
      }
      if(fAngle.Mag()!=0)
        ss << "$end_transform " << endl;
      ss << "* End of IT Support bodies" << endl;
   }

   if(FootDebugLevel(2)) {
      cout << "Foam " << fvFoamRegion.size() << endl;
      for(int i=0; i<fvFoamRegion.size(); i++)
         cout << setw(13) << setfill( ' ' ) << std::left << fvFoamRegion.at(i) <<endl;

      cout << endl;
      cout << "Kapton " << fvKaptonRegion.size() << endl;
      for(int i=0; i<fvKaptonRegion.size(); i++)
         cout << setw(13) << setfill( ' ' ) << std::left << fvKaptonRegion.at(i) <<endl;

      cout << endl;
      cout << "Al " << fvAlRegion.size() << endl;
      for(int i=0; i<fvAlRegion.size(); i++)
         cout << setw(13) << setfill( ' ' ) << std::left << fvAlRegion.at(i) <<endl;

      cout << endl;
      cout << "Epoxy " << fvEpoxyRegion.size() << endl;
      for(int i=0; i<fvEpoxyRegion.size(); i++)
         cout << setw(13) << setfill( ' ' ) << std::left << fvEpoxyRegion.at(i) <<endl;
   }
   return ss.str();
}

//_____________________________________________________________________________
//! Print Fluka regions
string TAITparGeo::PrintRegions()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeIT()){

    string name;

    ss << "* ***Inner Tracker regions" << endl;

    for(int i=0; i<fvEpiRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvEpiRegion.at(i)
    	 << "5 " << fvEpiBody.at(i) <<endl;
    }

    for(int i=0; i<fvModRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvModRegion.at(i)
	 << "5 " << fvModBody.at(i)
	 << " -" << fvEpiBody.at(i) << " -" << fvPixBody.at(i) <<endl;
    }

    for(int i=0; i<fvPixRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvPixRegion.at(i)
    	 << "5 " << fvPixBody.at(i) <<endl;
    }

     // for support here ??
    for(int i=0; i<fvFoamRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvFoamRegion.at(i)
    	 << "5 " << fvFoamBody.at(i) <<endl;
    }

    for(int i=0; i<fvKaptonRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvKaptonRegion.at(i)
    	 << "5 " << fvKaptonBody.at(i) <<endl;
    }

    for(int i=0; i<fvAlRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvAlRegion.at(i)
    	 << "5 " << fvAlBody.at(i) <<endl;
    }

    for(int i=0; i<fvEpoxyRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvEpoxyRegion.at(i)
    	 << "5 " << fvEpoxyBody.at(i) <<endl;
    }

  }

  return ss.str();
}

//_____________________________________________________________________________
//! Get epitaxial region in Fluka for a given layer
//!
//! \param[in] n layer index
Int_t TAITparGeo::GetRegEpitaxial(Int_t n)
{
  TString regname;
  regname.Form("ITRE%02d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get module region in Fluka for a given layer
//!
//! \param[in] n layer index
Int_t TAITparGeo::GetRegModule(Int_t n)
{
  TString regname;
  regname.Form("ITRM%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get pixel region in Fluka for a given layer
//!
//! \param[in] n layer index
Int_t TAITparGeo::GetRegPixel(Int_t n)
{
  TString regname;
  regname.Form("ITRP%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get Foam region in Fluka for a given layer
//!
//! \param[in] n layer index
Int_t TAITparGeo::GetRegFoam(Int_t n)
{
  TString regname;
  regname.Form("ITRF%02d",n*4);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get Kapton region in Fluka for a given layer
//!
//! \param[in] n sensor index
//! \param[in] lay layer index
//! \param[in] side side index
Int_t TAITparGeo::GetRegKapton(Int_t n, Int_t lay, Bool_t side)
{
  TString regname;
  regname.Form("ITRK%02d",n*4 + ((side==true) ? 0:100) + ((lay==0) ? 0:(lay+1)) );
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get Al region in Fluka for a given layer
//!
//! \param[in] n sensor index
//! \param[in] lay layer index
//! \param[in] side side index
Int_t TAITparGeo::GetRegAluminum(Int_t n, Int_t lay, Bool_t side)
{
  TString regname;
  regname.Form("ITRA%2d",n*4 + ((side==true) ? 0:100) + lay*2 );
  return GetCrossReg(regname);
}


//_____________________________________________________________________________
//! Get Epoxy region in Fluka for a given layer
//!
//! \param[in] n sensor index
//! \param[in] side side index
Int_t TAITparGeo::GetRegEpoxy(Int_t n, Bool_t side)
{
  TString regname;
  regname.Form("ITRY%2d",n*4 + ((side==true) ? 0:100));
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Print subtracted bodies from air in Fluka
string TAITparGeo::PrintSubtractBodiesFromAir()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeIT()){

    for(int i=0; i<fvModBody.size(); i++) {
      ss << " -" << fvModBody.at(i);
      if ((i+1)%10==0) ss << endl;
    }
    ss << endl;

   // something to do for support ??

    for(int i=0; i<fvFoamBody.size(); i++) {
      ss << " -" << fvFoamBody.at(i);
      if ((i+1)%10==0) ss << endl;
    }
    ss << endl;

    for(int i=0; i<fvKaptonBody.size(); i++) {
      ss << " -" << fvKaptonBody.at(i);
      if ((i+1)%10==0) ss << endl;
    }
    ss << endl;

    for(int i=0; i<fvAlBody.size(); i++) {
      ss << " -" << fvAlBody.at(i);
      if ((i+1)%10==0) ss << endl;
    }
    ss << endl;

    for(int i=0; i<fvEpoxyBody.size(); i++) {
      ss << " -" << fvEpoxyBody.at(i);
      if ((i+1)%10==0) ss << endl;
    }
    ss << endl;

  }

   return ss.str();
}

//_____________________________________________________________________________
//! Print assigned material in Fluka for a given material
//!
//! \param[in] material Root material
string TAITparGeo::PrintAssignMaterial(TAGmaterials* material)
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeIT()){

    TString flkmatMod, flkmatPix;
    TString flkmatFoam, flkmatKapton, flkmatEpoxy, flkmatAl;


    if (material == NULL){
      TAGmaterials::Instance()->PrintMaterialFluka();
      flkmatMod    = TAGmaterials::Instance()->GetFlukaMatName(fEpiMat.Data());
      flkmatPix    = TAGmaterials::Instance()->GetFlukaMatName(fPixMat.Data());
      flkmatFoam   = TAGmaterials::Instance()->GetFlukaMatName(fFoamMat.Data());
      flkmatKapton = TAGmaterials::Instance()->GetFlukaMatName(fKaptonMat.Data());
      flkmatEpoxy  = TAGmaterials::Instance()->GetFlukaMatName(fEpoxyMat.Data());
      flkmatAl     = TAGmaterials::Instance()->GetFlukaMatName(fAlMat.Data());

    }else{
      flkmatMod    = material->GetFlukaMatName(fEpiMat.Data());
      flkmatPix    = material->GetFlukaMatName(fPixMat.Data());
      flkmatFoam   = material->GetFlukaMatName(fFoamMat.Data());
      flkmatKapton = material->GetFlukaMatName(fKaptonMat.Data());
      flkmatEpoxy  = material->GetFlukaMatName(fEpoxyMat.Data());
      flkmatAl     = material->GetFlukaMatName(fAlMat.Data());
    }

    bool magnetic = false;
    if(TAGrecoManager::GetPar()->IncludeDI())
      magnetic = true;

    if (fvEpiRegion.size()==0 || fvModRegion.size()==0 || fvPixRegion.size()==0 )
      cout << "Error: IT regions vector not correctly filled!"<<endl;

    ss << PrintCard("ASSIGNMA", flkmatMod, fvEpiRegion.at(0), fvEpiRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatMod, fvModRegion.at(0), fvModRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatPix, fvPixRegion.at(0), fvPixRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatFoam, fvFoamRegion.at(0), fvFoamRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatEpoxy, fvEpoxyRegion.at(0), fvEpoxyRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatKapton, fvKaptonRegion.at(0), fvKaptonRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatAl, fvAlRegion.at(0), fvAlRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;

     // for support what else ??
     if (fvFoamRegion.size()==0 || fvKaptonRegion.size()==0 || fvAlRegion.size()==0 || fvEpoxyRegion.size()==0)
        cout << "Error: IT regions vector not correctly filled!"<<endl;

  }

  return ss.str();
}
