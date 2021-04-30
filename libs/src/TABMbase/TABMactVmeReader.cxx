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
  TABMparMap* p_bmmap = (TABMparMap*) fpParMap->Object();
	adc792.resize(p_bmmap->GetAdcMaxCh(),make_pair(-1000,-1000));
	sca830.resize(p_bmmap->GetScaMaxCh(),make_pair(0,0));
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

	//BM struct histos
	fpTotStatus=new TH1I( "BmStr_TotStatus", "tot_status; 0=ok otherwise=Error; Events", 11, -5.5, 5.5);
  AddHistogram(fpTotStatus);
	fpTdcStatus=new TH1I( "BmStr_TdcStatus", "tdc_status; 0=ok otherwise=Error; Events", 11, -5.5, 5.5);
  AddHistogram(fpTdcStatus);
	fpScaStatus=new TH1I( "BmStr_ScaStatus", "sca_status; 0=ok otherwise=Error; Events", 11, -5.5, 5.5);
  AddHistogram(fpScaStatus);
	fpScaNegative=new TH1I( "BmStr_ScaNegative", "Scaler negative measurements; Channel; Number of negative measurements", p_bmmap->GetScaMaxCh(), -0.5, p_bmmap->GetScaMaxCh()-0.5);
  AddHistogram(fpScaNegative);
	fpAdcStatus=new TH1I( "BmStr_AdcStatus", "adc_status; 0=ok otherwise=Error; Events", 11, -5.5, 5.5);
  AddHistogram(fpAdcStatus);
	fpTimeEvtoev=new TH1F("BmStr_RateAcquire","Acquisition rate from time_evtoev;rate (Hz); Events",500,0,5000);
  AddHistogram(fpTimeEvtoev);
	fpTimeRead=new TH1I("BmStr_TimeRead","Time occurred to read data from VME;Time (microsec); Events",1001,-0.5,1000.5);
  AddHistogram(fpTimeRead);

  fpRawError=new TH1I( "bmVmeDatHitAccDisc", "Number of Hits with errors in tdc; -1=Error 1=accepted; Hits", 3, -1.5, 1.5);
  AddHistogram(fpRawError);
	fpRawTrigTime=new TH1I( "bmVmeDatTrigger", "Trigger time; Trigger time [ns]; Events", 200, 0, 0);
	AddHistogram(fpRawTrigTime);
  fpRawTdcChannel=new TH1I( "TdcChaDist", "Number of hits in the tdc channels; tdc channel; Hits", p_bmmap->GetTdcMaxCh(), -0.5, p_bmmap->GetTdcMaxCh()-0.5);
  AddHistogram(fpRawTdcChannel);
  fpRawAdcAccDisc=new TH1I( "AdcAccDisc", "Adc overflow control; 0=ok  1=overflow; Hits",2,-0.5,1.5);
	AddHistogram(fpRawAdcAccDisc);
  TH1F *bmRawPlot;
  TH1I *bmRawIntPlot;
	for(Int_t i=0;i<p_bmmap->GetTdcMaxCh();i++){
    TString title="TdcRawCha_";
    title+=i;
    bmRawPlot=new TH1F( title.Data(), "Time;Time [ns]; Events", 1000, 0, 0);
    AddHistogram(bmRawPlot);
    fpRawTdcMeas.push_back(bmRawPlot);
  }
	for(Int_t i=0;i<p_bmmap->GetTdcMaxCh();i++){
    TString title="TdcLessSyncCha_";
    title+=i;
    bmRawPlot=new TH1F( title.Data(), "Tdc measurements;Time [ns]; Events", 1000, 0, 0);
    AddHistogram(bmRawPlot);
    fpRawTdcLessSync.push_back(bmRawPlot);
  }
	for(Int_t i=0;i<p_bmmap->GetScaMaxCh();i++){
    TString title="ScaRawCha_";
    title+=i;
    bmRawIntPlot=new TH1I( title.Data(), "Scaler measurements;Scaler counts; Events", 1001, -0.5, 1000.5);
    AddHistogram(bmRawIntPlot);
    fpRawScaMeas.push_back(bmRawIntPlot);
  }
  p_bmcal->ResetAdc(p_bmmap->GetAdcMaxCh());
	for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++){
    TString title="AdcRawCha_";
    title+=i;
    bmRawIntPlot=new TH1I( title.Data(), "Adc raw measurements;Adc counts; Events",2100, 0., 4200);
    AddHistogram(bmRawIntPlot);
    fpRawAdc.push_back(bmRawIntPlot);
  }
	for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++){
    TString title="AdcLessPed_";
    title+=i;
    bmRawPlot=new TH1F( title.Data(), "Adc meas. less pedestal;Adc counts; Events",2100, 0., 4200);
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

  Bool_t readed=ReadEvent();

	if(readed==kFALSE) //end of file
	  return kFALSE;

  //Let's do some checks
  if(fpEvtStruct->tot_status!=0 || fpEvtStruct->tdc_status!=0){
    fDataNumEv++;
    fDataSyncNumEv+=tdc_sync.size();
    cout<<"ERROR in TABMactVmeReader process: return ktrue; tot_status="<<fpEvtStruct->tot_status<<"  tdc_status="<<fpEvtStruct->tdc_status<<"  fDataNumEv="<<fDataNumEv<<"  fDataSyncNumEv="<<fDataSyncNumEv<<endl;
    fpDatRaw->SetBit(kValid);
    return kTRUE;
  }
  if(tdc_sync.size()==0) {
    fDataNumEv++;
    fDataSyncNumEv+=tdc_sync.size();
    Info("Action()","ERROR in TABMactVmeReader process: return ktrue; Trigger time is missing");
    fpDatRaw->SetBit(kValid);
    return kTRUE;
  }
  if(tdc_sync.size()>2) {
    fDataNumEv++;
    fDataSyncNumEv+=tdc_sync.size();
    Info("Action()","ERROR in TABMactVmeReader process: return ktrue; too many trigger time!");
    fpDatRaw->SetBit(kValid);
    return kTRUE;
  }
	if(tdc_event.size()!=1){
		fDataNumEv++;
		fDataSyncNumEv+=tdc_sync.size();
		Info("Action()","ERROR in TABMactVmeReader process: return ktrue; The tdc read more than one tdc event for the same global event!");
		fpDatRaw->SetBit(kValid);
		return kTRUE;
	}
  if (FootDebugLevel(1))
    cout<<"I'm in TABMactVmeReader::Process, fDataNumEv="<<fDataNumEv<<"   fpEvtStruct->evnum="<<fpEvtStruct->evnum<<"   number of tdc events="<<tdc_event.size()<<"   number of tdc syncs="<<tdc_sync.size()<<"  fDataSyncNumEv="<<fDataSyncNumEv<<endl;

  //TDC loop
  Double_t i_time, i_rdrift, synctime;
  Int_t lay, view, cell, up, cellid;
	//sometimes there are two syncs, in these cases the second one is the correct one (at least from the last data takings)
  synctime=((Double_t)tdc_sync.back())/10.;
	for (Int_t i = 0; i < tdc_event.at(0).first; i++) {
		if(ValidHistogram()){
			fpRawTdcChannel->Fill(tdc_meas[i].first);
      fpRawTdcMeas.at(tdc_meas[i].first)->Fill(tdc_meas[i].second/10.);
			fpRawTdcLessSync.at(tdc_meas[i].first)->Fill(tdc_meas[i].second/10.-synctime);
		}
    cellid=p_bmmap->tdc2cell(tdc_meas[i].first);
    if(cellid>=0 && p_bmcal->GetT0(cellid)!=-1000 ){
      p_bmgeo->GetBMNlvc(cellid, lay, view, cell);
      p_datraw->NewHit(cellid,lay,view,cell,tdc_meas[i].second/10.);
      if(FootDebugLevel(3))
        cout<<"hit charged: i="<<i<<"  tdc channel="<<tdc_meas[i].first<<"  tdc2cell="<<p_bmmap->tdc2cell(tdc_meas[i].first)<<"  tdc measurement="<<tdc_meas[i].second/10.<<"  T0="<<p_bmcal->GetT0(p_bmmap->tdc2cell(tdc_meas[i].first))<<"  trigtime="<<synctime<<"  hittime="<<((Double_t)  tdc_meas[i].second/10. -  p_bmcal->GetT0(p_bmmap->tdc2cell(tdc_meas[i].first)) - synctime)<<endl;
      if (ValidHistogram())
        fpRawError->Fill(1);
    }else if(cellid>=0){
      p_datraw->AddDischarged();
      if (ValidHistogram())
        fpRawError->Fill(-1);
      if(FootDebugLevel(3))
        cout<<"hit NOT charged since no T0 is set: i="<<i<<"  tdc channel="<<tdc_meas[i].first<<"  tdc2cell="<<p_bmmap->tdc2cell(tdc_meas[i].first)<<"  tdc measurement="<<tdc_meas[i].second/10.<<"  T0="<<p_bmcal->GetT0(p_bmmap->tdc2cell(tdc_meas[i].first))<<"  trigtime="<<synctime<<"  hittime="<<((Double_t)  tdc_meas[i].second/10. -  p_bmcal->GetT0(p_bmmap->tdc2cell(tdc_meas[i].first)) - synctime)<<endl;
      continue;
    }
  }

  //set the trigger time
  // p_timraw->SetTriggerTime(synctime);
  p_datraw->SetTrigtime(synctime);
  if (ValidHistogram())
    fpRawTrigTime->Fill(synctime);

  fDataNumEv++;
  fDataSyncNumEv+=tdc_sync.size();

	if(ValidHistogram()){
		//Bm struct
		fpTotStatus->Fill(fpEvtStruct->tot_status);
		fpTdcStatus->Fill(fpEvtStruct->tdc_status);
		fpScaStatus->Fill(fpEvtStruct->sca_status);
		fpAdcStatus->Fill(fpEvtStruct->adc_status);
    fpTimeEvtoev->Fill(1000000./fpEvtStruct->time_evtoev);
		fpTimeRead->Fill(fpEvtStruct->time_read);

		//Scaler
		for(Int_t i=0;i<p_bmmap->GetScaMaxCh();i++){
			fpRawScaMeas.at(i)->Fill(sca830[i].first);
			if(sca830[i].first<0)
				fpScaNegative->Fill(i);
		}
		//Adc
		for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++){
			fpRawAdc.at(i)->Fill(adc792[i].first);
			fpAdcLessPed.at(i)->Fill(adc792[i].first-(Int_t)(p_bmcal->GetAdcPed(i)+3*p_bmcal->GetAdcDevStd(i)));
			fpRawAdcAccDisc->Fill(adc792[i].second);
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


Bool_t TABMactVmeReader::ReadEvent() {
  TABMparMap* p_bmmap = (TABMparMap*) fpParMap->Object();

  Int_t tmp_int; //dummy int variable to read and convert the input file
  //~ vector<int> ev_words;//words of the event
  Bool_t new_event, read_meas;
  Int_t windex, endindex;
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

  //BOARD READING
  windex++;

  if(fpEvtStruct->tot_status==0 && tdc_wnum>0 && tdc_wnum>0) {//read the tdc words
    endindex=windex+tdc_wnum;//endindex-1 is the position of the last tdc word
    new_event=true;
    read_meas=false;
    for(;windex<endindex;windex++){
      if(new_event && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1)){//global header found
				tdc_event.push_back(make_pair(0,ev_words[windex++]));
        read_meas=true;
        if(FootDebugLevel(4))
          cout<<"global header found, windex="<<windex<<"  number of tdc event="<<tdc_event.size()<<endl;
        }
      //~ if(read_meas && ev_words[windex]<0 && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1)){//global trailer found //se uso acquisizione mio (yun)
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
      if(read_meas && ev_words[windex]==0 && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1)){//global trailer found //se uso acquisizione trento con 0 invece che -1000
        read_meas=false;
        new_event=true;
        fpEvtStruct->tdc_status=0;
        if(ev_words[windex]!=0){
          cout<<"Warning in TABMactVmeReader: global trailer found with error in tdc_evnum="<<tdc_event.back().second<<"  trailer="<<ev_words[windex]<<endl;
          new_event=false;
        }
        if(FootDebugLevel(4))
          cout<<"global trailer found, windex="<<windex<<"  ev_words="<<ev_words[windex]<<endl;
      }

      //only for data from cosmic rays taken in 2017 in Rome
      //~ if(read_meas && ev_words[windex]==0 && false && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1)){//global trailer found //se uso dati letti a Roma per BM refurbishment
        //~ read_meas=false;
        //~ new_event=true;
        //~ fpEvtStruct->tdc_status=0;
        //~ if(FootDebugLevel(1)>11 && (!(evt0 && FootDebugLevel(1)==99)))
          //~ cout<<"global trailer found, i="<<windex<<"  ev_words="<<ev_words[windex]<<endl;
      //~ }
      if(read_meas && (fpEvtStruct->tdc_status==0 || fpEvtStruct->tdc_status==-1)){//read measure
        if(ev_words[windex++]!=tdc_event.back().second){
          cout<<"ERROR in TABMactVmeReader:ReadEvent: number of tdc event="<<tdc_event.back().second<<"  measured event number="<<ev_words[windex-1]<<"  windex="<<windex<<"  fDataNumEv="<<fDataNumEv<<endl;
          fpEvtStruct->tdc_status=1;
        }
        if(ev_words[windex]>-1 && ev_words[windex]<p_bmmap->GetTdcMaxCh()){//measure found
          if(ev_words[windex]==p_bmmap->GetBmTrefCh()){
            tdc_sync.push_back(ev_words[++windex]);
          }else{
            tdc_meas.push_back(make_pair(ev_words[windex],ev_words[windex+1]));
						windex++;
            tdc_event.back().first++;
          }
        }else{
          cout<<"ERROR in TABMactVmeReader:ReadEvent: tdc_channel out of range!!! tdc_channel="<<ev_words[windex]<<endl;
          fpEvtStruct->tdc_status=2;
        }
        new_event=false;
        if(FootDebugLevel(4) && ev_words[windex-1]!=p_bmmap->GetBmTrefCh())
          cout<<"TABMactVmeReader::measure found: tdc_evnum="<<tdc_event.back().second<<" tdc channel="<<tdc_meas.back().first<<"  corresponding bm channel="<<p_bmmap->tdc2cell(tdc_meas.back().first)<<" measurement="<<tdc_meas.back().second<<endl;
        else if(FootDebugLevel(4) && ev_words[windex-1]==p_bmmap->GetBmTrefCh())
          cout<<"TABMactVmeReader::trigger found: sync registered="<<tdc_sync.size()<<"  time="<<tdc_sync.back()<<endl;
      }
    }//end of reading tdc words for loop
  }//end of tdc reading

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
      sca830.at(tmp_int).first=ev_words[windex]-sca830.at(tmp_int).second;
      sca830.at(tmp_int).second=ev_words[windex];
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

  if(((fpEvtStruct->tdc_status!=0 || fpEvtStruct->tot_status!=0 || fpEvtStruct->adc_status!=0 || fpEvtStruct->sca_status!=0) && FootDebugLevel(3)) || (FootDebugLevel(4))){
    cout<<"TABMactVmeReader::ReadEvent::fpEvtStruct->tdc_status="<<fpEvtStruct->tdc_status<<" fpEvtStruct->tot_status="<<fpEvtStruct->tot_status<<" fpEvtStruct->adc_status="<<fpEvtStruct->adc_status<<" fpEvtStruct->sca_status="<<fpEvtStruct->sca_status<<endl;
    if(fpEvtStruct->tdc_status!=0 || fpEvtStruct->tot_status!=0 || fpEvtStruct->adc_status!=0 || fpEvtStruct->sca_status!=0)
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
  fpEvtStruct->tot_status=0;
  fpEvtStruct->tdc_status=-1;
  fpEvtStruct->adc_status=0;
  fpEvtStruct->sca_status=0;
  fpEvtStruct->time_evtoev=0;
  fpEvtStruct->time_read=0;
  fpEvtStruct->time_acq=0;
	tdc_event.clear();
	tdc_sync.clear();
	tdc_meas.clear();
	if(forced){
		sca830.clear();
		adc792.clear();
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
          adc792.at(chan).second=1;
          fpEvtStruct->adc_status=2;
        }
        else
          adc792.at(chan).second=0;
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
        adc792.at(chan).first = qdc_cnt;
      }
    }

  }
  return;
}


void TABMactVmeReader::PrintBMstruct(){

	cout<<"PrintBMstruct:"<<endl;
  cout<<"evnum="<<fpEvtStruct->evnum<<endl;
  cout<<"words="<<fpEvtStruct->words<<endl;
	cout<<"tot_status="<<fpEvtStruct->tot_status<<endl;
	cout<<"tdc_status="<<fpEvtStruct->tdc_status<<endl;
  cout<<"adc_status="<<fpEvtStruct->adc_status<<endl;
  cout<<"sca_status="<<fpEvtStruct->sca_status<<endl;
	cout<<"time_evtoev="<<fpEvtStruct->time_evtoev<<endl;
	cout<<"time_read="<<fpEvtStruct->time_read<<endl;
	cout<<"time_acq="<<fpEvtStruct->time_acq<<endl;

	TABMparMap* p_bmmap = (TABMparMap*) fpParMap->Object();

  cout<<"tdc_event.size()="<<tdc_event.size()<<"  tdc_sync.size()="<<tdc_sync.size()<<"  tdc_meas.size()="<<tdc_meas.size()<<"  sca830.size()="<<sca830.size()<<"  adc792.size()="<<adc792.size()<<"  adcmaxch="<<p_bmmap->GetAdcMaxCh()<<"  scamaxcha="<<p_bmmap->GetScaMaxCh()<<endl;

	for(Int_t i=0;i<tdc_event.size();i++)
    cout<<"i="<<i<<"  tdc_event.first="<<tdc_event[i].first<<"  tdc_event[i].second="<<tdc_event[i].second<<endl;;
	for(Int_t i=0;i<tdc_sync.size();i++)
    cout<<"i="<<i<<"  tdc_sync="<<tdc_sync[i]<<endl;;
	for(Int_t i=0;i<tdc_meas.size();i++)
		cout<<"i="<<i<<"  tdc_meas.first="<<tdc_meas[i].first<<"  tdc_meas[i].second="<<tdc_meas[i].second<<endl;;
	for(Int_t i=0;i<p_bmmap->GetAdcMaxCh();i++)
    cout<<"adc:  channel="<<i<<" meas="<<adc792[i].first<<"  over="<<adc792[i].second<<endl;;
  for(Int_t i=0;i<p_bmmap->GetScaMaxCh();i++)
		cout<<"scaler: channel="<<i<<"  meas="<<sca830[i].first<<"  counts="<<sca830[i].second<<endl;;

  cout<<endl;

}
