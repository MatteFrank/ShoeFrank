
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

#include "GlobalPar.hxx"
#include "TAGgeoTrafo.hxx" 

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
   if(FootDebugLevel(1)) {
    printf("pixels material:\n");
    mix->Print();
  }
}

//_____________________________________________________________________________
TGeoVolume* TAVTparGeo::BuildVertex(const char *vertexName, const char* basemoduleName, Bool_t board)
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
string TAVTparGeo::PrintParameters()
{   
  stringstream outstr;
   
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

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    
    TVector3  fCenter = fpFootGeo->GetVTCenter();
    TVector3  fAngle = fpFootGeo->GetVTAngles();
    
    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

      //check if sensor or detector have a tilt
      if (fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0){

	//put the sensor in local coord before the rotation
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",-fCenter.X()),
			Form("%f",-fCenter.Y()),
			Form("%f",-fCenter.Z()),
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
	if(fAngle.Mag()!=0){
	  
	  if(fAngle.X()!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",fAngle.X()),"", "", 
			    "", Form("vt_%d",iSens)) << endl;
	  }
	  if(fAngle.Y()!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",fAngle.Y()),"", "", 
			    "", Form("vt_%d",iSens)) << endl;
	  }
	  if(fAngle.Z()!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",fAngle.Z()),"", "", 
			    "", Form("vt_%d",iSens)) << endl;
	  }
	}
      
	//put back the detector in global coord
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",fCenter.X()), Form("%f",fCenter.Y()),
			Form("%f",fCenter.Z()), Form("vt_%d",iSens)) << endl;
       
      }
    }
  }
  return ss.str();

}


//_____________________________________________________________________________
string TAVTparGeo::PrintBodies()
{

  stringstream ss;
   ss << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

  if(GlobalPar::GetPar()->IncludeVertex()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3  fCenter = fpFootGeo->GetVTCenter();
    TVector3  fAngle = fpFootGeo->GetVTAngles();
    TVector3 posEpi, posPix, posMod;

    string bodyname, regionname;
  
    ss << "* ***Vertex bodies" << endl;  
    
    for(int iSens=0; iSens<GetSensorsN(); iSens++) {

      if(fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0)
	ss << "$start_transform " << Form("vt_%d",iSens) << endl;  
               
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
      fvEpiBody.push_back(bodyname);
      fvEpiRegion.push_back(regionname);
    
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
    
      if(fSensorParameter[iSens].Tilt.Mag()!=0 || fAngle.Mag()!=0)
	ss << "$end_transform " << endl;
	  
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
string TAVTparGeo::PrintSubtractBodiesFromAir()
{
  
  stringstream ss;

  if(GlobalPar::GetPar()->IncludeVertex()){

    for(int i=0; i<fvModBody.size(); i++) {
      ss << " -" << fvModBody.at(i);
    }
    ss << endl;

  }

   return ss.str();   
}

//_____________________________________________________________________________
string TAVTparGeo::PrintAssignMaterial(TAGmaterials *Material)
{

  stringstream ss;
  
  if(GlobalPar::GetPar()->IncludeVertex()){

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


