#include <iostream>
#include <fstream>
#include <Riostream.h>
#include <stdio.h>

#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TList.h"
#include "TMath.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TSystem.h"

#include "TAGgeoTrafo.hxx"
#include "TAGionisMaterials.hxx"
#include "TAGmaterials.hxx"

#include "TACAparGeo.hxx"
#include "TAGroot.hxx"

#include "GlobalPar.hxx"

//##############################################################################

const TString TACAparGeo::fgkDefParaName     = "caGeo";
const TString TACAparGeo::fgkBaseName        = "CA";
const Color_t TACAparGeo::fgkDefaultModCol   = kAzure+6;
const Color_t TACAparGeo::fgkDefaultModColOn = kRed-5;
const TString TACAparGeo::fgkDefaultCrysName = "caCrys";
const TString TACAparGeo::fgkDefaultModName     = "caMod";
const Int_t   TACAparGeo::fgkDefaultModulesN    = 32;
const Int_t   TACAparGeo::fgkCrystalsNperModule = 9;



//_____________________________________________________________________________
TACAparGeo::TACAparGeo() 
: TAGparTools(),
  fIonisation(new TAGionisMaterials())
{
   fkDefaultGeoName = "./geomaps/TACAdetector.map";
}

//______________________________________________________________________________
TACAparGeo::~TACAparGeo()
{
   delete   fIonisation;
}

//______________________________________________________________________________
Bool_t TACAparGeo::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   // Crystal
   ReadStrings(fCrystalMat);
   if(fDebugLevel)
      cout  << "   Crystals material : " <<  fCrystalMat << endl;
   
   ReadItem(fCrystalDensity);
   if(fDebugLevel)
      cout  << "   Crystals density : " <<  fCrystalDensity << endl;
   
   ReadItem(fCrystalIonisMat);
   if(fDebugLevel)
      cout  << "   Crystals mean exciation energy : " <<  fCrystalIonisMat << endl;
   
   ReadVector3(fCrystalSize);
   if(fDebugLevel)
      cout << "   Crystal size: "
      << Form("%f %f %f", fCrystalSize[0], fCrystalSize[1], fCrystalSize[2]) << endl;
   
   ReadItem(fCrystalDelta);
   if(fDebugLevel)
      cout  << "   Crystals in-between distance : " <<  fCrystalDelta << endl;

   
   // Support
   ReadStrings(fSupportMat);
   if(fDebugLevel)
      cout  << "   Support material : " <<  fSupportMat << endl;
   
   ReadItem(fSupportDensity);
   if(fDebugLevel)
      cout  << "   Support density : " <<  fSupportDensity << endl;
   
   ReadVector3(fSupportSize);
   if(fDebugLevel)
      cout << "   Support size: "
      << Form("%f %f %f", fSupportSize[0], fSupportSize[1], fSupportSize[2]) << endl;

   // define material
   DefineMaterial();
   
   TVector3 position;
   TVector3 tilt;
   
   ReadVector3(fCaloSize);
   if(fDebugLevel)
      cout << "   Calorimeter size: "
      << Form("%f %f %f", fCaloSize[0], fCaloSize[1], fCaloSize[2]) << endl;
   
   ReadItem(fModulesN);
   if (fDebugLevel)
      cout  << "Number of modules: " <<  fModulesN << endl;
   
   Int_t nModule = 0;
   
   SetupMatrices(fgkDefaultModulesN);
     
   // Read transformtion info
      for (Int_t iModule = 0; iModule < fModulesN; ++iModule) {
         
         ReadItem(nModule);
         if(fDebugLevel)
            cout  << "Module id "<< nModule << endl;
         
         // read  position
         ReadVector3(position);
         if(fDebugLevel)
            cout << "   Position: "
            << Form("%f %f %f", position[0], position[1], position[2]) << endl;
         
         ReadVector3(tilt);
         if(fDebugLevel)
            cout  << "   tilt: " << Form("%f %f %f", tilt[0], tilt[1], tilt[2]) << endl;

         vTilt.push_back(tilt);
         
         TGeoRotation rot;
         rot.RotateX(tilt[0]);
         rot.RotateY(tilt[1]);
         rot.RotateZ(tilt[2]);
         
         TGeoTranslation trans(position[0], position[1], position[2]);
         
         TGeoHMatrix  transfo;
         transfo  = trans;
         transfo *= rot;
         AddTransMatrix(new TGeoHMatrix(transfo), nModule);
      }

   ComputeCrystalPos();

   return true;
}

//_____________________________________________________________________________
TVector3 TACAparGeo::GetCrystalPosition(Int_t idx)
{
   if (idx < 0 || idx > fgkCrystalsNperModule) {
      Warning("GetCrystalPosition()","Wrong crystal id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return fCrystalPos[idx];
}

//_____________________________________________________________________________
TVector3 TACAparGeo::GetCrystalAngle(Int_t idx)
{
   if (idx < 0 || idx > fgkCrystalsNperModule) {
      Warning("GetCrystalAngle()","Wrong crystal id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return fCrystalAng[idx];
}

//_____________________________________________________________________________
TVector3 TACAparGeo::Module2Detector(Int_t idx, TVector3& loc) const
{
   if (idx < 0 || idx > fModulesN) {
      Warning("Module2Detector()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return LocalToMaster(idx, loc);
}


//_____________________________________________________________________________
TVector3 TACAparGeo::Module2DetectorVect(Int_t idx, TVector3& loc) const
{
   if (idx < 0 || idx > fModulesN) {
      Warning("Module2DetectorVect()","Wrong detector id number: %d ", idx);
      TVector3(0,0,0);
   }
   
   return LocalToMasterVect(idx, loc);
}

//_____________________________________________________________________________
TVector3 TACAparGeo::Detector2Module(Int_t idx, TVector3& glob) const
{
   if (idx < 0 || idx > fModulesN) {
      Warning("Detector2Module()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return MasterToLocal(idx, glob);
}

//_____________________________________________________________________________
TVector3 TACAparGeo::Detector2ModuleVect(Int_t idx, TVector3& glob) const
{
   if (idx < 0 || idx > fModulesN) {
      Warning("Detector2ModuleVect()","Wrong detector id number: %d ", idx);
      return TVector3(0,0,0);
   }
   
   return MasterToLocalVect(idx, glob);
}


//_____________________________________________________________________________
void TACAparGeo::DefineMaterial()
{
   
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // TW material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fCrystalMat, fCrystalDensity);
   if (fDebugLevel) {
      printf("Crystal material:\n");
      mat->Print();
   }
   
   // TW material
   TGeoMaterial* matSup = TAGmaterials::Instance()->CreateMaterial(fSupportMat, fSupportDensity);
   if (fDebugLevel) {
      printf("Support material:\n");
      matSup->Print();
   }
   
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)
   fIonisation->SetMaterial(mat);
   fIonisation->AddBirksFactor(fCrystalIonisMat);
   
#else
   fIonisation->SetMeanExcitationEnergy(fCrystalIonisMat);
   
   // put it under Cerenkov since only this EM property is available
   mat->SetCerenkovProperties(fIonisation);
   
#endif
}

//_____________________________________________________________________________
//! compute crystal positions in a module
void TACAparGeo::ComputeCrystalPos()
{
   Float_t xdim1 = fCrystalSize[0];
   Float_t xdim2 = fCrystalSize[1];
   Float_t ydim1 = xdim1; // assume squart
   Float_t ydim2 = xdim2;
   Float_t zdim  = fCrystalSize[2];
   
   // half open angle of the truncate piramide
   double deltaX    = (xdim2 - xdim1);
   double trp_hipot = TMath::Sqrt( zdim * zdim * 4  + deltaX * deltaX );
   double alfa      = TMath::ASin (deltaX / trp_hipot);
   //alfa += 0.0005; // small increase to correct the 1mm between the crystals
   double alfa_degree = alfa  * TMath::RadToDeg();
   
   // compute some values of the full piramid dimensions
   double piramid_hipot = xdim2 / TMath::Sin(alfa);
   //cout << "piramid_hipot " << piramid_hipot << endl;
   double piramid_base = piramid_hipot * TMath::Cos(alfa);
   double piramid_base_c = piramid_base - zdim; // distance from center to the piramid vertex
   
   // translation of crystal center after 2*alfa rotation about vetex piramid
   double delta  = fCrystalDelta; // 1mm between crystals
   double deltax = delta * TMath::Cos(alfa*2);
   double deltaz = -delta * TMath::Sin(alfa*2);
   double posx   = TMath::Sin(alfa*2) * piramid_base_c + deltax;
   double posz   = TMath::Cos(alfa*2) * piramid_base_c + deltaz;
   
   // set rotations/translations
   Float_t dirX[] = {1, 1, 0,-1,-1,-1, 0, 1, 0};
   Float_t dirY[] = {0, 1 ,1, 1, 0,-1,-1,-1, 0};
   Float_t angX[] = {0,-1,-1,-1, 0, 1, 1, 1, 0};
   Float_t angY[] = {1, 1, 0,-1,-1,-1, 0, 1, 0};

   for (Int_t i = 0; i < fgkCrystalsNperModule; ++i) {
      fCrystalAng[i] = TVector3(alfa_degree * 2 * angX[i], alfa_degree * 2 * angY[i], 0);
      fCrystalPos[i] = TVector3(posx*dirX[i], posx*dirY[i], posz - piramid_base_c);
   }
}

//_____________________________________________________________________________
TGeoVolume* TACAparGeo::BuildCalorimeter(const char *caName)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager(TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   TGeoVolume* wall = gGeoManager->FindVolumeFast(caName);
   if ( wall == 0x0 ) {
      TGeoMedium*  med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
      wall = gGeoManager->MakeBox(caName, med,  GetCrystalHeight()/2.,  GetCrystalHeight()/2., GetCrystalThick()/2.);
   }
   
   for (Int_t i = 0; i < fgkDefaultModulesN; ++i) {
      
      TGeoVolume* module = BuildModule(i);
      
      TGeoHMatrix* hm = GetTransfo(i);
      if (hm) {
         Double_t* mat = hm->GetRotationMatrix();
         Double_t* dis = hm->GetTranslation();
         
         TGeoRotation rot;
         rot.SetMatrix(mat);
         
         TGeoTranslation trans;
         trans.SetTranslation(dis[0], dis[1], dis[2]);
         
         wall->AddNode(module, i, new TGeoCombiTrans(trans, rot));
      }
   }
   
   return wall;
}


//_____________________________________________________________________________
//! build module
TGeoVolume* TACAparGeo::BuildModule(Int_t iMod)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   Float_t xdim1 = fCrystalSize[0];
   Float_t xdim2 = fCrystalSize[1];
   Float_t ydim1 = xdim1; // assume squart
   Float_t ydim2 = xdim2;
   Float_t zdim  = fCrystalSize[2];
   
   const Char_t* matName = fCrystalMat.Data();
   TGeoMedium*   med     = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
  // crystal->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
   ////////////   MODULE
   ////////////   Create a 3x3 modules
   TGeoVolumeAssembly* module = new TGeoVolumeAssembly(GetDefaultModName(iMod));

   TGeoRotation* rot     = new TGeoRotation();
   TGeoTranslation* tras = new TGeoTranslation();

   for (Int_t i = 0; i < fgkCrystalsNperModule; ++i) {
      TGeoVolume* crystal   = gGeoManager->MakeTrd2(GetDefaultCrysName(i, iMod), med, xdim1, xdim2, ydim1, ydim2, zdim);
      crystal->SetLineColor(fgkDefaultModCol);
      crystal->SetFillColor(fgkDefaultModCol);

      rot->Clear();
      rot->RotateX(fCrystalAng[i].X());
      rot->RotateY(fCrystalAng[i].Y());
      tras->SetTranslation(fCrystalPos[i].X(), fCrystalPos[i].Y(), fCrystalPos[i].Z());
      module->AddNode(crystal, i, new TGeoCombiTrans(*tras, *rot));
   }
   
   //---- Crystal Support as truncate piramid (just for visual propouses)
   double xdimS1 = fSupportSize[0] ;
   double xdimS2 = fSupportSize[1];
   double ydimS1 = xdimS1;
   double ydimS2 = xdimS2;
   double zdimS  = fSupportSize[2];

   const Char_t* matSupName = fSupportMat.Data();
   TGeoMedium*   medSup     = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matSupName);

   TGeoVolume* support = gGeoManager->MakeTrd2("modSup", medSup, xdimS1, xdimS2, ydimS1, ydimS2, zdimS);
   support->SetLineColor(kGray);
   support->SetLineColor(kGray);
   module->AddNode(support, 0, new TGeoTranslation(0, 0, zdimS - 0.1));
   
   return module;
}

//_____________________________________________________________________________
//! set color on for fired bars
void TACAparGeo::SetCrystalColorOn(Int_t idx, Int_t iMod)
{
   if (!gGeoManager) {
      Error("SetBarcolorOn()", "No Geo manager defined");
      return;
   }
   
   TString name = GetDefaultCrysName(idx, iMod);
   
   TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
   if (vol)
      vol->SetLineColor(GetDefaultModColOn());
}

//_____________________________________________________________________________
//! reset color for unfired bars
void TACAparGeo::SetCrystalColorOff(Int_t idx, Int_t iMod)
{
   if (!gGeoManager) {
      Error("SetBarcolorOn()", "No Geo manager defined");
      return;
   }
   
   TString name = GetDefaultCrysName(idx, iMod);
   
   TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
   if (vol) {
      vol->SetLineColor(GetDefaultModCol());
   }
}


//_____________________________________________________________________________
string TACAparGeo::PrintRotations()
{
  stringstream ss;

  if(GlobalPar::GetPar()->IncludeCA()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3 fCenter = fpFootGeo->GetCACenter();
    TVector3  fAngle = fpFootGeo->GetCAAngles();
    
     for(int iCry=0; iCry<GetCrystalsN(); iCry++) {

      //check if crystal or detector have a tilt
      if (vTilt.at(iCry).Mag()!=0 || fAngle.Mag()!=0){

	//put the detector in local coord before the rotation
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",-fCenter.X()),
			Form("%f",-fCenter.Y()),
			Form("%f",-fCenter.Z()),
			Form("ca_%d",iCry) ) << endl;

	//check if crystal has a tilt
	if (vTilt.at(iCry).Mag()!=0){
	  
	  // put the crystal in 0,0,0 before the crystal's rot
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",-GetCrystalPosition(iCry).X()),
			  Form("%f",-GetCrystalPosition(iCry).Y()),
			  Form("%f",-GetCrystalPosition(iCry).Z()),
			  Form("ca_%d",iCry) ) << endl;
	  //rot around x
	  if(vTilt.at(iCry)[0]!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "",
			    Form("%f",vTilt.at(iCry)[0]),
			    "", "", "", Form("ca_%d",iCry) ) << endl;
	  }
	  //rot around y      
	  if(vTilt.at(iCry)[1]!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "",
			    Form("%f",vTilt.at(iCry)[1]),
			    "", "", "", Form("ca_%d",iCry) ) << endl;
	  }
	  //rot around z
	  if(vTilt.at(iCry)[2]!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "",
			    Form("%f",vTilt.at(iCry)[2]),
			    "", "", "", Form("ca_%d",iCry) ) << endl;
	  }
	  
	  //put back the crystal into its position in local coord
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",GetCrystalPosition(iCry).X()),
			  Form("%f",GetCrystalPosition(iCry).Y()),
			  Form("%f",GetCrystalPosition(iCry).Z()),
			  Form("ca_%d",iCry) ) << endl;
	}
      
	//check if detector has a tilt and then apply rot
	if(fAngle.Mag()!=0){
	  
	  if(fAngle.X()!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",fAngle.X()),"", "", 
			    "", Form("ca_%d",iCry)) << endl;
	  }
	  if(fAngle.Y()!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",fAngle.Y()),"", "", 
			    "", Form("ca_%d",iCry)) << endl;
	  }
	  if(fAngle.Z()!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",fAngle.Z()),"", "", 
			    "", Form("ca_%d",iCry)) << endl;
	  }
	}
      
	//put back the detector in global coord
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",fCenter.X()), Form("%f",fCenter.Y()),
			Form("%f",fCenter.Z()), Form("ca_%d",iCry)) << endl;
      }
     }
  }

  return ss.str();

}

//_____________________________________________________________________________
string TACAparGeo::PrintBodies(){
   
   stringstream ss;
   
   if ( GlobalPar::GetPar()->IncludeCA()){   

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3  fCenter = fpFootGeo->GetCACenter();
    TVector3  fAngle = fpFootGeo->GetCAAngles();
   
     ss << "* ***Calorimeter" << endl;

     for (int iCal=0; iCal<GetCrystalsN(); iCal++){
       
       if(vTilt.at(iCal).Mag()!=0 || fAngle.Mag()!=0)
	 ss << "$start_transform " << Form("ca_%d",iCal) << endl;
     
       string bodyname, regionname;

       bodyname = Form("cal%d",iCal);
       regionname = Form("CAL%d",iCal);
       vBody.push_back(bodyname);
       vRegion.push_back(regionname);

       ss << "RPP " << bodyname <<  "     " 
	      <<  fCenter.X() + GetCrystalPosition(iCal).X() - GetCrystalSize().X()/2.<< " "	
	      <<  fCenter.X() + GetCrystalPosition(iCal).X() + GetCrystalSize().X()/2.<< " "
	      <<  fCenter.Y() + GetCrystalPosition(iCal).Y() - GetCrystalSize().Y()/2.<< " "
	      <<  fCenter.Y() + GetCrystalPosition(iCal).Y() + GetCrystalSize().Y()/2.<< " "
	      <<  fCenter.Z() + GetCrystalPosition(iCal).Z() - GetCrystalSize().Z()/2.<< " "
	      <<  fCenter.Z() + GetCrystalPosition(iCal).Z() + GetCrystalSize().Z()/2.<< endl;
       
       if(vTilt.at(iCal).Mag()!=0 || fAngle.Mag()!=0)
       ss << "$end_transform" << endl;
	 
     }
     
   }
   
   return ss.str();
}



//_____________________________________________________________________________
string TACAparGeo::PrintRegions(){

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeCA()){

    string name;

    ss << "* ***Calorimeter regions" << endl;

    for(int i=0; i<vRegion.size(); i++) 
      ss << setw(13) << setfill( ' ' )  << std::left << vRegion.at(i) << "5 " << vBody.at(i) << endl;
  
  }
  
  return ss.str();

}


//_____________________________________________________________________________
string TACAparGeo::PrintParameters() {
  
  stringstream ss;
  ss << setiosflags(ios::fixed) << setprecision(5);

  ss << "c     CALORIMETER PARAMETERS " << endl;
  ss << endl;    
  
  string ncrystal = "ncryCAL";
  ss << "      integer " << ncrystal << endl;
  ss << "      parameter(" << ncrystal << " = " << fCrystalsN << ")" << endl;
  ss << endl;    
  
  return ss.str();

}
//_____________________________________________________________________________
string TACAparGeo::PrintAssignMaterial() {

  stringstream ss;
  
  if(GlobalPar::GetPar()->IncludeCA()){

    const Char_t* mat = fCrystalMat.Data();
      
    bool magnetic = false;
    if(GlobalPar::GetPar()->IncludeDI())
      magnetic = true;

    if (vRegion.size()==0 )
      cout << "Error: CA regions vector not correctly filled!" << endl;

    ss << PrintCard("ASSIGNMA", mat, vRegion.at(0), vRegion.back(),
		    "1.", Form("%d",magnetic), "", "") << endl;

  }

  return ss.str();
  
}



//_____________________________________________________________________________
string TACAparGeo::PrintSubtractBodiesFromAir() {

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeCA()){

    for(int i=0; i<vBody.size(); i++) {
      ss << " -" << vBody.at(i);
      if ((i+1)%10==0 && (i+1)<GetCrystalsN()) ss << endl;
    }
    ss << endl;

  }

    return ss.str();
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.

void TACAparGeo::Clear(Option_t*)
{
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TACAparGeo::ToStream(ostream& os, Option_t*) const
{
  //  os << "TACAparGeo " << GetName() << endl;
  //  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
  //     << "   ver_x   ver_y   ver_z  width" << endl;

  return;
}

