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
   
   ReadItem(fSensorsN);
   fPlaneId.reserve(fSensorsN);
   
   Int_t tmp;

   for (Int_t i = 0; i < fSensorsN; ++i) { // Loop on each charge
      
      ReadItem(tmp);
      
      fPlaneId.push_back(tmp);
      
      if(FootDebugLevel(1))
         printf("SensorId: %d PlaneId: %d\n", i, fPlaneId[i]);
   }

   return true;
}

//------------------------------------------+-----------------------------------
Int_t TAVTbaseParMap::GetPlaneId(Int_t sensorId)
{
   if (sensorId >= fSensorsN)
      return -1;
   else
      return fPlaneId[sensorId];
}

