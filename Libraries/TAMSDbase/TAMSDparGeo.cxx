/*!
  \file TAMSDparGeo.cxx
  \brief   Implementation of TAMSDparGeo.
*/

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TROOT.h"

#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAMSDparGeo.hxx"
#include "TAGroot.hxx"

const TString TAMSDparGeo::fgkBaseName    = "MSD";
const Int_t   TAMSDparGeo::fgkDefSensorsN = 6;

/*!
 \class TAMSDparGeo
 \brief Geometry parameters for MSD **
 */

//! Class imp
ClassImp(TAMSDparGeo);

//_____________________________________________________________________________
//! Standard constructor
TAMSDparGeo::TAMSDparGeo()
: TAVTbaseParGeo()
{
   fkDefaultGeoName = "./geomaps/TAMSDdetector.geo";
}

//_____________________________________________________________________________
//! Destructor
TAMSDparGeo::~TAMSDparGeo()
{
}


//_____________________________________________________________________________
//! Define material
void TAMSDparGeo::DefineMaterial()
{
    TAVTbaseParGeo::DefineMaterial();

    // Strip material
    TGeoMaterial* pixMat = TAGmaterials::Instance()->CreateMaterial(fPixMat, fPixMatDensity);
    if(FootDebugLevel(1)) {
        printf("strips material:\n");
        pixMat->Print();
    }
    // Metal material
    TGeoMaterial* metMat = TAGmaterials::Instance()->CreateMaterial(fMetalMat, fMetalDensity);
    if(FootDebugLevel(1)) {
        printf("metal material:\n");
        metMat->Print();
    }
}

//_____________________________________________________________________
//! From file
//!
//! \param[in] name input file
Bool_t TAMSDparGeo::FromFile(const TString& name)
{
   cout << setiosflags(ios::fixed) << setprecision(5);

   // simple file reading, waiting for real config file
   TString nameExp;

   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;

   if (!Open(nameExp)) return false;

   Info("FromFile()", "Open file %s for geometry\n", name.Data());

   ReadItem(fSensorsN);
   if(FootDebugLevel(1))
      cout << endl << "Sensors number "<< fSensorsN << endl;

   ReadStrings(fTypeName);
   if(FootDebugLevel(1))
      cout  << endl << "  Type Name : "<< fTypeName.Data() << endl;

   ReadItem(fTypeNumber);
   if(FootDebugLevel(1))
      cout  << endl << "  Type Number : "<< fTypeNumber << endl;

   ReadItem(fStripsN);
   if(FootDebugLevel(1))
      cout  << "  Number of strip: "<< fStripsN << endl;

   ReadItem(fPitch);
   if(FootDebugLevel(1))
      cout  << "  Pitch for strip: "<< fPitch << endl;

   ReadVector3(fTotalSize);
   if(FootDebugLevel(1))
      cout  << "  Total size of sensor:     "<< fTotalSize.X() << " " <<  fTotalSize.Y() << " "
      <<  fTotalSize.Z()  << endl;

   ReadVector3(fEpiSize);
   if(FootDebugLevel(1))
      cout  << endl << "  Sensitive size of sensor: "<< fEpiSize.X() << " " <<  fEpiSize.Y() << " "
      <<  fEpiSize.Z()  << endl;

   ReadVector3(fEpiOffset);
   if(FootDebugLevel(1))
      cout  << endl << "  Offset of sensitive area of sensor: "<< fEpiOffset.X() << " " <<  fEpiOffset.Y() << " "
      <<  fEpiOffset.Z()  << endl;

   ReadStrings(fEpiMat);
   if(FootDebugLevel(1))
      cout   << "  Sensitive material: "<< fEpiMat.Data() << endl;

   ReadItem(fEpiMatDensity);
   if(FootDebugLevel(1))
      cout  << "  Sensitive material density:  "<< fEpiMatDensity << endl;

    ReadItem(fPixThickness);
    if(FootDebugLevel(1))
       cout  << endl << "  Strip thickness: "<< fPixThickness << endl;

   ReadStrings(fPixMat);
   if(FootDebugLevel(1))
      cout   << "  Strip material: "<< fPixMat.Data() << endl;

   ReadItem(fPixMatDensity);
   if(FootDebugLevel(1))
     cout  << "  Strip material density:  "<< fPixMatDensity << endl;

   ReadItem(fMetalThickness);
   if(FootDebugLevel(1))
     cout  << endl << "  Metal thickness: "<< fMetalThickness << endl;

   ReadStrings(fMetalMat);
   if(FootDebugLevel(1))
     cout   << "  Metal material: "<< fMetalMat.Data() << endl;

   ReadItem(fMetalDensity);
   if(FootDebugLevel(1))
     cout  << "  Metal density:  "<< fMetalDensity << endl;

   ReadItem(fSupportInfo);
   if(FootDebugLevel(1))
      cout  << "  Info flag for support:  "<< fSupportInfo << endl;

   // read info for support only for IT
   if (fSupportInfo)
      ReadSupportInfo();

   if(FootDebugLevel(1))
      cout << endl << "Reading Sensor Parameters " << endl;

   SetupMatrices(fSensorsN);

   for (Int_t p = 0; p < fSensorsN; p++) { // Loop on each plane

      // read sensor index
      ReadItem(fSensorParameter[p].SensorIdx);
      if(FootDebugLevel(1))
         cout << endl << " - Parameters of Sensor " <<  fSensorParameter[p].SensorIdx << endl;

      // read sensor index
      ReadItem(fSensorParameter[p].TypeIdx);
      if(FootDebugLevel(1))
         cout  << "   Type of Sensor: " <<  fSensorParameter[p].TypeIdx << endl;

      // read sensor position
      ReadVector3(fSensorParameter[p].Position);
      if(FootDebugLevel(1))
         cout << "   Position: "
         << fSensorParameter[p].Position[0] << " " << fSensorParameter[p].Position[1] << " " << fSensorParameter[p].Position[2] << endl;

      // fill map
      fSensorMap[fSensorParameter[p].Position[2]].push_back(fSensorParameter[p].SensorIdx);
      
      // read sensor angles
      ReadVector3(fSensorParameter[p].Tilt);
      if(FootDebugLevel(1))
         cout  << "   Tilt: "
		       << fSensorParameter[p].Tilt[0] << " " << fSensorParameter[p].Tilt[1] << " " << fSensorParameter[p].Tilt[2] << endl;

      // read alignment
      ReadItem(fSensorParameter[p].AlignmentU);
      ReadItem(fSensorParameter[p].AlignmentV);
      if(FootDebugLevel(1))
         cout  << "   Alignment: " <<  fSensorParameter[p].AlignmentU << " " << fSensorParameter[p].AlignmentV << endl;

      // read tiltW
      ReadItem(fSensorParameter[p].TiltW);
      if(FootDebugLevel(1))
         cout  << "   Rotation tiltW: " << fSensorParameter[p].TiltW << endl;

      Float_t thetaX = fSensorParameter[p].Tilt[0];
      Float_t thetaY = fSensorParameter[p].Tilt[1];
      if (TMath::Nint(thetaY) == 180)
         fSensorParameter[p].IsReverseY = true;
      else
         fSensorParameter[p].IsReverseY = false;

      Float_t thetaZ = fSensorParameter[p].Tilt[2] - fSensorParameter[p].TiltW;
      TGeoRotation rot;
      rot.RotateX(thetaX);
      rot.RotateY(thetaY);
      rot.RotateZ(thetaZ);

      Float_t transX = fSensorParameter[p].Position[0] - fSensorParameter[p].AlignmentU + fSensorParameter[p].TiltW*TMath::DegToRad()*fSensorParameter[p].Position[1];
      Float_t transY = fSensorParameter[p].Position[1] - fSensorParameter[p].AlignmentV - fSensorParameter[p].TiltW*TMath::DegToRad()*fSensorParameter[p].Position[0];
      Float_t transZ = fSensorParameter[p].Position[2];

      TGeoTranslation trans(transX, transY, transZ);

      TGeoHMatrix  transfo;
      transfo  = trans;
      transfo *= rot;
      AddTransMatrix(new TGeoHMatrix(transfo), fSensorParameter[p].SensorIdx-1);

      // change to rad
      fSensorParameter[p].Tilt[0] = fSensorParameter[p].Tilt[0]*TMath::DegToRad();
      fSensorParameter[p].Tilt[1] = fSensorParameter[p].Tilt[1]*TMath::DegToRad();
      fSensorParameter[p].Tilt[2] = fSensorParameter[p].Tilt[2]*TMath::DegToRad();
      fSensorParameter[p].TiltW   = fSensorParameter[p].TiltW*TMath::DegToRad();
   }
   // Close file
   Close();

   // Define materials
   DefineMaterial();

   return true;
}


//_____________________________________________________________________________
//! Get strip position
//!
//! \param[in] strip strip number
Float_t TAMSDparGeo::GetPosition(Int_t strip) const
{
  Float_t x = (Float_t(2*strip - fStripsN + 1) * fPitch)/2.;
  return  x;
}

//_____________________________________________________________________________
//! Add module in MSD for Root geometry
//!
//! \param[in] basemoduleName module volume base name
//! \param[in] msdName MSD volume name
TGeoVolume* TAMSDparGeo::AddModule(const char* basemoduleName, const char *msdName)
{
   // create MSD module
   const Char_t* matName = fEpiMat.Data();
   TGeoMedium*   medMod = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);

   TGeoBBox *box = new TGeoBBox(Form("%s_Box",basemoduleName), fEpiSize.X()/2., fEpiSize.Y()/2.,
                                fTotalSize.Z()/2.);

   TGeoVolume *msdMod = new TGeoVolume(Form("%s_MSD",basemoduleName),box, medMod);
   msdMod->SetLineColor(kViolet-5);
   msdMod->SetTransparency(TAGgeoTrafo::GetDefaultTransp());

   return msdMod;
}

//_____________________________________________________________________________
//! Build MSD in Root geometry
//!
//! \param[in] basemoduleName  plsnr volume name
//! \param[in] msdName MSD base name
TGeoVolume* TAMSDparGeo::BuildMicroStripDetector(const char* basemoduleName, const char *msdName)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }

   // define box
   DefineMaxMinDimension();

   TGeoVolume* msd = gGeoManager->FindVolumeFast(msdName);
   if ( msd == 0x0 ) {
      TGeoMaterial* mat = (TGeoMaterial *)gGeoManager->GetListOfMaterials()->FindObject("AIR");
      TGeoMedium*   med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
      msd = gGeoManager->MakeBox(msdName, med, fSizeBox.X()/2., fSizeBox.Y()/2., fSizeBox.Z()/2.); // volume corresponding to msd
   }

   TGeoVolume* msdMod = 0x0;

   for(Int_t iSensor = 0; iSensor < fSensorsN; iSensor++) {

      TGeoCombiTrans* hm = GetCombiTransfo(iSensor);
      msdMod = AddModule(Form("%s%d",basemoduleName, iSensor), msdName);

      msd->AddNode(msdMod, iSensor, hm);
   }
   return msd;
}

//_____________________________________________________________________________
//! Print Fluka parameters
string TAMSDparGeo::PrintParameters()
{
  stringstream outstr;
  outstr << setiosflags(ios::fixed) << setprecision(5);

  if(TAGrecoManager::GetPar()->IncludeMSD()){

    string precision = "D+00";

    outstr << "c     MSD PARAMETERS " << endl;
    outstr << endl;

    map<string, int> intp;
    intp["nlayMSD"] = fSensorsN;
    for (auto i : intp){
      outstr << "      integer " << i.first << endl;
      outstr << "      parameter (" << i.first << " = " << i.second << ")" << endl;
    }

    outstr << endl;
  }

  return outstr.str();
}


//_____________________________________________________________________________
//! Print Fluka rotations
string TAMSDparGeo::PrintRotations()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeMSD()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    
    TVector3  center = fpFootGeo->GetMSDCenter();
    TVector3  angle = fpFootGeo->GetMSDAngles(); //invert the angles to take into account the FLUKA convention;
    angle *= -1;
     
    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

      //check if sensor or detector have a tilt
      if (fSensorParameter[iSens].Tilt.Mag()!=0 || angle.Mag()!=0){

	//put the sensor in local coord before the rotation
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",-center.X()),
			Form("%f",-center.Y()),
			Form("%f",-center.Z()),
			Form("msd_%d",iSens) ) << endl;

	//check if sensor has a tilt
	if (fSensorParameter[iSens].Tilt.Mag()!=0){
	  
	  // put the sensor in 0,0,0 before the sensor's rot
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",-GetSensorPosition(iSens).X()),
			  Form("%f",-GetSensorPosition(iSens).Y()),
			  Form("%f",-GetSensorPosition(iSens).Z()),
			  Form("msd_%d",iSens) ) << endl;
	  //rot around x
	  if(fSensorParameter[iSens].Tilt[0]!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "",
			    Form("%f",fSensorParameter[iSens].Tilt[0]*TMath::RadToDeg()),
			    "", "", "", Form("msd_%d",iSens) ) << endl;
	  }
	  //rot around y      
	  if(fSensorParameter[iSens].Tilt[1]!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "",
			    Form("%f",fSensorParameter[iSens].Tilt[1]*TMath::RadToDeg()),
			    "", "", "", Form("msd_%d",iSens) ) << endl;
	  }
	  //rot around z
	  if(fSensorParameter[iSens].Tilt[2]!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "",
			    Form("%f",fSensorParameter[iSens].Tilt[2]*TMath::RadToDeg()),
			    "", "", "", Form("msd_%d",iSens) ) << endl;
	  }
	  
	  //put back the sensor into its position in local coord
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",GetSensorPosition(iSens).X()),
			  Form("%f",GetSensorPosition(iSens).Y()),
			  Form("%f",GetSensorPosition(iSens).Z()),
			  Form("msd_%d",iSens) ) << endl;
	}

	//check if detector has a tilt and then apply rot
	if(angle.Mag()!=0){
	  
	  if(angle.X()!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",angle.X()),"", "",
			    "", Form("msd_%d",iSens)) << endl;
	  }
	  if(angle.Y()!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",angle.Y()),"", "",
			    "", Form("msd_%d",iSens)) << endl;
	  }
	  if(angle.Z()!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",angle.Z()),"", "",
			    "", Form("msd_%d",iSens)) << endl;
	  }
	}
      
	//put back the detector in global coord
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",center.X()), Form("%f",center.Y()),
			Form("%f",center.Z()), Form("msd_%d",iSens)) << endl;
       
      }
    }
  }
  
  return ss.str();

}

//_____________________________________________________________________________
//! Print Fluka bodies
string TAMSDparGeo::PrintBodies()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeMSD()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

    TVector3  center = fpFootGeo->GetMSDCenter();
    TVector3  angle = fpFootGeo->GetMSDAngles();
    
    TVector3 posMet, posStrip, posMod;
    string bodyname, regionname;

    ss << "* ***MSD bodies" << endl;

    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

      if(fSensorParameter[iSens].Tilt.Mag()!=0 || angle.Mag()!=0)
	ss << "$start_transform " << Form("msd_%d",iSens) << endl;
      
      //strip layer
      bodyname = Form("msds%d",iSens);
      regionname = Form("MSDS%d",iSens);
      if (iSens%2==0) {
	posStrip.SetXYZ( center.X() + GetSensorPosition(iSens).X(),
			 center.Y() + GetSensorPosition(iSens).Y(),
			 center.Z() + GetSensorPosition(iSens).Z() - fTotalSize.Z()/2. + fMetalThickness + fEpiSize.Z()/2. );
	ss <<  "RPP " << bodyname <<  "     "
	   << posStrip.x() - fEpiSize.X()/2. << " "
	   << posStrip.x() + fEpiSize.X()/2. << " "
	   << posStrip.y() - fEpiSize.Y()/2. << " "
	   << posStrip.y() + fEpiSize.Y()/2. << " "
	   << posStrip.z() - fEpiSize.Z()/2. << " "
	   << posStrip.z() + fEpiSize.Z()/2. << endl;
	fvStripBody.push_back(bodyname);
	fvStripRegion.push_back(regionname);

      //module
	bodyname = Form("msdp%d",iSens);
	regionname = Form("MSDP%d",iSens);
	posMod.SetXYZ( posStrip.X() + fEpiSize.X()/2. + fEpiOffset.X() - fTotalSize.X()/2.,
		       posStrip.Y() - fEpiSize.Y()/2. - fEpiOffset.Y() + fTotalSize.Y()/2.,
		       center.Z() + GetSensorPosition(iSens).Z() );
	ss <<  "RPP " << bodyname <<  "     "
	   << posMod.x() - fTotalSize.X()/2. << " "
	   << posMod.x() + fTotalSize.X()/2. << " "
	   << posMod.y() - fTotalSize.Y()/2. << " "
	   << posMod.y() + fTotalSize.Y()/2. << " "
	   << posMod.z() - fTotalSize.Z()/2. << " "
	   << posMod.z() + fTotalSize.Z()/2. << endl;
	fvModBody.push_back(bodyname);
	fvModRegion.push_back(regionname);

      //metal layer
	bodyname = Form("msdm%d",iSens);
	regionname = Form("MSDM%d",iSens);
	posMet.SetXYZ( posStrip.X(), posStrip.Y(), posStrip.Z() - fEpiSize.Z()/2. - fMetalThickness/2.);
	ss <<  "RPP " << bodyname <<  "     "
	   << posMet.x() - fEpiSize.X()/2. << " "
	   << posMet.x() + fEpiSize.X()/2. << " "
	   << posMet.y() - fEpiSize.Y()/2. << " "
	   << posMet.y() + fEpiSize.Y()/2. << " "
	   << posMet.z() - fMetalThickness/2. << " "
	   << posMet.z() + fMetalThickness/2. << endl;
	fvMetalBody.push_back(bodyname);
	fvMetalRegion.push_back(regionname);
      }	else {
	posStrip.SetXYZ( center.X() + GetSensorPosition(iSens).X(),
			 center.Y() + GetSensorPosition(iSens).Y(),
			 center.Z() + GetSensorPosition(iSens).Z() + fTotalSize.Z()/2. - fMetalThickness - fEpiSize.Z()/2. );
	ss <<  "RPP " << bodyname <<  "     "
	   << posStrip.x() - fEpiSize.X()/2. << " "
	   << posStrip.x() + fEpiSize.X()/2. << " "
	   << posStrip.y() - fEpiSize.Y()/2. << " "
	   << posStrip.y() + fEpiSize.Y()/2. << " "
	   << posStrip.z() - fEpiSize.Z()/2. << " "
	   << posStrip.z() + fEpiSize.Z()/2. << endl;
	fvStripBody.push_back(bodyname);
	fvStripRegion.push_back(regionname);

      //module
	bodyname = Form("msdp%d",iSens);
	regionname = Form("MSDP%d",iSens);
	posMod.SetXYZ( posStrip.X() + fEpiSize.X()/2. + fEpiOffset.X() - fTotalSize.X()/2.,
		       posStrip.Y() - fEpiSize.Y()/2. - fEpiOffset.Y() + fTotalSize.Y()/2.,
		       center.Z() + GetSensorPosition(iSens).Z() );
	ss <<  "RPP " << bodyname <<  "     "
	   << posMod.x() - fTotalSize.X()/2. << " "
	   << posMod.x() + fTotalSize.X()/2. << " "
	   << posMod.y() - fTotalSize.Y()/2. << " "
	   << posMod.y() + fTotalSize.Y()/2. << " "
	   << posMod.z() - fTotalSize.Z()/2. << " "
	   << posMod.z() + fTotalSize.Z()/2. << endl;
	fvModBody.push_back(bodyname);
	fvModRegion.push_back(regionname);

      //metal layer
	bodyname = Form("msdm%d",iSens);
	regionname = Form("MSDM%d",iSens);
	posMet.SetXYZ( posStrip.X(), posStrip.Y(), posStrip.Z() + fEpiSize.Z()/2. + fMetalThickness/2.);
	ss <<  "RPP " << bodyname <<  "     "
	   << posMet.x() - fEpiSize.X()/2. << " "
	   << posMet.x() + fEpiSize.X()/2. << " "
	   << posMet.y() - fEpiSize.Y()/2. << " "
	   << posMet.y() + fEpiSize.Y()/2. << " "
	   << posMet.z() - fMetalThickness/2. << " "
	   << posMet.z() + fMetalThickness/2. << endl;
	fvMetalBody.push_back(bodyname);
	fvMetalRegion.push_back(regionname);
	//
      }
      if(fSensorParameter[iSens].Tilt.Mag()!=0 || angle.Mag()!=0)
	ss << "$end_transform " << endl;

    }
  }

  return ss.str();
}

//_____________________________________________________________________________
//! Print Fluka regions
string TAMSDparGeo::PrintRegions()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeMSD()){

    string name;

    ss << "* ***MSD regions" << endl;

    for(int i=0; i<fvStripRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvStripRegion.at(i)
    	 << "5 " << fvStripBody.at(i) <<endl;
    }

    for(int i=0; i<fvModRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvModRegion.at(i)
	 << "5 " << fvModBody.at(i)
	 << " -" << fvStripBody.at(i) << " -" << fvMetalBody.at(i) <<endl;
    }

    for(int i=0; i<fvMetalRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << fvMetalRegion.at(i)
    	 << "5 " << fvMetalBody.at(i) <<endl;
    }

  }

  return ss.str();

}

//_____________________________________________________________________________
//! Get strip regions
//!
//! \param[in] n region number
Int_t TAMSDparGeo::GetRegStrip(Int_t n)
{
  TString regname;
  regname.Form("MSDS%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get module regions
//!
//! \param[in] n region number
Int_t TAMSDparGeo::GetRegModule(Int_t n)
{
  TString regname;
  regname.Form("MSDP%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get metal regions
//!
//! \param[in] n region number
Int_t TAMSDparGeo::GetRegMetal(Int_t n)
{
  TString regname;
  regname.Form("MSDM%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Print Fluka subtracted bodies from air
string TAMSDparGeo::PrintSubtractBodiesFromAir()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeMSD()){

    for(int i=0; i<fvModBody.size(); i++) {
      ss << " -" << fvModBody.at(i);
    }
    ss << endl;

  }

   return ss.str();
}

//_____________________________________________________________________________
//! Print Fluka assigned material
//!
//! \param[in] Material a given material
string TAMSDparGeo::PrintAssignMaterial(TAGmaterials* Material)
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeMSD()){

    TString flkmatMod, flkmatMetal, flkmatSupp;  
    
    if (Material == NULL){
      TAGmaterials::Instance()->PrintMaterialFluka();
      flkmatMod = TAGmaterials::Instance()->GetFlukaMatName(fEpiMat.Data());
      flkmatMetal = TAGmaterials::Instance()->GetFlukaMatName(fMetalMat.Data());
      flkmatSupp = TAGmaterials::Instance()->GetFlukaMatName(fPixMat.Data());
    }else{
      flkmatMod = Material->GetFlukaMatName(fEpiMat.Data());
      flkmatMetal = Material->GetFlukaMatName(fMetalMat.Data());
      flkmatSupp = Material->GetFlukaMatName(fPixMat.Data());
    }
        
    bool magnetic = false;
    if(TAGrecoManager::GetPar()->IncludeDI())
      magnetic = true;

    if (fvStripRegion.size()==0 || fvModRegion.size()==0 || fvMetalRegion.size()==0 )
      cout << "Error: MSD regions vector not correctly filled!"<<endl;
    
    ss << PrintCard("ASSIGNMA", flkmatMod, fvStripRegion.at(0), fvStripRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatMetal, fvMetalRegion.at(0), fvMetalRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatSupp, fvModRegion.at(0), fvModRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;

  }

  return ss.str();

}

string TAMSDparGeo::PrintMSDPhysics()
{
   stringstream str;
   str << PrintCard("STEPSIZE","0.000001","0.007",fvStripRegion.at(0),fvStripRegion.at(fvStripRegion.size()-1),"","","") << endl;
   str << PrintCard("STEPSIZE","0.000001","0.00005",fvMetalRegion.at(0),fvMetalRegion.at(fvMetalRegion.size()-1),"","","") << endl;
   str << PrintCard("STEPSIZE","0.000001","0.00005",fvModRegion.at(0),fvModRegion.at(fvModRegion.size()-1),"","","") << endl;
   return str.str();
}

