#ifndef _TACAparMap_HXX
#define _TACAparMap_HXX
/*!
  \file TACAparMap.hxx
  \brief   Declaration of TACAparMap.
*/
/*------------------------------------------+---------------------------------*/

#include <map>
using namespace std;

#include "TString.h"
#include <TSystem.h>

#include "TAGparTools.hxx"

//##############################################################################

class TACAparMap : public TAGparTools {
public:

  TACAparMap();
    virtual         ~TACAparMap();

    Bool_t          FromFile(const TString& name);

    void            Clear(Option_t* opt="");

    Int_t           GetCrystalId(Int_t boardId, Int_t channelId);
   
   //! Get number of crystals
    Int_t           GetCrystalsN()            const { return fCrystalsN;        }
   //! Get board id
    Int_t           GetBoardId(Int_t cryId)         { return fBoardId[cryId];   }
   //! Get channel id
    Int_t           GetChannelId(Int_t cryId)       { return fChannelId[cryId]; }
   //! Get module id
    Int_t           GetModuleId(Int_t cryId)        { return fModuleId[cryId];  }

private:
  Int_t                      fCrystalsN;  ///< number of crystal
  map< pair<int, int>, int > fCrysId;     ///< crystal map
  vector<Int_t>              fModuleId;   ///< module id vector
  vector<Int_t>              fBoardId;    ///< board id vector
  vector<Int_t>              fChannelId;  ///< channel id vector
  
  ClassDef(TACAparMap,1)
};

#endif
