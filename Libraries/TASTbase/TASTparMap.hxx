#ifndef _TASTparMap_HXX
#define _TASTparMap_HXX
/*!
  \file TASTparMap.hxx
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
   
   int getTDID(int add)       { return fTDchaID.at(add); }
   int getfTDboaID(int add)   { return fTDboaID.at(add); }
    vector<int> GetChannels()  { return fTDchaID;         }
   vector<int> GetBoards()    { return fTDboaID;         }
   
   Bool_t IsSTClock(int iCha);
   Bool_t IsSTChannel(int iCha);
   Bool_t IsSTBoard(int iCha);
   
   double GetChannelWeight(int ch_num, int bo_num);
   pair<int,int> GetClockChannel(int ch_num, int bo_num);
   
private:
   vector<int>                fTDclkID;
   vector<int>                fTDchaID;
   vector<int>                fTDboaID;
   map<pair<int,int>, double> fReso;
    
   ClassDef(TASTparMap,1)
};

#endif
