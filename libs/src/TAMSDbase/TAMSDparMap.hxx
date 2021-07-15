#ifndef _TAMSDparMap_HXX
#define _TAMSDparMap_HXX
/*!
 \file
 \version $Id: TAMSDparMap.hxx,v 1.1
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

private:

  Int_t nSens;
  vector<Int_t> fSensId;
  vector<Int_t> fBoardId;
  vector<Int_t> fViewId;
  TString                  fkDefaultMapName; // default detector mapping file
  
  ClassDef(TAMSDparMap,2)
  
};

#endif
