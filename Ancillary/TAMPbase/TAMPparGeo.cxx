
/*!
 \file TAMPparGeo.cxx
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

#include "TAMPparGeo.hxx"

using namespace std;

//##############################################################################

/*!
  \class TAMPparGeo
  \brief Map and Geometry parameters for VTX
*/

//! Class Imp
ClassImp(TAMPparGeo);

const Int_t TAMPparGeo::fgkDefSensorsN = 32;

//______________________________________________________________________________
//! Standard constructor
TAMPparGeo::TAMPparGeo()
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
TAMPparGeo::~TAMPparGeo()
{
   delete fIonisation;
   delete [] fSensorArray;
}

//_____________________________________________________________________________
//! Define material
void TAMPparGeo::DefineMaterial()
{
   if ( gGeoManager == 0x0 ) { // a new Geo Manager is created if needed
      new TGeoManager( TAGgeoTrafo::GetDefaultGeomName(), TAGgeoTrafo::GetDefaultGeomTitle());
   }
   
   // Epitaxial material
   for (Int_t t = 0; t < fTypesN; ++t) { // Loop on each type
      
      TGeoMaterial* mat = TAGmaterials::Instance()->CreateMaterial(fTypeParameter[t].EpiMat, fTypeParameter[t].EpiMatDensity);
      if(FootDebugLevel(1)) {
         printf("Expitaxial material:\n");
         mat->Print();
      }
   }
   
//   fIonisation->SetMaterial(mat);
//   fIonisation->AddMeanExcitationEnergy(fEpiMatExc);
}

//------------------------------------------+-----------------------------------
//! Read from file
//!
//! \param[in] name file name
Bool_t TAMPparGeo::FromFile(const TString& name)
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
   
   fSensorArray = new size_t[fgkDefSensorsN];
   
   ReadItem(fTypesN);
   if(FootDebugLevel(1))
      cout << endl << "Reading Parameters for "<< fTypesN << " types" << endl;
   
   for (Int_t t = 0; t < fTypesN; ++t) { // Loop on each type
      
      ReadStrings(fTypeParameter[t].TypeName);
      if(FootDebugLevel(1))
         cout  << endl << "  Type Name : "<< fTypeParameter[t].TypeName.Data() << endl;
      
      ReadItem(fTypeParameter[t].TypeNumber);
      if(FootDebugLevel(1))
         cout  << endl << "  Type Number : "<< fTypeParameter[t].TypeNumber << endl;
      
      ReadItem(fTypeParameter[t].PixelsNx);
      if(FootDebugLevel(1))
         cout  << "  Number of pixels in X: "<< fTypeParameter[t].PixelsNx << endl;
      
      ReadItem(fTypeParameter[t].PixelsNy);
      if(FootDebugLevel(1))
         cout  << "  Number of pixels in Y: "<< fTypeParameter[t].PixelsNy << endl;
      
      ReadItem(fTypeParameter[t].PitchX);
      if(FootDebugLevel(1))
         cout  << "  Pitch for pixels in X: "<< fTypeParameter[t].PitchX << endl;
      
      ReadItem(fTypeParameter[t].PitchY);
      if(FootDebugLevel(1))
         cout  << "  Pitch for pixels in Y: "<< fTypeParameter[t].PitchY << endl;
      
      ReadVector3(fTypeParameter[t].TotalSize);
      if(FootDebugLevel(1))
         cout  << "  Total size of sensor:     "<< fTypeParameter[t].TotalSize.X() << " " <<  fTypeParameter[t].TotalSize.Y() << " "
         <<  fTypeParameter[t].TotalSize.Z()  << endl;
      
      ReadVector3(fTypeParameter[t].EpiSize);
      if(FootDebugLevel(1))
         cout  << endl << "  Sensitive size of sensor: "<< fTypeParameter[t].EpiSize.X() << " " <<  fTypeParameter[t].EpiSize.Y() << " "
         <<  fTypeParameter[t].EpiSize.Z()  << endl;
      
      ReadVector3(fTypeParameter[t].EpiOffset);
      if(FootDebugLevel(1))
         cout  << endl << "  Offset of sensitive area of sensor: "<< fTypeParameter[t].EpiOffset.X() << " " <<  fTypeParameter[t].EpiOffset.Y() << " "
         <<  fTypeParameter[t].EpiOffset.Z()  << endl;
      
      ReadStrings(fTypeParameter[t].EpiMat);
      if(FootDebugLevel(1))
         cout   << "  Sensitive material: "<< fTypeParameter[t].EpiMat.Data() << endl;
      
      ReadItem(fTypeParameter[t].EpiMatDensity);
      if(FootDebugLevel(1))
         cout  << "  Sensitive material density:  "<< fTypeParameter[t].EpiMatDensity << endl;
      
      ReadItem(fTypeParameter[t].EpiMatExc);
      if(FootDebugLevel(1))
         cout  << "  Sensitive material mean excitation energy:  "<< fTypeParameter[t].EpiMatExc << endl;
      
      ReadItem(fTypeParameter[t].PixThickness);
      if(FootDebugLevel(1))
         cout  << endl << "  Pixel thickness: "<< fTypeParameter[t].PixThickness << endl;
      
      ReadStrings(fTypeParameter[t].PixMat);
      if(FootDebugLevel(1))
         cout   << "  Pixel material: "<< fTypeParameter[t].PixMat.Data() << endl;
      
      ReadStrings(fTypeParameter[t].PixMatDensities);
      if(FootDebugLevel(1))
         cout  << "  Pixel material component densities: "<< fTypeParameter[t].PixMatDensities.Data() << endl;
      
      ReadStrings(fTypeParameter[t].PixMatProp);
      if(FootDebugLevel(1))
         cout  << "  Pixel material proportion: "<< fTypeParameter[t].PixMatProp.Data() << endl;
      
      ReadItem(fTypeParameter[t].PixMatDensity);
      if(FootDebugLevel(1))
         cout  << "  Pixel material density:  "<< fTypeParameter[t].PixMatDensity << endl;
   }
   
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
TVector3 TAMPparGeo::GetSensorPosition(Int_t iSensor)
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
Int_t TAMPparGeo::GetIndex(Int_t line, Int_t column, Int_t type) const
{
   return line*fTypeParameter[type].PixelsNx + column;
}

//_____________________________________________________________________________
//! Get position in X from column number
//!
//! \param[in] column a given column
Float_t TAMPparGeo::GetPositionU(Int_t column, Int_t type) const
{
   Float_t x = (Float_t(2*column - fTypeParameter[type].PixelsNx + 1) * fTypeParameter[type].PitchX)/2.;// + fEpiOffset[0];
   return  x;
}

//_____________________________________________________________________________
//! Get position in Y from line number
//!
//! \param[in] line a given line
Float_t TAMPparGeo::GetPositionV(Int_t line, Int_t type) const
{
   Float_t y = -(Float_t(2*line - fTypeParameter[type].PixelsNy + 1) * fTypeParameter[type].PitchY)/2.;// + fEpiOffset[1];
   return  y;
}

//_____________________________________________________________________________
//! Get column number from x position
//!
//! \param[in] x position in X
Int_t TAMPparGeo::GetColumn(Float_t x, Int_t type) const
{
   Float_t xmin = -fTypeParameter[type].PixelsNx*fTypeParameter[type].PitchX/2.;// - fEpiOffset[0];

   if (x < xmin || x > -xmin) {
      if(FootDebugLevel(1))
		 Warning("GetColumn()", "Value of X: %f out of range +/- %f\n", x, xmin);
	  return -1;
   }
   
   Int_t col = floor((x-xmin)/fTypeParameter[type].PitchX);
   return col;
}

//_____________________________________________________________________________
//! Get line number from y position
//!
//! \param[in] y position in Y
Int_t TAMPparGeo::GetLine(Float_t y, Int_t type) const
{
   // equivalent to  floor((-y-ymin)/ffPitchY)-1
   Float_t ymin = -fTypeParameter[type].PixelsNy*fTypeParameter[type].PitchY/2.;// - fEpiOffset[1];

   if (y < ymin || y > -ymin) {
      if(FootDebugLevel(1))
		 Warning("GetLine()", "Value of Y: %f out of range +/- %f\n", y, ymin);
	  return -1;
   }
   
   Int_t line = floor((y-ymin)/fTypeParameter[type].PitchY);
   return fTypeParameter[type].PixelsNy - line - 1;
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
void TAMPparGeo::Detector2Sensor(Int_t detID,
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
TVector3 TAMPparGeo::Detector2Sensor(Int_t detID, TVector3& glob) const
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
TVector3 TAMPparGeo::Detector2SensorVect(Int_t detID, TVector3& glob) const
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
void TAMPparGeo::Sensor2Detector(Int_t detID,
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
TVector3 TAMPparGeo::Sensor2Detector(Int_t detID, TVector3& loc) const
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
TVector3 TAMPparGeo::Sensor2DetectorVect(Int_t detID, TVector3& loc) const
{
   if (detID < 0 || detID > GetSensorsN()) {
	  Warning("Sensor2DetectorVect()","Wrong detector id number: %d ", detID); 
	  TVector3(0,0,0);
   }

   return LocalToMasterVect(detID, loc);
}

//_____________________________________________________________________________
//! Fill array of sensor number per layer and position layer
void TAMPparGeo::FillSensorMap()
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
std::size_t* TAMPparGeo::GetSensorsPerLayer(Int_t iLayer)
{
   return &fSensorArray[iLayer*fSensPerLayer];
}

//_____________________________________________________________________________
//! Get position in Z direction for a given layer
//!
//! \param[in] layer a given layer
Float_t TAMPparGeo::GetLayerPosZ(Int_t layer)
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
void TAMPparGeo::DefineMaxMinDimension()
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
   fSizeBox += GetTotalSize(0);
}
