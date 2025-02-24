/*!
 \file TACAcalibrationMap.cxx
 \brief  Implementation of TACAcalibrationMap
 */

#include "TError.h"

#include "TACAcalibrationMap.hxx"
#include "TAGxmlParser.hxx"
#include "TAGrecoManager.hxx"


/*!
 \class TACAcalibrationMap
 \brief Calibration map class
 */

//! Class Imp
ClassImp(TACAcalibrationMap)

//_____________________________________________________________________
//! Constructor
TACAcalibrationMap::TACAcalibrationMap()
: TAGobject()
{
}

//_____________________________________________________________________
//! Load crystal temperature calibration map file
//!
//! \param[in] FileName input file
void TACAcalibrationMap::LoadCryTemperatureCalibrationMap(std::string FileName)
{
   if (gSystem->AccessPathName(FileName.c_str())) {
      Error("LoadTemperatureCalibrationMap()", "File %s doesn't exist", FileName.c_str());
   }

   ///////// read the file with Charge calibration

   ifstream fin_Q;
   fin_Q.open(FileName,std::ifstream::in);

   // parameters for energy calibration p0 and p1
   Int_t nCrystals = 0;

   if (fin_Q.is_open()) {

      int  cnt(0);
      char line[200];

      fin_Q.getline(line, 200, '\n');
      if(strchr(line,'/') || strchr(line,'#'))  // skip first line if comment
         fin_Q.getline(line, 200, '\n');
      sscanf(line, "%d", &nCrystals);
      fCalibTempMapCry.resize(nCrystals);

      int crysId;  // Id of the crystal
      double Q_corrp0, Q_corrp1, Q_corrp2;

      // loop over all the slat crosses ( nBarCross*nLayers ) for two TW layers
      while (fin_Q.getline(line, 108, '\n')) {

         if (strchr(line,'/') || strchr(line,'#')) {
            if(FootDebugLevel(1))
               Info("LoadTempCalibrationMap()","Skip comment line:: %s\n",line);
            continue;
         }

         sscanf(line, "%d %lf %lf %lf",&crysId, &Q_corrp0, &Q_corrp1, &Q_corrp2);
         if(FootDebugLevel(1))
            Info("LoadTempCalibrationMap()","%d %.5f %.5f %.5f\n",crysId, Q_corrp0, Q_corrp1, Q_corrp2);

         fCalibTempMapCry[crysId] = ADC2TempParam_t{Q_corrp0, Q_corrp1, Q_corrp2};
         cnt++;
      }
   } else
      Info("LoadTempCalibrationMap()","File Temperature Calibration %s not open!!",FileName.data());

   fin_Q.close();
}

//_____________________________________________________________________
//! Load crystal energy calibration map file
//!
//! \param[in] FileName input file
void TACAcalibrationMap::LoadEnergyCalibrationMap(std::string FileName)
{
   if (gSystem->AccessPathName(FileName.c_str()))
      Error("LoadEnergyCalibrationMap()","File %s doesn't exist",FileName.c_str());

   ifstream fin_Q;
   fin_Q.open(FileName,std::ifstream::in);

   Int_t nCrystals = 0;

   if (!fin_Q.is_open()) {
      Info("LoadEnergyCalibrationMap()","File Calibration Energy %s not open!!",FileName.data());
      return;
   }

   char line[200];
   int crysId;  // Id of the crystal
   double Q_corrp0, Q_corrp1, Q_corrp2, Q_corrp3, Q_corrp4, Q_corrp5, Q_corrp6, Q_corrp7, Q_corrp8, Q_corrp9, Q_corrp10, Q_corrp11;

   // parameters for energy equilisation with Z
   fin_Q.getline(line, 200, '\n');

   while (fin_Q.getline(line, 200, '\n')) {

      if(line[0] == '#' && line[1] == '#')
         break;

      if(strchr(line,'/') || strchr(line,'#')) {
         if(FootDebugLevel(1))
            Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
         continue;
      }

      sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf", &Q_corrp0, &Q_corrp1, &Q_corrp2, &Q_corrp3, &Q_corrp4, &Q_corrp5, &Q_corrp6, &Q_corrp7, &Q_corrp8);
      if(FootDebugLevel(1))
         Info("LoadEnergyCalibrationMap()"," %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\n", Q_corrp0, Q_corrp1, Q_corrp2, Q_corrp3, Q_corrp4, Q_corrp5, Q_corrp6, Q_corrp7, Q_corrp8);

      // fCalibEnergyMap = ADC2EnergyParam_t{Q_corrp0, Q_corrp1, Q_corrp2, Q_corrp3, Q_corrp4, Q_corrp5, Q_corrp6, Q_corrp7, Q_corrp8, Q_corrp9, Q_corrp10, Q_corrp11};
      fCalibEnergyMap = ADC2EnergyParam_t{Q_corrp0, Q_corrp1, Q_corrp2, Q_corrp3, Q_corrp4, Q_corrp5, Q_corrp6, Q_corrp7, Q_corrp8};

   }

   // parameters for energy calibration p0, p1 and p2
   fin_Q.getline(line, 200, '\n');
   if(strchr(line,'/') || strchr(line,'#'))  // skip first line if comment
      fin_Q.getline(line, 200, '\n');
   sscanf(line, "%d", &nCrystals);
    fCalibEnergyMapCry.resize(nCrystals);


   while (fin_Q.getline(line, 200, '\n')) {

      if(strchr(line,'/') || strchr(line,'#'))  {
         if(FootDebugLevel(1))
            Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
         continue;
      }

      sscanf(line, "%d %lf %lf %lf",&crysId, &Q_corrp9, &Q_corrp10, &Q_corrp11);
      if(FootDebugLevel(1))
         Info("LoadEnergyCalibrationMap()","cryId %d %.3f %.3f %.3f\n",crysId, Q_corrp9, Q_corrp10, Q_corrp11);

      fCalibEnergyMapCry[crysId] = ADC2EnergyParamCry_t{Q_corrp9, Q_corrp10, Q_corrp11};
    }

   fin_Q.close();
}

//_____________________________________________________________________
//! Export to xml file
//!
//! \param[in] FileName input file
void TACAcalibrationMap::ExportToFile(std::string FileName)
{
   TAGxmlParser x;
   XMLNodePointer_t main=x.CreateMainNode("CALIBRATION");
   x.AddElementWithContent(TString::Format("DATE").Data(),main," ");
   x.AddElementWithContent(TString::Format("DESCRIPTION").Data(),main," ");
   for (auto it=fCalibrationMap.begin();it!=fCalibrationMap.end();++it)
   {
      Int_t cryId=it->first;
      XMLNodePointer_t b=x.AddElement(TString::Format("CRY").Data(),main);
      x.AddElementWithContent("CRY_ID",b, TString::Format("%d",cryId).Data());
      x.AddElementWithContent("T",b,TString::Format("%f",fCalibrationMap[cryId][0]).Data());
      x.AddElementWithContent("EQ",b,TString::Format("%f",fCalibrationMap[cryId][0]).Data());
   }
   x.ExportToFile(FileName,main);
}

//_______________________________________________
//! Check crystal number
//!
//! \param[in] cryId crystal id
bool TACAcalibrationMap::Exists(Int_t cryId)
{
   if (fCalibrationMap.count(cryId)==0)
   {
      return false;
   }
   return true;
}

//_______________________________________________
//! Get energy calibration parameter
//!
//! \param[in] cryId crystal id
//! \param[in] parId parameter id
Double_t TACAcalibrationMap::GetADC2EnergyParam(UInt_t parId)
{
   if (parId == 0)
      return fCalibEnergyMap.p0;
   else if (parId == 1)
      return fCalibEnergyMap.p1;
   else if (parId == 2)
      return fCalibEnergyMap.p2;
   else if (parId == 3)
      return fCalibEnergyMap.p3;
   else if (parId == 4)
      return fCalibEnergyMap.p4;
   else if (parId == 5)
      return fCalibEnergyMap.p5;
   else if (parId == 6)
      return fCalibEnergyMap.p6;
   else if (parId == 7)
      return fCalibEnergyMap.p7;
   else if (parId == 8)
      return fCalibEnergyMap.p8;
   else {
      Error("GetADC2EnergyParam()", "No parameter %d found", parId);
      return -99999;
   }
}

//_______________________________________________
//! Get temperature calibration parameter
//!
//! \param[in] cryId crystal id
Double_t TACAcalibrationMap::GetChargeEqParam(UInt_t cryId)
{
  return 0; //temporary bug fix
//   return fCalibEnergyMapCry[cryId];
}

//_______________________________________________
//! Get temperature calibration parameter
//!
//! \param[in] cryId crystal id
//! \param[in] parId parameter id
Double_t TACAcalibrationMap::GetADC2TempParam(Int_t cryId, UInt_t parId)
{
   if (parId == 0)
      return fCalibTempMapCry[cryId].p0;
   else if (parId == 1)
      return fCalibTempMapCry[cryId].p1;
   else if (parId == 2)
      return fCalibTempMapCry[cryId].p2;
   else {
      Error("GetADC2TemperatureParam()", "No parameter %d found", parId);
      return -99999;
   }
}

//_______________________________________________
//! Get temperature calibration parameter
//!
//! \param[in] cryId crystal id
//! \param[in] parId parameter id
Double_t TACAcalibrationMap::GetADC2EnergyParamCry(Int_t cryId, UInt_t parId)
{
   if (parId == 0)
      return fCalibTempMapCry[cryId].p0;
   else if (parId == 1)
      return fCalibTempMapCry[cryId].p1;
   else if (parId == 2)
      return fCalibTempMapCry[cryId].p2;
   else {
      Error("GetADC2EnergyParamCry()", "No parameter %d found", parId);
      return -99999;
   }
}
