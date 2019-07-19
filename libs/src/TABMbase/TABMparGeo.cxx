/*!
  \file
  \version $Id: TABMparGeo.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TABMparGeo.
*/

#include <string.h>
#include <math.h>

#include <fstream>
#include <sstream>
#include "TEveTrans.h"
#include "TGeoBBox.h"
#include "TSystem.h"
#include "TString.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TList.h"
#include "TGeoBBox.h"
#include "TColor.h"
#include "TGeoManager.h"
#include "TROOT.h"
#include "TFile.h"


#include "TAGroot.hxx"
#include "TAGmaterials.hxx"
#include "TAGgeoTrafo.hxx" 
#include "TABMparGeo.hxx"

using namespace std;

//##############################################################################

/*!
  \class TABMparGeo TABMparGeo.hxx "TABMparGeo.hxx"
  \brief Map and Geometry parameters for the beam monitor. **
*/

const TString TABMparGeo::fgkDefParaName = "bmGeo";
const TString TABMparGeo::fgkBaseName = "BM";
Int_t TABMparGeo::fgkLayerOffset      = 6;

ClassImp(TABMparGeo);

///------------------------------------------+-----------------------------------
//! Default constructor.
TABMparGeo::TABMparGeo()
:  TAGparTools(),
   fWireLayersN(0),
   fLayersN(0),
   fSensesN(0),
   fSenseRadius(0),
   fSenseMat(""),
   fSenseDensity(0.0),
   fFieldRadius(0),
   fFieldMat(""),
   fFieldDensity(0.),
   fFoilThick(0),
   fFoilMat(""),
   fFoilDensity(0.),
   fShieldThick(0.),
   fBmStep(0),
   fBmCellWide(0),
   fBmDplane(0),
   fGasMixture(""),
   fGasProp(""),
   fGasDensities(""),
   fGasDensity(0.),
   fBmDeltaZ(0),
   fBmDeltaY(0),
   fBmDeltaX(0)
{
   fkDefaultGeoName = "./geomaps/TABMdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.
TABMparGeo::~TABMparGeo()
{
}

//_____________________________________________________________________________
Bool_t TABMparGeo::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   ReadItem(fWireLayersN);
   
   if(FootDebugLevel(1))
      cout<<"TABMparGeo::FromFile() read parameters:"<<endl;
   
   if(FootDebugLevel(1))
      cout << endl << "  Wire layer "<< fWireLayersN << endl;
   
   ReadItem(fLayersN);
   if(FootDebugLevel(1))
      cout  << "  Layers: " <<  fLayersN << endl;
   
   ReadItem(fSensesN);
   if(FootDebugLevel(1))
      cout  << "  Senses: " <<  fSensesN << endl;
   
   ReadItem(fSenseRadius);
   if(FootDebugLevel(1))
      cout  << "  Sense Radius:  "<< fSenseRadius << endl;
   
   ReadStrings(fSenseMat);
   if(FootDebugLevel(1))
      cout  << "  Sense wire material:  "<< fSenseMat << endl;
   
   ReadItem(fSenseDensity);
   if(FootDebugLevel(1))
      cout  << "  Sense wire density:  "<< fSenseDensity << endl;
   
   ReadItem(fFieldRadius);
   if(FootDebugLevel(1))
      cout  << "  Field wire Radius:  "<< fFieldRadius << endl;
   
   ReadStrings(fFieldMat);
   if(FootDebugLevel(1))
      cout  << "  Field wire material:  "<< fFieldMat << endl;
   
   ReadItem(fFieldDensity);
   if(FootDebugLevel(1))
      cout  << "  Field wire density:  "<< fFieldDensity << endl;
   
   
   ReadItem(fFoilThick);
   if(FootDebugLevel(1))
      cout  << "  Foil thickness:  "<< fFoilThick << endl;
   
   ReadStrings(fFoilMat);
   if(FootDebugLevel(1))
      cout  << "  Foil material:  "<< fFoilMat << endl;
   
   ReadItem(fFoilDensity);
   if(FootDebugLevel(1))
      cout  << "  Foil density:  "<< fFoilDensity << endl;

   ReadItem(fBmStep);
   if(FootDebugLevel(1))
      cout  << "  BM step  "<< fBmStep << endl;
   
   ReadItem(fBmCellWide);
   if(FootDebugLevel(1))
      cout  << "  BM cell width  "<< fBmCellWide << endl;
   
   ReadItem(fBmDplane);
   if(FootDebugLevel(1))
      cout  << "  BM delta plane  "<< fBmDplane << endl;
   
   ReadStrings(fGasMixture);
   if(FootDebugLevel(1))
      cout  << "  BM gas mixture  "<< fGasMixture << endl;
   
   ReadStrings(fGasProp);
   if(FootDebugLevel(1))
      cout  << "  BM gas proportion  "<< fGasProp << endl;
   
   ReadStrings(fGasDensities);
   if(FootDebugLevel(1))
      cout  << "  BM gas densities  "<< fGasDensities << endl;
   
   ReadItem(fGasDensity);
   if(FootDebugLevel(1))
      cout  << "  BM gas mixture density:  "<< fGasDensity << endl;

   ReadItem(fShieldThick);
   if(FootDebugLevel(1))
      cout  << "  Shielding Thickness:  "<< fShieldThick << endl;
   
   
   ReadVector3(fBmSideDch);
   if(FootDebugLevel(1))
      cout << "  Size: "
      << Form("%f %f %f", fBmSideDch[0], fBmSideDch[1], fBmSideDch[2]) << endl;
   
   ReadVector3(fBmDeltaDch);
   if(FootDebugLevel(1))
      cout << "  Delta: "
      << Form("%f %f %f", fBmDeltaDch[0], fBmDeltaDch[1], fBmDeltaDch[2]) << endl;
   
  
   SetupMatrices(36);
   for(Int_t i=0;i<36;i++){
     // read wire align position
     ReadVector3(fWireAlign);
     if(FootDebugLevel(2))
        cout << "  Alignment: "
        << Form("%f %f %f", fWireAlign[0], fWireAlign[1], fWireAlign[2]) << endl;
     
     // read wire tilt angles
     ReadVector3(fWireTilt);
     if(FootDebugLevel(2))
        cout  << "  Tilt: "
        << Form("%f %f %f", fWireTilt[0], fWireTilt[1], fWireTilt[2]) << endl;
  
     // Build wire matrix transformation
  
     TGeoTranslation trans(fWireAlign[0], fWireAlign[1], fWireAlign[2]);
  
     TGeoRotation rot;
     rot.RotateX(fWireTilt[0]);
     rot.RotateY(fWireTilt[1]);
     rot.RotateZ(fWireTilt[2]);
  
     TGeoHMatrix  transfo;
     transfo  = trans;
     transfo *= rot;
     AddTransMatrix(new TGeoHMatrix(transfo),i);
   }
   
   InitGeo();
   
   return true;
}

//______________________________________________________________________________
void TABMparGeo::InitGeo()
{
   fBmIdSense[0]= 8;
   fBmIdSense[1]= 10;
   fBmIdSense[2]= 12;
   
   fMylar1.SetXYZ(0.,0.,-(GetLength() + fFoilThick)/2.);
   fMylar2.SetXYZ(0.,0., (GetLength() + fFoilThick)/2.);
   
   Int_t modXY = fWireLayersN/fSensesN;
   Int_t modZ  = fSensesN;
   Int_t cellid, cell;
   TVector3 wiredir;
   
   if (FootDebugLevel(2)) 
     cout<<"TABMparGeo::InitGeo()::I'll printout the BM wire positions"<<endl;
     
   for(int iLayer = 0; iLayer < fLayersN; ++iLayer) {
      Int_t iWire = 0;
      
      for (Int_t i = 0; i< modZ; ++i) {
         for (Int_t j = 0; j < modXY; ++j) {
            
            fPosX[iWire][iLayer][0] = -fBmSideDch[0]/2;
            fPosY[iWire][iLayer][1] = -fBmSideDch[0]/2;
            
            if( (iLayer == 0) || (iLayer == 2) || (iLayer == 4) ){
               fPosY[iWire][iLayer][0] = -fBmSideDch[1]/2 + fBmDeltaDch[1] + (j+1)*fBmCellWide;
               fPosX[iWire][iLayer][1] = -fBmSideDch[1]/2 + fBmDeltaDch[1] + j*fBmCellWide;
            } else {
               fPosY[iWire][iLayer][0] = -fBmSideDch[1]/2 + fBmDeltaDch[1] + j*fBmCellWide;
               fPosX[iWire][iLayer][1] = -fBmSideDch[1]/2 + fBmDeltaDch[1] + (j+1)*fBmCellWide;
            }
            
            fPosZ[iWire][iLayer][0] = -fBmSideDch[2]/2 + fBmDeltaDch[2] + i*fBmStep + (2*iLayer)*(2*fBmStep + fBmDplane) ;
            fPosZ[iWire][iLayer][1] = -fBmSideDch[2]/2 + fBmDeltaDch[2] + i*fBmStep + (2*iLayer+1)*(2*fBmStep + fBmDplane) ;

            cell=-1;
            if(iWire==fBmIdSense[0]){
              cell=0;
            }else if(iWire==fBmIdSense[1]){
              cell=1;
            }else if (iWire==fBmIdSense[2]){
              cell=2;
            }
            if(cell!=-1){//it's a sense wire! I need to adjust it
              cellid=GetBMNcell(iLayer,0,cell);
              fPosX[iWire][iLayer][0]=fPosX[iWire][iLayer][0]+GetTransfo(cellid)->GetTranslation()[0];
              fPosY[iWire][iLayer][0]=fPosY[iWire][iLayer][0]+GetTransfo(cellid)->GetTranslation()[1];
              fPosZ[iWire][iLayer][0]=fPosZ[iWire][iLayer][0]+GetTransfo(cellid)->GetTranslation()[2];
              wiredir.SetXYZ(fBmSideDch[0],0.,0.);
              wiredir=MasterToLocalVect(cellid,wiredir);
              fPosCX[iWire][iLayer][0] = wiredir.X();
              fPosCY[iWire][iLayer][0] = wiredir.Y();
              fPosCZ[iWire][iLayer][0] = wiredir.Z();
              
              if (FootDebugLevel(2)){ 
                cout<<"cellid="<<cellid<<"   traslation="<<GetTransfo(cellid)->GetTranslation()[0]<<"  "<<GetTransfo(cellid)->GetTranslation()[1]<<"  "<<GetTransfo(cellid)->GetTranslation()[2]<<endl;
                cout<<"pos= "<<fPosX[iWire][iLayer][0]<<"  "<<fPosY[iWire][iLayer][0]<<"  "<<fPosZ[iWire][iLayer][0]<<endl;
                cout<<"dir= "<<fPosCX[iWire][iLayer][0]<<"  "<<fPosCY[iWire][iLayer][0]<<"  "<<fPosCZ[iWire][iLayer][0]<<endl;
              }
              
              cellid=GetBMNcell(iLayer,1,cell);
              fPosX[iWire][iLayer][1]=fPosX[iWire][iLayer][1]+GetTransfo(cellid)->GetTranslation()[0];
              fPosY[iWire][iLayer][1]=fPosY[iWire][iLayer][1]+GetTransfo(cellid)->GetTranslation()[1];
              fPosZ[iWire][iLayer][1]=fPosZ[iWire][iLayer][1]+GetTransfo(cellid)->GetTranslation()[2];
              wiredir.SetXYZ(0.,fBmSideDch[1],0.);
              wiredir=MasterToLocalVect(cellid,wiredir);
              fPosCX[iWire][iLayer][1] = wiredir.X();
              fPosCY[iWire][iLayer][1] = wiredir.Y();
              fPosCZ[iWire][iLayer][1] = wiredir.Z();              
              
              if (FootDebugLevel(2)) {
                cout<<"cellid="<<cellid<<"   traslation="<<GetTransfo(cellid)->GetTranslation()[0]<<"  "<<GetTransfo(cellid)->GetTranslation()[1]<<"  "<<GetTransfo(cellid)->GetTranslation()[2]<<endl;
                cout<<"pos= "<<fPosX[iWire][iLayer][1]<<"  "<<fPosY[iWire][iLayer][1]<<"  "<<fPosZ[iWire][iLayer][1]<<endl;
                cout<<"dir= "<<fPosCX[iWire][iLayer][1]<<"  "<<fPosCY[iWire][iLayer][1]<<"  "<<fPosCZ[iWire][iLayer][1]<<endl;
              }
              
            }else{//it's a field wire
              fPosCX[iWire][iLayer][0] = fBmSideDch[0];
              fPosCY[iWire][iLayer][0] = 0.;
              fPosCZ[iWire][iLayer][0] = 0.;
  
              fPosCX[iWire][iLayer][1] = 0.;
              fPosCY[iWire][iLayer][1] = fBmSideDch[1];
              fPosCZ[iWire][iLayer][1] = 0.;
            }
            
            iWire++;
         }
      }
   }
   
   DefineMaterial();
}


//_____________________________________________________________________________
void TABMparGeo::DefineMaterial()
{
   
   // Cathode material field wire
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fFieldMat, fFieldDensity);
   if (FootDebugLevel(1))
     cout<<"TABMparGeo::DefineMaterial::I'll printout the BM material"<<endl;
     
   if (FootDebugLevel(1)) {
      printf("Cathode material:\n");
      mat->Print();
   }
   
   // Anode material field wire
   mat = TAGmaterials::Instance()->CreateMaterial(fSenseMat, fSenseDensity);
   if (FootDebugLevel(1)) {
      printf("Anode material:\n");
      mat->Print();
   }
   
   // Gas mixture
   TGeoMixture* mix = TAGmaterials::Instance()->CreateMixture(fGasMixture, fGasDensities, fGasProp, fGasDensity);
   if (FootDebugLevel(1)) {
      printf("Gas mixture material:\n");
      mix->Print();
   }
   
   // Mylar
   mat = TAGmaterials::Instance()->CreateMaterial(fFoilMat, fFoilDensity);
   if (FootDebugLevel(1)) {
      printf("Anode material:\n");
      mat->Print();
   }
}

//_____________________________________________________________________________
TVector3 TABMparGeo::Detector2Wire(TVector3& glob) const
{
   return MasterToLocal(0, glob);
}

//_____________________________________________________________________________
TVector3 TABMparGeo::Detector2WireVect(TVector3& glob) const
{
   return MasterToLocalVect(0, glob);
}   

//_____________________________________________________________________________
void TABMparGeo::Detector2Wire(Double_t xg, Double_t yg, Double_t zg,
                                     Double_t& xl, Double_t& yl, Double_t& zl) const
{
   MasterToLocal(0, xg, yg, zg, xl, yl, zl);
}   

//_____________________________________________________________________________
TVector3 TABMparGeo::Wire2Detector(TVector3& loc) const
{
   return LocalToMaster(0, loc);
}


//_____________________________________________________________________________
TVector3 TABMparGeo::Wire2DetectorVect(TVector3& loc) const
{
   return LocalToMasterVect(0, loc);
}

//_____________________________________________________________________________
void TABMparGeo::Wire2Detector(Double_t xl, Double_t yl, Double_t zl,
                                 Double_t& xg, Double_t& yg, Double_t& zg) const
{
   LocalToMaster(0, xl, yl, zl, xg, yg, zg);
}   

//_____________________________________________________________________________
TVector3 TABMparGeo::ProjectFromPversR0(TVector3 Pvers, TVector3 R0, Double_t z)
{
   TVector3 projected(Pvers.X()/Pvers.Z()*z+R0.X() ,Pvers.Y()/Pvers.Z()*z+R0.Y(), z);
   
   return projected;
}

//_____________________________________________________________________________
TVector3 TABMparGeo::ProjectFromPversR0(Double_t PversXZ, Double_t PversYZ, Double_t R0X, Double_t R0Y, Double_t z)
{
   TVector3 projected(PversXZ*z+R0X ,PversYZ*z+R0Y, z);
   
   return projected;
}

//_____________________________________________________________________________
//~ void TABMparGeo::SetWireAlignment(Bool_t reverse)
//~ {
   //~ Int_t modZ  = 2;
   
      //~ for (Int_t iWire = 0; iWire< fWireLayersN; ++iWire) {
         //~ for(int iLayer = 0; iLayer < fLayersN; ++iLayer) {
         //~ for (Int_t j = 0; j < modZ; ++j) {
            //~ Double_t x = fPosX[iWire][iLayer][j];
            //~ Double_t y = fPosY[iWire][iLayer][j];
            //~ Double_t z = fPosZ[iWire][iLayer][j];
            
            //~ Double_t cx = fPosCX[iWire][iLayer][j];
            //~ Double_t cy = fPosCY[iWire][iLayer][j];
            //~ Double_t cz = fPosCZ[iWire][iLayer][j];
            
            //~ if (reverse) {
               //~ Detector2Wire(x, y, z, fPosX[iWire][iLayer][j], fPosY[iWire][iLayer][j], fPosZ[iWire][iLayer][j]);
               //~ Detector2Wire(cx, cy, cz, fPosCX[iWire][iLayer][j], fPosCY[iWire][iLayer][j], fPosCZ[iWire][iLayer][j]);

            //~ } else {
               //~ Wire2Detector(x, y, z, fPosX[iWire][iLayer][j], fPosY[iWire][iLayer][j], fPosZ[iWire][iLayer][j]);
               //~ Wire2Detector(cx, cy, cz, fPosCX[iWire][iLayer][j], fPosCY[iWire][iLayer][j], fPosCZ[iWire][iLayer][j]);
            //~ }
         //~ }
      //~ }
   //~ }
//~ }

//______________________________________________________________________________
TVector3 TABMparGeo::GetPlaneInfo(TVector3 pos, Int_t& view, Int_t& layer, Int_t& wire, Int_t& senseId)
{
   // view = 0 -> wire along X -> fPosY
   // view = 1 -> wire along Y -> fPosX
   Float_t offset = -fBmSideDch[2]/2 + fBmDeltaDch[2];
   pos[2] -= offset;
   
   Float_t width = (2*fBmStep + fBmDplane);
   
   layer = (pos[2]/width)/2;
   view  = int(pos[2]/width) % 2;
   
   Float_t minDist = 999;

   for(Int_t i = 0; i < fSensesN; ++i) {
      Float_t dist = 0.;
      wire = fBmIdSense[i];
      if (view == 0)
         dist = TMath::Abs(fPosY[wire][layer][view] - pos[1]);
      else
         dist = TMath::Abs(fPosX[wire][layer][view] - pos[0]);
      
      if (dist < minDist) {
         minDist = dist;
         senseId = i;
      }
   }
   
   return GetWirePos(view, layer, wire);
}

//______________________________________________________________________________
Bool_t TABMparGeo::GetBMNlvc(const Int_t cellid, Int_t& ilay, Int_t& iview, Int_t& icell)
{
   if(cellid>35 || cellid<0){
      cout<<"ERROR in TABMparGeo::GetBMNcell, cellid is wrong: cellid="<<cellid<<endl;
      return kFALSE;
   }
   
   icell = cellid % 3;
   iview = (((Int_t)(cellid/3)) % 2 == 0) ? 0:1;
   ilay  = (Int_t)(cellid/6);
   
   return kTRUE;
}

//______________________________________________________________________________
void TABMparGeo::GetCellInfo(Int_t view, Int_t plane, Int_t cellID, Double_t& h_x, Double_t& h_y, Double_t& h_z, Double_t& h_cx, Double_t& h_cy, Double_t& h_cz) {
   
   /* Set Chamber center positioning */
   int my_ID = GetSenseId(cellID);
   
   h_x = fPosX[my_ID][plane][view];
   h_y = fPosY[my_ID][plane][view];
   h_z = fPosZ[my_ID][plane][view];
   
   h_cx =  fPosCX[my_ID][plane][view];
   h_cy =  fPosCY[my_ID][plane][view];
   h_cz =  fPosCZ[my_ID][plane][view];
   
   return;
}


//______________________________________________________________________________
TVector3 TABMparGeo::GetWirePos(Int_t view, Int_t layer, Int_t wire) const
{
   return TVector3(fPosX[wire][layer][view], fPosY[wire][layer][view], fPosZ[wire][layer][view]);
}

//______________________________________________________________________________
TVector3 TABMparGeo::GetWireDir(Int_t view) const
{
   return (view == 0) ? TVector3(1, 0, 0) : TVector3(0, 1, 0);
}

//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TABMparGeo::Clear(Option_t*)
{
   for(Int_t i = 0; i < 3; ++i)
      fBmIdSense[i] = 0;
}

//______________________________________________________________________________
void TABMparGeo::ToStream(ostream& os, Option_t*) const
{
   os << "TABMparGeo " << GetName() << endl;
   os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
   << "   ver_x   ver_y   ver_z  width" << endl;
}

//_____________________________________________________________________________
TGeoVolume* TABMparGeo::BuildBeamMonitor(const char *bmName )
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager(TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // create module
   const Char_t* matName = fGasMixture.Data();
   TGeoMedium*  medBM = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
   
   TGeoVolume* box = gGeoManager->MakeBox(bmName, medBM, GetWidth()/2., GetHeigth()/2., GetLength()/2.);
   box->SetVisibility(true);
   box->SetLineColor(kGray);
   box->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
   
    for(auto j = 0 ; j < GetLayersN() ; ++j){
        for(auto i = 0 ; i < 2 ; ++i){

            TGeoVolume* layer = BuildLayer( j + 6*i );
            
            auto tShift = (fBmDeltaDch[0] - GetCellsN()*GetCellWidth()) * ( j%2 == 0 ? +1 : -1);
            TGeoTranslation tTranslation{-i*tShift ,
                                         (1-i)*tShift ,
                                         -GetLength()/2 +fBmDeltaDch[2] + GetCellHeight() +i*(GetCellHeight()*2 + GetDeltaPlane()) + j*2*(GetCellHeight()*2 + GetDeltaPlane())};
            TGeoRotation tRotation{"tRotation", static_cast<Double_t>( 90*(1-i) ), 0, 0};
            auto pTransfo = new TGeoCombiTrans(tTranslation, tRotation);
        
            box->AddNode(layer, i, pTransfo);
        }
    }
    
   if ( GlobalPar::GetPar()->geoFLUKA() ) {

      matName = fFieldMat.Data();
      TGeoMedium *c_wire_med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
      
      matName = fSenseMat.Data();
      TGeoMedium *a_wire_med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
      
      TGeoVolume *c_x_wire = gGeoManager->MakeTubs("c_x_wire", c_wire_med, 0, fFieldRadius, fBmSideDch[0]/2.,0.,0.); //filo catodo lungo z
      c_x_wire->SetLineColor(kBlue);
      c_x_wire->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
      
      TGeoVolume *c_y_wire = gGeoManager->MakeTubs("c_y_wire", c_wire_med, 0, fFieldRadius, fBmSideDch[0]/2.,0.,0.);
      c_y_wire->SetLineColor(kRed);
      c_y_wire->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
      
      TGeoVolume *a_x_wire = gGeoManager->MakeTubs("a_x_wire", a_wire_med, 0, fSenseRadius, fBmSideDch[0]/2.,0.,0.); //BISOGNERÀ POI SETTARE MEGLIO IL TUTTO
      a_x_wire->SetLineColor(kYellow);
      a_x_wire->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
      
      TGeoVolume *a_y_wire = gGeoManager->MakeTubs("a_y_wire", a_wire_med, 0, fSenseRadius, fBmSideDch[0]/2.,0.,0.);
      a_y_wire->SetLineColor(kGreen);
      a_y_wire->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
      
      Int_t c=0;
      
      for(Int_t il=0; il<fLayersN;il++){
         for (Int_t kk =0; kk<fWireLayersN;kk++){
            
            if(kk==fBmIdSense[0] || kk==fBmIdSense[1] || kk==fBmIdSense[2]){//se è filo di sense
               box->AddNode(a_x_wire, c++ , new TGeoCombiTrans(0.,fPosY[kk][il][0],fPosZ[kk][il][0], new TGeoRotation("a_x_wire,",90.,90.,0.)));
               box->AddNode(a_y_wire, c++ , new TGeoCombiTrans(fPosX[kk][il][1],0.,fPosZ[kk][il][1], new TGeoRotation("a_y_wire,",0.,90.,0.)));
            } else { // filo di catodo
               box->AddNode(c_x_wire, c++ , new TGeoCombiTrans(0.,fPosY[kk][il][0],fPosZ[kk][il][0], new TGeoRotation("c_x_wire,",90.,90.,0.)));
               box->AddNode(c_y_wire, c++ , new TGeoCombiTrans(fPosX[kk][il][1],0.,fPosZ[kk][il][1], new TGeoRotation("c_y_wire,",0.,90.,0.)));
            }
         }
      }
   }
  
   return box;
}


//_____________________________________________________________________________
TGeoVolume* TABMparGeo::BuildLayer(Int_t idx )
{
    const char* layerName = Form("%s_%d", GetBaseName(), idx) ;
    TGeoVolume* layer     = gGeoManager->FindVolumeFast(layerName);
    if ( !layer ) {
        const Char_t* matName = fGasMixture.Data();
        TGeoMedium*  med = (TGeoMedium *)gGeoManager->GetListOfMedia()->FindObject(matName);
        layer = gGeoManager->MakeBox(layerName, med,  3. * fBmCellWide, GetHeigth()/2 , fBmStep);
    }
    
    layer->SetVisibility(true);
    layer->SetLineColor(kGray);
    layer->SetTransparency(TAGgeoTrafo::GetDefaultTransp());
    
    return layer;
}

//_____________________________________________________________________________
//! set color on for fired bars
void TABMparGeo::SetLayerColorOn(Int_t idx)
{
    if (!gGeoManager) {
        Error("SetBarcolorOn()", "No Geo manager defined");
        return;
    }
    
    TString name(Form("%s_%d", GetBaseName(), idx ));
    
    TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
    if (vol)
        vol->SetLineColor(kRed-3);
}

//_____________________________________________________________________________
//! reset color for unfired bars
void TABMparGeo::SetLayerColorOff(Int_t idx)
{
    if (!gGeoManager) {
        Error("SetBarcolorOn()", "No Geo manager defined");
        return;
    }
    
    TString name(Form("%s_%d", GetBaseName(), idx ));
    
    TGeoVolume* vol = gGeoManager->FindVolumeFast(name.Data());
    if (vol)
        vol->SetLineColor(kGray);
}

//_____________________________________________________________________________
string TABMparGeo::PrintBodies(){
    
  stringstream outstr;
    
  if(GlobalPar::GetPar()->IncludeBM()){
    
    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
    if (!fpFootGeo)
      printf("No default GeoTrafo action available yet\n");
    else 
      printf("GeoTrafo default action found\n");

    //Reading the BM global position from global map file.
    TVector3  fCenter = fpFootGeo->GetBMCenter();

    outstr << "* ***Beam Monitor" << endl;

    int iSense[2]={-1,-1}, iField[2]={-1,-1};
    // stringstream ss;
    char bodyname[20];
    double xmin, xmax, ymin, ymax, zmin, zmax;
  
    double shift = 0.00001;
  
    outstr << setiosflags(ios::fixed) << setprecision(6);

    outstr << "RPP BmnShiOu    "
	   << fCenter[0]-fBmSideDch[0]/2.-fShieldThick << " "
	   << fCenter[0]+fBmSideDch[0]/2.+fShieldThick << " "
	   << fCenter[1]-fBmSideDch[1]/2.-fShieldThick << " "
	   << fCenter[1]+fBmSideDch[1]/2.+fShieldThick << " "
	   << fCenter[2]-fBmSideDch[2]/2.-1. << " "
	   << fCenter[2]+fBmSideDch[2]/2.+1. << endl;
  
    outstr << "RPP BmnShiIn    "
	   << fCenter[0]-fBmSideDch[0]/2. << " "
	   << fCenter[0]+fBmSideDch[0]/2. << " "
	   << fCenter[1]-fBmSideDch[1]/2. << " "
	   << fCenter[1]+fBmSideDch[1]/2. << " "
	   << fCenter[2]-fBmSideDch[2]/2.-1. << " "
	   << fCenter[2]+fBmSideDch[2]/2.+1. << endl;
  
    outstr << "XYP BmnMyl0     " << fCenter[2]-fBmSideDch[2]/2.-fFoilThick << endl;
    outstr << "XYP BmnMyl1     " << fCenter[2]-fBmSideDch[2]/2. << endl;
    outstr << "XYP BmnMyl2     " << fCenter[2]+fBmSideDch[2]/2. << endl;
    outstr << "XYP BmnMyl3     " << fCenter[2]+fBmSideDch[2]/2.+fFoilThick << endl;
 
	
    // Cells
    // prima lungo x, poi lungo y
    int cella=0;
    for (int il=0;il<fLayersN;il++){ // loop on layers
      for (int ic =0; ic<fWireLayersN;ic++){  // loop on cells
	if ( (ic==fBmIdSense[0]) ||(ic==fBmIdSense[1]) ||
	     (ic==fBmIdSense[2]) ){
	  for (int iv =0; iv<2;iv++){      // loop on views
	    if ( iv == 0 ){
	      xmin = fCenter[0] - fBmSideDch[0]/2. + shift;
	      xmax = fCenter[0] + fBmSideDch[0]/2. - shift;
	      ymin = fCenter[1] + fPosY[ic][il][iv] - fBmCellWide + fFieldRadius + shift;
	      ymax = fCenter[1] + fPosY[ic][il][iv] + fBmCellWide - fFieldRadius -shift;
	    }else{
	      xmin = fCenter[0] + fPosX[ic][il][iv] - fBmCellWide + fFieldRadius + shift;
	      xmax = fCenter[0] + fPosX[ic][il][iv] + fBmCellWide - fFieldRadius - shift;
	      ymin = fCenter[1] - fBmSideDch[1]/2. + shift;
	      ymax = fCenter[1] + fBmSideDch[1]/2. - shift;
	    }
	    zmin = fCenter[2] + fPosZ[ic][il][iv] - fBmStep + fFieldRadius + shift;
	    zmax = fCenter[2] + fPosZ[ic][il][iv] + fBmStep - fFieldRadius -shift;
	    outstr << "RPP BmnC" << iv << cella << "   "
		   << xmin << " " << xmax << " " << ymin << " " << ymax
		   << " " << zmin << " " << zmax << endl;
	  }
	  cella++;
	}
      }
    }
 
    // Wires  
    for (int il=0;il<fLayersN;il++){
      for (int iw =0; iw<fWireLayersN;iw++){ 
	for (int iv =0; iv<2;iv++){
	  // int iv=1;
	  if ( (iw==fBmIdSense[0]) ||(iw==fBmIdSense[1]) ||
	       (iw==fBmIdSense[2]) ){	
	    iSense[iv]++;	
	    outstr << "RCC BmnS" << iv << iSense[iv] << "   "
		   << fCenter[0] + fPosX[iw][il][iv] << " "
		   << fCenter[1] + fPosY[iw][il][iv] << " "
		   << fCenter[2] + fPosZ[iw][il][iv] << " "
		   << fPosCX[iw][il][iv] << " " << fPosCY[iw][il][iv] << " "
		   << fPosCZ[iw][il][iv] << " " << fSenseRadius << endl;
	  } else {
	    iField[iv]++;     // loop on views    		
	    outstr << "RCC BmnF" << iv << iField[iv] << "   "
		   << fCenter[0] + fPosX[iw][il][iv] << " "
		   << fCenter[1] + fPosY[iw][il][iv] << " "
		   << fCenter[2] + fPosZ[iw][il][iv] << " "
		   << fPosCX[iw][il][iv] << " " << fPosCY[iw][il][iv] << " "
		   << fPosCZ[iw][il][iv] << " " << fFieldRadius << endl;
	  }
	}
      }
    }
  }
  
  return outstr.str();
}


//_____________________________________________________________________________
string TABMparGeo::PrintRegions(){

  stringstream outstr;
    
  if(GlobalPar::GetPar()->IncludeBM()){

    outstr << "* ***Beam Monitor" << endl;

    int iCell[2]={-1,-1}, iSense[2]={-1,-1}, iField[2]={-1,-1};
    char stringa[100];

    outstr << "BMN_SHI      5 BmnShiOu -BmnShiIn" << endl;
    outstr << "BMN_MYL0     5 BmnShiIn -BmnMyl0 +BmnMyl1" << endl;
    outstr << "BMN_MYL1     5 BmnShiIn -BmnMyl2 +BmnMyl3" << endl;
 
    for (int iv =0; iv<2;iv++){      // loop on views
      for (int il=0;il<fLayersN;il++){ // loop on layers
	for (int ic =0; ic<fSensesN;ic++){  // loop on cells
	  iCell[iv]++;
	  outstr << "BMN_C" << iv << setw(2) << setfill('0') << iCell[iv]
		 << "     5 BmnC" << iv << iCell[iv]
		 << " -BmnS" << iv << iCell[iv] << endl;
	}
      }
    }
 
    outstr << "BMN_FWI      5";
    int count=0;
    for (int iv = 0; iv<2; iv++){
      for (int il=0;il<fLayersN;il++){ // loop on layers
	for (int jj = 0; jj < (fWireLayersN-fSensesN); jj++) {
	  if ((( count  % 4 ) == 0) && count>0 )
	    outstr << "\n              ";
	  outstr << " | " << "BmnShiIn + BmnF" << iv  << (fWireLayersN-fSensesN)*il+jj;
	  count++;
	}
      }
    }
    outstr << endl;
 
    outstr << "BMN_SWI      5";
    count =0;
    for (int iv =0; iv<2;iv++){      // loop on views
      for (int il=0;il<fLayersN;il++){ // loop on layers
	for (int ic =0; ic<fSensesN;ic++){  // loop on cells
	  if ((( count  % 4 ) == 0) && count>0 )
	    outstr << "\n              ";
	  outstr << " | " << "BmnC" << iv << fSensesN*il+ic << " + BmnS" << iv  << fSensesN*il+ic;
	  count++;
	}
      }
    }
 
    outstr << endl;
    outstr << "BMN_GAS      5 BmnShiIn -BmnMyl1 +BmnMyl2";
    count =0;
    for (int iv =0; iv<2;iv++){      // loop on views
      for (int il=0;il<fLayersN;il++){ // loop on layers
	for (int ic =0; ic<(fWireLayersN-fSensesN);ic++){  // loop on field wires
	  if ((( count  % 6 ) == 0) )
	    outstr << "\n              ";
	  outstr << " -BmnF" << iv << (fWireLayersN-fSensesN)*il+ic;
	  count++;
	}
	for (int ic =0; ic<fSensesN;ic++){  // loop on cells
	  if ((( count  % 6 ) == 0) )
	    outstr << "\n              ";
	  outstr << " -BmnC" << iv << fSensesN*il+ic;
	  count++;
	}
      }
    }
 
    outstr << endl;
  }

  return outstr.str();
 
}




//_____________________________________________________________________________
string TABMparGeo::PrintSubtractBodiesFromAir() {

  stringstream ss;

  if(GlobalPar::GetPar()->IncludeBM()){

    ss << "-(BmnShiOu -BmnShiIn) -(BmnShiIn -BmnMyl0 +BmnMyl3)" << endl;

  }
  
  return ss.str();

}



//_____________________________________________________________________________
string TABMparGeo::PrintAssignMaterial() {

    // if ( !GlobalPar::GetPar()->geoFLUKA() ) 
    //     cout << "ERROR << TABMparGeo::PrintAssignMaterial()  -->  Calling this function without enabling the correct parameter in the param file.\n", exit(0);


    // loop in order of the material alfabeth
    stringstream outstr; 
 
    if(GlobalPar::GetPar()->IncludeBM()){
      
      bool magnetic = false;
      if(GlobalPar::GetPar()->IncludeDI())
	magnetic = true;
         
      string matFWire = "ALUMINUM";//fFieldMat.Data();
      if (fFieldMat.CompareTo("Al")!=0)
	cout << "ATTENTION in TABMparGeo PrintAssignMaterial: check the BM FWires material"<<endl;
      // matFWire[1] =toupper(matFWire[1]);
      const Char_t* matGas = fGasMixture.Data();
      // const Char_t* matFWire = fFieldMat.Data();
      const Char_t* matSWire = "TUNGSTEN";//fSenseMat.Data();
      if (fSenseMat.CompareTo("W")!=0)
	cout << "ATTENTION in TABMparGeo PrintAssignMaterial: check the BM SWires material"<<endl;
      const Char_t* matFoil = fFoilMat.Data();
      
      outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	     << setw(10) << setfill(' ') << std::right << matFWire// same mat as Field Wires
	     << setw(10) << setfill(' ') << std::right << "BMN_SHI"
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << magnetic
	     << endl;
    
      outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	     << setw(10) << setfill(' ') << std::right << matFoil
	     << setw(10) << setfill(' ') << std::right << "BMN_MYL0"
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << magnetic
	     << endl; 
    
      outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	     << setw(10) << setfill(' ') << std::right << matFoil
	     << setw(10) << setfill(' ') << std::right << "BMN_MYL1"
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << magnetic
	     << endl;
    
      outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	     << setw(10) << setfill(' ') << std::right << matGas
	     << setw(10) << setfill(' ') << std::right << "BMN_C000"
	     << setw(10) << setfill(' ') << std::right << "BMN_C017"
	     << setw(10) << setfill(' ') << std::right << "1."
	     << setw(10) << setfill(' ') << std::right << magnetic
	     << endl;
    
      outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	     << setw(10) << setfill(' ') << std::right << matGas
	     << setw(10) << setfill(' ') << std::right << "BMN_C100"
	     << setw(10) << setfill(' ') << std::right << "BMN_C117"
	     << setw(10) << setfill(' ') << std::right << "1."
	     << setw(10) << setfill(' ') << std::right << magnetic
	     << endl;
    
      outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	     << setw(10) << setfill(' ') << std::right << matGas
	     << setw(10) << setfill(' ') << std::right << "BMN_GAS"
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << magnetic
	     << endl;
    
      outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	     << setw(10) << setfill(' ') << std::right << matFWire
	     << setw(10) << setfill(' ') << std::right << "BMN_FWI"
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << magnetic
	     << endl;
    
      outstr << setw(10) << setfill(' ') << std::left << "ASSIGNMA"
	     << setw(10) << setfill(' ') << std::right << matSWire
	     << setw(10) << setfill(' ') << std::right << "BMN_SWI"
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << ""
	     << setw(10) << setfill(' ') << std::right << magnetic
	     << endl;

    }
    
    return outstr.str();
}

//_____________________________________________________________________________
string TABMparGeo::PrintParameters() {
  
  stringstream outstr;
  outstr << setiosflags(ios::fixed) << setprecision(5);

  if(GlobalPar::GetPar()->IncludeBM()){

    outstr << "c     BEAM MONITOR PARAMETERS " << endl;
    outstr << endl;    
  
    map<string, int> intp;
    intp["nlayBMN"] = fLayersN;
    intp["ncellBMN"] = fSensesN;
    for (auto i : intp){
      outstr << "      integer " << i.first << endl;
      outstr << "      parameter (" << i.first << " = " << i.second << ")" << endl;
    }
    outstr << endl;    

  }
  
  return outstr.str();

}




//______________________________________________________________________________
/// Get cell id from position of the hit, layer and view
Int_t TABMparGeo::GetCell(TVector3 pos, int layer, int view)
{
    Int_t cell = -1 ;
    Float_t shift = fBmDeltaDch[1]-fSensesN*fBmCellWide ;
    if(layer%2==0) shift = -shift;
    Float_t limit = -fSensesN*fBmCellWide + shift;

    int axis ;
    if(view==0) axis = 1 ;  /// view 0 along y axis
    else axis = 0 ;         /// view 1 along x axis

    for(int i=0 ; i<fSensesN ; ++i){
        if((pos[axis]>= limit) && (pos[axis]<(limit+2.0*fBmCellWide))){
            cell = i ;
        }
        limit = limit+(2.0*fBmCellWide) ;
    }
    return cell ;
}














