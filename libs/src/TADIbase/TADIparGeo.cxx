
/*!
 \file TADIparGeo.cxx
 \brief Implementation of TADIparGeo.cxx
 */

#include <Riostream.h>

#include "TGeoBBox.h"
#include "TGeoTube.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TList.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"

#include "TAGrecoManager.hxx"
#include "TAGgeoTrafo.hxx"
#include "TADIparGeo.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \class TADIparGeo
  \brief Map and Geometry parameters for magnets. **
*/

//! Class Imp
ClassImp(TADIparGeo);

const TString TADIparGeo::fgkDefParaName     = "diGeo";
      TString TADIparGeo::fgDefaultGeoName = "./geomaps/TADIdetector.geo";
const TString TADIparGeo::fgkDevBaseName   = "DI";
const Int_t   TADIparGeo::fgkDefMagnetsN   = 2;

//______________________________________________________________________________
//! Standard constructor
TADIparGeo::TADIparGeo()
: TAGparTools(),
  fMagnetsN(0),
  fType(-1),
  fMapName("")
{
}

//______________________________________________________________________________
//! Destructor
TADIparGeo::~TADIparGeo()
{
}

//------------------------------------------+-----------------------------------
//! Read from file
//!
//! \param[in] name file name
Bool_t TADIparGeo::FromFile(const TString& name)
{
   // simple file reading, waiting for real config file
   TString nameExp;
   
   if (name.IsNull())
	  nameExp = fgDefaultGeoName;
   else 
	  nameExp = name;

   if (!Open(nameExp)) return false;
   Info("FromFile()", "Open file %s for geometry", name.Data());

   ReadItem(fMagnetsN);  
   if(FootDebugLevel(1))
	  cout << endl << "Magnets number "<< fMagnetsN << endl;
   
   // read Magnet index
	  ReadItem(fType);
	  if(FootDebugLevel(1))
        cout  << "   Type of Magnet: " <<  fType << endl;
   
   if (fType == 2) {
      ReadStrings(fMapName);
      if(FootDebugLevel(1))
         cout  << "   Map File Name:  "<< fMapName.Data() << endl;
      
      ReadItem(fMapMesh);
      if(FootDebugLevel(1))
         cout  << "   Map mesh size:  "<< fMapMesh << endl;
      
      ReadVector3(fMapLimLo);
      if(FootDebugLevel(1))
         cout << "   Field map lower bound: "
         << Form("%f %f %f", fMapLimLo[0], fMapLimLo[1], fMapLimLo[2]) << endl;
      
      ReadVector3(fMapLimUp);
      if(FootDebugLevel(1))
         cout << "   Field map upper bound: "
         << Form("%f %f %f", fMapLimUp[0], fMapLimUp[1], fMapLimUp[2]) << endl;
      
      Info("FromFile()", "with field map file %s\n", fMapName.Data());
   }
  
   if (fType == 0) {
     ReadVector3(fMagCstValue);
     if(FootDebugLevel(1))
       cout << "   Field constant value "
       << Form("%f %f %f", fMagCstValue[0], fMagCstValue[1], fMagCstValue[2]) << endl;
      Info("FromFile()", "with constant field (%f %f %f)\n", fMagCstValue[0], fMagCstValue[1], fMagCstValue[2]);

   }
  

   // Read cover thickness
   ReadItem(fShieldThick);
   if(FootDebugLevel(1))
      cout << endl << "   Shield thickness: "<< fShieldThick << endl;
   
   // Read cover material
   ReadStrings(fShieldMat);
   if(FootDebugLevel(1))
      cout  << "   Shield material:  "<< fShieldMat.Data() << endl;

   // read cover material density
	  ReadItem(fShieldDensity);
	  if(FootDebugLevel(1))
        cout  << "   Shield material density: " <<  fShieldDensity << endl;

   // Read magnet material
   ReadStrings(fMagMat);
   if(FootDebugLevel(1))
      cout  << "   Magnet material:  "<< fMagMat.Data() << endl;
   
   // read magnet material density
	  ReadItem(fMagDensity);
	  if(FootDebugLevel(1))
        cout  << "   Magnet material density: " <<  fMagDensity << endl;


   if(FootDebugLevel(1))
      cout << endl << "Reading Magnet Parameters " << endl;
   
   SetupMatrices(fMagnetsN);

   for (Int_t p = 0; p < fMagnetsN; p++) { // Loop on each plane
	  
	  // read Magnet index
	  ReadItem(fMagnetParameter[p].MagnetIdx);
	  if(FootDebugLevel(1))
		 cout << endl << " - Parameters of Magnet " <<  fMagnetParameter[p].MagnetIdx << endl;
      
      // read Magnet size
      ReadVector3(fMagnetParameter[p].Size);
      if(FootDebugLevel(1))
         cout << "   Size: "
         << Form("%f %f %f", fMagnetParameter[p].Size[0], fMagnetParameter[p].Size[1], fMagnetParameter[p].Size[2]) << endl;

      fMagnetParameter[p].ShieldSize[0] = fMagnetParameter[p].Size[0] - fShieldThick;
      fMagnetParameter[p].ShieldSize[1] = fMagnetParameter[p].Size[1] + fShieldThick;
      fMagnetParameter[p].ShieldSize[2] = fMagnetParameter[p].Size[2] + 2*fShieldThick;
      
      if(FootDebugLevel(1))
         cout << "   Shield Size: "
         << Form("%f %f %f", fMagnetParameter[p].ShieldSize[0], fMagnetParameter[p].ShieldSize[1], fMagnetParameter[p].ShieldSize[2]) << endl;
      

	  // read Magnet position
	  ReadVector3(fMagnetParameter[p].Position);
	  if(FootDebugLevel(1))
		 cout << "   Position: "
	         << Form("%f %f %f", fMagnetParameter[p].Position[0], fMagnetParameter[p].Position[1], fMagnetParameter[p].Position[2]) << endl;
	  
	  // read Magnet angles
	  ReadVector3(fMagnetParameter[p].Tilt);
	  if(FootDebugLevel(1))
		 cout  << "   Tilt: "
		       << Form("%f %f %f", fMagnetParameter[p].Tilt[0], fMagnetParameter[p].Tilt[1], fMagnetParameter[p].Tilt[2]) << endl;
	  
      Float_t thetaX = fMagnetParameter[p].Tilt[0];
      Float_t thetaY = fMagnetParameter[p].Tilt[1];
      Float_t thetaZ = fMagnetParameter[p].Tilt[2];

      TGeoRotation rot;
	  rot.RotateX(thetaX);   
	  rot.RotateY(thetaY);   
	  rot.RotateZ(thetaZ);
      
     Float_t transX = fMagnetParameter[p].Position[0];
     Float_t transY = fMagnetParameter[p].Position[1];
     Float_t transZ = fMagnetParameter[p].Position[2];
      
     TGeoTranslation trans(transX, transY, transZ);
	  
	  TGeoHMatrix  transfo;
	  transfo  = trans;
	  transfo *= rot;
	  AddTransMatrix(new TGeoHMatrix(transfo), fMagnetParameter[p].MagnetIdx-1);
	 
	 // change to rad
	 fMagnetParameter[p].Tilt[0] = fMagnetParameter[p].Tilt[0]*TMath::DegToRad();
	 fMagnetParameter[p].Tilt[1] = fMagnetParameter[p].Tilt[1]*TMath::DegToRad();
	 fMagnetParameter[p].Tilt[2] = fMagnetParameter[p].Tilt[2]*TMath::DegToRad();
   }	  

   // define material
   DefineMaterial();

   // Close file
   Close();
   
   return true;
}

//_____________________________________________________________________________
//! Define material
void TADIparGeo::DefineMaterial()
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // Shield material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fShieldMat, fShieldDensity);
    if(FootDebugLevel(1)) {
      printf("Shield material:\n");
      mat->Print();
   }

   
   // Magnet material
   mat = TAGmaterials::Instance()->CreateMaterial(fMagMat, fMagDensity);
   if(FootDebugLevel(1)) {
      printf("Magnet material:\n");
      mat->Print();
   }
}

//_____________________________________________________________________________
//! Get magnet position
//!
//! \param[in] iMagnet a given magnet
TVector3 TADIparGeo::GetPosition(Int_t iMagnet)
{
   TGeoHMatrix* hm = GetTransfo(iMagnet);
   if (hm) {
	  TVector3 local(0,0,0);
	  fCurrentPosition =  Local2Global(iMagnet,local);
   }
   return fCurrentPosition;
}

//_____________________________________________________________________________
//! Transformation from global detector to local magnet framework
//!
//! \param[in] detID magnet id
//! \param[in] xg X position in detector framework
//! \param[in] yg Y position in detector framework
//! \param[in] zg Z position in detector framework
//! \param[out] xl X position in magnet framework
//! \param[out] yl Y position in magnet framework
//! \param[out] zl Z position in magnet framework
void TADIparGeo::Global2Local(Int_t detID,
									Double_t xg, Double_t yg, Double_t zg, 
									Double_t& xl, Double_t& yl, Double_t& zl) const
{  
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Global2Local()","Wrong detector id number: %d ", detID); 
	  return ;
   }
   
   MasterToLocal(detID, xg, yg, zg, xl, yl, zl);
}   

//_____________________________________________________________________________
//! Transformation from global detector to local magnet framework
//!
//! \param[in] detID magnet id
//! \param[in] glob position in detector framework
//! \return position in magnet framework
TVector3 TADIparGeo::Global2Local(Int_t detID, TVector3& glob) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Global2Local()","Wrong detector id number: %d ", detID); 
	  return TVector3(0,0,0);
   }
   
   return MasterToLocal(detID, glob);
}

//_____________________________________________________________________________
//! Transformation from global detector to local magnet framework for vector (no translation)
//!
//! \param[in] detID magnet id
//! \param[in] glob position in detector framework
//! \return position in magnet framework
TVector3 TADIparGeo::Global2LocalVect(Int_t detID, TVector3& glob) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Global2LocalVect()","Wrong detector id number: %d ", detID); 
	  return TVector3(0,0,0);
   }
   
   return MasterToLocalVect(detID, glob);
}   

//_____________________________________________________________________________
//! Transformation from local magnet to global detector framework
//!
//! \param[in] detID magnet id
//! \param[in] xl X position in magnet framework
//! \param[in] yl Y position in magnet framework
//! \param[in] zl Z position in magnet framework
//! \param[out] xg X position in detector framework
//! \param[out] yg Y position in detector framework
//! \param[out] zg Z position in detector framework
void TADIparGeo::Local2Global(Int_t detID,
									Double_t xl, Double_t yl, Double_t zl, 
									Double_t& xg, Double_t& yg, Double_t& zg) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Local2Global()","Wrong detector id number: %d ", detID); 
	  return;
   }
   
   LocalToMaster(detID, xl, yl, zl, xg, yg, zg);
}

//_____________________________________________________________________________
//! Transformation from local magnet to global detector framework
//!
//! \param[in] detID magnet id
//! \param[in] loc position in magnet framework
//! \return position in detector framework
TVector3 TADIparGeo::Local2Global(Int_t detID, TVector3& loc) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Local2Global()","Wrong detector id number: %d ", detID); 
	  TVector3(0,0,0);
   }
   
   return LocalToMaster(detID, loc);
}

//_____________________________________________________________________________
//! Transformation from local magnet to global detector framework for vector (no translation)
//!
//! \param[in] detID magnet id
//! \param[in] loc position in magnet framework
//! \return position in detector framework
TVector3 TADIparGeo::Local2GlobalVect(Int_t detID, TVector3& loc) const
{
   if (detID < 0 || detID > GetMagnetsN()) {
	  Warning("Local2GlobalVect()","Wrong detector id number: %d ", detID); 
	  TVector3(0,0,0);
   }

   return LocalToMasterVect(detID, loc);
}

//_____________________________________________________________________________
//! Build vertex in Root geometry
//!
//! \param[in] basemoduleName  base module name for magnet
//! \param[in] magnetName dipole volume name
TGeoVolume* TADIparGeo::BuildMagnet(const char* basemoduleName, const char *magnetName)
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
	  new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   TGeoVolume* magnet = gGeoManager->FindVolumeFast(magnetName);
   if ( magnet == 0x0 ) {
      TGeoMedium*  med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject("AIR");
      magnet = gGeoManager->MakeBox(magnetName,med,fSizeBox.X()/2.,fSizeBox.Y()/2.,fSizeBox.Z()/2.); // volume corresponding to vertex
   }
   
   // create module
   const Char_t* matName = fMagMat.Data();
   TGeoMedium*  medMod = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);

   TGeoVolume* magnetMod = 0x0;
   for(Int_t iMag = 0; iMag < GetMagnetsN(); ++iMag) {
      TVector3 size = GetMagnetPar(iMag).Size;

      TGeoCombiTrans* hm = GetCombiTransfo(iMag);
      TGeoTube* tube = new TGeoTube(Form("TubeMagnet%d", iMag+1), size[0], size[1], size[2]/2.);
      
      TGeoVolume *magnetMod = new TGeoVolume(Form("%s_Magnet",basemoduleName), tube, medMod);
      magnetMod->SetLineColor(kBlue-3);
      magnetMod->SetTransparency( TAGgeoTrafo::GetDefaultTransp());
      
      magnet->AddNode(magnetMod, iMag, hm);
   }

   return magnet;
}

//_____________________________________________________________________________
//! Compute envelop of detector
void TADIparGeo::DefineMaxMinDimension()
{
   TVector3 size(0, 0, 0);
   TVector3 posAct(0, 0, 0);
   
   Int_t nSens = GetMagnetsN();
   
   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   for (Int_t iS = 0; iS < nSens; iS++) {
      posAct = GetMagnetPar(iS).Position;
      size  = GetMagnetPar(iS).Size;
      
      for(Int_t i = 0; i < 3; i++) {
         minPosition[i] = (minPosition[i] <= posAct[i] ) ? minPosition[i] : posAct[i];
         maxPosition[i] = (maxPosition[i] >= posAct[i] ) ? maxPosition[i] : posAct[i];
      }
   }
   
   minPosition[0] -= size[1];    maxPosition[0] += size[1];
   minPosition[1] -= size[1];    maxPosition[1] += size[1];
   minPosition[2] -= size[2]/2.; maxPosition[2] += size[2]/2.;
   
   fMinPosition = minPosition;
   fMaxPosition = maxPosition;
   
   for(Int_t i = 0; i< 3; ++i)
      fSizeBox[i] = (fMaxPosition[i] - fMinPosition[i]);
}

//_____________________________________________________________________________
//! Print Fluka rotations
string TADIparGeo::PrintRotations()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeDI()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3 fCenter = fpFootGeo->GetDICenter();
    TVector3  fAngle = fpFootGeo->GetDIAngles();
    
     for(int iMag=0; iMag<GetMagnetsN(); iMag++) {

      //check if a dipole or the magnets system have a tilt
       if (fMagnetParameter[iMag].Tilt.Mag()!=0 || fAngle.Mag()!=0){

	//put the magnets system in local coord before the rotation
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",-fCenter.X()),
			Form("%f",-fCenter.Y()),
			Form("%f",-fCenter.Z()),
			Form("di_%d",iMag) ) << endl;

	//check if a dipole has a tilt
	if (fMagnetParameter[iMag].Tilt.Mag()!=0){
	  
	  // put the a dipole in 0,0,0 before the crystal's rot
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",-GetMagnetPar(iMag).Position.X()),
			  Form("%f",-GetMagnetPar(iMag).Position.Y()),
			  Form("%f",-GetMagnetPar(iMag).Position.Z()),
			  Form("di_%d",iMag) ) << endl;
	  //rot around x
	  if(fMagnetParameter[iMag].Tilt[0]!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "",
			    Form("%f",fMagnetParameter[iMag].Tilt[0]*TMath::RadToDeg()),
			    "", "", "", Form("di_%d",iMag) ) << endl;
	  }
	  //rot around y      
	  if(fMagnetParameter[iMag].Tilt[1]!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "",
			    Form("%f",fMagnetParameter[iMag].Tilt[1]*TMath::RadToDeg()),
			    "", "", "", Form("di_%d",iMag) ) << endl;
	  }
	  //rot around z
	  if(fMagnetParameter[iMag].Tilt[2]!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "",
			    Form("%f",fMagnetParameter[iMag].Tilt[2]*TMath::RadToDeg()),
			    "", "", "", Form("di_%d",iMag) ) << endl;
	  }
	  
	  //put back the a dipole into its position in local coord
	  ss << PrintCard("ROT-DEFI", "", "", "",
			  Form("%f",GetMagnetPar(iMag).Position.X()),
			  Form("%f",GetMagnetPar(iMag).Position.Y()),
			  Form("%f",GetMagnetPar(iMag).Position.Z()),
			  Form("di_%d",iMag) ) << endl;
	}
      
	//check if the magnets system has a tilt and then apply rot
	if(fAngle.Mag()!=0){
	  
	  if(fAngle.X()!=0){
	    ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",fAngle.X()),"", "", 
			    "", Form("di_%d",iMag)) << endl;
	  }
	  if(fAngle.Y()!=0){
	    ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",fAngle.Y()),"", "", 
			    "", Form("di_%d",iMag)) << endl;
	  }
	  if(fAngle.Z()!=0){
	    ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",fAngle.Z()),"", "", 
			    "", Form("di_%d",iMag)) << endl;
	  }
	}
      
	//put back the magnets system in global coord
	ss << PrintCard("ROT-DEFI", "", "", "",
			Form("%f",fCenter.X()), Form("%f",fCenter.Y()),
			Form("%f",fCenter.Z()), Form("di_%d",iMag)) << endl;
      }
     }
  }

  return ss.str();
}


//_____________________________________________________________________________
//! Print Fluka bodies
string TADIparGeo::PrintBodies()
{
   stringstream ss;
   
   if ( TAGrecoManager::GetPar()->IncludeDI()){   

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
  
    TVector3  fCenter = fpFootGeo->GetDICenter();
    TVector3  fAngle = fpFootGeo->GetDIAngles();
    
    string bodyname, regionname;
   
     ss << "* ***Dipoles bodies" << endl;
     
     for (int iMag=0; iMag<GetMagnetsN(); iMag++){
       
       if(fMagnetParameter[iMag].Tilt.Mag()!=0 || fAngle.Mag()!=0)
	 ss << "$start_transform " << Form("di_%d",iMag) << endl; 
       
       regionname = Form("MAG_SH%d",iMag);
       fvRegShield.push_back(regionname);
       
       //shield outer body
       bodyname = Form("MagShOu%d",iMag);
       fvBodyOut.push_back(bodyname);
       ss << "RCC " << bodyname <<  "     " 
	  << fCenter.X() + GetMagnetPar(iMag).Position.X() << " "	
	  << fCenter.Y() + GetMagnetPar(iMag).Position.Y() << " "
	  << fCenter.Z() + GetMagnetPar(iMag).Position.Z() - GetMagnetPar(iMag).ShieldSize.Z()/2.
	  << " 0.000000 0.000000 " << GetMagnetPar(iMag).ShieldSize.Z() << " "
	  << GetMagnetPar(iMag).ShieldSize.Y() << endl;

       //shield inner body
       bodyname = Form("MagShIn%d",iMag);
       fvBodyIn.push_back(bodyname);
       ss << "ZCC " << bodyname <<  "     " 
	  << fCenter.X() + GetMagnetPar(iMag).Position.X() << " "	
	  << fCenter.Y() + GetMagnetPar(iMag).Position.Y() << " "
	  << GetMagnetPar(iMag).ShieldSize.X() << endl;

       regionname = Form("MAG%d",iMag);
       fvReg.push_back(regionname);

       //magnet outer body
       ss << "RCC MagOu" << iMag <<  "     " 
	  << fCenter.X() + GetMagnetPar(iMag).Position.X() << " "	
	  << fCenter.Y() + GetMagnetPar(iMag).Position.Y() << " "
	  << fCenter.Z() + GetMagnetPar(iMag).Position.Z() - GetMagnetPar(iMag).Size.Z()/2.
	  << " 0.000000 0.000000 " << GetMagnetPar(iMag).Size.Z() << " "
	  << GetMagnetPar(iMag).Size.Y() << endl;

       //magnet inner body
       ss << "RCC MagIn" << iMag <<  "     " 
	  << fCenter.X() + GetMagnetPar(iMag).Position.X() << " "	
	  << fCenter.Y() + GetMagnetPar(iMag).Position.Y() << " "
	  << fCenter.Z() + GetMagnetPar(iMag).Position.Z() - GetMagnetPar(iMag).Size.Z()/2.
	  << " 0.000000 0.000000 " << GetMagnetPar(iMag).Size.Z() << " "
	  << GetMagnetPar(iMag).Size.X() << endl;
       
      if(fMagnetParameter[iMag].Tilt.Mag()!=0 || fAngle.Mag()!=0)
	ss << "$end_transform " << endl;
      
     }
     
   }
   return ss.str();
}

//_____________________________________________________________________________
//! Print Fluka regions
string TADIparGeo::PrintRegions(){

  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeDI()){

    string name;

    ss << "* ***Dipoles regions" << endl;
   

    if (fvReg.size()==0 || fvRegShield.size()==0)
      cout << "Error: DI regions vectors not correctly filled!" << endl;
       
    for (int iMag=0; iMag<fvReg.size(); iMag++)
      ss << fvReg.at(iMag) << "         5 MagOu" << iMag << " -MagIn" << iMag << endl;
       
       
    for (int iMag=0; iMag<fvRegShield.size(); iMag++){
      ss << fvRegShield.at(iMag) << "      5 " << fvBodyOut.at(iMag) << " -(MagOu" << iMag
	 << " -MagIn" << iMag << ") -" << fvBodyIn.at(iMag) << endl;
    }

  }
  
  return ss.str();

}

//_____________________________________________________________________________
//! Get magnet region in Fluka for a given layer
//!
//! \param[in] n magnet index
Int_t TADIparGeo::GetRegMagnet(Int_t n)
{
  TString regname;
  regname.Form("MAG%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get magnet shieldregion in Fluka for a given layer
//!
//! \param[in] n shield index
Int_t TADIparGeo::GetRegShield(Int_t n)
{
  TString regname;
  regname.Form("MAG_SH%d",n);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Print assigned material in Fluka for a given material
//!
//! \param[in] Material Root material
string TADIparGeo::PrintAssignMaterial(TAGmaterials* Material)
{
  stringstream ss;
  
  if(TAGrecoManager::GetPar()->IncludeDI()){

    TString flkmatMag, flkmatShi;  
    
    if (Material == NULL){
      TAGmaterials::Instance()->PrintMaterialFluka();
      flkmatMag = TAGmaterials::Instance()->GetFlukaMatName(GetMagMat().Data());
      flkmatShi = TAGmaterials::Instance()->GetFlukaMatName(GetShieldMat().Data());
    }else{
      flkmatMag = Material->GetFlukaMatName(GetMagMat().Data());
      flkmatShi = Material->GetFlukaMatName(GetShieldMat().Data());
    }
      
    if (fvReg.size()==0 || fvRegShield.size()==0)
      cout << "Error: DI regions vectors not correctly filled!" << endl;

    ss << PrintCard("ASSIGNMA", flkmatMag, fvReg.at(0), fvReg.back(),
    		    "1.", "1.", "", "") << endl;
    ss << PrintCard("ASSIGNMA", flkmatShi, fvRegShield.at(0), fvRegShield.back(),
    		    "1.", "1.", "", "") << endl;
    
  }

  return ss.str();
}

//_____________________________________________________________________________
//! Print subtracted bodies from air in Fluka
string TADIparGeo::PrintSubtractBodiesFromAir()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeDI()){

    if (fvBodyOut.size()==0 || fvBodyIn.size()==0 || fvBodyOut.size()!=fvBodyIn.size())
      cout << "Error: DI body vectors not correctly filled!" << endl;
    
    for(int i=0; i<fvBodyOut.size(); i++) {
      ss << " -(" << fvBodyOut.at(i) << " -" << fvBodyIn.at(i) << ")" ;
    }
    ss << endl;

  }

    return ss.str();
}

//_____________________________________________________________________________
//!
//! Print Fluka parameters
 string TADIparGeo::PrintParameters()
 {   
   stringstream outstr;
    
   if(TAGrecoManager::GetPar()->IncludeDI()){
     
     string precision = "D+00";
     
     TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   
     TVector3  fCenter = fpFootGeo->GetDICenter();
     outstr << setiosflags(ios::fixed) << setprecision(5);
    
     outstr << "c     MAGNETS PARAMETERS " << endl;
     outstr << endl;
    
     outstr << "      double precision MagCenterX, MagCenterY, MagCenterZ" << endl;
     outstr << "      parameter (MagCenterX=" << fCenter.X() << precision << ")" << endl;
     outstr << "      parameter (MagCenterY=" << fCenter.Y() << precision << ")" << endl;
     outstr << "      parameter (MagCenterZ=" << fCenter.Z() << precision << ")" << endl;
     
     outstr << "      character*50 mapname" << endl;
     outstr << "      parameter (mapname='" << fMapName.Data() << "')" << endl;
     
        
     outstr << endl;
   }
    
   return outstr.str();
 }
