/*!
  \file
  \version $Id: TACAparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TACAparMap.
*/

#include <string.h>

#include <fstream>

#include "TSystem.h"
#include "TString.h"

#include "TACAparMap.hxx"

//##############################################################################

/*!
  \class TACAparMap TACAparMap.hxx "TACAparMap.hxx"
  \brief Map parameters  **
*/

ClassImp(TACAparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAparMap::TACAparMap() {

   vector<int> tdchaID;     tdchaID.clear();   
   vector<int> tdboaID;     tdboaID.clear();   
   vector<int> adchaID;     adchaID.clear();   
   vector<int> adcboaID;    adcboaID.clear();   
   vector<int> deID;        deID.clear();   
   vector<int> dechaID;     dechaID.clear();   

   TDchaID  =  tdchaID;   
   TDboaID  =  tdboaID;   
   ADchaID  =  adchaID;   
   ADboaID  =  adcboaID;   
   DetchaID =  dechaID;   
   DetID    =  deID;   
  
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACAparMap::~TACAparMap()
{}


//------------------------------------------+-----------------------------------
//! Read mapping data from file \a name .

Bool_t TACAparMap::FromFile(const TString& name) 
{

   Clear();
   
   TString name_exp = name;
   gSystem->ExpandPathName(name_exp);

   char bufConf[1024];
   int myArg1(0), myArg2(0), myArg3(0), myArg4(0), myArg5(0), myArg6(0); 
   
   ifstream incF;
   incF.open(name_exp.Data());
   if (!incF) {
      Error("FromFile()", "failed to open file '%s'", name_exp.Data());
      return kTRUE;
   }

   while (incF.getline(bufConf, 200, '\n')) {
      if(strchr(bufConf,'!')) {
         //      Info("FromFile()","Skip comment line:: %s",bufConf);
      } else if(strchr(bufConf,'#')) {
         //Det id, Det channel, tdc, adc, adc board.
         sscanf(bufConf, "#%d %d %d %d %d %d",&myArg1,&myArg2,&myArg3,&myArg4,&myArg5,&myArg6);
         if((myArg1>-1 && myArg1<2) && (myArg2>-1 && myArg2<4) && (myArg3>-1 && myArg3<128) && (myArg4>-1 && myArg4<32) && (myArg5>-1 && myArg5<32)) {
            DetID.push_back(myArg1);
            DetchaID.push_back(myArg2);
            TDchaID.push_back(myArg3);
            TDboaID.push_back(myArg4);
            ADchaID.push_back(myArg5);
            ADboaID.push_back(myArg6);
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

void TACAparMap::Clear(Option_t*)
{
   TAGpara::Clear();
   TDchaID.clear();   
   TDboaID.clear();   
   ADchaID.clear();   
   ADboaID.clear();   
   DetchaID.clear();   
   DetID.clear();   
   return;
}

/*------------------------------------------+---------------------------------*/
bool TACAparMap::GetIDFromTDC(int channel, int board, int &detID, int &chaID) 
{

   bool found = kFALSE;
   for (int iw=0; iw<(int)TDchaID.size(); iw++) {
      if(getTDID(iw) == channel && getTDboaID(iw) == board)  { 
         chaID = getDetChaID(iw);	
         detID = getDetID(iw);    
         found = kTRUE;
         break;
      }
   }

   return found;

}

/*------------------------------------------+---------------------------------*/
bool TACAparMap::GetIDFromADC(int channel, int board, int &detID, int &chaID) 
{

   bool found = kFALSE;
   for(int iw=0; iw<(int)TDchaID.size(); iw++) {
      if(getADID(iw) == channel && getADboaID(iw) == board)  { 
         chaID = getDetChaID(iw);	
         detID = getDetID(iw);    
         found = kTRUE;
         break;
      }
   }
   
   return found;

}

