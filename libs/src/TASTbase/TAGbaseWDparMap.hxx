#ifndef _TAGbaseWDparMap_HXX
#define _TAGbaseWDparMap_HXX
/*!
  \file TAGbaseWDparMap.hxx
  \brief   Declaration of TAGbaseWDparMap.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TString.h"
#include <map>
#include "TAGpara.hxx"

//##############################################################################

class TAGbaseWDparMap : public TAGpara {
public:
  TAGbaseWDparMap();
  virtual         ~TAGbaseWDparMap();
  
  Bool_t          FromFile(const TString& name);
  
  virtual void    Clear(Option_t* opt="");
   
  string          GetChannelType(int,int);
  vector<int>     GetBoards(string);
  
private:
  map <pair<int,int>, string> fChmap;
  map <string, vector<int>>   fBolist;
  
   ClassDef(TAGbaseWDparMap,1)
};

#endif
