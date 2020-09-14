#ifndef _TACAparMap_HXX
#define _TACAparMap_HXX
/*!
  \file
  \version $Id: TACAparMap.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
  \brief   Declaration of TACAparMap.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TString.h"

#include "TAGpara.hxx"

//##############################################################################

class TACAparMap : public TAGpara {
public:

  TACAparMap();
    virtual         ~TACAparMap();

    Bool_t          FromFile(const TString& name);

    bool            GetIDFromTDC(int channel, int board, int &detID, int &chaID);
    bool            GetIDFromADC(int channel, int board, int &detID, int &chaID);

    virtual void    Clear(Option_t* opt="");

    int getTDID(int add)     const { return fTDchaID.at(add);  }
    int getTDboaID(int add)  const { return fTDboaID.at(add);  }
    int getADID(int add)     const { return fADchaID.at(add);  }
    int getADboaID(int add)  const { return fADboaID.at(add);  }
    int getDetChaID(int add) const { return fDetchaID.at(add); }
    int getDetID(int add)    const { return fDetID.at(add);    }

    ClassDef(TACAparMap,1)

private:
    Bool_t          CheckAddr(Int_t i_c) const;

private:
    int trefCh;
    vector<int> fTDchaID;
    vector<int> fTDboaID;
    vector<int> fADchaID;
    vector<int> fADboaID;
    vector<int> fDetchaID;
    vector<int> fDetID;
};

#endif
