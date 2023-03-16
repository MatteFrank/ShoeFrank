/*!
  \file TAWDparMap.cxx
  \brief   Implementation of TAWDparMap.
*/

#include <string.h>
#include <fstream>
#include "TSystem.h"
#include "TString.h"
#include "TAWDparMap.hxx"

//##############################################################################

/*!
  \class TAWDparMap 
  \brief Map parameters for onion and daisy. **
*/

//! Class Imp
ClassImp(TAWDparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAWDparMap::TAWDparMap()
{
  TAWDparMap::Clear();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAWDparMap::~TAWDparMap()
{}

//------------------------------------------+-----------------------------------
Bool_t TAWDparMap::FromFile(const TString& name)
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
        fChmap[key] = detector;
        if(fBolist.count(detector)){
          vector<int> tmplist = fBolist.find(detector)->second;
          if(find(tmplist.begin(), tmplist.end(),board)==tmplist.end())
            fBolist[detector].push_back(board);
        }else{
          fBolist[detector].push_back(board);
        }
      }
    }
  }

  return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAWDparMap::Clear(Option_t*)
{
  TAGpara::Clear();
}

//------------------------------------------+-----------------------------------
//! Get channel type
string TAWDparMap::GetChannelType(int board, int channel)
{
  string res = "CORRUPTED";
  if(fChmap.count(make_pair(board, channel))) {
    return fChmap.find(make_pair(board, channel))->second;
  }  else {
    return res;
  }
}

//------------------------------------------+-----------------------------------
vector<int> TAWDparMap::GetBoards(string det)
{
  if(fBolist.count(det)){
    return fBolist.find(det)->second;
  }else{
    return vector<int>();
  }
  
}
