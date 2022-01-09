#ifndef _TASTparMap_HXX
#define _TASTparMap_HXX
/*!
  \file
  \version $Id: TASTparMap.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
  \brief   Declaration of TASTparMap.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TString.h"
#include <map>
#include "TAGpara.hxx"

//##############################################################################

class TASTparMap : public TAGpara {
  public:

  TASTparMap();
    virtual         ~TASTparMap();

    Bool_t          FromFile(const TString& name);

    virtual void    Clear(Option_t* opt="");

  inline int getTDID(int add) { return TDchaID.at(add); }
  inline int getTDboaID(int add) { return TDboaID.at(add); }

  inline vector<int> GetChannels() { return TDchaID; }
  inline vector<int> GetBoards() { return TDboaID; }
  
  Bool_t IsSTClock(int iCha);
  Bool_t IsSTChannel(int iCha);
  Bool_t IsSTBoard(int iCha);

  double GetChannelWeight(int ch_num, int bo_num);
  pair<int,int> GetClockChannel(int ch_num, int bo_num);
  
  private:

   vector<int> TDclkID;
   vector<int> TDchaID;
   vector<int> TDboaID;

   map<pair<int,int>, double> reso;

   ClassDef(TASTparMap,1)

};

#endif
