#ifndef _TAGbaseWDparMap_HXX
#define _TAGbaseWDparMap_HXX
/*!
  \file
  \version $Id: TAGbaseWDparMap.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
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
    
  ClassDef(TAGbaseWDparMap,1)

  string GetChannelType(int,int);
  vector<int> GetBoards(string);
  
  private:

  map <pair<int,int>, string> chmap;
  map <string, vector<int>> bolist;
  
};

#endif
