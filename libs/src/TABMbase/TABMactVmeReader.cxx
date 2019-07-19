/*!
  \file
  \version $Id: TABMactVmeReader.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TABMactVmeReader.
*/

#include "TABMparMap.hxx"
#include "TABMparCon.hxx"
#include "TABMrawHit.hxx"
#include "TABMdatRaw.hxx"
#include "TASTntuRaw.hxx"
#include "TABMactVmeReader.hxx"
#include <iomanip>

/*!
  \class TABMactVmeReader TABMactVmeReader.hxx "TABMactVmeReader.hxx"
  \brief Get Beam Monitor standalone raw data. **
*/

ClassImp(TABMactVmeReader);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactVmeReader::TABMactVmeReader(const char* name,
			     TAGdataDsc* p_datraw,
			     TAGparaDsc* p_parmap,
			     TAGparaDsc* p_parcon,
			     TAGparaDsc* p_pargeo,
           TAGdataDsc* p_timraw)
  : TAGaction(name, "TABMactVmeReader - Unpack standalone BM raw data"),
    fpDatRaw(p_datraw),
    fpParMap(p_parmap),
    fpParCon(p_parcon),
    fpParGeo(p_pargeo),
    fpTimRaw(p_timraw)
{
  AddDataOut(p_datraw, "TABMdatRaw");
  AddPara(p_parmap, "TABMparMap");
  AddPara(p_parcon, "TABMparCon");
  AddPara(p_pargeo, "TABMparGeo");
  AddDataOut(p_timraw, "TASTntuRaw");  
  fpEvtStruct=new BM_struct;
  clear_bmstruct(kTRUE);
  data_num_ev=-1000;
  data_sync_num_ev=0;
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactVmeReader::~TABMactVmeReader()
{
}


//------------------------------------------+-----------------------------------
//! Action.
//----------------------------------------------------------------------------------------------------

void TABMactVmeReader::CreateHistogram(){
   
   DeleteHistogram();

   fpRawMapX=new TH2I( "cell_raw_occupancy_2d_x", "Cell occupancy for raw hits; z; x", 11, -5.5, 5.5,7, -3.5,3.5);
   fpRawMapY=new TH2I( "cell_raw_occupancy_2d_y", "Cell occupancy for raw hits; z; y", 11, -5.5, 5.5,7, -3.5,3.5);
   for(Int_t i=0;i<77;i++){//necessary to use addbincontent
     fpRawMapX->SetBinContent(i,1);
     fpRawMapY->SetBinContent(i,1);
   }
   AddHistogram(fpRawMapX);   
   AddHistogram(fpRawMapY);   
   
   SetValidHistogram(kTRUE);
}


Bool_t TABMactVmeReader::Process() {
  
  TABMparCon* bmcon = (TABMparCon*) fpParCon->Object();
  TABMparMap* bmmap = (TABMparMap*) fpParMap->Object();
  TABMparGeo* bmgeo = (TABMparGeo*) fpParGeo->Object();
  TABMdatRaw* p_datraw= (TABMdatRaw*) fpDatRaw->Object();   
  TASTntuRaw* p_timraw= (TASTntuRaw*) fpTimRaw->Object();   
  
  read_event(kFALSE);
  //some check on bm_struct
  if(fpEvtStruct->tot_status!=0 || fpEvtStruct->tdc_status!=-1000){
    data_num_ev++;
    data_sync_num_ev+=fpEvtStruct->tdc_numsync;
    cout<<"ERROR in TABMactVmeReader process: return ktrue; tot_status="<<fpEvtStruct->tot_status<<"  tdc_status="<<fpEvtStruct->tdc_status<<endl;
    fpDatRaw->SetBit(kValid);
    fpTimRaw->SetBit(kValid);
    return kTRUE;
  }
  if(fpEvtStruct->tdc_sync[0] == -10000) {
    data_num_ev++;
    data_sync_num_ev+=fpEvtStruct->tdc_numsync;    
    Info("Action()","ERROR in TABMactVmeReader process: return ktrue; Trigger time is missing");
    fpDatRaw->SetBit(kValid);
    fpTimRaw->SetBit(kValid);
    return kTRUE;
  }
  if(fpEvtStruct->tdc_numsync>2) {
    data_num_ev++;
    data_sync_num_ev+=fpEvtStruct->tdc_numsync;    
    Info("Action()","ERROR in TABMactVmeReader process: return ktrue; too many trigger time!");
    fpDatRaw->SetBit(kValid);
    fpTimRaw->SetBit(kValid);
    return kTRUE;
  }
  if (FootDebugLevel(1))
    cout<<"I'm in TABMactVmeReader::Process, data_num_ev="<<data_num_ev<<"   fpEvtStruct->evnum="<<fpEvtStruct->evnum<<"   tdcev="<<fpEvtStruct->tdcev<<"   tdc_numsync="<<fpEvtStruct->tdc_numsync<<"  data_sync_num_ev="<<data_sync_num_ev<<endl;   
    
  Double_t i_time, i_rdrift, synctime;
  Int_t lay, view, cell, up, cellid;
  synctime=(fpEvtStruct->tdc_numsync==2) ? fpEvtStruct->tdc_sync[1]/10. : fpEvtStruct->tdc_sync[0]/10.;
  for (Int_t i = 0; i < fpEvtStruct->tdc_hitnum[0]; i++) {
    cellid=bmmap->tdc2cell(fpEvtStruct->tdc_id[i]);
    if(fpEvtStruct->tdc_meas[i]!=-10000 && bmcon->GetT0(cellid)!=-1000 &&  cellid>=0 && ((Double_t)  fpEvtStruct->tdc_meas[i]/10. -  bmcon->GetT0(cellid) - synctime)<bmcon->GetHitTimecut()){
      bmgeo->GetBMNlvc(cellid, lay, view, cell);
      p_datraw->SetHitData(cellid,lay,view,cell,fpEvtStruct->tdc_meas[i]/10.);
      if (ValidHistogram()){
        if(view==0){
          up=(lay%2==0) ? 1:0;
          fpRawMapY->AddBinContent(fpRawMapY->GetBin(lay*2+1,cell*2+up+1),1);
          fpRawMapY->AddBinContent(fpRawMapY->GetBin(lay*2+1,cell*2+up+2),1);
        }else{
          up=(lay%2==0) ? 0:1;
          fpRawMapX->AddBinContent(fpRawMapX->GetBin(lay*2+1,cell*2+up+1),1);
          fpRawMapX->AddBinContent(fpRawMapX->GetBin(lay*2+1,cell*2+up+2));
        }  
      }    
      if(FootDebugLevel(3))
        cout<<"hit charged: i="<<i<<"  tdc_id="<<fpEvtStruct->tdc_id[i]<<"  tdc2cell="<<bmmap->tdc2cell(fpEvtStruct->tdc_id[i])<<"  tdc_meas/10.="<<fpEvtStruct->tdc_meas[i]/10.<<"  T0="<<bmcon->GetT0(bmmap->tdc2cell(fpEvtStruct->tdc_id[i]))<<"  trigtime="<<synctime<<"  timecut="<<bmcon->GetHitTimecut()<<"  hittime="<<((Double_t)  fpEvtStruct->tdc_meas[i]/10. -  bmcon->GetT0(bmmap->tdc2cell(fpEvtStruct->tdc_id[i])) - synctime)<<endl;
    }else{
      p_datraw->AddDischarged();
      if(FootDebugLevel(3))
        cout<<"hit NOT charged: i="<<i<<"  tdc_id="<<fpEvtStruct->tdc_id[i]<<"  tdc2cell="<<bmmap->tdc2cell(fpEvtStruct->tdc_id[i])<<"  tdc_meas/10.="<<fpEvtStruct->tdc_meas[i]/10.<<"  T0="<<bmcon->GetT0(bmmap->tdc2cell(fpEvtStruct->tdc_id[i]))<<"  trigtime="<<synctime<<"  timecut="<<bmcon->GetHitTimecut()<<"  hittime="<<((Double_t)  fpEvtStruct->tdc_meas[i]/10. -  bmcon->GetT0(bmmap->tdc2cell(fpEvtStruct->tdc_id[i])) - synctime)<<endl;
      continue;
    }
  }
  
  //set the trigger time
  p_timraw->SetTriggerTime(synctime);  
  p_datraw->SetTrigtime(synctime);
    
  data_num_ev++;
  data_sync_num_ev+=fpEvtStruct->tdc_numsync;
  
  fpDatRaw->SetBit(kValid);
  fpTimRaw->SetBit(kValid);
  
  if (FootDebugLevel(2))
    cout<<"TABMactVmeReader::Process():: done"<<endl;
  
  return kTRUE;
}


Int_t TABMactVmeReader::Open(const TString& name)
{
  fbmfile.open(name.Data(), ios::in | ios::binary);
  if(!fbmfile.is_open()){
    cout<<"ERROR in TABMactVmeReader::cannot open the fbmfile="<<name.Data()<<endl;
  return kFALSE;
  }else
    cout<<"TABMactVmeReader::file "<<name.Data()<<" opened"<<endl;
  return kTRUE;
}

void TABMactVmeReader::Close(){
fbmfile.close();
}

Bool_t TABMactVmeReader::read_event(Bool_t evt0) {
  TABMparCon* bmcon = (TABMparCon*) fpParCon->Object();
  TABMparMap* bmmap = (TABMparMap*) fpParMap->Object();
  
  Int_t tmp_int; //dummy int variable to read and convert the input file
  //~ vector<int> ev_words;//words of the event
  Bool_t new_event, read_meas;
  Int_t sync_evnum=0, windex, endindex;
  Int_t tdc_wnum=0, adc_wnum=0, sca_wnum=0;
  
  clear_bmstruct(kFALSE);
  
  if(FootDebugLevel(4))
    cout<<"I'm in TABMactVmeReader:read_event"<<endl;
  if(fbmfile.read((char *) &tmp_int,sizeof(int))){//read number of words of this event
    fpEvtStruct->words=tmp_int;
    }
  else{
    if(FootDebugLevel(4))
      cout<<"file ended"<<endl;
    return kFALSE;
  }
  Int_t ev_words[fpEvtStruct->words];
  for(Int_t i=0;i<fpEvtStruct->words;i++){
    fbmfile.read((char *) &tmp_int,sizeof(int));
    //~ cout<<"i="<<i<<"  word="<<tmp_int<<endl;
    ev_words[i]=tmp_int;
  }
  //some check on the event words:
  fpEvtStruct->evnum=ev_words[0];
  if(data_num_ev<0){
    data_num_ev=fpEvtStruct->evnum;
  } else if(data_num_ev!=fpEvtStruct->evnum){
    cout<<"ERROR in TABMactVmeReader:read_event: data_num_ev="<<data_num_ev<<"  fpEvtStruct->evnum="<<fpEvtStruct->evnum<<endl; 
    fpEvtStruct->tot_status=-1;
  } 
  if(ev_words[1]!=bmmap->GetBoardNum()){
    cout<<"ERROR in TABMactVmeReader:read_event: board num != "<<bmmap->GetBoardNum()<<"  ev_words[1]="<<ev_words[1]<<endl;
    fpEvtStruct->tot_status=1;
  }

  tdc_wnum=ev_words[2];
  if(tdc_wnum<=0)
    fpEvtStruct->tdc_status=3;
  windex=2;
  if(bmmap->GetAdc792Ch()>=0){
    windex++;
    adc_wnum=ev_words[windex];
    if(adc_wnum!=bmmap->GetAdc792Ch()+2 && adc_wnum!=0){
      cout<<"ERROR in TABMactVmeReader:read_event: adc_wnum="<<adc_wnum<<"  bmmap->GetAdc792Ch()+2="<<bmmap->GetAdc792Ch()+2<<endl;
      fpEvtStruct->adc_status=1;
    }
  }
  if(bmmap->GetSca830Ch()>=0){
    windex++;
    sca_wnum=ev_words[windex];
    if(sca_wnum!=bmmap->GetSca830Ch() && sca_wnum!=0){
      cout<<"ERROR in TABMactVmeReader:read_event: sca_wnum="<<sca_wnum<<"  bmmap->GetSca830Ch()="<<bmmap->GetSca830Ch()<<endl;
      fpEvtStruct->sca_status=1;
    }
  }
  
  if(ev_words[++windex]!=3){//provv, da modificare: da togliere nell'acquisizione e qua...
    cout<<"ERROR in TABMactVmeReader:read_event: ev_words["<<windex<<"]="<<ev_words[windex]<<" ,!=3"<<endl;
    fpEvtStruct->tot_status=3;          
  }
  
  if((windex+tdc_wnum+adc_wnum+sca_wnum+4) != fpEvtStruct->words){
    cout<<"ERROR in TABMactVmeReader:read_event: counter for the event is wrong:  windex="<<windex<<"  tdc_wnum="<<tdc_wnum<<"  adc_wnum="<<adc_wnum<<"  sca_wnum="<<sca_wnum<<"  total number of words="<<fpEvtStruct->words<<"  windex+tdc_wnum+adc_wnum+sca_wnum+4="<<windex+tdc_wnum+adc_wnum+sca_wnum+4<<endl;
    fpEvtStruct->tot_status=4;
  }
  
  //~ if((fpEvtStruct->tot_status!=0 && FootDebugLevel(1)>0) || FootDebugLevel(1)>11)
    //~ for(Int_t i=0;i<fpEvtStruct->words;i++)
      //~ cout<<"data_num_ev="<<data_num_ev<<"   ev_words["<<i<<"]="<<ev_words[i]<<endl;
  
  //board reading
  windex++;

  if(fpEvtStruct->tot_status==0 && tdc_wnum>0 && tdc_wnum>0) {//read the tdc words
    endindex=windex+tdc_wnum;//endindex-1 is the position of the last tdc word
    new_event=true;
    read_meas=false;
    for(;windex<endindex;windex++){
      if(new_event && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1000)){//global header found
        fpEvtStruct->tdcev++;
        fpEvtStruct->tdc_evnum[fpEvtStruct->tdcev-1]=ev_words[windex++];
        read_meas=true;
        if(FootDebugLevel(4))
          cout<<"global header found, windex="<<windex<<"  tdcev="<<fpEvtStruct->tdcev<<endl;
        }
      //~ if(read_meas && ev_words[windex]<0 && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1000)){//global trailer found //se uso acquisizione mio (yun)
        //~ read_meas=false;
        //~ new_event=true;
        //~ fpEvtStruct->tdc_status=ev_words[windex];
        //~ if(ev_words[windex]!=-1000){
          //~ cout<<"Warning in TABMactVmeReader: global trailer found with error in tdc_evnum="<<fpEvtStruct->tdc_evnum[fpEvtStruct->tdcev-1]<<"  trailer="<<ev_words[windex]<<endl;
          //~ new_event=false;
        //~ }
        //~ if(FootDebugLevel(1)>11 && (!(evt0 && FootDebugLevel(1)==99)))
          //~ cout<<"global trailer found, windex="<<windex<<"  ev_words="<<ev_words[windex]<<endl;
      //~ }
      //~ //old trento software...i wanna get rid of this!!!
      if(read_meas && ev_words[windex]==0 && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1000)){//global trailer found //se uso acquisizione trento con 0 invece che -1000
        read_meas=false;
        new_event=true;
        fpEvtStruct->tdc_status=-1000;
        if(ev_words[windex]!=0){
          cout<<"Warning in TABMactVmeReader: global trailer found with error in tdc_evnum="<<fpEvtStruct->tdc_evnum[fpEvtStruct->tdcev-1]<<"  trailer="<<ev_words[windex]<<endl;
          new_event=false;
        }
        if(FootDebugLevel(4))
          cout<<"global trailer found, windex="<<windex<<"  ev_words="<<ev_words[windex]<<endl;
      }      
              
      //only for data from cosmic rays taken in 2017 in Rome        
      //~ if(read_meas && ev_words[windex]==0 && false && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1000)){//global trailer found //se uso dati letti a Roma per BM refurbishment
        //~ read_meas=false;
        //~ new_event=true;
        //~ fpEvtStruct->tdc_status=-1000;
        //~ if(FootDebugLevel(1)>11 && (!(evt0 && FootDebugLevel(1)==99)))
          //~ cout<<"global trailer found, i="<<windex<<"  ev_words="<<ev_words[windex]<<endl;
      //~ }        
      if(read_meas && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1000)){//read measure  
        if(ev_words[windex++]!=fpEvtStruct->tdc_evnum[fpEvtStruct->tdcev-1]){
          cout<<"ERROR in TABMactVmeReader:read_event: tdc_evnum="<<fpEvtStruct->tdc_evnum[fpEvtStruct->tdcev-1]<<"  measured event number="<<ev_words[windex-1]<<"  windex="<<windex<<"  data_num_ev="<<data_num_ev<<endl;
          fpEvtStruct->tdc_status=1;
        }
        if(ev_words[windex]>-1 && ev_words[windex]<bmmap->GetTdcMaxcha()){//measure found
          if(ev_words[windex]==bmmap->GetTrefCh()){
            fpEvtStruct->tdc_sync[sync_evnum]=ev_words[++windex];
            sync_evnum++;
          }else{      
            fpEvtStruct->tdc_id[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]]=ev_words[windex++];
            fpEvtStruct->tdc_meas[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]]=ev_words[windex];
            fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]++;
          }
        }else{
          cout<<"ERROR in TABMactVmeReader:read_event: tdc_channel out of range!!! tdc_channel="<<ev_words[windex]<<endl;
          fpEvtStruct->tdc_status=2;
        }
        new_event=false;
        if(FootDebugLevel(4) && ev_words[windex-1]!=bmmap->GetTrefCh())
          cout<<"TABMactVmeReader::measure found: tdc_evnum="<<fpEvtStruct->tdc_evnum[fpEvtStruct->tdcev-1]<<" tdc_id="<<fpEvtStruct->tdc_id[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]-1]<<"  corresponding bm channel="<<bmmap->tdc2cell(fpEvtStruct->tdc_id[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]-1])<<" hit_meas="<<fpEvtStruct->tdc_meas[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]-1]<<endl;
        else if(FootDebugLevel(4) && ev_words[windex-1]==bmmap->GetTrefCh())
          cout<<"TABMactVmeReader::trigger found: sync registered="<<sync_evnum<<"  time="<<fpEvtStruct->tdc_sync[sync_evnum-1]<<endl;
      }
    }//end of reading tdc words for loop
  }//end of tdc reading
  fpEvtStruct->tdc_numsync=sync_evnum;
  
  vector<Int_t> adc792_words;
  if(adc_wnum>0 && fpEvtStruct->tot_status==0 && fpEvtStruct->adc_status==0){//adc reading
    endindex=windex+adc_wnum;
    for(;windex<endindex;windex++)
      adc792_words.push_back(ev_words[windex]);
    monitorQDC(adc792_words);
  }

  if(sca_wnum>0 && fpEvtStruct->tot_status==0 && fpEvtStruct->sca_status==0){//scaler reading
    endindex=windex+sca_wnum;
    tmp_int=0;
    for(;windex<endindex;windex++){
      fpEvtStruct->sca830_meas[tmp_int]=ev_words[windex]-fpEvtStruct->sca830_counts[tmp_int];
      fpEvtStruct->sca830_counts[tmp_int++]=ev_words[windex];
    }
  }
  
  fpEvtStruct->time_evtoev=ev_words[windex++];
  fpEvtStruct->time_read=ev_words[windex++];
  fpEvtStruct->time_acq=ev_words[windex++];
  
  if(windex!=fpEvtStruct->words){
    cout<<"ERROR in TABMactVmeReader:read_event: there are missing words: read word="<<windex<<"  fpEvtStruct->words="<<fpEvtStruct->words<<""<<endl;
    cout<<"windex="<<windex<<"  tdc_wnum="<<tdc_wnum<<"  adc_wnum="<<adc_wnum<<"  sca_wnum="<<sca_wnum<<"  total number of words="<<fpEvtStruct->words<<"  tot_statu="<<fpEvtStruct->tot_status<<"  tdc_status="<<fpEvtStruct->tdc_status<<"  sca_status="<<fpEvtStruct->sca_status<<"  adc_status="<<fpEvtStruct->adc_status<<endl;
    fpEvtStruct->tot_status=4;
  }
  
  if(((fpEvtStruct->tdc_status!=-1000 || fpEvtStruct->tot_status!=0 || fpEvtStruct->adc_status!=0 || fpEvtStruct->sca_status!=0) && FootDebugLevel(3)) || (FootDebugLevel(4))){
    cout<<"TABMactVmeReader::read_event::fpEvtStruct->tdc_status="<<fpEvtStruct->tdc_status<<" fpEvtStruct->tot_status="<<fpEvtStruct->tot_status<<" fpEvtStruct->adc_status="<<fpEvtStruct->adc_status<<" fpEvtStruct->sca_status="<<fpEvtStruct->sca_status<<endl;
    if(fpEvtStruct->tdc_status!=-1000 || fpEvtStruct->tot_status!=0 || fpEvtStruct->adc_status!=0 || fpEvtStruct->sca_status!=0)
      cout<<"Error detected previously; ";
    cout<<"The whole event read is:    data_num_ev="<<data_num_ev<<endl;
    for(Int_t i=0;i<fpEvtStruct->words;i++)
      cout<<"ev_words["<<i<<"]="<<ev_words[i]<<endl;
  }
  
  if(FootDebugLevel(4))
    PrintBMstruct();
  
  //provv:
  //~ if(fpEvtStruct->tdc_hitnum[0]>10){
    //~ cout<<"TROVATO EVENTO CON "<<fpEvtStruct->tdc_hitnum[0]<<" eventi!!"<<endl;
    //~ for(Int_t i=0;i<fpEvtStruct->words;i++)
      //~ cout<<"ev_words["<<i<<"]="<<ev_words[i]<<endl;   
    //~ PrintBMstruct(); 
  //~ }
  
  //~ else{//read tdc words if    
    //~ cout<<"data_num_ev="<<data_num_ev<<endl;
    //~ for(Int_t i=0;i<fpEvtStruct->words;i++)
        //~ cout<<"ev_words["<<i<<"]="<<ev_words[i]<<endl;
  //~ }
  if(FootDebugLevel(4))
    cout<<"TABMactVmeReader::read_event()::done"<<endl;  
  return kTRUE;
}

void TABMactVmeReader::clear_bmstruct(Bool_t forced){
  fpEvtStruct->evnum=0;
  fpEvtStruct->words=0;
  fpEvtStruct->tdcev=0;
  fpEvtStruct->tot_status=0;
  fpEvtStruct->tdc_status=0;
  fpEvtStruct->adc_status=0;
  fpEvtStruct->sca_status=0;
  fpEvtStruct->time_evtoev=0;
  fpEvtStruct->time_read=0;
  fpEvtStruct->time_acq=0;
  fpEvtStruct->tdc_numsync=0;
  if(forced){
    for(Int_t i=0;i<MAXHITTDC;i++){
     fpEvtStruct->tdc_id[i]=-10000;
     fpEvtStruct->tdc_meas[i]=-10000;
     fpEvtStruct->tdc_sync[i]=-10000;
    }
    for(Int_t i=0;i<MAXEVTDC;i++){
     fpEvtStruct->tdc_hitnum[i]=0;
     fpEvtStruct->tdc_evnum[i]=-10000;
    }
    for(Int_t i=0;i<SCA830MAX;i++){
      fpEvtStruct->sca830_meas[i]=-10000;
      fpEvtStruct->sca830_counts[i]=0;
    }
    for(Int_t i=0;i<ADC792MAX;i++){
      fpEvtStruct->adc792_meas[i]=-10000;
      fpEvtStruct->adc792_over[i]=-10000;
    }
    return;
  }
  Int_t tmp_int=0;
  while(fpEvtStruct->tdc_sync[tmp_int]!=-10000){
    fpEvtStruct->tdc_sync[tmp_int]=-10000;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_id[tmp_int]!=-10000){
    fpEvtStruct->tdc_id[tmp_int]=-10000;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_meas[tmp_int]!=-10000){
    fpEvtStruct->tdc_meas[tmp_int]=-10000;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_evnum[tmp_int]!=-10000){
    fpEvtStruct->tdc_evnum[tmp_int]=-10000;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_hitnum[tmp_int]!=0){
    fpEvtStruct->tdc_hitnum[tmp_int]=0;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->sca830_meas[tmp_int]!=-10000){
    fpEvtStruct->sca830_meas[tmp_int]=-10000;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->adc792_meas[tmp_int]!=-10000){
    fpEvtStruct->adc792_meas[tmp_int]=-10000;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->adc792_over[tmp_int]!=-10000){
    fpEvtStruct->adc792_over[tmp_int]=-10000;
    tmp_int++;
  }
return;
}

void TABMactVmeReader::monitorQDC(vector<Int_t>& adc792_words) {

  TABMparCon* bmcon = (TABMparCon*) fpParCon->Object();
  TABMparMap* bmmap = (TABMparMap*) fpParMap->Object();

  Int_t qdc_cnt(0);
  Int_t data, dt_type, evtnum, chan;

  Bool_t bltread = kTRUE;
  //~ Double_t mulf = 400./4095;  //not used...
  
  //12 bit scale --> 400 pc

  for(Int_t iac=0; iac<bmmap->GetAdc792Ch()+2; iac++) {
    
    qdc_cnt = 0;
    data = adc792_words.at(iac);
    if(bltread) {
      dt_type = data>>24 & 0x7;
      if(FootDebugLevel(3))      
        cout<<"TABMactVmeReader::monitorQDC:: dt_type="<<dt_type<<endl;
      if(!(dt_type & 0x7)) {
        qdc_cnt = data & 0xFFF;
        chan = data>>17 & 0xF;
        if(FootDebugLevel(3)) 
          cout<<"TABMactVmeReader::monitorQDC:: qdc_cnt="<<qdc_cnt<<"   chan="<<chan<<" "<<"  adc792_words["<<iac<<"]="<<adc792_words.at(iac)<<endl;
        if(data>>12 & 0x1) {
          if(FootDebugLevel(3)) 
            cout<<"TABMactVmeReader::monitorQDC:: Overflow, my dear !!  chan="<<chan<<" qdc_cnt="<<qdc_cnt<<endl;
          fpEvtStruct->adc792_over[chan]=1;
          fpEvtStruct->adc_status=2;
        }
        else
          fpEvtStruct->adc792_over[chan]=0;//non ne sono sicuro...
      } else if(dt_type & 0x4) {
        //EOB
        evtnum = data & 0xFFFFFF;
        if(FootDebugLevel(1)) 
          cout<<"EvtNum "<<evtnum<<endl;
      } else if(dt_type & 0x2) {
        //Header
        if(FootDebugLevel(4)) 
          cout<<"TABMactVmeReader::monitorQDC:: Header found. "<<endl;
      }
    } else {//end of bltread==true
      dt_type = 0;
      qdc_cnt = data;
      chan = iac;
    }
    if(!dt_type) {
      if(FootDebugLevel(3)) 
        cout<<"TABMactVmeReader::monitorQDC::chan="<<chan<<" meas="<<qdc_cnt<<endl;
      if(chan>=0 && chan<bmmap->GetAdc792Ch()){
        fpEvtStruct->adc792_meas[chan] = qdc_cnt;
      }
    } 
   
  }
  
  for(Int_t i=0;i<bmmap->GetAdc792Ch();i++)
    if(fpEvtStruct->adc792_over[i]==-10000)
      fpEvtStruct->adc_status=3;
      
  return;
}


void TABMactVmeReader::PrintBMstruct(){
  cout<<"PrintBMstruct:"<<endl;
  cout<<"fpEvtStruct->evnum="<<fpEvtStruct->evnum<<endl;
  cout<<"fpEvtStruct->words="<<fpEvtStruct->words<<endl;
  cout<<"fpEvtStruct->tdcev="<<fpEvtStruct->tdcev<<endl;
  cout<<"fpEvtStruct->tdc_numsync="<<fpEvtStruct->tdc_numsync<<endl;
  cout<<"fpEvtStruct->tot_status="<<fpEvtStruct->tot_status<<endl;
  cout<<"fpEvtStruct->adc_status="<<fpEvtStruct->adc_status<<endl;
  cout<<"fpEvtStruct->sca_status="<<fpEvtStruct->sca_status<<endl;
  cout<<"fpEvtStruct->tdc_status="<<fpEvtStruct->tdc_status<<endl;
  Int_t tmp_int=0;
  while(fpEvtStruct->tdc_sync[tmp_int]!=-10000){
    cout<<"i="<<tmp_int<<"  fpEvtStruct->tdc_sync[i]="<<fpEvtStruct->tdc_sync[tmp_int]<<endl;;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_id[tmp_int]!=-10000){
    cout<<"i="<<tmp_int<<"  fpEvtStruct->tdc_id[i]="<<fpEvtStruct->tdc_id[tmp_int]<<endl;;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_meas[tmp_int]!=-10000){
    cout<<"i="<<tmp_int<<"  fpEvtStruct->tdc_meas[i]="<<fpEvtStruct->tdc_meas[tmp_int]<<endl;;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_evnum[tmp_int]!=-10000){
    cout<<"i="<<tmp_int<<"  fpEvtStruct->tdc_evnum[i]="<<fpEvtStruct->tdc_evnum[tmp_int]<<endl;;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_hitnum[tmp_int]!=0){
    cout<<"i="<<tmp_int<<"  fpEvtStruct->tdc_hitnum[i]="<<fpEvtStruct->tdc_hitnum[tmp_int]<<endl;;
    tmp_int++;
  }
  tmp_int=0;
  tmp_int=0;
  while(fpEvtStruct->adc792_meas[tmp_int]!=-10000){
    cout<<"i="<<tmp_int<<"  fpEvtStruct->adc792_meas[i]="<<fpEvtStruct->adc792_meas[tmp_int]<<endl;;
    tmp_int++;
  }
  while(fpEvtStruct->adc792_over[tmp_int]!=-10000){
    cout<<"i="<<tmp_int<<"  fpEvtStruct->adc792_over[i]="<<fpEvtStruct->adc792_over[tmp_int]<<endl;;
    tmp_int++;
  }
  while(fpEvtStruct->sca830_meas[tmp_int]!=-10000){
    cout<<"i="<<tmp_int<<"  fpEvtStruct->sca830_meas[i]="<<fpEvtStruct->sca830_meas[tmp_int]<<endl;;
    tmp_int++;
  }
  cout<<"fpEvtStruct->time_evtoev="<<fpEvtStruct->time_evtoev<<endl;
  cout<<"fpEvtStruct->time_read="<<fpEvtStruct->time_read<<endl;
  cout<<"fpEvtStruct->time_acq="<<fpEvtStruct->time_acq<<endl;

  cout<<endl;
  
}



