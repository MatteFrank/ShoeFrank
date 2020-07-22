#ifndef _TATWparMap_HXX
#define _TATWparMap_HXX
/*!
  \file
  \version $Id: TATWparMap.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
  \brief   Declaration of TATWparMap.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TString.h"
#include "TATWchannelMap.hxx"
#include "TAGpara.hxx"

//##############################################################################

class TATWparMap : public TAGpara {
  public:

  TATWparMap();
    virtual         ~TATWparMap();

    Bool_t          FromFile(const TString& name);

  //    bool            GetIDFromTDC(int channel, int board, int &detID, int &chaID);

    virtual void    Clear(Option_t* opt="");

    Int_t GetTDID(int add)    const { return fTDchaID.at(add); }
    Int_t GetTDboaID(int add) const { return fTDboaID.at(add); }
    Int_t GetDetID(int add)   const { return fDetID.at(add);   }
    TATWchannelMap* GetChannelMap()    { return fcMap;            }

    Bool_t IsTWChannel(int iCha);
    Bool_t IsTWBoard(int iBo);
    pair<int,int> GetClockChannel(int ch_num, int bo_num);
   
    ClassDef(TATWparMap,1)

  private:
    Bool_t  CheckAddr(Int_t i_c) const;
   
  private:
    TATWchannelMap* fcMap;
    Int_t        fTrefCh;
    vector<int>  fTDchaID;
    vector<int>  fTDboaID;
    vector<int>  fDetID;
};

#endif
