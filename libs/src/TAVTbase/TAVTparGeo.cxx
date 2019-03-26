
////////////////////////////////////////////////////////////////////////////////
//                                   MI28
//
//                 0
//                ------------------------------------
//              0 |                 |                 |
//                |                 |                 |
//  Y(+/-9604.8)  |                 |                 |
//                |                 |                 |
//                |                 |                 |
//                |                 |                 |Nv = Nline = 928
//                |                 | (0,0)           |
//                -------------------------------------
//                |                 |                 |
//                |                 |                 |
//                |                 |                 |
//                |                 |                 |
//                |                 |                 |
//                |                 |                 |
//                |                 |                 |
//                -------------------------------------
//                              Nu = Ncol = 960
//                                X (+/-9936)
////////////////////////////////////////////////////////////////////////////////

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx" 
#include "TAGmaterials.hxx"

#include "TAVTparGeo.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TAVTparGeo TAVTparGeo.hxx "TAVTparGeo.hxx"
  \brief Map and Geometry parameters for vertex. **
*/

ClassImp(TAVTparGeo);

const TString TAVTparGeo::fgkBaseName      = "VT";
const TString TAVTparGeo::fgkDefParaName   = "vtGeo";


//______________________________________________________________________________
TAVTparGeo::TAVTparGeo()
  : TAVTbaseParGeo()
{
  // Standard constructor
  fkDefaultGeoName = "./geomaps/TAVTdetector.map";
}

//______________________________________________________________________________
TAVTparGeo::~TAVTparGeo()
{
  // Destructor
}



//_____________________________________________________________________________
void TAVTparGeo::DefineMaterial()
{
  // Silicon material
  TAVTbaseParGeo::DefineMaterial();
   
  // Pixel material
  TGeoMixture* mix = TAGmaterials::Instance()->CreateMixture(fPixMat, fPixMatDensities, fPixMatProp, fPixMatDensity);
  if (fDebugLevel) {
    printf("pixels material:\n");
    mix->Print();
  }
}

//_____________________________________________________________________________
TGeoVolume* TAVTparGeo::BuildVertex(const char *vertexName, const char* basemoduleName)
{
  if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
    new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
  }
 
  // define box
  DefineMaxMinDimension();
   
  TGeoVolume* vertex = gGeoManager->FindVolumeFast(vertexName);
  if ( vertex == 0x0 ) {
    TGeoMedium*   med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
    vertex = gGeoManager->MakeBox(vertexName,med,fSizeBox.X()/2.,fSizeBox.Y()/2.,fSizeBox.Z()/2.); // volume corresponding to vertex
  }
   
  TGeoVolume* vertexMod = 0x0; 
   
  for(Int_t iSensor = 0; iSensor < GetNSensors(); iSensor++) {
      
    TGeoHMatrix* hm = GetTransfo(iSensor);
    vertexMod = AddModule(Form("%s%d",basemoduleName, iSensor), vertexName);
      
    TGeoHMatrix* transf = (TGeoHMatrix*)hm->Clone();
    vertex->AddNode(vertexMod, iSensor, transf);
  }
   
  return vertex;
}

//_____________________________________________________________________________
TGeoVolume* TAVTparGeo::AddModule(const char* basemoduleName, const char *vertexName)
{
    // create M28 module
   const Char_t* matName = fEpiMat.Data();
   TGeoMedium*   medMod = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);

   TGeoBBox *box = new TGeoBBox(Form("%s_Box",basemoduleName), fEpiSize.X()/2., fEpiSize.Y()/2.,
                                fTotalSize.Z()/2.);

   TGeoVolume *vertexMod = new TGeoVolume(Form("%s_Vertex",basemoduleName),box, medMod);
   vertexMod->SetLineColor(kAzure-5);
   vertexMod->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   // if (GlobalPar::GetPar()->geoFLUKA())
   //    PrintFluka();
   
   return vertexMod;
}


//_____________________________________________________________________________
string TAVTparGeo::PrintParameters()
{   
  stringstream outstr;
  outstr << setiosflags(ios::fixed) << setprecision(5);
   
  if(GlobalPar::GetPar()->IncludeVertex()){
    
    string precision = "D+00";
   
    outstr << "c     VERTEX PARAMETERS " << endl;
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
string TAVTparGeo::PrintRotations()
{
  stringstream ss;

  if(GlobalPar::GetPar()->IncludeVertex()){
  
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
	   << setfill(' ') << std::left << Form("vtZ_%d",iSens) 
	   << endl;
      }

    }
  }
  return ss.str();

}


//_____________________________________________________________________________
string TAVTparGeo::PrintBodies()
{

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeVertex()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3  fCenter = fpFootGeo->GetVTCenter();
    TVector3 posEpi, posPix, posMod;

    string bodyname, regionname;
  
    ss << "* ***Vertex bodies" << endl;  

    for(int iSens=0; iSens<GetNSensors(); iSens++) {

      // if(fSensorParameter[iSens].Tilt[0]!=0)
      // 	ss << "$start_transform " << Form("vtX_%d",iSens) << endl;
      // if(fSensorParameter[iSens].Tilt[1]!=0)
      // 	ss << "$start_transform " << Form("vtY_%d",iSens) << endl;
      if(fSensorParameter[iSens].Tilt[2]!=0)
	ss << "$start_transform " << Form("vtZ_%d",iSens) << endl;
        
      //epitaxial layer
      bodyname = Form("vtxe%d",iSens);
      regionname = Form("VTXE%d",iSens);
      posEpi.SetXYZ( fCenter.X() + GetSensorPosition(iSens).X(),
		     fCenter.Y() + GetSensorPosition(iSens).Y(),
		     fCenter.Z() + GetSensorPosition(iSens).Z() - fTotalSize.Z()/2. + fPixThickness + fEpiSize.Z()/2. );
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
      bodyname = Form("vtxm%d",iSens);
      regionname = Form("VTXM%d",iSens);
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
      bodyname = Form("vtxp%d",iSens);
      regionname = Form("VTXP%d",iSens);
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
string TAVTparGeo::PrintRegions()
{

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeVertex()){

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
string TAVTparGeo::PrintSubtractBodiesFromAir()
{
  
  stringstream ss;

  if(GlobalPar::GetPar()->IncludeVertex()){

    for(int i=0; i<vModBody.size(); i++) {
      ss << " -" << vModBody.at(i);
    }
    ss << endl;

  }

   return ss.str();   
}

//_____________________________________________________________________________
string TAVTparGeo::PrintAssignMaterial()
{

  stringstream ss;
  
  if(GlobalPar::GetPar()->IncludeVertex()){

    const Char_t* matMod = fEpiMat.Data();
    const Char_t* matPix = fPixMat.Data();
    bool magnetic = false;
    if(GlobalPar::GetPar()->IncludeDI())
      magnetic = true;

    if (vEpiRegion.size()==0 || vModRegion.size()==0 || vPixRegion.size()==0 )
      cout << "Error: VT regions vector not correctly filled!"<<endl;
    
    ss << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
       << setw(10) << setfill(' ') << std::right << "SILICON"//matMod
       << setw(10) << setfill(' ') << std::right << vEpiRegion.at(0)
       << setw(10) << setfill(' ') << std::right << vEpiRegion.back()
       << setw(10) << setfill(' ') << std::right << "1."
       << setw(10) << setfill(' ') << std::right << magnetic
       << endl;

    ss << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
       << setw(10) << setfill(' ') << std::right << "SILICON"//matMod
       << setw(10) << setfill(' ') << std::right << vModRegion.at(0)
       << setw(10) << setfill(' ') << std::right << vModRegion.back()
       << setw(10) << setfill(' ') << std::right << "1."
       << setw(10) << setfill(' ') << std::right << magnetic
       << endl;

    ss << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
       << setw(10) << setfill(' ') << std::right << "SILICON"//matPix
       << setw(10) << setfill(' ') << std::right << vPixRegion.at(0)
       << setw(10) << setfill(' ') << std::right << vPixRegion.back()
       << setw(10) << setfill(' ') << std::right << "1."
       << setw(10) << setfill(' ') << std::right << magnetic
       << endl;

  }

  return ss.str();

}


