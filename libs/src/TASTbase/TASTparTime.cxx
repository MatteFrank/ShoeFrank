/*!
  \file
  \version $Id: TASTparTime.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TASTparTime.
*/

#include <string.h>

#include <fstream>

#include "TSystem.h"
#include "TString.h"

#include "TAGroot.hxx"
#include "TASTparTime.hxx"
#include "TAGactWDreader.hxx"

//##############################################################################

/*!
  \class TASTparTime TASTparTime.hxx "TASTparTime.hxx"
  \brief Map parameters for onion and daisy. **
*/

ClassImp(TASTparTime);

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTparTime::TASTparTime() {


  InitMap();
  m_debug=GetDebugLevel();



}

//------------------------------------------+-----------------------------------
//! Destructor.

TASTparTime::~TASTparTime(){}






bool TASTparTime::FromFile(string expName, int iRunNumber){

  string runnumber;
   
  if (iRunNumber == 0)
     runnumber = Form("%d", gTAGroot->CurrentRunNumber());
   else
      runnumber = Form("%d", iRunNumber);

  string tcal_filename("");
  if (expName != "")
     expName += "/";
  tcal_filename+=Form("./config/%stcalib", expName.data());
  tcal_filename+=runnumber;
  tcal_filename+=".dat";

  
  FILE *stream = fopen(tcal_filename.c_str(), "r");

  if(stream==NULL){
    printf("\n\n WARNING:: ST WD time calibration file %s not found\n\n", tcal_filename.c_str());
    return false;
  }else{
     if (fDebugLevel > 1)
        printf("\n\nLoading ST WD time calibration from file::%s \n\n", tcal_filename.c_str());
  }

  
  u_int word;
  int board_id=0, ch_num=0,ret=0;
  float time_bin=0;
  vector<float> w_tcal;
  int key=0;

  ret = fread(&word, 4,1,stream);
  if(word == FILE_HEADER){
    if(m_debug)printf("found time calibration header::%08x %08x\n", word, word);
    
    ret = fread(&word, 4,1,stream);
    while(fread(&word, 4,1,stream) !=0 && (word & 0xffff)== BOARD_HEADER){
      
      if(m_debug)printf("found board header::%08x num%d\n", word, board_id);
      board_id = (word>>16)  & 0xffff;
      
      while(fread(&word, 4,1,stream) !=0 && (word & 0xffff)== CH_HEADER){
	char tmp_chstr[3]={'0','0','\0'};
	tmp_chstr[1] = (word>>24)  & 0xff;
	tmp_chstr[0] = (word>>16)  & 0xff;
	ch_num = atoi(tmp_chstr);
	if(m_debug)printf("found channel header::%08x num%d\n",word, ch_num);
	
	w_tcal.clear();
	for(int iCal=0;iCal<1024;iCal++){
	  ret = fread(&word, 4,1,stream);
	  time_bin = *((float*)&word);
	  w_tcal.push_back(time_bin);
	}

	key = board_id*18+ch_num;
	m_GotCalib[key] = true;
	SetTimeCal(board_id, ch_num,  w_tcal);
      }
      fseek(stream, -4, SEEK_CUR);
    }
    fseek(stream, -4, SEEK_CUR);
  }


  return true;
  
}

void TASTparTime::InitMap(){

  int key=0;
  
  for(int iBo=0;iBo<NMAX_BO_ID;iBo++){
    for(int iCh=0;iCh<NMAX_CH_ID;iCh++){
      key = iBo*18+iCh;
      for(int i=0;i<1024;i++){
	time_parcal[key].push_back(0.2);
      }
      m_GotCalib[key] = false;
    }
  }


}
 
//------------------------------------------+-----------------------------------
//! Clear event.

void TASTparTime::Clear(Option_t*){
  time_parcal.clear();
  return;
}


void TASTparTime::ToStream(ostream& os, Option_t*) const
{
  
}

//set time calibration (get from first event)
void TASTparTime::SetTimeCal(int iBo, int iCha,  vector<float> tvec){
  
  int key = iBo*18+iCha;
  for(int i=0;i<tvec.size();i++){
    time_parcal[key].at(i) = (double)tvec.at(i)*sec2Nano;
  }
  m_GotCalib[key] = true;
  
  return;
}



//get time array
bool TASTparTime::GetTimeArray(int iBo, int iCha, int TrigCell, vector<double> *timecal){

  
  int key = iBo*18+iCha;
      
  double t=0;
  vector<double> tmp_calib = time_parcal.find(key)->second;
  
  //the sampling time is retreived summing the time bin starting from the trigger cell
  for(int i=0;i<1024;i++){
    timecal->push_back(t);
    t+=tmp_calib.at((i+TrigCell)%1024);
  }
  

  
  return true;
}
