/*!
 \file
 \version $Id: TAMPparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
 \brief   Implementation of TAMPparMap.
 */

#include <string.h>
#include <fstream>
#include "TSystem.h"
#include "TString.h"
#include "TAMPparMap.hxx"

//##############################################################################

/*!
 \class TAMPparMap TAMPparMap.hxx "TAMPparMap.hxx"
 \brief Map parameters for onion and daisy. **
 */

ClassImp(TAMPparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMPparMap::TAMPparMap()
 : TAGpara(),
   fDebugLevel(0)
{
   Clear();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMPparMap::~TAMPparMap()
{}


//------------------------------------------+-----------------------------------
Bool_t TAMPparMap::FromFile(const TString& name)
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
   
   Info("FromFile()", "Open file %s for geometry\n", name.Data());

   while (incF.getline(bufConf, 200, '\n')) {
      if(strchr(bufConf,'#')) continue;
      
      //scan
      sscanf(bufConf, "%d %c %s", &channel, &isenabled, detector);
         
      if (fDebugLevel)
         printf("%d\t%c\t%s\n", channel, isenabled, detector);

      if (isenabled == 'y')
         fChannelMap[detector] = channel;
   }
   
   return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TAMPparMap::Clear(Option_t*)
{
   TAGpara::Clear();
}

//------------------------------------------+-----------------------------------
//! Get channel
Int_t TAMPparMap::GetChannel(TString detector)
{
   return fChannelMap.find(detector)->second;
}
