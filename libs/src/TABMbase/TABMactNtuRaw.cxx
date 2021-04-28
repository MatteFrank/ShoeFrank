/*!
  \file
  \version $Id: TABMactNtuRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TABMactNtuRaw.
*/


#include "TABMactNtuRaw.hxx"

/*!
  \class TABMactNtuRaw TABMactNtuRaw.hxx "TABMactNtuRaw.hxx"
  \brief Get Beam Monitor raw data from DAQ. **
*/

ClassImp(TABMactNtuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactNtuRaw::TABMactNtuRaw(const char* name,
                             TAGdataDsc* dscdatraw,
                             TAGdataDsc* dscdatdaq,
                             TAGparaDsc* dscparmap,
                             TAGparaDsc* dscparcal,
                             TAGparaDsc* dscpargeo,
                             TAGdataDsc* dsctimraw)
  : TAGaction(name, "TABMactNtuRaw - Unpack BM raw data"),
    fpDatRaw(dscdatraw),
    fpDatDaq(dscdatdaq),
    fpParMap(dscparmap),
    fpParCal(dscparcal),
    fpParGeo(dscpargeo),
    fpTimRaw(dsctimraw)
{
  if (FootDebugLevel(1))
    cout<<"TABMactNtuRaw::default constructor::Creating the Beam Monitor hit Ntuplizer"<<endl;
  AddDataOut(dscdatraw, "TABMntuRaw");
  AddPara(dscparmap, "TABMparMap");
  AddPara(dscparcal, "TABMparCal");
  AddPara(dscpargeo, "TABMparGeo");
  AddDataIn(dsctimraw, "TASTntuHit");
  AddDataIn(dscdatdaq, "TAGdaqEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactNtuRaw::~TABMactNtuRaw()
{
}

//------------------------------------------+-----------------------------------

void TABMactNtuRaw::CreateHistogram()
{
  DeleteHistogram();
  TABMparMap*    p_parmap = (TABMparMap*)    fpParMap->Object();
  // fpRawMapX=new TH2I( "BM_Dat_cell_raw_occupancy_2d_x", "Cell occupancy for raw hits; z; x", 11, -5.5, 5.5,7, -3.5,3.5);
  // AddHistogram(fpRawMapX);
  // fpRawMapY=new TH2I( "BM_Dat_cell_raw_occupancy_2d_y", "Cell occupancy for raw hits; z; y", 11, -5.5, 5.5,7, -3.5,3.5);
  // AddHistogram(fpRawMapY);
  fpRawTdcChannel=new TH1I( "bmRawTdcHitDistribution", "Number of hits x channel (-1=error); TDC channel; Number of hits", p_parmap->GetTdcMaxCh()+2, -1.5, p_parmap->GetTdcMaxCh()+0.5);
  AddHistogram(fpRawTdcChannel);
  fpRawTrigTime=new TH1I( "bmRawTrigger", "Trigger time; Trigger time [ns]; Events", 200, 0, 0);
  AddHistogram(fpRawTrigTime);
  TH1F *RawTdcPlot;
  for(Int_t i=0;i<p_parmap->GetTdcMaxCh();i++){
    TString title="bmRawTdcCha_";
    title+=i;
    RawTdcPlot=new TH1F( title.Data(), "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
    AddHistogram(RawTdcPlot);
    fpRawTdcMeas.push_back(RawTdcPlot);
  }
  for(Int_t i=0;i<p_parmap->GetTdcMaxCh();i++){
    TString title="bmRawTdcLessSyncCha_";
    title+=i;
    RawTdcPlot=new TH1F( title.Data(), "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
    AddHistogram(RawTdcPlot);
    fpRawTdcLessSync.push_back(RawTdcPlot);
  }

  SetValidHistogram(kTRUE);
}


//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactNtuRaw::Action() {


   TAGdaqEvent*   p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();
   TASTntuHit*    p_timraw = (TASTntuHit*)    fpTimRaw->Object();

   Int_t nFragments = p_datdaq->GetFragmentsN();

  if(FootDebugLevel(1))
    cout<<"TABMactNtuRaw::Action():: I'm going to charge "<<nFragments<<" number of fragments"<<endl;

  if(p_timraw->GetTriggerTime()<0.0001){//No start counter trigger time! --> No BM hits
    if(FootDebugLevel(1))
      cout<<"TABMactNtuRaw::Action():: No ST trigger time --> no BM measurements for this event"<<endl;
    fpDatRaw->SetBit(kValid);
    return kTRUE;
  }

   for (Int_t i = 0; i < nFragments; ++i) {
       TString type = p_datdaq->GetClassType(i);
       if (type.Contains("TDCEvent")) {
         const TDCEvent* evt = static_cast<const TDCEvent*> (p_datdaq->GetFragment(i));
         DecodeHits(evt, p_timraw->GetTriggerTime());
         break;
       }
   }

   fpDatRaw->SetBit(kValid);

  if(FootDebugLevel(2))
    cout<<"TABMactNtuRaw::Action():: done"<<endl;

   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactNtuRaw::DecodeHits(const TDCEvent* evt, const double sttrigger) {

   TABMntuRaw*    p_datraw = (TABMntuRaw*)    fpDatRaw->Object();
   TABMparMap*    p_parmap = (TABMparMap*)    fpParMap->Object();
   TABMparCal*    p_parcal = (TABMparCal*)    fpParCal->Object();
   TABMparGeo*    p_pargeo = (TABMparGeo*)    fpParGeo->Object();

   //From there we get the Mapping of the wires into the Chamber to the TDC channels
  Int_t view,plane,cell, channel,up, hitnum=0, discharged=0, bmcellid;
  Double_t used_trigger=0., measurement;
  for(Int_t i = 0; i < ((int)evt->measurement.size());++i) {
    if(((evt->measurement.at(i)>>19) & 0x7f) == p_parmap->GetBmTrefCh()){
      used_trigger+=(evt->measurement.at(i) & 0x7ffff)/10. - sttrigger;
      if(p_parmap->GetDaqTrefCh()<0)
        break;
    }
    if(p_parmap->GetDaqTrefCh()>=0){
      if(((evt->measurement.at(i)>>19) & 0x7f) == p_parmap->GetDaqTrefCh())
        used_trigger-=(evt->measurement.at(i) & 0x7ffff)/10.;
    }
  }

  p_datraw->SetTrigtime(used_trigger);
  if (ValidHistogram())
    fpRawTrigTime->Fill(used_trigger);
  for(Int_t i = 0; i < ((int)evt->measurement.size());i++) {
    measurement=(Double_t) (evt->measurement.at(i) & 0x7ffff)/10.;
    channel=(evt->measurement.at(i)>>19) & 0x7f;
    if(channel>p_parmap->GetTdcMaxCh() || channel<0){
      cout<<"TABMactNtuRaw::ERROR!!!!!!!   channel="<<channel<<"  TDC maximum number of channel="<<p_parmap->GetTdcMaxCh()<<endl;
      cout<<"Something nasty just happened!!! please check the BM .map configuration file and/or the input Decoded data"<<endl;
      p_datraw->AddDischarged();
      if(ValidHistogram())
        fpRawTdcChannel->Fill(-1);
      continue;
    }
    bmcellid=p_parmap->tdc2cell(channel);
    if(ValidHistogram()){
      fpRawTdcChannel->Fill(channel);
      fpRawTdcMeas.at(channel)->Fill(measurement);
      fpRawTdcLessSync.at(channel)->Fill(measurement-used_trigger);
    }
    if(bmcellid!=-1 && bmcellid!=-1000){//-1000=syncTime, -1=not set
      p_pargeo->GetBMNlvc(bmcellid,plane,view,cell);
      p_datraw->NewHit(bmcellid, plane,view,cell,measurement);
      hitnum++;
      if(FootDebugLevel(3))
        cout<<"BM hit charged : channel="<<channel<<"  tdc2cell="<<bmcellid<<"  measurement="<<measurement<<"  T0="<<p_parcal->GetT0(bmcellid)<<"  triggertime="<<used_trigger<<"  hittime="<<measurement - p_parcal->GetT0(bmcellid)-used_trigger<<endl;
    }else if(channel!=p_parmap->GetBmTrefCh()){
      p_datraw->AddDischarged();
      if(FootDebugLevel(3))
        cout<<"BM hit DIScharged: tdc channel="<<channel<<"  measurement="<<measurement<<endl;
    }
  }


   return kTRUE;
}

void TABMactNtuRaw::EvaluateT0time(){
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
      if(start_bin==-1000)
        cout<<"WARNING: check if your tdc bin is ok!!! "<<"  tdcchannel="<<i<<"   peak_bin="<<peak_bin<<"   start_bin="<<start_bin<<"   MaximumBin="<<fpRawTdcLessSync.at(i)->GetMaximumBin()<<endl;
      if(fpRawTdcLessSync.at(i)->GetEntries()<1000)
        cout<<"WARNING: the number of hits is low:   tdcchannel="<<i<<"  cellid="<<cellid<<" number of hits:"<<fpRawTdcLessSync.at(i)->GetEntries()<<endl;
    }
 }
  return;
}
