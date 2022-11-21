#ifndef _TANEparMap_HXX
#define _TANEparMap_HXX
/*!
  \file TANEparMap.hxx
  \brief   Declaration of TANEparMap.
*/
/*------------------------------------------+---------------------------------*/

#include <map>
using namespace std;

#include "TString.h"
#include <TSystem.h>

#include "TAGparTools.hxx"

//##############################################################################

class TANEparMap : public TAGparTools {
public:

   TANEparMap();
   virtual         ~TANEparMap();

   Bool_t          FromFile(const TString& name);

   void            Clear(Option_t* opt="");

   Int_t           GetModuleId(Int_t boardId, Int_t channelId);
   
   //! Get number of Modules
   Int_t           GetModulesN()             const { return fModulesN;        }
   //! Get board id
   Int_t           GetBoardId(Int_t cryId)         { return fBoardId[cryId];   }
   //! Get channel id
   Int_t           GetChannelId(Int_t cryId)       { return fChannelId[cryId]; }
   //! Get module id
   Int_t           GetModuleId(Int_t cryId)        { return fModuleId[cryId];  }

private:
   Int_t                      fModulesN;        ///< number of Module
   map< pair<int, int>, int > fModId;           ///< Module map WD boards
   vector<Int_t>              fModuleId;         ///< module id vector
   vector<Int_t>              fBoardId;          ///< board id vector
   vector<Int_t>              fChannelId;        ///< channel id vector
   
   ClassDef(TANEparMap,1)
};

#endif
