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
#include "CChannelMap.hxx"
#include "TAGpara.hxx"

//##############################################################################

class TATWparMap : public TAGpara {
  public:

  TATWparMap();
    virtual         ~TATWparMap();

    Bool_t          FromFile(const TString& name);

  //    bool            GetIDFromTDC(int channel, int board, int &detID, int &chaID);

    virtual void    Clear(Option_t* opt="");

    inline int getTDID(int add) { return TDchaID.at(add); }
    inline int getTDboaID(int add) { return TDboaID.at(add); }
    inline int getDetID(int add) { return DetID.at(add); }
  
  Bool_t IsTWChannel(int iCha);
  Bool_t IsTWBoard(int iBo);
  pair<int,int> GetClockChannel(int ch_num, int bo_num);

  
  
    inline CChannelMap* getChannelMap() { return cMap; }
  
    ClassDef(TATWparMap,1)

  private:
    Bool_t          CheckAddr(Int_t i_c) const;
    CChannelMap *cMap;
    int trefCh;
    vector<int> TDchaID;
    vector<int> TDboaID;
    vector<int> DetID;
};

#endif
