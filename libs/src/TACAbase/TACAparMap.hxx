#ifndef _TACAparMap_HXX
#define _TACAparMap_HXX
/*!
  \file
  \version $Id: TACAparMap.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
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

    // void            FromFile(std::string FileName);
    Bool_t          FromFile(const TString& name);

    void            Clear(Option_t* opt="");

    Int_t           GetCrystalId(Int_t boardId, Int_t channelId);
    Int_t           GetCrystalsN() {return nCrystals;}
    Int_t           GetBoardId(Int_t cryId) {return fBoardId[cryId]; }
    Int_t           GetChannelId(Int_t cryId) {return fChannelId[cryId]; }
    Int_t           GetModuleId(Int_t cryId) {return fModuleId[cryId]; }

private:
  Int_t nCrystals;
  map< pair<int, int>, int > fCrysId;
  vector<Int_t> fModuleId;
  vector<Int_t> fBoardId;
  vector<Int_t> fChannelId;
  
  ClassDef(TACAparMap,1)

};

#endif
