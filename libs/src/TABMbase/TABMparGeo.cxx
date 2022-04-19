/*!
  \file    TABMparGeo.cxx
  \brief   Geometry parameters of the beam monitor
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
#include "TAGgeoTrafo.hxx"
#include "TABMparGeo.hxx"

using namespace std;

//##############################################################################

/*!
  \file TABMpargeo.cxx
  \brief Geometry parameters of the beam monitor
*/

ClassImp(TABMparGeo);

const TString TABMparGeo::fgkDefParaName = "bmGeo";
const TString TABMparGeo::fgkBaseName = "BM";
Int_t TABMparGeo::fgkLayerOffset      = 6;


///------------------------------------------+-----------------------------------
//! Default constructor
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
   fGasDensity(0.)
{
   fkDefaultGeoName = "./geomaps/TABMdetector.geo";
}

//------------------------------------------+-----------------------------------
//! Default destructor
TABMparGeo::~TABMparGeo()
{
}

//_____________________________________________________________________________
//! Read the geometry input file
//!
//! \param[in] name input file name
Bool_t TABMparGeo::FromFile(const TString& name)
{
   cout << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

   TString nameExp;

   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;

   if (!Open(nameExp)) return false;

   Info("FromFile()", "Open file %s for geometry", name.Data());

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
      << fBmSideDch[0]  << " " <<  fBmSideDch[1] << " " <<  fBmSideDch[2] << endl;

   ReadVector3(fBmDeltaDch);
   if(FootDebugLevel(1))
      cout << "  Delta: "
      << fBmDeltaDch[0] << " " << fBmDeltaDch[1] << " " << fBmDeltaDch[2] << endl;


   SetupMatrices(36);
   TVector3 wiredisp, wiretilt;
   for(Int_t i=0;i<36;i++){
     // read wire align position
     ReadVector3(wiredisp);
     if(FootDebugLevel(2))
        cout << "  Alignment: "
        << wiredisp[0] << " " << wiredisp[1] << " " << wiredisp[2] << endl;

     // read wire tilt angles
     ReadVector3(wiretilt);
     if(FootDebugLevel(2))
        cout  << "  Tilt: "
        << wiretilt[0] << " " << wiretilt[1] << " " << wiretilt[2] << endl;

     // Build wire matrix transformation

     TGeoTranslation trans(wiredisp[0], wiredisp[1], wiredisp[2]);

     //use the Euler convention adopted in TGeoRotation (SetAngles(phi,theta,psi)):
     //Phi is the rotation angle about Z axis and is done first, theta is the rotation about new X and is done second, psi is the rotation angle about new Z and is done third. All angles are in degrees.
     //Please Note: Fluka apply a different convention! The small aligment and tilts will not be included in Simulation
     TGeoRotation rot;
     rot.SetAngles(wiretilt[0],wiretilt[1],wiretilt[2]);
     // rot.RotateX(fWireTilt[0]);
     // rot.RotateY(fWireTilt[1]);
     // rot.RotateZ(fWireTilt[2]);

     TGeoHMatrix  transfo;
     transfo  = trans;
     transfo *= rot;
     AddTransMatrix(new TGeoHMatrix(transfo),i);
   }

   InitGeo();

   return true;
}

//______________________________________________________________________________
//! initialize the BM geometry
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

//! Define the BM material
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
//! Transformation from detector to sensor framework
//!
//! \param[in] cellid cell id index [0-35]
//! \param[in] glob position in detector framework
//! \return position in sensor framework
TVector3 TABMparGeo::Detector2Wire(Int_t cellid, TVector3& glob) const
{
  if (cellid < 0 || cellid > 36) {
   Warning("Detector2Wire()","Wrong cellid number: %d ", cellid);
   return TVector3(0,0,0);
  }

  Int_t cell, view, layer;
  GetBMNlvc(cellid, layer, view, cell);
  TVector3 wirepos(glob.X()-fPosX[GetSenseId(cell)][layer][view],glob.Y()- fPosY[GetSenseId(cell)][layer][view],glob.Z()- fPosZ[GetSenseId(cell)][layer][view]);
 return MasterToLocal(cellid, wirepos);
}

//_____________________________________________________________________________
//! Transformation from detector to sensor framework for vectors (no translation)
//!
//! \param[in] cellid cellid  index [0-35]
//! \param[in] glob position in detector framework
//! \return position in sensor framework
TVector3 TABMparGeo::Detector2WireVect(Int_t cellid, TVector3& glob) const
{
   return MasterToLocalVect(cellid, glob);
}

//_____________________________________________________________________________
//! Transformation from detector to wire framework
//!
//! \param[in] cellid cell index [0-35]
//! \param[in] xg X position in detector framework
//! \param[in] yg Y position in detector framework
//! \param[in] zg Z position in detector framework
//! \param[out] xl X position in wire framework
//! \param[out] yl Y position in wire framework
//! \param[out] zl Z position in wire framework
void TABMparGeo::Detector2Wire(Int_t cellid, Double_t xg, Double_t yg, Double_t zg,
                                     Double_t& xl, Double_t& yl, Double_t& zl) const
{
  if (cellid < 0 || cellid > 36) {
   Warning("Detector2Wire()","Wrong cellid number: %d ", cellid);
   return;
  }
  Int_t cell, view, layer;
  GetBMNlvc(cellid, layer, view, cell);
  TVector3 prov(xg-fPosX[GetSenseId(cell)][layer][view], yg-fPosY[GetSenseId(cell)][layer][view], zg-fPosZ[GetSenseId(cell)][layer][view]);
  TVector3 loc=MasterToLocal(cellid, prov);
  xl=loc.X();
  yl=loc.Y();
  zl=loc.Z();
}

//_____________________________________________________________________________
//! Transformation from sensor to detector framework
//!
//! \param[in] cellid cell index [0-35]
//! \param[in] loc position in sensor framework
//! \return position in detector framework
TVector3 TABMparGeo::Wire2Detector(Int_t cellid, TVector3& loc) const
{
  if (cellid < 0 || cellid > 36) {
   Warning("Wire2Detector()","Wrong cellid number: %d ", cellid);
   return TVector3(0,0,0);
  }
  Int_t cell, view, layer;
  GetBMNlvc(cellid, layer, view, cell);
  TVector3 wirepos(fPosX[GetSenseId(cell)][layer][view], fPosY[GetSenseId(cell)][layer][view], fPosZ[GetSenseId(cell)][layer][view]);
  return LocalToMaster(cellid, loc)+wirepos;
}


//_____________________________________________________________________________
//! Transformation from sensor to detector framework for vectors (no translation)
//!
//! \param[in] cellid cell [0-35]
//! \param[in] loc position in sensor framework
//! \return position in detector framework
TVector3 TABMparGeo::Wire2DetectorVect(Int_t cellid, TVector3& loc) const
{
  if (cellid < 0 || cellid > 36) {
   Warning("Wire2Detector()","Wrong cellid number: %d ", cellid);
   return TVector3(0,0,0);
  }
   return LocalToMasterVect(cellid, loc);
}

//_____________________________________________________________________________
//! Transformation from wire to detector framework
//!
//! \param[in] cellid cell index [0-35]
//! \param[in] xl X position in sensor framework
//! \param[in] yl Y position in sensor framework
//! \param[in] zl Z position in sensor framework
//! \param[out] xg X position in detector framework
//! \param[out] yg Y position in detector framework
//! \param[out] zg Z position in detector framework
void TABMparGeo::Wire2Detector(Int_t cellid, Double_t xl, Double_t yl, Double_t zl,
                                 Double_t& xg, Double_t& yg, Double_t& zg) const
{
  if (cellid < 0 || cellid > 36) {
   Warning("Wire2Detector()","Wrong cellid number: %d ", cellid);
   return;
  }
  Int_t cell, view, layer;
  GetBMNlvc(cellid, layer, view, cell);
  TVector3 loc(xl,yl,zl);
  TVector3 prov=LocalToMaster(cellid, loc);
  xg=prov.X()+fPosX[GetSenseId(cell)][layer][view];
  yg=prov.Y()+fPosY[GetSenseId(cell)][layer][view];
  zg=prov.Z()+fPosZ[GetSenseId(cell)][layer][view];
}

//_____________________________________________________________________________
//! Give the position of a point along a track for a given z position
//!
//! \param[in] Pvers direction of the track (m)
//! \param[in] R0 intercept of the track (q)
//! \param[in] z coordinate along the track where to calculate the point position
//! \return TVecotor3 of the point position along the track
TVector3 TABMparGeo::ProjectFromPversR0(TVector3 Pvers, TVector3 R0, Double_t z) const
{
  return TVector3(Pvers.X()/Pvers.Z()*z+R0.X() ,Pvers.Y()/Pvers.Z()*z+R0.Y(), z);
}

//_____________________________________________________________________________
//! Give the position of a point along a track for a given z position
//!
//! \param[in] PversXZ XZ direction of the track (mx)
//! \param[in] PversYZ YZ direction of the track (my)
//! \param[in] R0X X coordinate of the track intercept (qx)
//! \param[in] R0Y Y coordinate of the track intercept (qy)
//! \param[in] z coordinate along the track where to calculate the point position
//! \return TVector3 of the point position along the track
TVector3 TABMparGeo::ProjectFromPversR0(Double_t PversXZ, Double_t PversYZ, Double_t R0X, Double_t R0Y, Double_t z) const
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
//! Given a position vector, it will retrieve and set the view, layer, wire and cell index parameters, returning the position of the closest sense wire without the wire displament or tilt
//!
//! \param[in] pos position in the space where to calculate the detector parameters
//! \param[out] view view of the cell containing pos vector [0-1]
//! \param[out] layer layer or plane of the cell containing pos vector [0-5]
//! \param[out] wire wire index of the cell containing pos vector [0-20]
//! \param[out] senseId cell of the cell containing pos vector [0-2]
//! \return position of the sense wire of the cell containing pos vector
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
//! Given a cellid index [0-35], it will retrieve and set the layer, view and cell index parameters
//!
//! \param[in] cellid cellid index where the view, layer and cell indices will be set
//! \param[out] ilay layer the cellid cell [0-5]
//! \param[out] iview view of the cellid cell [0-1]
//! \param[out] icell cell of the cellid cell [0-2]
Bool_t TABMparGeo::GetBMNlvc(Int_t cellid, Int_t& ilay, Int_t& iview, Int_t& icell) const
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
//! Given a layer, view and cell indices of a BM cell, it will set the sense wire position and directions
//!
//! \param[in] view view index of the cell
//! \param[in] layer layer index of the cell
//! \param[in] cellID cell index of the cell
//! \param[out] h_x X coordinate of the sense wire position
//! \param[out] h_y Y coordinate of the sense wire position
//! \param[out] h_z Z coordinate of the sense wire position
//! \param[out] h_cx X coordinate of the sense wire direction
//! \param[out] h_cy Y coordinate of the sense wire direction
//! \param[out] h_cz Z coordinate of the sense wire direction
void TABMparGeo::GetCellInfo(Int_t view, Int_t layer, Int_t cellID, Double_t& h_x, Double_t& h_y, Double_t& h_z, Double_t& h_cx, Double_t& h_cy, Double_t& h_cz) const {

   int my_ID = GetSenseId(cellID);

   h_x = fPosX[my_ID][layer][view];
   h_y = fPosY[my_ID][layer][view];
   h_z = fPosZ[my_ID][layer][view];

   h_cx =  fPosCX[my_ID][layer][view];
   h_cy =  fPosCY[my_ID][layer][view];
   h_cz =  fPosCZ[my_ID][layer][view];

   return;
}


//______________________________________________________________________________
//! Get the sense wire displacements read from the geometry file
//!
//! \param[in] i cellid index of the cell [0-35]
//! \return wire displacement vector
   TVector3 TABMparGeo::GetWireAlign(Int_t i)
{
  if(i<0 || i>35){
    cout<<"TABMparGeo::GetWireAlign::Eroor!!!!  wire index="<<i<<" it should be >0 && <36, I'll return a dummy 0 TVector3"<<endl;
    return TVector3(0,0,0);
  }
  return TVector3(GetTransfo(i)->GetTranslation()[0],GetTransfo(i)->GetTranslation()[1],GetTransfo(i)->GetTranslation()[2]);
}

//______________________________________________________________________________
//! Get the sense wire tilt read from the geometry file
//!
//! \param[in] i cellid index of the cell [0-35]
//! \return wire tilt vector
TVector3 TABMparGeo::GetWireTilt(Int_t i)
{
  if(i<0 || i>35){
    cout<<"TABMparGeo::GetWireTilt::Eroor!!!!  wire index="<<i<<" it should be >0 && <36, I'll return a dummy 0 TVector3"<<endl;
    return TVector3(0,0,0);
  }
  TGeoRotation rotmatrix(*GetTransfo(i));
  Double_t phi, theta,psi;
  rotmatrix.GetAngles(phi,theta,psi);
  return TVector3(phi,theta,psi);
}

//______________________________________________________________________________
//! Get the sense wire position without the tilt or the displacement
//!
//! \param[in] view view index of the cell [0-1]
//! \param[in] layer layer index of the cell [0-5]
//! \param[in] wire wire number of the cell [0-20]
//! \return wire position vector without the tilt or the displacement
TVector3 TABMparGeo::GetWirePos(Int_t view, Int_t layer, Int_t wire) const
{
   return TVector3(fPosX[wire][layer][view], fPosY[wire][layer][view], fPosZ[wire][layer][view]);
}

//______________________________________________________________________________
//! For a given view, get the direction of the sense wire (along X or Y)
//!
//! \param[in] view view index of the cell
//! \return direction of the sense wire
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
//!Print to stream
void TABMparGeo::ToStream(ostream& os, Option_t*) const
{
   os << "TABMparGeo " << GetName() << endl;
   os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
   << "   ver_x   ver_y   ver_z  width" << endl;
}

//_____________________________________________________________________________
//! build the BM
//!
//! \param[in] bmName name of the detector
//! /return the root representaion of the BM
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

   return box;
}


//_____________________________________________________________________________
//! Build a layer of the BM
//!
//! \param[in] idx layer index of the bm
//! \return the root representation of the layer
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
//! set color for fired layers, useful for the event display
//!
//! \param[in] idx index of the fired layer
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
//! reset color for unfired layers, useful for the event display
//!
//! \param[in] idx index of the layer
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
//! Print the BM bodies for the FLUKA geometry file
string TABMparGeo::PrintBodies(){

  stringstream ss;
  ss << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

  if(TAGrecoManager::GetPar()->IncludeBM()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

    //Reading the BM global position from global map file.
    TVector3  fCenter = fpFootGeo->GetBMCenter();
    TVector3  fAngle = fpFootGeo->GetBMAngles();

    ss << "* ***Beam Monitor" << endl;

    int iSense[2]={-1,-1}, iField[2]={-1,-1};
    // stringstream ss;
    char bodyname[20];
    double xmin, xmax, ymin, ymax, zmin, zmax;

    double shift = 0.00001;

    ss << setiosflags(ios::fixed) << setprecision(6);

    if(fAngle.Mag()!=0)
      ss << "$start_transform bm" << endl;

    ss << "RPP BmnShiOu    "
       << fCenter[0]-fBmSideDch[0]/2.-fShieldThick << " "
       << fCenter[0]+fBmSideDch[0]/2.+fShieldThick << " "
       << fCenter[1]-fBmSideDch[1]/2.-fShieldThick << " "
       << fCenter[1]+fBmSideDch[1]/2.+fShieldThick << " "
       << fCenter[2]-fBmSideDch[2]/2.-1. << " "
       << fCenter[2]+fBmSideDch[2]/2.+1. << endl;

    ss << "RPP BmnShiIn    "
       << fCenter[0]-fBmSideDch[0]/2. << " "
       << fCenter[0]+fBmSideDch[0]/2. << " "
       << fCenter[1]-fBmSideDch[1]/2. << " "
       << fCenter[1]+fBmSideDch[1]/2. << " "
       << fCenter[2]-fBmSideDch[2]/2.-1. << " "
       << fCenter[2]+fBmSideDch[2]/2.+1. << endl;

    ss << "XYP BmnMyl0     " << fCenter[2]-fBmSideDch[2]/2.-fFoilThick << endl;
    ss << "XYP BmnMyl1     " << fCenter[2]-fBmSideDch[2]/2. << endl;
    ss << "XYP BmnMyl2     " << fCenter[2]+fBmSideDch[2]/2. << endl;
    ss << "XYP BmnMyl3     " << fCenter[2]+fBmSideDch[2]/2.+fFoilThick << endl;


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
	    ss << "RPP BmnC" << iv << cella << "   "
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
	    ss << "RCC BmnS" << iv << iSense[iv] << "   "
	       << fCenter[0] + fPosX[iw][il][iv] << " "
	       << fCenter[1] + fPosY[iw][il][iv] << " "
	       << fCenter[2] + fPosZ[iw][il][iv] << " "
	       << fPosCX[iw][il][iv] << " " << fPosCY[iw][il][iv] << " "
	       << fPosCZ[iw][il][iv] << " " << fSenseRadius << endl;
	  } else {
	    iField[iv]++;     // loop on views
	    ss << "RCC BmnF" << iv << iField[iv] << "   "
	       << fCenter[0] + fPosX[iw][il][iv] << " "
	       << fCenter[1] + fPosY[iw][il][iv] << " "
	       << fCenter[2] + fPosZ[iw][il][iv] << " "
	       << fPosCX[iw][il][iv] << " " << fPosCY[iw][il][iv] << " "
	       << fPosCZ[iw][il][iv] << " " << fFieldRadius << endl;
	  }
	}
      }
    }


    if(fAngle.Mag()!=0)
      ss << "$end_transform" << endl;

  }

  return ss.str();
}


//_____________________________________________________________________________
//! Print the BM rotations for the FLUKA geometry file
string TABMparGeo::PrintRotations()
{
  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeBM()){

    TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

    TVector3 fCenter = fpFootGeo->GetBMCenter();
    TVector3  fAngle = fpFootGeo->GetBMAngles();

    if(fAngle.X()!=0 || fAngle.Y()!=0 || fAngle.Z()!=0){

      ss << PrintCard("ROT-DEFI", "300.", "", "",
		      Form("%f",-fCenter.X()), Form("%f",-fCenter.Y()),
		      Form("%f",-fCenter.Z()), "bm") << endl;
      if(fAngle.X()!=0)
	ss << PrintCard("ROT-DEFI", "100.", "", Form("%f",fAngle.X()),"", "", "", "bm") << endl;
      if(fAngle.Y()!=0)
	ss << PrintCard("ROT-DEFI", "200.", "", Form("%f",fAngle.Y()),"", "", "", "bm") << endl;
      if(fAngle.Z()!=0)
	ss << PrintCard("ROT-DEFI", "300.", "", Form("%f",fAngle.Z()),"", "", "", "bm") << endl;
      ss << PrintCard("ROT-DEFI", "300.", "", "",
		      Form("%f",fCenter.X()), Form("%f",fCenter.Y()),
		      Form("%f",fCenter.Z()), "bm") << endl;

    }
  }

  return ss.str();

}


//_____________________________________________________________________________
//! Print the BM regions for the FLUKA geometry file
string TABMparGeo::PrintRegions(){

  stringstream outstr;

  if(TAGrecoManager::GetPar()->IncludeBM()){

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
//! Get the FLUKA region index of the bm cell
//!
//! \param[in] cellid cellid index [0-35]
//! \return region index
Int_t TABMparGeo::GetRegCell(Int_t cellid) {
  TString regname("BMN_C");
  regname.Append((((Int_t)(cellid/3)) % 2 == 0) ? "0":"1");
  regname.Append(((Int_t)(cellid/6))*3 + cellid % 3);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Get the region index of the bm cell
//!
//! \param[in] ilay layer index of the cell [0-5]
//! \param[in] iview view index of the cell [0-1]
//! \param[in] icell cell index of the cell [0-2]
//! \return region index
Int_t TABMparGeo::GetRegCell(Int_t ilay, Int_t iview, Int_t icell) {
  TString regname("BMN_C");
  regname.Append(iview);
  regname.Append(ilay*3 + icell);
  return GetCrossReg(regname);
}

//_____________________________________________________________________________
//! Print the BM regions that need to be subtracted from the air for the FLUKA geometry file
string TABMparGeo::PrintSubtractBodiesFromAir() {

  stringstream ss;

  if(TAGrecoManager::GetPar()->IncludeBM()){

    ss << "-(BmnShiOu -BmnShiIn) -(BmnShiIn -BmnMyl0 +BmnMyl3)" << endl;

  }

  return ss.str();

}



//_____________________________________________________________________________
//! Print the BM material for the FLUKA geometry file
string TABMparGeo::PrintAssignMaterial(TAGmaterials *Material) {

    // loop in order of the material alfabeth
    stringstream outstr;

    if(TAGrecoManager::GetPar()->IncludeBM()){

      TString flkmatFWire, flkmatSWire, flkmatGas, flkmatFoil;

      if (Material == NULL){
	TAGmaterials::Instance()->PrintMaterialFluka();
	flkmatFWire = TAGmaterials::Instance()->GetFlukaMatName(fFieldMat.Data());
	flkmatSWire = TAGmaterials::Instance()->GetFlukaMatName(fSenseMat.Data());
	flkmatGas   = TAGmaterials::Instance()->GetFlukaMatName(fGasMixture.Data());
	flkmatFoil  = TAGmaterials::Instance()->GetFlukaMatName(fFoilMat.Data());
      }
      else{
	flkmatFWire = Material->GetFlukaMatName(fFieldMat.Data());
	flkmatSWire = Material->GetFlukaMatName(fSenseMat.Data());
	flkmatGas   = Material->GetFlukaMatName(fGasMixture.Data());
	flkmatFoil  = Material->GetFlukaMatName(fFoilMat.Data());
      }

      bool magnetic = false;
      if(TAGrecoManager::GetPar()->IncludeDI())
	magnetic = true;

      outstr << PrintCard("ASSIGNMA", flkmatFWire, "BMN_SHI", "", "", Form("%d",magnetic), "", "") << endl;//shield has the same mat as field wires
      outstr << PrintCard("ASSIGNMA", flkmatFWire, "BMN_FWI", "", "", Form("%d",magnetic), "", "") << endl;
      outstr << PrintCard("ASSIGNMA", flkmatSWire, "BMN_SWI", "", "", Form("%d",magnetic), "", "") << endl;
      outstr << PrintCard("ASSIGNMA", flkmatGas, "BMN_GAS", "", "", Form("%d",magnetic), "", "") << endl;
      outstr << PrintCard("ASSIGNMA", flkmatGas, "BMN_C000", "BMN_C017", "1.", Form("%d",magnetic), "", "") << endl;
      outstr << PrintCard("ASSIGNMA", flkmatGas, "BMN_C100", "BMN_C117", "1.", Form("%d",magnetic), "", "") << endl;
      outstr << PrintCard("ASSIGNMA", flkmatFoil, "BMN_MYL0", "BMN_MYL1", "1.", Form("%d",magnetic), "", "") << endl;

    }

    return outstr.str();
}

//_____________________________________________________________________________
//! Print the BM parameters
string TABMparGeo::PrintParameters() {

  stringstream outstr;
  outstr << setiosflags(ios::fixed) << setprecision(5);

  if(TAGrecoManager::GetPar()->IncludeBM()){

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
//! given a layer, view and a point position, it returns the cell index of the cell that contains the point or that is the closest one
//!
//! \param[in] pos position of the point
//! \param[in] layer layer index
//! \param[in] view view index
//! \return cell index
Int_t TABMparGeo::GetCell(TVector3 pos, int layer, int view)
{
    Int_t cell = -1 ;
    Float_t min = 999999;

    for(int i=0 ; i<fSensesN ; ++i){
      Int_t kk = fBmIdSense[i];
      Float_t distX = TMath::Abs(pos[0] - fPosX[kk][layer][view]);
      Float_t distY = TMath::Abs(pos[1] - fPosY[kk][layer][view]);

      if (view == 0) {
        if (distY < min) {
          min = distY;
          cell = i;
        }
      } else {
        if (distX < min) {
          min = distX;
          cell = i;
        }
      }
    }

    return cell ;
}

//______________________________________________________________________________
//! Caluculate the minimum distance between two tracks, useful to evaluate the drift distances of fitted tracks or of the BM MC hits
//!
//! \param[in] pos a point position of the first track (e.g.: the fitted track intercept)
//! \param[in] dir direction of the first track (e.g.: the fitted track direction)
//! \param[in] A0 point position of the second track (e.g.: the sense wire position)
//! \param[in] A0 direction of the second track (e.g.: the sense wire direction)
//! \param[in] isTrack if it is true, it will disable the warning messages in case the output drift distance is greater than the maximum drift distance allowed by the FOOT BM, otherwise it will print a warning message useful if the MC file and the BM geometry do not match
//! \return drift distance in cm
Double_t TABMparGeo::FindRdrift(TVector3 pos, TVector3 dir, TVector3 A0, TVector3 Wvers, Bool_t isTrack) const {

  Double_t tp = 0., tf= 0., rdrift;
  Wvers.SetMag(1.);
  dir.SetMag(1.);

  TVector3 D0 = pos - A0;//distance between position of reference point of current wire and current particle position
  Double_t prosca = dir*Wvers ;//scalar product of directions
  Double_t D0W = D0*Wvers;//distance projected on wire
  Double_t D0P = D0*dir;//distance projected on particle direction

  if(prosca!= 1.) {//if the don't fly parallel
    tp = (D0W*prosca - D0P)/(1.-prosca*prosca);
    tf = (-D0P*prosca + D0W)/(1.-prosca*prosca);
    rdrift = sqrt( abs(D0.Mag2() + tp*tp + tf*tf + 2.*tp*D0P -2.*tf*D0W -2.*prosca*tf*tp ));
    }
  else  //if they go parallel
    rdrift = sqrt(abs( D0.Mag2() - D0W*D0W));

  if((rdrift<0 || rdrift>0.945) && isTrack==false){
    cout<<"WARNING!!!!! SOMETHING IS WRONG IN THE BM RDRIFT!!!!!!!!!  look at TABMactNtuMC;   rdrift="<<rdrift<<endl;
    rdrift= (rdrift<0) ? 0. : 0.945 ;
    cout<<"now rdrift="<<rdrift<<endl;
    cout<<"pos=("<<pos.X()<<","<<pos.Y()<<","<<pos.Z()<<")  dir=("<<dir.X()<<","<<dir.Y()<<","<<dir.Z()<<")"<<endl;
    cout<<"A0=("<<A0.X()<<","<<A0.Y()<<","<<A0.Z()<<")  Wvers=("<<Wvers.X()<<","<<Wvers.Y()<<","<<Wvers.Z()<<")"<<endl;
  }else if(rdrift<0 && isTrack==true){
    cout<<"WARNING!!!!! SOMETHING IS WRONG, YOU HAVE A NEGATIVE RDRIFT!!!!!!!!! look at TABMactNtuTrack::FindRdrift    rdrift="<<rdrift<<endl;
    cout<<"pos=("<<pos.X()<<","<<pos.Y()<<","<<pos.Z()<<")  dir=("<<dir.X()<<","<<dir.Y()<<","<<dir.Z()<<")"<<endl;
    cout<<"A0=("<<A0.X()<<","<<A0.Y()<<","<<A0.Z()<<")  Wvers=("<<Wvers.X()<<","<<Wvers.Y()<<","<<Wvers.Z()<<")"<<endl;
    rdrift=0.;
  }

  return rdrift;
}
