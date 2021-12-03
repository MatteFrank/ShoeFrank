#ifndef _TAVTbaseParMap_HXX
#define _TAVTbaseParMap_HXX
/*!
 \file
 \version $Id: TAVTbaseParMap.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
 \brief   Declaration of TAVTbaseParMap.
 */
/*------------------------------------------+---------------------------------*/

using namespace std;
#include <vector> 

#include "TAGparTools.hxx"

//##############################################################################

class TAVTbaseParMap : public TAGparTools {
   
public:
   TAVTbaseParMap();
   virtual ~TAVTbaseParMap();

   //! return plane id
   Int_t        GetPlaneId(Int_t sensorId, Int_t dataLink = 0);
   
   //! Read from file
   Bool_t       FromFile(const TString& name = "");

protected:
   Int_t                    fSensorsN;        /// number of sensors
   Int_t                    fDataLinksN;      /// numver of data links
   map<pair<int, int>, int> fPlaneId;         /// map of plane identification
   TString                  fkDefaultMapName; /// default detector mapping file
   
   ClassDef(TAVTbaseParMap,1)
  
};

#endif
