#ifndef _TAMPparMap_HXX
#define _TAMPparMap_HXX
/*!
 \file
 \version $Id: TAMPparMap.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
 \brief   Declaration of TAMPparMap.
 */
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TString.h"
#include <map>
#include "TAGpara.hxx"

//##############################################################################

class TAMPparMap : public TAGpara {
   
public:
   TAMPparMap();
   virtual         ~TAMPparMap();
   
   Bool_t          FromFile(const TString& name);
   
   virtual void    Clear(Option_t* opt="");
   
   TString GetChannelType(int channelId);
   
private:
   map <TString, int> fChannelMap;
   Int_t              fDebugLevel;
   
   ClassDef(TAMPparMap,1)
};

#endif
