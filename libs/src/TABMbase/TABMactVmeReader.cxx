/*!
  \file
  \version $Id: TABMactVmeReader.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TABMactVmeReader.
*/

#include "TABMparMap.hxx"
#include "TABMparCal.hxx"
#include "TABMrawHit.hxx"
#include "TABMntuRaw.hxx"
#include "TASTntuHit.hxx"
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
			     TAGdataDsc* dscdatraw,
			     TAGparaDsc* dscbmmap,
			     TAGparaDsc* dscbmcal,
			     TAGparaDsc* dscbmgeo)
           // TAGdataDsc* p_timraw)
  : TAGaction(name, "TABMactVmeReader - Unpack standalone BM raw data"),
    fpDatRaw(dscdatraw),
    fpParMap(dscbmmap),
    fpParCal(dscbmcal),
    fpParGeo(dscbmgeo)
    // fpTimRaw(p_timraw)
{
  AddDataOut(dscdatraw, "TABMntuRaw");
  AddPara(dscbmmap, "TABMparMap");
  AddPara(dscbmcal, "TABMparCal");
  AddPara(dscbmgeo, "TABMparGeo");
  // AddDataOut(p_timraw, "TASTntuHit");
  fpEvtStruct=new BM_struct;
  ClearBmstruct(kTRUE);
  fDataNumEv=-1000;
  fDataSyncNumEv=0;
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

  TABMparMap* p_bmmap = (TABMparMap*) fpParMap->Object();
  TABMparCal* p_bmcal = (TABMparCal*) fpParCal->Object();

  DeleteHistogram();
  fpRawError=new TH1I( "bmVmeDatHitAccDisc", "Number of Hits with errors in tdc; -1=Error 1=accepted; Hits", 3, -1, 2);
  AddHistogram(fpRawError);
	fpRawTrigTime=new TH1I( "bmVmeDatTrigger", "Trigger time; Trigger time [ns]; Events", 200, 0, 0);
	AddHistogram(fpRawTrigTime);
  fpRawTdcChannel=new TH1I( "TdcChaDist", "Number of hits in the tdc channels; tdc channel; Hits", p_bmmap->GetTdcMaxCh(), -0.5, p_bmmap->GetTdcMaxCh()-0.5);
  AddHistogram(fpRawTdcChannel);
  fpRawAdcAccDisc=new TH1I( "AdcAccDisc", "Adc overflow control; 0=ok  1=overflow; Hits",2,-0.5,1.5);
	AddHistogram(fpRawAdcAccDisc);
  TH1F *bmRawPlot;
	for(Int_t i=0;i<p_bmmap->GetTdcMaxCh();i++){
    TString title="TdcRawCha_";
    title+=i;
    bmRawPlot=new TH1F( title.Data(), "Time;Time [ns]; Events", 3000, 0, 0);
    AddHistogram(bmRawPlot);
    fpRawTdcMeas.push_back(bmRawPlot);
  }
	for(Int_t i=0;i<p_bmmap->GetTdcMaxCh();i++){
    TString title="TdcLessSyncCha_";
    title+=i;
    bmRawPlot=new TH1F( title.Data(), "Tdc measurements;Time [ns]; Events", 3000, 0, 0);
    AddHistogram(bmRawPlot);
    fpRawTdcLessSync.push_back(bmRawPlot);
  }
	for(Int_t i=0;i<p_bmmap->GetScaMaxCh();i++){
    TString title="ScaRawCha_";
    title+=i;
    bmRawPlot=new TH1F( title.Data(), "Scaler measurements;Scaler counts; Events", 2000, 10000, 30000);
    AddHistogram(bmRawPlot);
    fpRawSca.push_back(bmRawPlot);
  }
  p_bmcal->ResetAdc(p_bmmap->GetAdcMaxCh());
	for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++){
    TString title="AdcRawCha_";
    title+=i;
    bmRawPlot=new TH1F( title.Data(), "Adc raw measurements;Adc counts; Events",42001, -0.5, 4200.5);
    AddHistogram(bmRawPlot);
    fpRawAdc.push_back(bmRawPlot);
  }
	for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++){
    TString title="AdcLessPed_";
    title+=i;
    bmRawPlot=new TH1F( title.Data(), "Adc meas. less pedestal;Adc counts; Events",4201, -0.5, 4200.5);
    AddHistogram(bmRawPlot);
    fpAdcLessPed.push_back(bmRawPlot);
  }

  SetValidHistogram(kTRUE);
}


Bool_t TABMactVmeReader::Process() {

  TABMparCal* p_bmcal = (TABMparCal*) fpParCal->Object();
  TABMparMap* p_bmmap = (TABMparMap*) fpParMap->Object();
  TABMparGeo* p_bmgeo = (TABMparGeo*) fpParGeo->Object();
  TABMntuRaw* p_datraw= (TABMntuRaw*) fpDatRaw->Object();
  // TASTntuHit* p_timraw= (TASTntuHit*) fpTimRaw->Object();

  Bool_t readed=ReadEvent(kFALSE);

	if(readed==kFALSE) //end of file
	  return kFALSE;

  //some check on bm_struct
  if(fpEvtStruct->tot_status!=0 || fpEvtStruct->tdc_status!=-1000){
    fDataNumEv++;
    fDataSyncNumEv+=fpEvtStruct->tdc_numsync;
    cout<<"ERROR in TABMactVmeReader process: return ktrue; tot_status="<<fpEvtStruct->tot_status<<"  tdc_status="<<fpEvtStruct->tdc_status<<"  fDataNumEv="<<fDataNumEv<<"  fDataSyncNumEv="<<fDataSyncNumEv<<endl;
    fpDatRaw->SetBit(kValid);
    // fpTimRaw->SetBit(kValid);
    return kTRUE;
  }
  if(fpEvtStruct->tdc_sync[0] == -10000) {
    fDataNumEv++;
    fDataSyncNumEv+=fpEvtStruct->tdc_numsync;
    Info("Action()","ERROR in TABMactVmeReader process: return ktrue; Trigger time is missing");
    fpDatRaw->SetBit(kValid);
    // fpTimRaw->SetBit(kValid);
    return kTRUE;
  }
  if(fpEvtStruct->tdc_numsync>2) {
    fDataNumEv++;
    fDataSyncNumEv+=fpEvtStruct->tdc_numsync;
    Info("Action()","ERROR in TABMactVmeReader process: return ktrue; too many trigger time!");
    fpDatRaw->SetBit(kValid);
    // fpTimRaw->SetBit(kValid);
    return kTRUE;
  }
  if (FootDebugLevel(1))
    cout<<"I'm in TABMactVmeReader::Process, fDataNumEv="<<fDataNumEv<<"   fpEvtStruct->evnum="<<fpEvtStruct->evnum<<"   tdcev="<<fpEvtStruct->tdcev<<"   tdc_numsync="<<fpEvtStruct->tdc_numsync<<"  fDataSyncNumEv="<<fDataSyncNumEv<<endl;

  //TDC loop
  Double_t i_time, i_rdrift, synctime;
  Int_t lay, view, cell, up, cellid;
  synctime=(fpEvtStruct->tdc_numsync==2) ? fpEvtStruct->tdc_sync[1]/10. : fpEvtStruct->tdc_sync[0]/10.;
	for (Int_t i = 0; i < fpEvtStruct->tdc_hitnum[0]; i++) {
		if(ValidHistogram()){
			fpRawTdcChannel->Fill(fpEvtStruct->tdc_id[i]);
      fpRawTdcMeas.at(fpEvtStruct->tdc_id[i])->Fill(fpEvtStruct->tdc_meas[i]/10.);
			fpRawTdcLessSync.at(fpEvtStruct->tdc_id[i])->Fill(fpEvtStruct->tdc_meas[i]/10.-synctime);
		}
    cellid=p_bmmap->tdc2cell(fpEvtStruct->tdc_id[i]);
    if(fpEvtStruct->tdc_meas[i]!=-10000 && p_bmcal->GetT0(cellid)!=-1000 &&  cellid>=0 ){
      p_bmgeo->GetBMNlvc(cellid, lay, view, cell);
      p_datraw->NewHit(cellid,lay,view,cell,fpEvtStruct->tdc_meas[i]/10.);
      if(FootDebugLevel(3))
        cout<<"hit charged: i="<<i<<"  tdc_id="<<fpEvtStruct->tdc_id[i]<<"  tdc2cell="<<p_bmmap->tdc2cell(fpEvtStruct->tdc_id[i])<<"  tdc_meas/10.="<<fpEvtStruct->tdc_meas[i]/10.<<"  T0="<<p_bmcal->GetT0(p_bmmap->tdc2cell(fpEvtStruct->tdc_id[i]))<<"  trigtime="<<synctime<<"  hittime="<<((Double_t)  fpEvtStruct->tdc_meas[i]/10. -  p_bmcal->GetT0(p_bmmap->tdc2cell(fpEvtStruct->tdc_id[i])) - synctime)<<endl;
      if (ValidHistogram())
        fpRawError->Fill(1);
    }else if(cellid>=0){
      p_datraw->AddDischarged();
      if (ValidHistogram())
        fpRawError->Fill(-1);
      if(FootDebugLevel(3))
        cout<<"hit NOT charged: i="<<i<<"  tdc_id="<<fpEvtStruct->tdc_id[i]<<"  tdc2cell="<<p_bmmap->tdc2cell(fpEvtStruct->tdc_id[i])<<"  tdc_meas/10.="<<fpEvtStruct->tdc_meas[i]/10.<<"  T0="<<p_bmcal->GetT0(p_bmmap->tdc2cell(fpEvtStruct->tdc_id[i]))<<"  trigtime="<<synctime<<"  hittime="<<((Double_t)  fpEvtStruct->tdc_meas[i]/10. -  p_bmcal->GetT0(p_bmmap->tdc2cell(fpEvtStruct->tdc_id[i])) - synctime)<<endl;
      continue;
    }
  }

  //set the trigger time
  // p_timraw->SetTriggerTime(synctime);
  p_datraw->SetTrigtime(synctime);
  if (ValidHistogram())
    fpRawTrigTime->Fill(synctime);

  fDataNumEv++;
  fDataSyncNumEv+=fpEvtStruct->tdc_numsync;

	if(ValidHistogram()){
		//Scaler
		for(Int_t i=0;i<p_bmmap->GetScaMaxCh();i++){
			fpRawSca.at(i)->Fill(fpEvtStruct->sca830_meas[i]);
		}
		//Adc
		for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++){
			fpRawAdc.at(i)->Fill(fpEvtStruct->adc792_meas[i]);
			fpAdcLessPed.at(i)->Fill(fpEvtStruct->adc792_meas[i]-(Int_t)(p_bmcal->GetAdcPed(i)+3*p_bmcal->GetAdcDevStd(i)));
			fpRawAdcAccDisc->Fill(fpEvtStruct->adc792_over[i]);
		}
  }


  fpDatRaw->SetBit(kValid);
  // fpTimRaw->SetBit(kValid);

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

void TABMactVmeReader::EvaluateT0time(){
  int  start_bin, peak_bin, cellid;
  TABMparCal*    p_parcal = (TABMparCal*)    fpParCal->Object();
  TABMparMap*    p_parmap = (TABMparMap*)    fpParMap->Object();
  for(Int_t i=0;i<fpRawTdcLessSync.size();i++){
		cellid=p_parmap->tdc2cell(i);
    if( cellid>=0){
      start_bin=-1000;
      peak_bin=fpRawTdcLessSync.at(i)->GetMaximumBin();
      for(Int_t j=fpRawTdcLessSync.at(i)->GetMaximumBin();j>0;j--)
        if(fpRawTdcLessSync.at(i)->GetBinContent(j)>fpRawTdcLessSync.at(i)->GetBinContent(fpRawTdcLessSync.at(i)->GetMaximumBin())/10.)
          start_bin=j;
      p_parcal->SetT0(cellid,((Float_t)fpRawTdcLessSync.at(i)->GetBinCenter(start_bin)));
      if(start_bin==0)
        cout<<"WARNING: check if your tdc bin is ok!!! "<<"  tdcchannel="<<i<<"   peak_bin="<<peak_bin<<"   start_bin="<<start_bin<<"   MaximumBin="<<fpRawTdcLessSync.at(i)->GetMaximumBin()<<endl;
			if(fpRawTdcLessSync.at(i)->GetEntries()<1000)
        cout<<"EvaluateT0time:WARNING: only few events to evaluate T0 in tdcchannel="<<i<<"  cellid="<<cellid<<"  Number of events="<<fpRawTdcLessSync.at(i)->GetEntries()<<endl;
    }
 }
  return;
}

void TABMactVmeReader::EvaluateAdcPedestals(){
  int  channel;
  TABMparCal*    p_parcal = (TABMparCal*)    fpParCal->Object();
  TABMparMap*    p_parmap = (TABMparMap*)    fpParMap->Object();

	p_parcal->ResetAdc(p_parmap->GetAdcMaxCh());
  TF1 *gaus = new TF1("gaus","gaus", 0.,1000);
  for(Int_t i=0;i<fpRawAdc.size();i++){
		gaus->SetParameter(0,fpRawAdc.at(i)->GetEntries());
		gaus->SetParameter(1,fpRawAdc.at(i)->GetMean());
		gaus->SetParameter(2,fpRawAdc.at(i)->GetStdDev());
    fpRawAdc.at(i)->Fit("gaus", "QB+");
		p_parcal->SetAdc(i,gaus->GetParameter(1),gaus->GetParameter(2));
		if(fpRawAdc.at(i)->GetEntries()<500)
			cout<<"EvaluateAdcPedestals:WARNING: only="<<fpRawAdc.at(i)->GetEntries()<<"  events for the adc pedestal fit in channel "<<i<<endl;
  }
  return;
}

//********************************************** BM STANDALONE READER ***********************************************************


Bool_t TABMactVmeReader::ReadEvent(Bool_t evt0) {
  TABMparMap* p_bmmap = (TABMparMap*) fpParMap->Object();

  Int_t tmp_int; //dummy int variable to read and convert the input file
  //~ vector<int> ev_words;//words of the event
  Bool_t new_event, read_meas;
  Int_t sync_evnum=0, windex, endindex;
  Int_t tdc_wnum=0, adc_wnum=0, sca_wnum=0;

  ClearBmstruct(kFALSE);

  if(FootDebugLevel(4))
    cout<<"I'm in TABMactVmeReader:ReadEvent"<<endl;
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
  if(fDataNumEv<0){
    fDataNumEv=fpEvtStruct->evnum;
  } else if(fDataNumEv!=fpEvtStruct->evnum){
    cout<<"ERROR in TABMactVmeReader:ReadEvent: fDataNumEv="<<fDataNumEv<<"  fpEvtStruct->evnum="<<fpEvtStruct->evnum<<endl;
    fpEvtStruct->tot_status=-1;
  }
  if(ev_words[1]!=p_bmmap->GetBoardNum()){
    cout<<"ERROR in TABMactVmeReader:ReadEvent: board num != "<<p_bmmap->GetBoardNum()<<"  ev_words[1]="<<ev_words[1]<<endl;
    fpEvtStruct->tot_status=1;
  }

  tdc_wnum=ev_words[2];
  if(tdc_wnum<=0)
    fpEvtStruct->tdc_status=3;
  windex=2;
  if(p_bmmap->GetAdcMaxCh()>=0){
    windex++;
    adc_wnum=ev_words[windex];
    if(adc_wnum!=p_bmmap->GetAdcMaxCh()+2 && adc_wnum!=0){
      cout<<"ERROR in TABMactVmeReader:ReadEvent: adc_wnum="<<adc_wnum<<"  p_bmmap->GetAdcMaxCh()+2="<<p_bmmap->GetAdcMaxCh()+2<<endl;
      fpEvtStruct->adc_status=1;
    }
  }
  if(p_bmmap->GetScaMaxCh()>=0){
    windex++;
    sca_wnum=ev_words[windex];
    if(sca_wnum!=p_bmmap->GetScaMaxCh() && sca_wnum!=0){
      cout<<"ERROR in TABMactVmeReader:ReadEvent: sca_wnum="<<sca_wnum<<"  p_bmmap->GetScaMaxCh()="<<p_bmmap->GetScaMaxCh()<<endl;
      fpEvtStruct->sca_status=1;
    }
  }

  if(ev_words[++windex]!=3){//provv, da modificare: da togliere nell'acquisizione e qua...
    cout<<"ERROR in TABMactVmeReader:ReadEvent: ev_words["<<windex<<"]="<<ev_words[windex]<<" ,!=3"<<endl;
    fpEvtStruct->tot_status=3;
  }

  if((windex+tdc_wnum+adc_wnum+sca_wnum+4) != fpEvtStruct->words){
    cout<<"ERROR in TABMactVmeReader:ReadEvent: counter for the event is wrong:  windex="<<windex<<"  tdc_wnum="<<tdc_wnum<<"  adc_wnum="<<adc_wnum<<"  sca_wnum="<<sca_wnum<<"  total number of words="<<fpEvtStruct->words<<"  windex+tdc_wnum+adc_wnum+sca_wnum+4="<<windex+tdc_wnum+adc_wnum+sca_wnum+4<<endl;
    fpEvtStruct->tot_status=4;
  }

  //~ if((fpEvtStruct->tot_status!=0 && FootDebugLevel(1)>0) || FootDebugLevel(1)>11)
    //~ for(Int_t i=0;i<fpEvtStruct->words;i++)
      //~ cout<<"fDataNumEv="<<fDataNumEv<<"   ev_words["<<i<<"]="<<ev_words[i]<<endl;

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
          cout<<"ERROR in TABMactVmeReader:ReadEvent: tdc_evnum="<<fpEvtStruct->tdc_evnum[fpEvtStruct->tdcev-1]<<"  measured event number="<<ev_words[windex-1]<<"  windex="<<windex<<"  fDataNumEv="<<fDataNumEv<<endl;
          fpEvtStruct->tdc_status=1;
        }
        if(ev_words[windex]>-1 && ev_words[windex]<p_bmmap->GetTdcMaxCh()){//measure found
          if(ev_words[windex]==p_bmmap->GetBmTrefCh()){
            fpEvtStruct->tdc_sync[sync_evnum]=ev_words[++windex];
            sync_evnum++;
          }else{
            fpEvtStruct->tdc_id[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]]=ev_words[windex++];
            fpEvtStruct->tdc_meas[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]]=ev_words[windex];
            fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]++;
          }
        }else{
          cout<<"ERROR in TABMactVmeReader:ReadEvent: tdc_channel out of range!!! tdc_channel="<<ev_words[windex]<<endl;
          fpEvtStruct->tdc_status=2;
        }
        new_event=false;
        if(FootDebugLevel(4) && ev_words[windex-1]!=p_bmmap->GetBmTrefCh())
          cout<<"TABMactVmeReader::measure found: tdc_evnum="<<fpEvtStruct->tdc_evnum[fpEvtStruct->tdcev-1]<<" tdc_id="<<fpEvtStruct->tdc_id[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]-1]<<"  corresponding bm channel="<<p_bmmap->tdc2cell(fpEvtStruct->tdc_id[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]-1])<<" hit_meas="<<fpEvtStruct->tdc_meas[fpEvtStruct->tdc_hitnum[fpEvtStruct->tdcev-1]-1]<<endl;
        else if(FootDebugLevel(4) && ev_words[windex-1]==p_bmmap->GetBmTrefCh())
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
    MonitorQDC(adc792_words);
  }

  if(sca_wnum>0 && fpEvtStruct->tot_status==0 && fpEvtStruct->sca_status==0){//scaler reading
    endindex=windex+sca_wnum;
    tmp_int=0;
    for(;windex<endindex;windex++){
			cout<<"provv: vediamo parola nuova: tmp_int="<<tmp_int<<" evwords="<<ev_words[windex]<<"  old_counts="<<fpEvtStruct->sca830_counts[tmp_int]<<"  nuova meas="<<ev_words[windex]-fpEvtStruct->sca830_counts[tmp_int]<<endl;
      fpEvtStruct->sca830_meas[tmp_int]=ev_words[windex]-fpEvtStruct->sca830_counts[tmp_int];
      fpEvtStruct->sca830_counts[tmp_int]=ev_words[windex];
			cout<<"scrittura counts="<<fpEvtStruct->sca830_counts[tmp_int]<<endl;
			tmp_int++;
    }
  }

  fpEvtStruct->time_evtoev=ev_words[windex++];
  fpEvtStruct->time_read=ev_words[windex++];
  fpEvtStruct->time_acq=ev_words[windex++];

  if(windex!=fpEvtStruct->words){
    cout<<"ERROR in TABMactVmeReader:ReadEvent: there are missing words: read word="<<windex<<"  fpEvtStruct->words="<<fpEvtStruct->words<<""<<endl;
    cout<<"windex="<<windex<<"  tdc_wnum="<<tdc_wnum<<"  adc_wnum="<<adc_wnum<<"  sca_wnum="<<sca_wnum<<"  total number of words="<<fpEvtStruct->words<<"  tot_statu="<<fpEvtStruct->tot_status<<"  tdc_status="<<fpEvtStruct->tdc_status<<"  sca_status="<<fpEvtStruct->sca_status<<"  adc_status="<<fpEvtStruct->adc_status<<endl;
    fpEvtStruct->tot_status=4;
  }

  if(((fpEvtStruct->tdc_status!=-1000 || fpEvtStruct->tot_status!=0 || fpEvtStruct->adc_status!=0 || fpEvtStruct->sca_status!=0) && FootDebugLevel(3)) || (FootDebugLevel(4))){
    cout<<"TABMactVmeReader::ReadEvent::fpEvtStruct->tdc_status="<<fpEvtStruct->tdc_status<<" fpEvtStruct->tot_status="<<fpEvtStruct->tot_status<<" fpEvtStruct->adc_status="<<fpEvtStruct->adc_status<<" fpEvtStruct->sca_status="<<fpEvtStruct->sca_status<<endl;
    if(fpEvtStruct->tdc_status!=-1000 || fpEvtStruct->tot_status!=0 || fpEvtStruct->adc_status!=0 || fpEvtStruct->sca_status!=0)
      cout<<"Error detected previously; ";
    cout<<"The whole event read is:    fDataNumEv="<<fDataNumEv<<endl;
    for(Int_t i=0;i<fpEvtStruct->words;i++)
      cout<<"ev_words["<<i<<"]="<<ev_words[i]<<endl;
  }

  if(FootDebugLevel(4))
    PrintBMstruct();

  if(FootDebugLevel(4))
    cout<<"TABMactVmeReader::ReadEvent()::done"<<endl;
  return kTRUE;
}

void TABMactVmeReader::ClearBmstruct(Bool_t forced){
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

void TABMactVmeReader::MonitorQDC(vector<Int_t>& adc792_words) {

  TABMparMap* p_bmmap = (TABMparMap*) fpParMap->Object();

  Int_t qdc_cnt(0);
  Int_t data, dt_type, evtnum, chan;

  Bool_t bltread = kTRUE;
  //~ Double_t mulf = 400./4095;  //not used...

  //12 bit scale --> 400 pc

  for(Int_t iac=0; iac<p_bmmap->GetAdcMaxCh()+2; iac++) {

    qdc_cnt = 0;
    data = adc792_words.at(iac);
    if(bltread) {
      dt_type = data>>24 & 0x7;
      if(FootDebugLevel(3))
        cout<<"TABMactVmeReader::MonitorQDC:: dt_type="<<dt_type<<endl;
      if(!(dt_type & 0x7)) {
        qdc_cnt = data & 0xFFF;
        chan = data>>17 & 0xF;
        if(FootDebugLevel(3))
          cout<<"TABMactVmeReader::MonitorQDC:: qdc_cnt="<<qdc_cnt<<"   chan="<<chan<<" "<<"  adc792_words["<<iac<<"]="<<adc792_words.at(iac)<<endl;
        if(data>>12 & 0x1) {
          if(FootDebugLevel(3))
            cout<<"TABMactVmeReader::MonitorQDC:: Overflow, my dear !!  chan="<<chan<<" qdc_cnt="<<qdc_cnt<<endl;
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
          cout<<"TABMactVmeReader::MonitorQDC:: Header found. "<<endl;
      }
    } else {//end of bltread==true
      dt_type = 0;
      qdc_cnt = data;
      chan = iac;
    }
    if(!dt_type) {
      if(FootDebugLevel(3))
        cout<<"TABMactVmeReader::MonitorQDC::chan="<<chan<<" meas="<<qdc_cnt<<endl;
      if(chan>=0 && chan<p_bmmap->GetAdcMaxCh()){
        fpEvtStruct->adc792_meas[chan] = qdc_cnt;
      }
    }

  }

  for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++)
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
    cout<<"i="<<tmp_int<<"  tdc_sync="<<fpEvtStruct->tdc_sync[tmp_int]<<endl;;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_id[tmp_int]!=-10000){
    cout<<"i="<<tmp_int<<"  tdc_id="<<fpEvtStruct->tdc_id[tmp_int]<<"  tdc_meas="<<fpEvtStruct->tdc_meas[tmp_int]<<endl;;
    tmp_int++;
  }
  tmp_int=0;
  while(fpEvtStruct->tdc_evnum[tmp_int]!=-10000){
    cout<<"i="<<tmp_int<<"  tdc_hitnum="<<fpEvtStruct->tdc_hitnum[tmp_int]<<"  tdc_evnum="<<fpEvtStruct->tdc_evnum[tmp_int]<<endl;;
    tmp_int++;
  }
	TABMparMap* p_bmmap = (TABMparMap*) fpParMap->Object();
	for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++)
    cout<<"i="<<i<<"  adc792_meas="<<fpEvtStruct->adc792_meas[i]<<"  adc792_over="<<fpEvtStruct->adc792_over[i]<<endl;;
  for(Int_t i=0;i<p_bmmap->GetScaMaxCh();i++)
		cout<<"i="<<i<<"  sca830_meas="<<fpEvtStruct->sca830_meas[i]<<"  sca830_counts="<<fpEvtStruct->sca830_counts[i]<<endl;;

  cout<<"fpEvtStruct->time_evtoev="<<fpEvtStruct->time_evtoev<<endl;
  cout<<"fpEvtStruct->time_read="<<fpEvtStruct->time_read<<endl;
  cout<<"fpEvtStruct->time_acq="<<fpEvtStruct->time_acq<<endl;

  cout<<endl;

}
