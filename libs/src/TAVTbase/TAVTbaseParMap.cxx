/*!
 \file
 \version $Id: TAVTbaseParMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
 \brief   Implementation of TAVTbaseParMap.
 */
#include "TAGrecoManager.hxx"
#include "TAVTbaseParMap.hxx"

//##############################################################################

/*!
 \class TAVTbaseParMap TAVTbaseParMap.hxx "TAVTbaseParMap.hxx"
 \brief Map and Geometry parameters for Tof wall. **
 */

ClassImp(TAVTbaseParMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAVTbaseParMap::TAVTbaseParMap() 
 : TAGparTools(),
   fSensorsN(0),
   fkDefaultMapName("")
{
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAVTbaseParMap::~TAVTbaseParMap()
{
}

//------------------------------------------+-----------------------------------
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
   
   ReadItem(fDataLinksN);
   
   for (Int_t l = 0; l < fDataLinksN; ++l) { // Loop on each data link
      
      Int_t dataLink;
      ReadItem(dataLink);

      ReadItem(fSensorsN);
      
      for (Int_t i = 0; i < fSensorsN; ++i) { // Loop on each sensor
      
         Int_t sensorId;
         ReadItem(sensorId);

         pair<int, int> idx(dataLink, sensorId);

         fPlaneId[idx] = i;
      
         if(FootDebugLevel(1))
            printf("DataLink %d SensorId: %d PlaneId: %d\n", dataLink, sensorId, i);
      }
   }
   
   return true;
}

//------------------------------------------+-----------------------------------
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

