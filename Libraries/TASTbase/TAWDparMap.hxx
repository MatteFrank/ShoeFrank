#ifndef _TAWDparMap_HXX
#define _TAWDparMap_HXX
/*!
  \file TAWDparMap.hxx
  \brief   Declaration of TAWDparMap.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TString.h"
#include <map>
#include "TAGpara.hxx"

//##############################################################################

class TAWDparMap : public TAGpara {
public:
  TAWDparMap();
  virtual         ~TAWDparMap();
  
  Bool_t          FromFile(const TString& name);
  
  virtual void    Clear(Option_t* opt="");
   
  string          GetChannelType(int,int);
  vector<int>&    GetBoards(string);
   int GetNboards(){return  fNboards;}
  
private:
  map <pair<int,int>, string> fChmap;
  map <string, vector<int>>   fBolist;
  int fNboards;
  
   ClassDef(TAWDparMap,1)
};

#endif
