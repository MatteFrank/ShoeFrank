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
   fkDefaultGeoName = "./geomaps/TAMSDdetector.map";
}

//_____________________________________________________________________________
TAMSDparGeo::~TAMSDparGeo()
{
}


//_____________________________________________________________________________
//void TAMSDparGeo::DefineMaterial()
//{
//   // material for strip, same as M28 ?
//   TAVTbaseParGeo::DefineMaterial();
//}

//_____________________________________________________________________________
Bool_t TAMSDparGeo::FromFile(const TString& name)
{
   // simple file reading, waiting for real config file
   TString nameExp;

   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;

   if (!Open(nameExp)) return false;

   ReadItem(fSensorsN);
   if(fDebugLevel)
      cout << endl << "Sensors number "<< fSensorsN << endl;

   ReadStrings(fTypeName);
   if(fDebugLevel)
      cout  << endl << "  Type Name : "<< fTypeName.Data() << endl;

   ReadItem(fTypeNumber);
   if(fDebugLevel)
      cout  << endl << "  Type Number : "<< fTypeNumber << endl;

   ReadItem(fStripN);
   if(fDebugLevel)
      cout  << "  Number of strip: "<< fStripN << endl;

   ReadItem(fPitch);
   if(fDebugLevel)
      cout  << "  Pitch for strip: "<< fPitch << endl;

   ReadVector3(fTotalSize);
   if(fDebugLevel)
      cout  << "  Total size of sensor:     "<< fTotalSize.X() << " " <<  fTotalSize.Y() << " "
      <<  fTotalSize.Z()  << endl;

   ReadVector3(fEpiSize);
   if(fDebugLevel)
      cout  << endl << "  Sensitive size of sensor: "<< fEpiSize.X() << " " <<  fEpiSize.Y() << " "
      <<  fEpiSize.Z()  << endl;

   ReadVector3(fEpiOffset);
   if(fDebugLevel)
      cout  << endl << "  Offset of sensitive area of sensor: "<< fEpiOffset.X() << " " <<  fEpiOffset.Y() << " "
      <<  fEpiOffset.Z()  << endl;

   ReadStrings(fEpiMat);
   if(fDebugLevel)
      cout   << "  Sensitive material: "<< fEpiMat.Data() << endl;

   ReadItem(fEpiMatDensity);
   if(fDebugLevel)
      cout  << "  Sensitive material density:  "<< fEpiMatDensity << endl;

    ReadItem(fPixThickness);
    if(fDebugLevel)
       cout  << endl << "  Pixel thickness: "<< fPixThickness << endl;

   ReadStrings(fPixMat);
   if(fDebugLevel)
      cout   << "  Pixel material: "<< fPixMat.Data() << endl;

   ReadItem(fPixMatDensity);
   if(fDebugLevel)
     cout  << "  Pixel material density:  "<< fPixMatDensity << endl;
   
   ReadItem(fMetalThickness);
   if(fDebugLevel)
     cout  << endl << "  Metal thickness: "<< fMetalThickness << endl;
   
   ReadStrings(fMetalMat);
   if(fDebugLevel)
     cout   << "  Metal material: "<< fMetalMat.Data() << endl;
   
   ReadItem(fMetalDensity);
   if(fDebugLevel)
     cout  << "  Metal density:  "<< fMetalDensity << endl;
 
   ReadItem(fSupportInfo);
   if(fDebugLevel)
      cout  << "  Info flag for support:  "<< fSupportInfo << endl;

   // read info for support only for IT
   if (fSupportInfo)
      ReadSupportInfo();

   if(fDebugLevel)
      cout << endl << "Reading Sensor Parameters " << endl;

   SetupMatrices(fSensorsN);

   for (Int_t p = 0; p < fSensorsN; p++) { // Loop on each plane

      // read sensor index
      ReadItem(fSensorParameter[p].SensorIdx);
      if(fDebugLevel)
         cout << endl << " - Parameters of Sensor " <<  fSensorParameter[p].SensorIdx << endl;

      // read sensor index
      ReadItem(fSensorParameter[p].TypeIdx);
      if(fDebugLevel)
         cout  << "   Type of Sensor: " <<  fSensorParameter[p].TypeIdx << endl;

      // read sensor position
      ReadVector3(fSensorParameter[p].Position);
      if(fDebugLevel)
         cout << "   Position: "
         << Form("%f %f %f", fSensorParameter[p].Position[0], fSensorParameter[p].Position[1], fSensorParameter[p].Position[2]) << endl;

      // read sensor angles
      ReadVector3(fSensorParameter[p].Tilt);
      if(fDebugLevel)
         cout  << "   Tilt: "
		       << Form("%f %f %f", fSensorParameter[p].Tilt[0], fSensorParameter[p].Tilt[1], fSensorParameter[p].Tilt[2]) << endl;

      // read alignment
      ReadItem(fSensorParameter[p].AlignmentU);
      ReadItem(fSensorParameter[p].AlignmentV);
      if(fDebugLevel)
         cout  << "   Alignment: " <<  fSensorParameter[p].AlignmentU << " " << fSensorParameter[p].AlignmentV << endl;

      // read tiltW
      ReadItem(fSensorParameter[p].TiltW);
      if(fDebugLevel)
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
TGeoVolume* TAMSDparGeo::AddModule(const char* basemoduleName, const char *vertexName)
{
   // create MSD module
   const Char_t* matName = fEpiMat.Data();
   TGeoMedium*   medMod = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   TGeoBBox *box = new TGeoBBox(Form("%s_Box",basemoduleName), fEpiSize.X()/2., fEpiSize.Y()/2.,
                                fTotalSize.Z()/2.);
   
   TGeoVolume *vertexMod = new TGeoVolume(Form("%s_MSD",basemoduleName),box, medMod);
   vertexMod->SetLineColor(kAzure-5);
   vertexMod->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   // if (GlobalPar::GetPar()->geoFLUKA())
   //    PrintFluka();
   
   return vertexMod;
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

      TGeoHMatrix* hm = GetTransfo(iSensor);
      msdMod = AddModule(Form("%s%d",basemoduleName, iSensor), msdName);
   
      TGeoHMatrix* transf = (TGeoHMatrix*)hm->Clone();
      msd->AddNode(msdMod, iSensor, transf);
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
    intp["nlayVTX"] = fSensorsN;
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

    for(int iSens=0; iSens<GetNSensors(); iSens++) {

      // if(fSensorParameter[iSens].Tilt[0]!=0){
      // 	ss << setw(10) << setfill(' ') << std::left << "ROT-DEFI"
      // 	   << setw(10) << setfill(' ') << std::right << "100."
      // 	   << setw(10) << setfill(' ') << std::right << " "
      // 	   << setw(10) << setfill(' ') << std::right << fSensorParameter[iSens].Tilt[0]*TMath::RadToDeg()
      // 	   << setw(10) << setfill(' ') << std::right << " "
      // 	   << setw(10) << setfill(' ') << std::right << " "
      // 	   << setw(10) << setfill(' ') << std::right << " "
      // 	   << setfill(' ') << std::left << Form("vtX_%d",iSens)
      // 	   << endl;
      // }
      // if(fSensorParameter[iSens].Tilt[1]!=0){
      // 	ss << setw(10) << setfill(' ') << std::left << "ROT-DEFI"
      // 	   << setw(10) << setfill(' ') << std::right << "200."
      // 	   << setw(10) << setfill(' ') << std::right << " "
      // 	   << setw(10) << setfill(' ') << std::right << fSensorParameter[iSens].Tilt[1]*TMath::RadToDeg()
      // 	   << setw(10) << setfill(' ') << std::right << " "
      // 	   << setw(10) << setfill(' ') << std::right << " "
      // 	   << setw(10) << setfill(' ') << std::right << " "
      // 	   << setfill(' ') << std::left << Form("vtY_%d",iSens)
      // 	   << endl;
      // }
      if(fSensorParameter[iSens].Tilt[2]!=0){
	ss << setw(10) << setfill(' ') << std::left << "ROT-DEFI"
	   << setw(10) << setfill(' ') << std::right << "300."
	   << setw(10) << setfill(' ') << std::right << " "
	   << setw(10) << setfill(' ') << std::right << fSensorParameter[iSens].Tilt[2]*TMath::RadToDeg()
	   << setw(10) << setfill(' ') << std::right << " "
	   << setw(10) << setfill(' ') << std::right << " "
	   << setw(10) << setfill(' ') << std::right << " "
	   << setfill(' ') << std::left << Form("msdZ_%d",iSens)
	   << endl;
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
    TVector3 posMet, posStrip, posMod;

    string bodyname, regionname;

    ss << "* ***MSD bodies" << endl;

    for(int iSens=0; iSens<GetNSensors(); iSens++) {

      // if(fSensorParameter[iSens].Tilt[0]!=0)
      // 	ss << "$start_transform " << Form("vtX_%d",iSens) << endl;
      // if(fSensorParameter[iSens].Tilt[1]!=0)
      // 	ss << "$start_transform " << Form("vtY_%d",iSens) << endl;
      if(fSensorParameter[iSens].Tilt[2]!=0)
	ss << "$start_transform " << Form("msdZ_%d",iSens) << endl;

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

      // if(fSensorParameter[iSens].Tilt[0]!=0){
      // 	ss << "$end_transform " << endl;
      // }
      // if(fSensorParameter[iSens].Tilt[1]!=0){
      // 	ss << "$end_transform " << endl;
      // }
      if(fSensorParameter[iSens].Tilt[2]!=0){
	ss << "$end_transform " << endl;
      }

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
string TAMSDparGeo::PrintAssignMaterial()
{

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeMSD()){

    string matMod = "SILICON";//fEpiMat.Data();
    if (fEpiMat.CompareTo("Si")!=0)
      cout << "ATTENTION in TAMSDparGeo PrintAssignMaterial: check the MSD material"<<endl;
    // matMod[1] =toupper(matMod[1]);
    // const Char_t* matMod = fEpiMat.Data();
    const Char_t* matMetal = "ALUMINUM";//fMetalMat.Data();
    const Char_t* matSupp = fPixMat.Data();
    bool magnetic = false;
    if(GlobalPar::GetPar()->IncludeDI())
      magnetic = true;

    if (vStripRegion.size()==0 || vModRegion.size()==0 || vMetalRegion.size()==0 )
      cout << "Error: MSD regions vector not correctly filled!"<<endl;

    ss << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
       << setw(10) << setfill(' ') << std::right << matMod
       << setw(10) << setfill(' ') << std::right << vStripRegion.at(0)
       << setw(10) << setfill(' ') << std::right << vStripRegion.back()
       << setw(10) << setfill(' ') << std::right << "1."
       << setw(10) << setfill(' ') << std::right << magnetic
       << endl;

    ss << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
       << setw(10) << setfill(' ') << std::right << matSupp
       << setw(10) << setfill(' ') << std::right << vModRegion.at(0)
       << setw(10) << setfill(' ') << std::right << vModRegion.back()
       << setw(10) << setfill(' ') << std::right << "1."
       << setw(10) << setfill(' ') << std::right << magnetic
       << endl;

    ss << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
       << setw(10) << setfill(' ') << std::right << matMetal
       << setw(10) << setfill(' ') << std::right << vMetalRegion.at(0)
       << setw(10) << setfill(' ') << std::right << vMetalRegion.back()
       << setw(10) << setfill(' ') << std::right << "1."
       << setw(10) << setfill(' ') << std::right << magnetic
       << endl;

  }

  return ss.str();

}
