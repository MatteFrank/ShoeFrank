/*!
  \file
  \version $Id: TAMSDparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TAMSDparGeo.
*/

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TROOT.h"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAMSDparGeo.hxx"
#include "TAGroot.hxx"

const TString TAMSDparGeo::fgkBaseName      = "MSD";
const TString TAMSDparGeo::fgkDefParaName   = "msdGeo";


ClassImp(TAMSDparGeo);

//_____________________________________________________________________________
TAMSDparGeo::TAMSDparGeo()
: TAVTbaseParGeo()
{
   fkDefaultGeoName = "./geomaps/TAMSDdetector.geo";
}

//_____________________________________________________________________________
TAMSDparGeo::~TAMSDparGeo()
{
}


//_____________________________________________________________________________
void TAMSDparGeo::DefineMaterial()
{
    TAVTbaseParGeo::DefineMaterial();

    // Pixel material
    TGeoMaterial* pixMat = TAGmaterials::Instance()->CreateMaterial(fPixMat, fPixMatDensity);
    if(FootDebugLevel(1)) {
        printf("pixels material:\n");
        pixMat->Print();
    }
    // Metal material
    TGeoMaterial* metMat = TAGmaterials::Instance()->CreateMaterial(fMetalMat, fMetalDensity);
    if(FootDebugLevel(1)) {
        printf("metal material:\n");
        metMat->Print();
    }
}

//_____________________________________________________________________________
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

   ReadItem(fSensorsN);
   if(FootDebugLevel(1))
      cout << endl << "Sensors number "<< fSensorsN << endl;

   ReadStrings(fTypeName);
   if(FootDebugLevel(1))
      cout  << endl << "  Type Name : "<< fTypeName.Data() << endl;

   ReadItem(fTypeNumber);
   if(FootDebugLevel(1))
      cout  << endl << "  Type Number : "<< fTypeNumber << endl;

   ReadItem(fStripN);
   if(FootDebugLevel(1))
      cout  << "  Number of strip: "<< fStripN << endl;

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
       cout  << endl << "  Pixel thickness: "<< fPixThickness << endl;

   ReadStrings(fPixMat);
   if(FootDebugLevel(1))
      cout   << "  Pixel material: "<< fPixMat.Data() << endl;

   ReadItem(fPixMatDensity);
   if(FootDebugLevel(1))
     cout  << "  Pixel material density:  "<< fPixMatDensity << endl;

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
Float_t TAMSDparGeo::GetPosition(Int_t strip) const
{
  Float_t x = (Float_t(2*strip - fStripN + 1) * fPitch)/2.;
  return  x;
}

//_____________________________________________________________________________
TGeoVolume* TAMSDparGeo::AddModule(const char* basemoduleName, const char *vertexName)
{
   // create MSD module
   const Char_t* matName = fEpiMat.Data();
   TGeoMedium*   medMod = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);

   TGeoBBox *box = new TGeoBBox(Form("%s_Box",basemoduleName), fEpiSize.X()/2., fEpiSize.Y()/2.,
                                fTotalSize.Z()/2.);

   TGeoVolume *msdMod = new TGeoVolume(Form("%s_MSD",basemoduleName),box, medMod);
   msdMod->SetLineColor(kViolet-5);
   msdMod->SetTransparency(TAGgeoTrafo::GetDefaultTransp());

   // if (GlobalPar::GetPar()->geoFLUKA())
   //    PrintFluka();

   return msdMod;
}

//_____________________________________________________________________________
TGeoVolume* TAMSDparGeo::BuildMultiStripDetector(const char* basemoduleName, const char *msdName)
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
string TAMSDparGeo::PrintParameters()
{
  stringstream outstr;
  outstr << setiosflags(ios::fixed) << setprecision(5);

  if(GlobalPar::GetPar()->IncludeMSD()){

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
string TAMSDparGeo::PrintRotations()
{
  stringstream ss;

  if(GlobalPar::GetPar()->IncludeMSD()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    
    TVector3  fCenter = fpFootGeo->GetMSDCenter();
    TVector3  fAngle = fpFootGeo->GetMSDAngles();
    
    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

      //check if sensor or detector have a tilt
      if (fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0){

	//put the sensor in local coord before the rotation
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",-fCenter.X()),
			Form("%f",-fCenter.Y()),
			Form("%f",-fCenter.Z()),
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
	if(fAngle.Mag()!=0){
	  
	  if(fAngle.X()!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",fAngle.X()),"", "", 
			    "", Form("msd_%d",iSens)) << endl;
	  }
	  if(fAngle.Y()!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",fAngle.Y()),"", "", 
			    "", Form("msd_%d",iSens)) << endl;
	  }
	  if(fAngle.Z()!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",fAngle.Z()),"", "", 
			    "", Form("msd_%d",iSens)) << endl;
	  }
	}
      
	//put back the detector in global coord
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",fCenter.X()), Form("%f",fCenter.Y()),
			Form("%f",fCenter.Z()), Form("msd_%d",iSens)) << endl;
       
      }
    }
  }
  
  return ss.str();

}


//_____________________________________________________________________________
string TAMSDparGeo::PrintBodies()
{

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeMSD()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

    TVector3  fCenter = fpFootGeo->GetMSDCenter();
    TVector3  fAngle = fpFootGeo->GetMSDAngles();
    
    TVector3 posMet, posStrip, posMod;
    string bodyname, regionname;

    ss << "* ***MSD bodies" << endl;

    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

      if(fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0)
	ss << "$start_transform " << Form("msd_%d",iSens) << endl;
      
      //strip layer
      bodyname = Form("msds%d",iSens);
      regionname = Form("MSDS%d",iSens);
      posStrip.SetXYZ( fCenter.X() + GetSensorPosition(iSens).X(),
		     fCenter.Y() + GetSensorPosition(iSens).Y(),
		     fCenter.Z() + GetSensorPosition(iSens).Z() - fTotalSize.Z()/2. + fMetalThickness + fEpiSize.Z()/2. );
      ss <<  "RPP " << bodyname <<  "     "
	 << posStrip.x() - fEpiSize.X()/2. << " "
	 << posStrip.x() + fEpiSize.X()/2. << " "
	 << posStrip.y() - fEpiSize.Y()/2. << " "
	 << posStrip.y() + fEpiSize.Y()/2. << " "
	 << posStrip.z() - fEpiSize.Z()/2. << " "
	 << posStrip.z() + fEpiSize.Z()/2. << endl;
      vStripBody.push_back(bodyname);
      vStripRegion.push_back(regionname);

      //module
      bodyname = Form("msdp%d",iSens);
      regionname = Form("MSDP%d",iSens);
      posMod.SetXYZ( posStrip.X() + fEpiSize.X()/2. + fEpiOffset.X() - fTotalSize.X()/2.,
		     posStrip.Y() - fEpiSize.Y()/2. - fEpiOffset.Y() + fTotalSize.Y()/2.,
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
      vMetalBody.push_back(bodyname);
      vMetalRegion.push_back(regionname);

      if(fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0)
	ss << "$end_transform " << endl;

    }
  }

  return ss.str();
}

//_____________________________________________________________________________
string TAMSDparGeo::PrintRegions()
{

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeMSD()){

    string name;

    ss << "* ***MSD regions" << endl;

    for(int i=0; i<vStripRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << vStripRegion.at(i)
    	 << "5 " << vStripBody.at(i) <<endl;
    }

    for(int i=0; i<vModRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << vModRegion.at(i)
	 << "5 " << vModBody.at(i)
	 << " -" << vStripBody.at(i) << " -" << vMetalBody.at(i) <<endl;
    }

    for(int i=0; i<vMetalRegion.size(); i++) {
      ss << setw(13) << setfill( ' ' ) << std::left << vMetalRegion.at(i)
    	 << "5 " << vMetalBody.at(i) <<endl;
    }

  }

  return ss.str();

}

//_____________________________________________________________________________
string TAMSDparGeo::PrintSubtractBodiesFromAir()
{

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeMSD()){

    for(int i=0; i<vModBody.size(); i++) {
      ss << " -" << vModBody.at(i);
    }
    ss << endl;

  }

   return ss.str();
}

//_____________________________________________________________________________
string TAMSDparGeo::PrintAssignMaterial(TAGmaterials *Material)
{

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeMSD()){

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
    if(GlobalPar::GetPar()->IncludeDI())
      magnetic = true;

    if (vStripRegion.size()==0 || vModRegion.size()==0 || vMetalRegion.size()==0 )
      cout << "Error: MSD regions vector not correctly filled!"<<endl;
    
    ss << PrintCard("ASSIGNMA", flkmatMod, vStripRegion.at(0), vStripRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatMetal, vMetalRegion.at(0), vMetalRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatSupp, vModRegion.at(0), vModRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;

  }

  return ss.str();

}
