/*!
  \file
  \version $Id: TAGbaseWDparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TAGbaseWDparMap.
*/

#include <string.h>
#include <fstream>
#include "TSystem.h"
#include "TString.h"
#include "TAGbaseWDparMap.hxx"

//##############################################################################

/*!
  \class TAGbaseWDparMap TAGbaseWDparMap.hxx "TAGbaseWDparMap.hxx"
  \brief Map parameters for onion and daisy. **
*/

ClassImp(TAGbaseWDparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGbaseWDparMap::TAGbaseWDparMap() {
  TAGbaseWDparMap::Clear();
  
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGbaseWDparMap::~TAGbaseWDparMap()
{}



Bool_t TAGbaseWDparMap::FromFile(const TString& name) {

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

  pair<int,int> key;
  
  while (incF.getline(bufConf, 200, '\n')) {
    if(strchr(bufConf,'!')) {
      //do nothing
    }else if(strchr(bufConf,'B')) {
      sscanf(bufConf, "B%d",&board);
      //      printf("Board%d\n",board);
      for(int iCh=0;iCh<18;iCh++){
	incF.getline(bufConf, 200, '\n');
	sscanf(bufConf, "%d\t%c\t%s", &channel, &isenabled, detector);
	//printf("bo::%d %d\t%c\t%s\n", board, iCh, isenabled, detector);
	key = make_pair(board, iCh);
	chmap[key] = detector;
      }
    }
  }


  
  return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TAGbaseWDparMap::Clear(Option_t*){
  TAGpara::Clear();

  
  return;
}

//------------------------------------------+-----------------------------------
//! Get channel type

string TAGbaseWDparMap::GetChannelType(int board, int channel)
{
  string res = "CORRUPTED";
  if(chmap.count(make_pair(board, channel))) {
    return chmap.find(make_pair(board, channel))->second;
  }  else {
    return res;
  }
  
}
