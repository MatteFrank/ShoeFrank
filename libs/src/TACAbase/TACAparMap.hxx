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

#include "TAGparTools.hxx"

//##############################################################################

class TACAparMap : public TAGparTools {
public:

  TACAparMap();
    virtual         ~TACAparMap();

    Bool_t          FromFile(const TString& name);

    void            Clear(Option_t* opt="");

    Int_t           GetCrystalId(Int_t boardId, Int_t channelId);

private:
   map< pair<int, int>, int > fCrysId;
  
  ClassDef(TACAparMap,1)

};

#endif
