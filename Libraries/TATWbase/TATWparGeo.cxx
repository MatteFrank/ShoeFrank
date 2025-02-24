/*!
 \file TATWparGeo.cxx
 \brief   Implementation of TATWparGeo.
 */

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TEveGeoShapeExtract.h"
#include "TEveTrans.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TList.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TGeoMatrix.h"
#include "TMatrixD.h"
#include "TArrayD.h"

#include "TAGionisMaterials.hxx"
#include "TAGgeoTrafo.hxx"

#include "TATWparGeo.hxx"
#include "TAGroot.hxx"
#include "TATWparameters.hxx"

const TString TATWparGeo::fgkBaseName        = "TW";
      Int_t   TATWparGeo::fgkLayerOffset     = 100;
const Color_t TATWparGeo::fgkDefaultModCol   = kGray+1;
const Color_t TATWparGeo::fgkDefaultModColOn = kRed-6;
const TString TATWparGeo::fgkDefaultBarName = "twBar";


/*!
 \class TATWparGeo
 \brief geometry class for TW
 */

//! Class Imp
ClassImp(TATWparGeo);

//_____________________________________________________________________________
TATWparGeo::TATWparGeo()
: TAGparTools(),
  fIonisation(new TAGionisMaterials())
{
   fkDefaultGeoName = "./geomaps/TATWdetector.geo";
}

//______________________________________________________________________________
TATWparGeo::~TATWparGeo()
{
   delete fIonisation;
}

//______________________________________________________________________________
Bool_t TATWparGeo::FromFile(const TString& name)
{
   cout << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   Info("FromFile()", "Open file %s for geometry", name.Data());

   ReadItem(fLayersN);
   if(FootDebugLevel(1))
      cout << endl << "Number of layers "<< fLayersN << endl;
   
   ReadItem(fBarsN);
   if(FootDebugLevel(1))
      cout  << "   Number of bars: " <<  fBarsN << endl;
   
   ReadStrings(fBarMat);
   if(FootDebugLevel(1))
      cout  << "   Bars material : " <<  fBarMat << endl;
   
   ReadItem(fBarDensity);
   if(FootDebugLevel(1))
      cout  << "   Bars density : " <<  fBarDensity << endl;
   
   ReadItem(fBarIonisMat);
   if(FootDebugLevel(1))
      cout  << "   Bars material mean excitation energy : " <<  fBarIonisMat << endl;
   
   ReadItem(fBarBirkMat);
   if(FootDebugLevel(1))
      cout  << "   Bars material Birk factor : " <<  fBarBirkMat << endl;
   
   ReadVector3(fBarSize);
   if(FootDebugLevel(1))
      cout << "   Bar size: "
      << fBarSize[0] << " " << fBarSize[1] << " " << fBarSize[2] << endl;
   
   // define material
   DefineMaterial();
   
   fSize.SetXYZ(fBarSize[1], fBarSize[1], fBarSize[2]*2);
   
   TVector3 position;
   TVector3 tilt;
   Int_t barId;
   Int_t layerId;
   
   SetupMatrices(fLayersN*fBarsN);
   
   // Read transformtion info
   for (Int_t iLayer = 0; iLayer < fLayersN; ++iLayer) {

     vector<TVector3> mytilt;
       
      for (Int_t iBar = 0; iBar < fBarsN; ++iBar) {

         ReadItem(layerId);
         if(FootDebugLevel(1))
            cout  << "Layer id "<< layerId << endl;
         
         ReadItem(barId);
         if(FootDebugLevel(1))
            cout  << "Bar id "<< barId << endl;
         
         // read  position
         ReadVector3(position);
         if(FootDebugLevel(1))
            cout << "   Position: "
            << position[0] << " " << position[1] << " " << position[2] << endl;
         
         ReadVector3(tilt);
         if(FootDebugLevel(1))
            cout  << "   Tilt: "
            << tilt[0] << " " << tilt[1] << " " << tilt[2] << endl;

         mytilt.push_back(tilt);
	 
         TGeoRotation rot;
         rot.RotateX(tilt[0]);
         rot.RotateY(tilt[1]);
         rot.RotateZ(tilt[2]);
         
         TGeoTranslation trans(position[0], position[1], position[2]);
         
         TGeoHMatrix  transfo;
         transfo  = trans;
         transfo *= rot;
         Int_t idx = iLayer*fBarsN + iBar;
         AddTransMatrix(new TGeoHMatrix(transfo), idx);
      }
      
      fvTilt.push_back(mytilt);
   }
   
   Close();
   return true;
}

//_____________________________________________________________________________
TGeoCombiTrans* TATWparGeo::GetTransfo(Int_t iLayer, Int_t iBar)
{
   Int_t idx = iLayer*fBarsN + iBar;

   return TAGparTools::GetCombiTransfo(idx);
}

//_____________________________________________________________________________
TVector3 TATWparGeo::GetBarPosition(Int_t iLayer, Int_t iBar)
{

   Int_t idx = iLayer*fBarsN + iBar;

   TGeoHMatrix* hm = TAGparTools::GetTransfo(idx);
   if (hm) {
      TVector3 local(0,0,0);
      fCurrentPosition =  Sensor2Detector(iLayer, iBar, local);
   }
   return fCurrentPosition;
}

//_____________________________________________________________________________
TVector3 TATWparGeo::GetLayerPosition(Int_t iLayer)
{
   Int_t iBar = 0;
   fCurrentPosition = GetBarPosition(iLayer, iBar);
   fCurrentPosition.SetX(0.);
   fCurrentPosition.SetY(0.);
   
   return fCurrentPosition;
}

//_____________________________________________________________________________
Float_t TATWparGeo::GetCoordinate_sensorFrame(Int_t iLayer, Int_t iBar)
{
   Int_t idx = iLayer*fBarsN + iBar;
   
   TGeoHMatrix* hm = TAGparTools::GetTransfo(idx);
   if (hm) {
      TVector3 local(0,0,0);
      fCurrentPosition =  Sensor2Detector(iLayer, iBar, local);
   }
   if (iLayer == 0)
      return fCurrentPosition.X();
   else
      return fCurrentPosition.Y();
}

//_____________________________________________________________________________
Float_t TATWparGeo::GetZ_sensorFrame(Int_t iLayer, Int_t iBar)
{
   Int_t idx = iLayer*fBarsN + iBar;
   
   TGeoHMatrix* hm = TAGparTools::GetTransfo(idx);
   if (hm) {
      TVector3 local(0,0,0);
      fCurrentPosition =  Sensor2Detector(iLayer, iBar, local);
   }
      return fCurrentPosition.Z();
}



//_____________________________________________________________________________
TVector3 TATWparGeo::Sensor2Detector(Int_t iLayer, Int_t iBar, TVector3& loc) const
{
   Int_t idx = iLayer*fBarsN + iBar;

   if (idx < 0 || idx > fBarsN*fLayersN) {
      Warning("Sensor2Detector()","Wrong detector id number: %d ", idx);
      TVector3(0,0,0);
   }
   
   return LocalToMaster(idx, loc);
}


//_____________________________________________________________________________
TVector3 TATWparGeo::Sensor2DetectorVect(Int_t iLayer, Int_t iBar, TVector3& loc) const
{
   Int_t idx = iLayer*fBarsN + iBar;
   
   if (idx < 0 || idx > fBarsN*fLayersN) {
      Warning("Sensor2DetectorVect()","Wrong detector id number: %d ", idx);
      TVector3(0,0,0);
   }
   
   return LocalToMasterVect(idx, loc);
}

//_____________________________________________________________________________
TVector3 TATWparGeo::Detector2Sensor(Int_t iLayer, Int_t iBar, TVector3& glob) const
{
   Int_t idx = iLayer*fBarsN + iBar;
   
   if (idx < 0 || idx > fBarsN*fLayersN) {
      Warning("Detector2Sensor()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return MasterToLocal(idx, glob);
}

//_____________________________________________________________________________
TVector3 TATWparGeo::Detector2SensorVect(Int_t iLayer, Int_t iBar, TVector3& glob) const
{
   Int_t idx = iLayer*fBarsN + iBar;
   
   if (idx < 0 || idx > fBarsN*fLayersN) {
      Warning("Detector2SensorVect()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return MasterToLocalVect(idx, glob);
}


//_____________________________________________________________________________
void TATWparGeo::DefineMaterial()
{
   
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // TW material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fBarMat, fBarDensity);
    if(FootDebugLevel(1)) {
      printf("ToF Wall material:\n");
      mat->Print();
   }
   
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
   fIonisation->SetMaterial(mat);
   fIonisation->AddMeanExcitationEnergy(fBarIonisMat);
   fIonisation->AddBirksFactor(fBarBirkMat);

#else
   fIonisation->SetMeanExcitationEnergy(fBarIonisMat);
   fIonisation->SetBirksConstant(fBarBirkMat);

   // put it under Cerenkov since only this EM property is available
   mat->SetCerenkovProperties(fIonisation);
   
#endif

}

//_____________________________________________________________________________
TGeoVolume* TATWparGeo::BuildTofWall(const char *wallName)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager(TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   TGeoVolume* wall = gGeoManager->FindVolumeFast(wallName);
   if ( wall == 0x0 ) {
      TGeoMedium*  med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
      wall = gGeoManager->MakeBox(wallName, med,  fBarSize[1]/2.,  fBarSize[1]/2., GetBarThick()*2);
   }
   
   for (Int_t i = 0; i < fLayersN; ++i) {
      
      for (Int_t j = 0; j < fBarsN; ++j) {
         
         TGeoVolume* module = BuildModule(j, i);
         module->SetLineColor(fgkDefaultModCol);
         module->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
         
         TGeoCombiTrans* hm = GetTransfo(i, j);
         wall->AddNode(module, i*fBarsN+j, hm);
      }
   }
   
   return wall;
}

/*------------------------------------------+---------------------------------*/
//! build module

TGeoVolume* TATWparGeo::BuildModule(Int_t iMod, Int_t iLayer)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   const char* moduleName = GetDefaultBarName(iMod, iLayer);
   TGeoVolume* module     = gGeoManager->FindVolumeFast(moduleName);
   if ( module == 0x0 ) {
      const Char_t* matName = fBarMat.Data();
      TGeoMedium*  med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
      module = gGeoManager->MakeBox(moduleName, med,  fBarSize[0]/2., fBarSize[1]/2., GetBarThick()/2.);
   }
   
   module->SetLineColor(fgkDefaultModCol);
   module->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   return module;
}

//_____________________________________________________________________________
//! set color on for fired bars
void TATWparGeo::SetBarColorOn(Int_t bar, Int_t view)
{
   if (!gGeoManager) {
      Error("SetBarcolorOn()", "No Geo manager defined");
      return;
   }
   
   TString name = GetDefaultBarName(bar, view);
   
   TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
   if (vol)
      vol->SetLineColor(GetDefaultModColOn());
}

//_____________________________________________________________________________
//! reset color for unfired bars
void TATWparGeo::SetBarColorOff(Int_t bar, Int_t view)
{
   if (!gGeoManager) {
      Error("SetBarcolorOn()", "No Geo manager defined");
      return;
   }
   
   TString name = GetDefaultBarName(bar, view);
   
   TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
   if (vol)
      vol->SetLineColor(GetDefaultModCol());
}


//_____________________________________________________________________________
Int_t TATWparGeo::GetBarId(Int_t layer, Float_t xGlob, Float_t yGlob)
{
   Int_t barId = -1;

   // TW acceptance btw (-20,20) in the bar crossing region
   if(xGlob>fBarsN || xGlob<-fBarsN || yGlob>fBarsN || yGlob<-fBarsN) {
     if(FootDebugLevel(1)) Error("GetBarId()", " Reconstructed position out of TW acceptance: (x,y)=(%.1f,%.1f)",xGlob, yGlob);
     return -1;
   }   // remove to take in the TW acceptance also the not crossed regions btw (-22,22)

   if (layer == LayerY)
      barId =  -xGlob/GetBarWidth() + fBarsN/2;

   else if (layer == LayerX)
      barId =  -yGlob/GetBarWidth() + fBarsN/2;
   
   if (barId < 0 || barId > fBarsN) {
   // if (barId < 0 || barId >= fBarsN) {
     if(FootDebugLevel(1)) Error("GetBarId()", " Wrong bar Id (%d) for global position (%.1f,%.1f)", barId, xGlob, yGlob);
      
      return -1;
   }
   else if(barId==fBarsN) {
     barId--;
      Info("GetBarId()", " bar Id (%d) for global position (%.1f,%.1f)", barId, xGlob, yGlob);
   }
   
   
   barId += fgkLayerOffset*layer;
   
   return barId;
}

//_____________________________________________________________________________
string TATWparGeo::PrintRotations()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeTW()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3 center = fpFootGeo->GetTWCenter();
    TVector3  angle = fpFootGeo->GetTWAngles(); //invert the angles to take into account the FLUKA convention;
    angle *= -1;
     
    TVector3 pos;
    
    for (int i=0; i<GetLayersN(); i++){
      for (int j=0; j<GetNBars(); j++){      
  
	pos.SetXYZ(  GetBarPosition(i,j).X(),
		     GetBarPosition(i,j).Y(),
		     GetBarPosition(i,j).Z());
	  
	//check if bars or detector have a tilt
	if(fvTilt.at(i).at(j).Mag()!=0 || angle.Mag()!=0){

	  //put the bars in local coord before the rotation
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",-center.X()),
			  Form("%f",-center.Y()),
			  Form("%f",-center.Z()),
			  Form("tw_%d%02d",i,j) ) << endl;

	  //check if bar has a tilt
	  if (fvTilt.at(i).at(j).Mag()!=0){
	  
	    // put the bar in 0,0,0 before the bar's rot
	    ss << PrintCard("ROT-DEFI", "", "", "",
			    Form("%f",-pos.X()), Form("%f",-pos.Y()), Form("%f",-pos.Z()),
			    Form("tw_%d%02d",i,j) ) << endl;
	    //rot around x
	    if(fvTilt.at(i).at(j)[0]!=0){
	      ss << PrintCard("ROT-DEFI", "100.", "",
			      Form("%f",fvTilt.at(i).at(j)[0]),
			      "", "", "", Form("tw_%d%02d",i,j) ) << endl;
	    }
	    //rot around y      
	    if(fvTilt.at(i).at(j)[1]!=0){
	      ss << PrintCard("ROT-DEFI", "200.", "",
			      Form("%f",fvTilt.at(i).at(j)[1]),
			      "", "", "", Form("tw_%d%02d",i,j) ) << endl;
	    }
	    //rot around z
	    if(fvTilt.at(i).at(j)[2]!=0){
	      ss << PrintCard("ROT-DEFI", "300.", "",
			      Form("%f",fvTilt.at(i).at(j)[2]),
			      "", "", "", Form("tw_%d%02d",i,j) ) << endl;
	    }
	  
	    //put back the bar into its position in local coord
	    ss << PrintCard("ROT-DEFI", "", "", "",
			    Form("%f",pos.X()), Form("%f",pos.Y()), Form("%f",pos.Z()),
			    Form("tw_%d%02d",i,j) ) << endl;
	  }

	  //check if detector has a tilt and then apply rot
	  if(angle.Mag()!=0){
	  
	    if(angle.X()!=0){
	      ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",angle.X()),"", "",
			      "", Form("tw_%d%02d",i,j)) << endl;
	    }
	    if(angle.Y()!=0){
	      ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",angle.Y()),"", "",
			      "", Form("tw_%d%02d",i,j)) << endl;
	    }
	    if(angle.Z()!=0){
	      ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",angle.Z()),"", "",
			      "", Form("tw_%d%02d",i,j)) << endl;
	    }
	  }
      
	  //put back the detector in global coord
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",center.X()), Form("%f",center.Y()),
			  Form("%f",center.Z()), Form("tw_%d%02d",i,j)) << endl;
	
	}	
      }
    }
  }

  return ss.str();

}

//_____________________________________________________________________________
string TATWparGeo::PrintBodies()
{
  
  stringstream ss;
  ss << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

  if(TAGrecoManager::GetPar()->IncludeTW()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3  center = fpFootGeo->GetTWCenter();
    TVector3  angle = fpFootGeo->GetTWAngles();
    TVector3 pos;

    string bodyname, regionname;
  
    ss << "* ***Scintillator bodies" << endl;
            
    for (int i=0; i<GetLayersN(); i++){
      for (int j=0; j<GetNBars(); j++){
	
 	if (fvTilt.at(i).at(j).Mag()!=0 || angle.Mag()!=0)
	  ss << "$start_transform " << Form("tw_%d%02d",i,j) << endl;
      	
	bodyname = Form("scn%d%02d",i,j);
	regionname = Form("SCN%d%02d",i,j);
	pos.SetXYZ( center.X() + GetBarPosition(i,j).X(),
		    center.Y() + GetBarPosition(i,j).Y(),
		    center.Z() + GetBarPosition(i,j).Z());
	ss <<  "RPP " << bodyname <<  "     "
	   << pos.x() - fBarSize.X()/2. << " "
	   << pos.x() + fBarSize.X()/2. << " "
	   << pos.y() - fBarSize.Y()/2. << " "
	   << pos.y() + fBarSize.Y()/2. << " "
	   << pos.z() - fBarSize.Z()/2. << " "
	   << pos.z() + fBarSize.Z()/2. << endl;
	fvBody.push_back(bodyname);
	fvRegion.push_back(regionname);

 	if (fvTilt.at(i).at(j).Mag()!=0 || angle.Mag()!=0)
	  ss << "$end_transform " << endl;
	
      } 
    }       
  }

  return ss.str();
}


//_____________________________________________________________________________
string TATWparGeo::PrintRegions()
{

  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeTW()){

    string name;

    ss << "* ***Scintillator regions" << endl;

    for(int i=0; i<fvRegion.size(); i++)
      ss << setw(13) << setfill( ' ' )  << std::left << fvRegion.at(i) << "5 " << fvBody.at(i) << endl;
    
  }
  return ss.str();
}

//_____________________________________________________________________________
Int_t TATWparGeo::GetRegStrip(Int_t lay, Int_t bar){
  TString regname;
  regname.Form("SCN%d%02d",lay,bar);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
string TATWparGeo::PrintSubtractBodiesFromAir() {

  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeTW()){

    for (int i=0; i<fvBody.size(); i++) {
      ss << " -" << fvBody.at(i);
      if ((i+1)%10==0 && (i+1)<(GetLayersN()*GetNBars())) ss << endl;
    }
    ss << endl;

  }
  
  return ss.str();

}


//_____________________________________________________________________________
string TATWparGeo::PrintAssignMaterial(TAGmaterials *Material)
{

  stringstream ss; 
  
  if(TAGrecoManager::GetPar()->IncludeTW()){

      TString flkmat;  
    
    if (Material == NULL){
      TAGmaterials::Instance()->PrintMaterialFluka();
      flkmat = TAGmaterials::Instance()->GetFlukaMatName(fBarMat.Data());
    }
    else
      flkmat = Material->GetFlukaMatName(fBarMat.Data());
      
    bool magnetic = false;
    if(TAGrecoManager::GetPar()->IncludeDI())
      magnetic = true;

    if (fvRegion.size()==0 )
      cout << "Error: TW regions vector not correctly filled!"<<endl;

    //    ss << PrintCard("ASSIGNMA", flkmat, fvRegion.at(0), fvRegion.back(),
    //		    "1.", Form("%d",magnetic), "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmat, fvRegion.at(0), fvRegion.back(),
		    "1.", "", "", "") << endl;
    
  }

  return ss.str();

}





//_____________________________________________________________________________
string TATWparGeo::PrintParameters() {

  stringstream ss;
  ss << setiosflags(ios::fixed) << setprecision(5);

  ss << "c     SCINTILLATOR PARAMETERS " << endl;
  ss << endl;
  
  string nstrip = "nstripSCN";
  ss << "      integer " << nstrip << endl;
  ss << "      parameter(" << nstrip << " = " << GetNBars() << ")" << endl;
  ss << endl;  

  return ss.str();

}



//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TATWparGeo::Clear(Option_t*)
{
  return;
}



/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TATWparGeo::ToStream(ostream& os, Option_t*) const
{
  //  os << "TATWparGeo " << GetName() << endl;
  //  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
  //     << "   ver_x   ver_y   ver_z  width" << endl;

  return;
}
