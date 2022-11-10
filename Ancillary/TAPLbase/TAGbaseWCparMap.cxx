/*!
 \file
 \version $Id: TAGbaseWCparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
 \brief   Implementation of TAGbaseWCparMap.
 */

#include <string.h>
#include <fstream>
#include "TSystem.h"
#include "TString.h"
#include "TAGbaseWCparMap.hxx"

//##############################################################################

/*!
 \class TAGbaseWCparMap TAGbaseWCparMap.hxx "TAGbaseWCparMap.hxx"
 \brief Map parameters for onion and daisy. **
 */

ClassImp(TAGbaseWCparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGbaseWCparMap::TAGbaseWCparMap()
 : TAGpara(),
   fDebugLevel(0)
{
   Clear();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAGbaseWCparMap::~TAGbaseWCparMap()
{}


//------------------------------------------+-----------------------------------
Bool_t TAGbaseWCparMap::FromFile(const TString& name)
{
   Clear();
   
   TString name_exp = name;
   gSystem->ExpandPathName(name_exp);
   
   char bufConf[1024];
   int board(0), channel(0);
   char detector[10]="";
   char isenabled;
   
   ifstream incF;
   incF.open(name_exp.Data());
   
   if (!incF) {
      Error("FromFile()", "failed to open file '%s'", name_exp.Data());
      return kTRUE;
   }
   
   while (incF.getline(bufConf, 200, '\n')) {
      if(strchr(bufConf,'#')) continue;
      
      //scan
      sscanf(bufConf, "%d %c %s", &channel, &isenabled, detector);
         
      if (fDebugLevel)
         printf("%d\t%c\t%s\n", channel, isenabled, detector);

      if (isenabled == 'y')
         fChannelMap[channel] = detector;
   }
   
   return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TAGbaseWCparMap::Clear(Option_t*){
   TAGpara::Clear();
   
   
   return;
}

//------------------------------------------+-----------------------------------
//! Get channel type
TString TAGbaseWCparMap::GetChannelType(int channel)
{
//   string res = "CORRUPTED";
//   if(fChannelMap.count(channel)) {
      return fChannelMap.find(channel)->second;
//   }  else {
//      return res;
//   }
   
}
