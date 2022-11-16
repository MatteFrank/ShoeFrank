#ifndef _TAGbaseWCparMap_HXX
#define _TAGbaseWCparMap_HXX
/*!
 \file
 \version $Id: TAGbaseWCparMap.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
 \brief   Declaration of TAGbaseWCparMap.
 */
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TString.h"
#include <map>
#include "TAGpara.hxx"

//##############################################################################

class TAGbaseWCparMap : public TAGpara {
   
public:
   TAGbaseWCparMap();
   virtual         ~TAGbaseWCparMap();
   
   Bool_t          FromFile(const TString& name);
   
   virtual void    Clear(Option_t* opt="");
   
   TString GetChannelType(int channelId);
   
private:
   map <int, TString> fChannelMap;
   Int_t              fDebugLevel;
   
   ClassDef(TAGbaseWCparMap,1)
};

#endif
