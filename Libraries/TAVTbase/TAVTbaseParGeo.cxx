
/*!
 \file TAVTbaseParGeo.cxx
 \brief Base class of geometrical parameters for VTX
 */

#include <Riostream.h>

#include "TGeoBBox.h"
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
#include "TAGmaterials.hxx"
#include "TAGionisMaterials.hxx"

#include "TAVTbaseParGeo.hxx"

using namespace std;

//##############################################################################

/*!
  \class TAVTbaseParGeo
  \brief Map and Geometry parameters for VTX
*/

//! Class Imp
ClassImp(TAVTbaseParGeo);

//______________________________________________________________________________
//! Standard constructor
TAVTbaseParGeo::TAVTbaseParGeo()
 : TAGparTools(),
   fIonisation(new TAGionisMaterials()),
   fSensorsN(0),
   fkDefaultGeoName(""),
   fLayersN(fSensorsN),
   fSubLayersN(fSensorsN),
   fFlagMC(false),
   fFlagIt(false),
   fSensPerLayer(0),
   fSensorArray(0x0)
{
}

//______________________________________________________________________________
//! Destructor
TAVTbaseParGeo::~TAVTbaseParGeo()
{
   delete fIonisation;
   delete [] fSensorArray;
}

//_____________________________________________________________________________
//! Define material
void TAVTbaseParGeo::DefineMaterial()
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // Epitaxial material
   TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fEpiMat, fEpiMatDensity);
   if(FootDebugLevel(1)) {
      printf("Expitaxial material:\n");
      mat->Print();
   }
   
   fIonisation->SetMaterial(mat);
   fIonisation->AddMeanExcitationEnergy(fEpiMatExc);
}

//------------------------------------------+-----------------------------------
//! Read from file
//!
//! \param[in] name file name
Bool_t TAVTbaseParGeo::FromFile(const TString& name)
{
   cout << setiosflags(ios::fixed) << setprecision(fgPrecisionLevel);

   // simple file reading, waiting for real config file
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultGeoName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   Info("FromFile()", "Open file %s for geometry\n", name.Data());

   ReadItem(fSensorsN);
   if(FootDebugLevel(1))
      cout << endl << "Sensors number "<< fSensorsN << endl;
   
   fSensorArray = new size_t[fSensorsN];
   
   ReadStrings(fTypeName);
   if(FootDebugLevel(1))
      cout  << endl << "  Type Name : "<< fTypeName.Data() << endl;
   
   ReadItem(fTypeNumber);
   if(FootDebugLevel(1))
      cout  << endl << "  Type Number : "<< fTypeNumber << endl;
   
   ReadItem(fPixelsNx);
   if(FootDebugLevel(1))
      cout  << "  Number of pixels in X: "<< fPixelsNx << endl;
   
   ReadItem(fPixelsNy);
   if(FootDebugLevel(1))
      cout  << "  Number of pixels in Y: "<< fPixelsNy << endl;
   
   ReadItem(fPitchX);
   if(FootDebugLevel(1))
      cout  << "  Pitch for pixels in X: "<< fPitchX << endl;
   
   ReadItem(fPitchY);
   if(FootDebugLevel(1))
      cout  << "  Pitch for pixels in Y: "<< fPitchY << endl;
   
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
   
   ReadItem(fEpiMatExc);
   if(FootDebugLevel(1))
      cout  << "  Sensitive material mean excitation energy:  "<< fEpiMatExc << endl;
   
   ReadItem(fPixThickness);
   if(FootDebugLevel(1))
      cout  << endl << "  Pixel thickness: "<< fPixThickness << endl;
   
   ReadStrings(fPixMat);
   if(FootDebugLevel(1))
      cout   << "  Pixel material: "<< fPixMat.Data() << endl;
   
   ReadStrings(fPixMatDensities);
   if(FootDebugLevel(1))
      cout  << "  Pixel material component densities: "<< fPixMatDensities.Data() << endl;
   
   ReadStrings(fPixMatProp);
   if(FootDebugLevel(1))
      cout  << "  Pixel material proportion: "<< fPixMatProp.Data() << endl;
   
   ReadItem(fPixMatDensity);
   if(FootDebugLevel(1))
      cout  << "  Pixel material density:  "<< fPixMatDensity << endl;
   
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
      if (fFlagMC && !fFlagIt)
         fSensorParameter[p].Position[0] = fSensorParameter[p].Position[1] = 0.;
   
      if(FootDebugLevel(1))
         cout << "   Position: " << fSensorParameter[p].Position[0] << " " << fSensorParameter[p].Position[1] << " " << fSensorParameter[p].Position[2] << endl;
      
      // fill map
      fSensorMap[fSensorParameter[p].Position[2]].push_back(fSensorParameter[p].SensorIdx-1);
      
      // read sensor angles
      ReadVector3(fSensorParameter[p].Tilt);
      if(FootDebugLevel(1))
         cout  << "   Tilt: "
		       << fSensorParameter[p].Tilt[0] << " " <<  fSensorParameter[p].Tilt[1] << " " << fSensorParameter[p].Tilt[2] << endl;
      
      // read alignment
      ReadItem(fSensorParameter[p].AlignmentU);
      ReadItem(fSensorParameter[p].AlignmentV);
      if (fFlagMC)
         fSensorParameter[p].AlignmentU = fSensorParameter[p].AlignmentV = 0.;
      
      if(FootDebugLevel(1))
         cout  << "   Alignment: " <<  fSensorParameter[p].AlignmentU << " " << fSensorParameter[p].AlignmentV << endl;
      
      // read tiltW
      ReadItem(fSensorParameter[p].TiltW);
      if (fFlagMC)
         fSensorParameter[p].TiltW = 0.;
      
      if(FootDebugLevel(1))
         cout  << "   Rotation tiltW: " << fSensorParameter[p].TiltW << endl;
      
      Float_t thetaX = fSensorParameter[p].Tilt[0];
      if (TMath::Nint(thetaX) == 180)
         fSensorParameter[p].IsReverseX = true;
      else
         fSensorParameter[p].IsReverseX = false;

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
   
   // fill sensor map
   FillSensorMap();
   
   return true;
}

//_____________________________________________________________________________
//! Get sensor position
//!
//! \param[in] iSensor a given sensor
TVector3 TAVTbaseParGeo::GetSensorPosition(Int_t iSensor)
{
   TGeoHMatrix* hm = GetTransfo(iSensor);
   if (hm) {
	  TVector3 local(0,0,0);
	  fCurrentPosition =  Sensor2Detector(iSensor,local);
   }
   return fCurrentPosition;
}

// Mapping
//_____________________________________________________________________________
//! Get index number from line and column numbers
//!
//! \param[in] line line number
//! \param[in] column column number
Int_t TAVTbaseParGeo::GetIndex(Int_t line, Int_t column) const
{
   return line*fPixelsNx + column;
}

//_____________________________________________________________________________
//! Get position in X from column number
//!
//! \param[in] column a given column
Float_t TAVTbaseParGeo::GetPositionU(Int_t column) const
{
   Float_t x = (Float_t(2*column - fPixelsNx + 1) * fPitchX)/2.;// + fEpiOffset[0];
   return  x;
}

//_____________________________________________________________________________
//! Get position in Y from line number
//!
//! \param[in] line a given line
Float_t TAVTbaseParGeo::GetPositionV(Int_t line) const
{
   Float_t y = -(Float_t(2*line - fPixelsNy + 1) * fPitchY)/2.;// + fEpiOffset[1];
   return  y;
}

//_____________________________________________________________________________
//! Get column number from x position
//!
//! \param[in] x position in X
Int_t TAVTbaseParGeo::GetColumn(Float_t x) const
{
   Float_t xmin = -fPixelsNx*fPitchX/2.;// - fEpiOffset[0];

   if (x < xmin || x > -xmin) {
      if(FootDebugLevel(1))
		 Warning("GetColumn()", "Value of X: %f out of range +/- %f\n", x, xmin);
	  return -1;
   }
   
   Int_t col = floor((x-xmin)/fPitchX);
   return col;
}

//_____________________________________________________________________________
//! Get line number from y position
//!
//! \param[in] y position in Y
Int_t TAVTbaseParGeo::GetLine(Float_t y) const
{
   // equivalent to  floor((-y-ymin)/ffPitchY)-1
   Float_t ymin = -fPixelsNy*fPitchY/2.;// - fEpiOffset[1];

   if (y < ymin || y > -ymin) {
      if(FootDebugLevel(1))
		 Warning("GetLine()", "Value of Y: %f out of range +/- %f\n", y, ymin);
	  return -1;
   }
   
   Int_t line = floor((y-ymin)/fPitchY);
   return fPixelsNy - line - 1;
}

// transformation
//_____________________________________________________________________________
//! Transformation from detector to sensor framework
//!
//! \param[in] detID sensor id
//! \param[in] xg X position in detector framework
//! \param[in] yg Y position in detector framework
//! \param[in] zg Z position in detector framework
//! \param[out] xl X position in sensor framework
//! \param[out] yl Y position in sensor framework
//! \param[out] zl Z position in sensor framework
void TAVTbaseParGeo::Detector2Sensor(Int_t detID,
									Double_t xg, Double_t yg, Double_t zg, 
									Double_t& xl, Double_t& yl, Double_t& zl) const
{  
   if (detID < 0 || detID > GetSensorsN()) {
	  Warning("Detector2Sensor()","Wrong detector id number: %d ", detID); 
	  return ;
   }
   
   MasterToLocal(detID, xg, yg, zg, xl, yl, zl);
}   

//_____________________________________________________________________________
//! Transformation from detector to sensor framework
//!
//! \param[in] detID sensor id
//! \param[in] glob position in detector framework
//! \return position in sensor framework
TVector3 TAVTbaseParGeo::Detector2Sensor(Int_t detID, TVector3& glob) const
{
   if (detID < 0 || detID > GetSensorsN()) {
	  Warning("Detector2Sensor()","Wrong detector id number: %d ", detID); 
	  return TVector3(0,0,0);
   }
   
   return MasterToLocal(detID, glob);
}

//_____________________________________________________________________________
//! Transformation from detector to sensor framework for vector (no translation)
//!
//! \param[in] detID sensor id
//! \param[in] glob position in detector framework
//! \return position in sensor framework
TVector3 TAVTbaseParGeo::Detector2SensorVect(Int_t detID, TVector3& glob) const
{
   if (detID < 0 || detID > GetSensorsN()) {
	  Warning("Detector2SensorVect()","Wrong detector id number: %d ", detID); 
	  return TVector3(0,0,0);
   }
   
   return MasterToLocalVect(detID, glob);
}   

//_____________________________________________________________________________
//! Transformation from sensor to detector framework
//!
//! \param[in] detID sensor id
//! \param[in] xl X position in sensor framework
//! \param[in] yl Y position in sensor framework
//! \param[in] zl Z position in sensor framework
//! \param[out] xg X position in detector framework
//! \param[out] yg Y position in detector framework
//! \param[out] zg Z position in detector framework
void TAVTbaseParGeo::Sensor2Detector(Int_t detID,
									Double_t xl, Double_t yl, Double_t zl, 
									Double_t& xg, Double_t& yg, Double_t& zg) const
{
   if (detID < 0 || detID > GetSensorsN()) {
	  Warning("Sensor2Detector()","Wrong detector id number: %d ", detID); 
	  return;
   }
   
   LocalToMaster(detID, xl, yl, zl, xg, yg, zg);
}   

//_____________________________________________________________________________
//! Transformation from sensor to detector framework
//!
//! \param[in] detID sensor id
//! \param[in] loc position in sensor framework
//! \return position in detector framework
TVector3 TAVTbaseParGeo::Sensor2Detector(Int_t detID, TVector3& loc) const
{
   if (detID < 0 || detID > GetSensorsN()) {
	  Warning("Sensor2Detector()","Wrong detector id number: %d ", detID); 
	  TVector3(0,0,0);
   }
   
   return LocalToMaster(detID, loc);
}


//_____________________________________________________________________________
//! Transformation from sensor to detector framework for vector (no translation)
//!
//! \param[in] detID sensor id
//! \param[in] loc position in sensor framework
//! \return position in detector framework
TVector3 TAVTbaseParGeo::Sensor2DetectorVect(Int_t detID, TVector3& loc) const
{
   if (detID < 0 || detID > GetSensorsN()) {
	  Warning("Sensor2DetectorVect()","Wrong detector id number: %d ", detID); 
	  TVector3(0,0,0);
   }

   return LocalToMasterVect(detID, loc);
}

//_____________________________________________________________________________
//! Fill array of sensor number per layer and position layer
void TAVTbaseParGeo::FillSensorMap()
{
   map<float, vector<size_t> >::iterator itr = fSensorMap.begin();
   vector<size_t> v;
   Int_t iLayer = 0;
   
   while (itr != fSensorMap.end()) {
      if (FootDebugLevel(2))
         cout << itr->first << endl;
      v = itr->second;
      std::copy(v.begin(), v.end(), &fSensorArray[iLayer*fSensPerLayer]);
      iLayer++;
      itr++;
   }
}

//_____________________________________________________________________________
//! Get number of sensors for a given layer
//!
//! \param[in] iLayer a given layer
std::size_t* TAVTbaseParGeo::GetSensorsPerLayer(Int_t iLayer)
{
   return &fSensorArray[iLayer*fSensPerLayer];
}

//_____________________________________________________________________________
//! Get position in Z direction for a given layer
//!
//! \param[in] layer a given layer
Float_t TAVTbaseParGeo::GetLayerPosZ(Int_t layer)
{
   map<float, vector<size_t> >::iterator itr = fSensorMap.begin();
   Int_t iLayer = 0;
   
   while (itr != fSensorMap.end()) {
      if (FootDebugLevel(2))
         cout << itr->first << endl;
      
      if (iLayer++ == layer)
         return itr->first;
      itr++;
   }
   
   return -99;
}

//_____________________________________________________________________________
//! Define envelop of the detector
void TAVTbaseParGeo::DefineMaxMinDimension()
{
   TVector3 posAct(0, 0, 0);
   TVector3 EnvDim(0,0,0);
   TVector3 shift(0,0,0);
   
   TVector3 minPosition(10e10, 10e10, 10e10);
   TVector3 maxPosition(-10e10, -10e10, -10e10);
   
   Int_t nSens = GetSensorsN();
   for (Int_t iS = 0; iS < nSens; iS++) {
      posAct = GetSensorPar(iS).Position;
      
      for(Int_t i = 0; i < 3; i++) {
         shift[i] = TMath::Abs(shift[i]);
         minPosition[i] = (minPosition[i] <= posAct[i]) ? minPosition[i] : posAct[i];
         maxPosition[i] = (maxPosition[i] >= posAct[i]) ? maxPosition[i] : posAct[i];
      }
   }
   
   fMinPosition = minPosition;
   fMaxPosition = maxPosition;
      
   for(Int_t i = 0; i< 3; ++i)
      fSizeBox[i] = (fMaxPosition[i] - fMinPosition[i]);
   fSizeBox += GetTotalSize();
}
