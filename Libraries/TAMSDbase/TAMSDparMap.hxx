#ifndef _TAMSDparMap_HXX
#define _TAMSDparMap_HXX

/*!
 \file TAMSDparMap.hxx
 \brief   Declaration of TAMSDparMap.
 */
/*------------------------------------------+---------------------------------*/

#include <vector>

#include "TAGparTools.hxx"

//##############################################################################

class TAMSDparMap : public TAGparTools {
   
public:
   TAMSDparMap();
   virtual ~TAMSDparMap();

   Bool_t          FromFile(const TString& name);
   Int_t           GetSensorId(Int_t boardId, Int_t viewId);
   
   //! Get number of sensors
   Int_t           GetSensorsN() const { return fSensorsN; }
  
private:
  Int_t            fSensorsN;        ///< number of sensors
  vector<Int_t>    fSensId;          ///< sensor id vector
  vector<Int_t>    fBoardId;         ///< board id vector
  vector<Int_t>    fViewId;          ///< view id vector
  TString          fkDefaultMapName; ///< default detector mapping file
  
  ClassDef(TAMSDparMap,2)
  
};

#endif
