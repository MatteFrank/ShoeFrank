/*!
 \file TAVTbaseParMap.cxx
  \brief   Base class of map parameters for VTX
 */

#include "TAGrecoManager.hxx"
#include "TAVTbaseParMap.hxx"

//##############################################################################

/*!
 \class TAVTbaseParMap
 \brief Base class of map parameters for VTX
 */

//! Class Imp
ClassImp(TAVTbaseParMap);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTbaseParMap::TAVTbaseParMap() 
 : TAGparTools(),
   fkDefaultMapName("")
{
   memset(fSensorsN, 0, sizeof(fSensorsN));
   fPlaneId.clear();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTbaseParMap::~TAVTbaseParMap()
{
}

//------------------------------------------+-----------------------------------
//! Read from file
//!
//! \param[in] name file name
Bool_t TAVTbaseParMap::FromFile(const TString& name)
{
   // Reading mapping file
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultMapName;
   else
      nameExp = name;
   
   if (!Open(nameExp))
      return false;
   
   Info("FromFile()", "Open file %s for mapping\n", name.Data());

   ReadItem(fDataLinksN);
   
   for (Int_t l = 0; l < fDataLinksN; ++l) { // Loop on each data link
      
      Int_t dataLink;
      ReadItem(dataLink);

      ReadItem(fSensorsN[dataLink]);
      for (Int_t i = 0; i < fSensorsN[dataLink]; ++i) { // Loop on each sensor
      
         Int_t sensorId;
         Int_t planeId;

         ReadItem(planeId, sensorId);

         pair<int, int> idx(dataLink, sensorId);
         fSensorIdxInLink[dataLink].push_back(sensorId);
         
         fPlaneId[idx] = planeId;
      
         if(FootDebugLevel(1))
            printf("DataLink %d SensorId: %d PlaneId: %d\n", dataLink, sensorId, fPlaneId[idx]);
      }
   }
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Get plane id from sensor and datalink
//!
//! \param[in] sensorId a given sensor
//! \param[in] dataLink a given datalink
Int_t TAVTbaseParMap::GetPlaneId(Int_t sensorId, Int_t dataLink)
{
   pair<int, int> idx(dataLink, sensorId);

   auto itr = fPlaneId.find(idx);
   if (itr == fPlaneId.end()) {
      Error("GetPlaneId()", "Wrong sensor or/and data link number");
      return -1;
   }
   
   return fPlaneId[idx];
}

//------------------------------------------+-----------------------------------
//! Get sensor numberfor a given index and  data link
Int_t TAVTbaseParMap::GetSensorId(Int_t idx, Int_t dataLink)
{
   vector<int> vec = fSensorIdxInLink[dataLink];
   
   return vec[idx];
}

//------------------------------------------+-----------------------------------
//! Get sensor id and datalink pair from planeId
//!
//! \param[in] sensorId a given sensor
pair<int, int> TAVTbaseParMap::GetMapId(Int_t planeId)
{
   for (auto const& [key, val] : fPlaneId) {
    if (val == planeId)
       return key;
   }
   
   pair<int, int> idx(-1, -1);
   
   return idx;
}

