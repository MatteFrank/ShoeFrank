
/*!
 \file TAVTparGeo.cxx
 \brief  Geometry parameters for VTX.
 */


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
#include "TGeoCompositeShape.h"

#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx" 

#include "TAVTparGeo.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TAVTparGeo
  \brief Geometry parameters for VTX. **
*/

//! Class Imp
ClassImp(TAVTparGeo);

const TString TAVTparGeo::fgkBaseName      = "VT";

//______________________________________________________________________________
//! Standard constructor
TAVTparGeo::TAVTparGeo()
  : TAVTbaseParGeo()
{
  fkDefaultGeoName = "./geomaps/TAVTdetector.geo";
}

//______________________________________________________________________________
//! Destructor
TAVTparGeo::~TAVTparGeo()
{
}

//_____________________________________________________________________________
//! Define materials
void TAVTparGeo::DefineMaterial()
{
  // Silicon material
  TAVTbaseParGeo::DefineMaterial();
   
  // Pixel material
  TGeoMixture* mix = TAGmaterials::Instance()->CreateMixture(fPixMat, fPixMatDensities, fPixMatProp, fPixMatDensity);
   if(FootDebugLevel(1)) {
    printf("pixels material:\n");
    mix->Print();
  }
}

//_____________________________________________________________________________
//! Build vertex in Root geometry
//!
//! \param[in] vertexName  vertex volume name
//! \param[in] basemoduleName module volume base name
//! \param[in] board flag for implemented passive mother board
TGeoVolume* TAVTparGeo::BuildVertex(const char* vertexName, const char* basemoduleName, Bool_t board)
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
   
  TGeoVolume* vertexBoard = 0x0;
  TGeoVolume* vertexMod   = 0x0;
   
  if (board)
     vertexBoard = BuildBoard();
   
  for(Int_t iSensor = 0; iSensor < GetSensorsN(); iSensor++) {
      
    TGeoCombiTrans* hm = GetCombiTransfo(iSensor);
    vertexMod = AddModule(Form("%s%d",basemoduleName, iSensor), vertexName);
      
    vertex->AddNode(vertexMod, iSensor, hm);

    // Board
     if (board) {
       Float_t signY = fSensorParameter[iSensor].IsReverseX ? +1. : -1.;
       Float_t signX = fSensorParameter[iSensor].IsReverseY ? +1. : -1.;

       const Double_t* mat = hm->GetRotationMatrix();
       const Double_t* dis = hm->GetTranslation();
     
       TGeoRotation rot;
       rot.SetMatrix(mat);
     
       TGeoTranslation trans;
       trans.SetTranslation(dis[0] + signX*fEpiOffset[0]/2., dis[1] + signY*fEpiOffset[1]/2., dis[2] - fEpiOffset[2]);
        
       vertex->AddNode(vertexBoard, iSensor+GetSensorsN(), new TGeoCombiTrans(trans, rot));
    }
  }
   
  return vertex;
}

//_____________________________________________________________________________
//! Add module in vertex for Root geometry
//!
//! \param[in] basemoduleName module volume base name
//! \param[in] vertexName vertex volume name
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
   
   return vertexMod;
}

//_____________________________________________________________________________
//! Add module in vertex for Root geometry
//!
//! \param[in] boardName board volume name
//! \param[in] moduleName module volume name
TGeoVolume* TAVTparGeo::BuildBoard(const char* boardName, const char *moduleName)
{
   // check if board exists
   TGeoVolume* board = gGeoManager->FindVolumeFast(boardName);
   if (board)
      return board;

   // create M28 board
   const Char_t* matName = fEpiMat.Data();
   TGeoMedium*   medMod = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   Float_t eps = 1e-4;
   
   TGeoBBox *boxBoard  = new TGeoBBox(boardName,  fTotalSize.X()/2., fTotalSize.Y()/2., fTotalSize.Z()/2.);
   TGeoBBox *boxModule = new TGeoBBox(moduleName, fEpiSize.X()/2.,   fEpiSize.Y()/2.,   fTotalSize.Z()/2. + eps);

   TGeoTranslation *tr = new TGeoTranslation(fEpiOffset.X()/2., fEpiOffset.Y()/2., fEpiOffset.Z());
   tr->SetName("offset");
   tr->RegisterYourself();

   TGeoCompositeShape *cs  = new TGeoCompositeShape("cs", Form("%s - %s:offset", boardName, moduleName));

   TGeoVolume *vertexBoard = new TGeoVolume(boardName, cs, medMod);
   vertexBoard->SetLineColor(kRed);
   vertexBoard->SetTransparency(TAGgeoTrafo::GetDefaultTransp());

   return vertexBoard;
}

//_____________________________________________________________________________
//! Print Fluka parameters
string TAVTparGeo::PrintParameters()
{   
  stringstream outstr;
   
  if(TAGrecoManager::GetPar()->IncludeVT()){
    
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
//! Print Fluka rotations
string TAVTparGeo::PrintRotations()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeVT()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    
    TVector3  center = fpFootGeo->GetVTCenter();
    TVector3  angle = fpFootGeo->GetVTAngles(); //invert the angles to take into account the FLUKA convention;
    angle *= -1;
     
    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

      //check if sensor or detector have a tilt
      if (fSensorParameter[iSens].Tilt.Mag()!=0 || angle.Mag()!=0){

	//put the sensor in local coord before the rotation
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",-center.X()),
			Form("%f",-center.Y()),
			Form("%f",-center.Z()),
			Form("vt_%d",iSens) ) << endl;

	//check if sensor has a tilt
	if (fSensorParameter[iSens].Tilt.Mag()!=0){
	  
	  // put the sensor in 0,0,0 before the sensor's rot
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",-GetSensorPosition(iSens).X()),
			  Form("%f",-GetSensorPosition(iSens).Y()),
			  Form("%f",-GetSensorPosition(iSens).Z()),
			  Form("vt_%d",iSens) ) << endl;
	  //rot around x
	  if(fSensorParameter[iSens].Tilt[0]!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "",
			    Form("%f",fSensorParameter[iSens].Tilt[0]*TMath::RadToDeg()),
			    "", "", "", Form("vt_%d",iSens) ) << endl;
	  }
	  //rot around y      
	  if(fSensorParameter[iSens].Tilt[1]!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "",
			    Form("%f",fSensorParameter[iSens].Tilt[1]*TMath::RadToDeg()),
			    "", "", "", Form("vt_%d",iSens) ) << endl;
	  }
	  //rot around z
	  if(fSensorParameter[iSens].Tilt[2]!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "",
			    Form("%f",fSensorParameter[iSens].Tilt[2]*TMath::RadToDeg()),
			    "", "", "", Form("vt_%d",iSens) ) << endl;
	  }
	  
	  //put back the sensor into its position in local coord
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",GetSensorPosition(iSens).X()),
			  Form("%f",GetSensorPosition(iSens).Y()),
			  Form("%f",GetSensorPosition(iSens).Z()),
			  Form("vt_%d",iSens) ) << endl;
	}

	//check if detector has a tilt and then apply rot
	if(angle.Mag()!=0){
	  
	  if(angle.X()!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",angle.X()),"", "",
			    "", Form("vt_%d",iSens)) << endl;
	  }
	  if(angle.Y()!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",angle.Y()),"", "",
			    "", Form("vt_%d",iSens)) << endl;
	  }
	  if(angle.Z()!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",angle.Z()),"", "",
			    "", Form("vt_%d",iSens)) << endl;
	  }
	}
      
	//put back the detector in global coord
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",center.X()), Form("%f",center.Y()),
			Form("%f",center.Z()), Form("vt_%d",iSens)) << endl;
       
      }
    }
  }
   
  return ss.str();
}

//_____________________________________________________________________________
//! Print Fluka bodies
string TAVTparGeo::PrintBodies()
{

  stringstream ss;
   ss << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

  if(TAGrecoManager::GetPar()->IncludeVT()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3  center = fpFootGeo->GetVTCenter();
    TVector3  angle = fpFootGeo->GetVTAngles();
    TVector3 posEpi, posPix, posMod;

    string bodyname, regionname;
  
    ss << "* ***Vertex bodies" << endl;  
    
    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

      if(fSensorParameter[iSens].Tilt.Mag()!=0 || angle.Mag()!=0)
	ss << "$start_transform " << Form("vt_%d",iSens) << endl;  
               
      //epitaxial layer
      bodyname = Form("vtxe%d",iSens);
      regionname = Form("VTXE%d",iSens);
      posEpi.SetXYZ( center.X() + GetSensorPosition(iSens).X(),
		     center.Y() + GetSensorPosition(iSens).Y(),
		     center.Z() + GetSensorPosition(iSens).Z() - fTotalSize.Z()/2. + fPixThickness + fEpiSize.Z()/2. );
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
      bodyname = Form("vtxm%d",iSens);
      regionname = Form("VTXM%d",iSens);
      posMod.SetXYZ( posEpi.X() + fEpiSize.X()/2. + fEpiOffset.X() - fTotalSize.X()/2.,
		     posEpi.Y() - fEpiSize.Y()/2. - fEpiOffset.Y() + fTotalSize.Y()/2.,
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
      fvPixBody.push_back(bodyname);
      fvPixRegion.push_back(regionname);
    
      if(fSensorParameter[iSens].Tilt.Mag()!=0 || angle.Mag()!=0)
	ss << "$end_transform " << endl;
	  
    }
  }
  
  return ss.str();
}

//_____________________________________________________________________________
//! Print Fluka regions
string TAVTparGeo::PrintRegions()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeVT()){

    string name;

    ss << "* ***Vertex regions" << endl;

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

  }
  
  return ss.str();
}

//_____________________________________________________________________________
//! Get epitaxial region in Fluka for a given layer
//!
//! \param[in] n layer index
Int_t TAVTparGeo::GetRegEpitaxial(Int_t n)
{
  TString regname;
  regname.Form("VTXE%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get module region in Fluka for a given layer
//!
//! \param[in] n layer index
Int_t TAVTparGeo::GetRegModule(Int_t n)
{
  TString regname;
  regname.Form("VTXM%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get pixel region in Fluka for a given layer
//!
//! \param[in] n layer index
Int_t TAVTparGeo::GetRegPixel(Int_t n)
{
  TString regname;
  regname.Form("VTXP%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Print subtracted bodies from air in Fluka
string TAVTparGeo::PrintSubtractBodiesFromAir()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeVT()){

    for(int i=0; i<fvModBody.size(); i++) {
      ss << " -" << fvModBody.at(i);
    }
    ss << endl;
  }

   return ss.str();   
}

//_____________________________________________________________________________
//! Print assigned material in Fluka for a given material
//!
//! \param[in] Material Root material
string TAVTparGeo::PrintAssignMaterial(TAGmaterials* Material)
{
  stringstream ss;
  
  if(TAGrecoManager::GetPar()->IncludeVT()){

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
    if(TAGrecoManager::GetPar()->IncludeDI())
      magnetic = true;
    
    if (fvEpiRegion.size()==0 || fvModRegion.size()==0 || fvPixRegion.size()==0 )
      cout << "Error: VT regions vector not correctly filled!"<<endl;
    
    ss << PrintCard("ASSIGNMA", flkmatMod, fvEpiRegion.at(0), fvEpiRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatMod, fvModRegion.at(0), fvModRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatPix, fvPixRegion.at(0), fvPixRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;

  }

  return ss.str();
}
