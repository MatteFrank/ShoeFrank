#ifndef _TAVTbaseParMap_HXX
#define _TAVTbaseParMap_HXX
/*!
 \file TAVTbaseParMap.hxx
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

   // return plane id
   Int_t        GetPlaneId(Int_t sensorId, Int_t dataLink = 0);
   
   // Read from file
   Bool_t       FromFile(const TString& name = "");
   
   //! return number of data links (i.e. terasic boards)
   Int_t        GetDataLinksN()         const { return fDataLinksN;    }
   
   //! return number sensors per data link
   Int_t        GetSensorsN(Int_t idx ) const { return fSensorsN[idx]; }

protected:
   Int_t                    fSensorsN[32];    ///< number of sensors per datalink
   Int_t                    fDataLinksN;      ///< numver of data links
   map<pair<int, int>, int> fPlaneId;         ///< map of plane identification
   TString                  fkDefaultMapName; ///< default detector mapping file
   
   ClassDef(TAVTbaseParMap,1)

};

#endif
