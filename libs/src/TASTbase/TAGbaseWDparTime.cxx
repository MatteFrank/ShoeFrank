/*!
  \file
  \version $Id: TAGbaseWDparTime.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TAGbaseWDparTime.
*/

#include <string.h>

#include <fstream>

#include "TSystem.h"
#include "TString.h"

#include "TAGrecoManager.hxx"
#include "TAGroot.hxx"
#include "TAGbaseWDparTime.hxx"
#include "TAGactWDreader.hxx"

//##############################################################################

/*!
  \class TAGbaseWDparTime TAGbaseWDparTime.hxx "TAGbaseWDparTime.hxx"
  \brief Map parameters for onion and daisy. **
*/

ClassImp(TAGbaseWDparTime);



//------------------------------------------+-----------------------------------
//! Default constructor.

TAGbaseWDparTime::TAGbaseWDparTime()
{
  InitMap();


}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGbaseWDparTime::~TAGbaseWDparTime()
{
}

//------------------------------------------+-----------------------------------
bool TAGbaseWDparTime::FromFile(TString tcal_filename, TString cfd_filename)
{
  FILE *stream = fopen(tcal_filename.Data(), "r");
  
  if(stream==NULL){
    printf("\n\n WARNING:: ST WD time calibration file %s not found\n\n", tcal_filename.Data());
    return false;
  }else{
    // if (FootDebugLevel(1) > 1)
    Info("FromFile()", "Open file %s for WD time calibration\n", tcal_filename.Data());
  }

  
  u_int word;
  int board_id=0, ch_num=0,ret=0;
  float time_bin=0;
  vector<float> w_tcal;


  ret = fread(&word, 4,1,stream);
  if(word == FILE_HEADER){
    if(FootDebugLevel(1))printf("found time calibration header::%08x %08x\n", word, word);
    
    ret = fread(&word, 4,1,stream);
    while(fread(&word, 4,1,stream) !=0 && (word & 0xffff)== BOARD_HEADER){
      

      board_id = (word>>16)  & 0xffff;
      if(FootDebugLevel(1))printf("found board header::%08x num%d\n", word, board_id);
      
      while(fread(&word, 4,1,stream) !=0 && (word & 0xffff)== CH_HEADER){
	char tmp_chstr[3]={'0','0','\0'};
	tmp_chstr[1] = (word>>24)  & 0xff;
	tmp_chstr[0] = (word>>16)  & 0xff;
	ch_num = atoi(tmp_chstr);
	if(FootDebugLevel(1))printf("found channel header::%08x num%d\n",word, ch_num);
	
	w_tcal.clear();
	for(int iCal=0;iCal<1024;iCal++){
	  ret = fread(&word, 4,1,stream);
	  time_bin = *((float*)&word);
	  w_tcal.push_back(time_bin);
	}
	if(FootDebugLevel(1))printf("set time calbration\n");
	SetTimeCal(board_id, ch_num,  w_tcal);
      }
      fseek(stream, -4, SEEK_CUR);
    }
    fseek(stream, -4, SEEK_CUR);
  }



 

 fclose(stream);
 
   
   //get cfd parameters for each detectors
   stream = fopen(cfd_filename.Data(), "r");
   if(stream==NULL){
      return false;
   }else{
      Info("FromFile()", "Open file %s for cfd algorithm selection\n", cfd_filename.Data());
   }
   char line[100];
   while(fgets(line,sizeof(line),stream)!=NULL){
      if(line[0]=='!'){
         //skip
      }else{
         char det[100],algo[100];
         double frac, del;
         sscanf(line, "%s %s", det, algo);
         if(FootDebugLevel(1))
            printf("line::%s\n", line);
         if(strcmp(algo,"hwCFD")==0){
            sscanf(line, "%s %s %lf %lf", det, algo,&frac,&del);
            cfdalgo[string(det)] = string(algo);
            cfdfrac[string(det)] = frac;
            cfddel[string(det)] = del;
            if(FootDebugLevel(1))
               cout << "detector::" << det << "   CFDalgo::" << algo << "   frac::" << frac << "   del::" << del << endl;
         }else if(strcmp(algo,"simpleCFD")==0){
            sscanf(line, "%s %s %lf %lf", det, algo,&frac,&del);
            cfdalgo[string(det)] = string(algo);
            cfdfrac[string(det)] = frac;
            cfddel[string(det)] = 0;
            if(FootDebugLevel(1))
               cout << "detector::" << det << "   CFDalgo::" << algo << "   frac::" << frac << endl;
	 }else if(strcmp(algo,"zarrCFD")==0){
	   sscanf(line, "%s %s %lf %lf", det, algo,&frac,&del);
	   cfdalgo[string(det)] = string(algo);
	   cfdfrac[string(det)] = frac;
	   cfddel[string(det)] = 0;
	   if(FootDebugLevel(1))
	     cout << "detector::" << det << "   CFDalgo::" << algo << "   frac::" << frac << endl;
         }else{
	   cfdalgo[string(det)] = "simpleCFD";
	   cfdfrac[string(det)] = 0.3;
	   cfddel[string(det)] = 0;
	   if(FootDebugLevel(1))
	     cout << "detector::" << det << "   CFDalgo::" << algo << "   frac::" << frac << endl;
         }
      }
   }
   
   fclose(stream);
   
   
   return true;
  
}




void TAGbaseWDparTime::InitMap(){

 
  pair<int,int> key;
  for(int iBo=0;iBo<NMAX_BO_ID;iBo++){
    for(int iCh=0;iCh<NMAX_CH_ID;iCh++){
      key = make_pair(iBo,iCh);
      for(int i=0;i<1024;i++){
	time_parcal[key].push_back(0.2);
      }
      
    }
  }


}
 
//------------------------------------------+-----------------------------------
//! Clear event.

void TAGbaseWDparTime::Clear(Option_t*){

  time_parcal.clear();
 
  return;
}


void TAGbaseWDparTime::ToStream(ostream& os, Option_t*) const{
  
}


//set time calibration (get from first event)
void TAGbaseWDparTime::SetTimeCal(int iBo, int iCha,  vector<float> tvec){
  
  pair<int,int> key = make_pair(iBo,iCha);
  for(int i=0;i<tvec.size();i++){
    time_parcal[key].at(i) = (double)tvec.at(i)*sec2Nano;
  }

  
  return;
}



//get time array
vector<double> TAGbaseWDparTime::GetRawTimeArray(int iBo, int iCha, int TrigCell){

  vector<double> time;
  
  pair<int,int> key = make_pair(iBo,iCha);
  double t=0;
  vector<double> tmp_calib = time_parcal.find(key)->second;
  
  //the sampling time is retreived summing the time bin starting from the trigger cell
  for(int i=0;i<1024;i++){
    time.push_back(t);
    t+=tmp_calib.at((i+TrigCell)%1024);
  }


  return time;
}




