/*!
  \file
  \version $Id: TASTparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TASTparMap.
*/

#include <string.h>

#include <fstream>

#include "TSystem.h"
#include "TString.h"

#include "TASTparMap.hxx"

//##############################################################################

/*!
  \class TASTparMap TASTparMap.hxx "TASTparMap.hxx"
  \brief Map parameters for onion and daisy. **
*/

ClassImp(TASTparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTparMap::TASTparMap() {

  vector<int> tdchaID;             tdchaID.clear();   
  vector<int> tdboaID;             tdboaID.clear();   

  TDchaID =    tdchaID;   
  TDboaID =    tdboaID;   
  
}

//------------------------------------------+-----------------------------------
//! Destructor.

TASTparMap::~TASTparMap()
{}


//------------------------------------------+-----------------------------------
//! Read mapping data from file \a name .


Bool_t TASTparMap::IsSTChannel(int iCha){

  if(std::find(TDchaID.begin(),TDchaID.end(),iCha) == TDchaID.end()){
    return false;
  }
  return true;

    
}



Bool_t TASTparMap::IsSTBoard(int iBo){

  if(std::find(TDboaID.begin(),TDboaID.end(),iBo) == TDboaID.end()){
    return false;
  }
  return true;

    
}


Bool_t TASTparMap::FromFile(const TString& name) {

  Clear();
  
  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);


  
  char bufConf[1024];
  int myArg1(0), myArg2(0); 
 
  ifstream incF;
  incF.open(name_exp.Data());


  
  if (!incF) {
    Error("FromFile()", "failed to open file '%s'", name_exp.Data());
    return kTRUE;
  }

  while (incF.getline(bufConf, 200, '\n')) {
    if(strchr(bufConf,'!')) {
      //      Info("FromFile()","Skip comment line:: %s",bufConf);
    } else if(strchr(bufConf,'T')) {
      //Cha id and board
      //      printf("%s\n",bufConf);
      sscanf(bufConf, "T%d %d",&myArg1,&myArg2);
      // printf("taken channel %d\n",myArg1);
      if((myArg1>-1 && myArg1<18) && (myArg2>-1 && myArg2<100)) {
	if(myArg1<16){
	  TDchaID.push_back(myArg1);
	  TDboaID.push_back(myArg2);
	  printf("taken channel again %d\n",myArg1);
	}else{
	  TDclkID.push_back(myArg1);
	}
      } else {
	Error(""," Plane Map Error:: check config file!!");
	return kTRUE;
      }
    }
  }



  
  return kFALSE;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TASTparMap::Clear(Option_t*)
{
  TAGpara::Clear();
  TDchaID.clear();   
  return;
}

