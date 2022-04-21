/*!
  \file   TABMactNtuRaw.cxx
  \brief   Declaration of TABMactNtuRaw, the class that convert the FOOT DAQ input to create a BM raw hit object (TABMntuRaw)
*/


#include "TABMactNtuRaw.hxx"

/*!
\file   TABMactNtuRaw.cxx
\brief   Declaration of TABMactNtuRaw, the class that convert the FOOT DAQ input to create a BM raw hit object (TABMntuRaw)
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
//! Create the histograms
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
  fpRawTrigTrigger=new TH1F( "bmRawTrigger", "BM used trigger time; Trigger time [ns]; Events", 200, 0, 0);
  AddHistogram(fpRawTrigTrigger);
  fpRawSTFitTrigger=new TH1F( "bmSTFITTrigger", "SC Fitted Trigger time; Trigger time [ns]; Events", 200, 0, 0);
  AddHistogram(fpRawSTFitTrigger);
  fpRawTdcTrigger=new TH1F( "bmTdcTrigger", "Tdc Trigger time; Trigger time [ns]; Events", 200, 0, 0);
  AddHistogram(fpRawTdcTrigger);
  if(p_parmap->GetDaqTrefCh()>=0){
    fpRawMagorTrigger=new TH1F( "bmmagortrigger", "Margherita Or Trigger time; Trigger time [ns]; Events", 200, 0, 0);
    AddHistogram(fpRawMagorTrigger);
    fpRawbmTdcLessSTFITLessMagorTrigger=new TH1F( "bmTdcLessSTFITLessMagorTrigger", "Tdc Trigger Less STFITT time; Trigger time [ns]; Events", 200, 0, 0);
    AddHistogram(fpRawbmTdcLessSTFITLessMagorTrigger);
    fpRawbmTdcLessSTFITPlusMagorTrigger=new TH1F( "bmTdcLessSTFITPlusMagorTrigger", "Tdc Trigger Less STFITT time; Trigger time [ns]; Events", 200, 0, 0);
    AddHistogram(fpRawbmTdcLessSTFITPlusMagorTrigger);
    fpRawbmTdcPlusSTFITPlusMagorTrigger=new TH1F( "bmTdcPlusSTFITPlusMagorTrigger", "Tdc Trigger Less STFITT time; Trigger time [ns]; Events", 200, 0, 0);
    AddHistogram(fpRawbmTdcPlusSTFITPlusMagorTrigger);
    fpRawbmTdcPlusSTFITLessMagorTrigger=new TH1F( "bmTdcPlusSTFITLessMagorTrigger", "Tdc Trigger Less STFITT time; Trigger time [ns]; Events", 200, 0, 0);
    AddHistogram(fpRawbmTdcPlusSTFITLessMagorTrigger);
    fpRawMagorDouble=new TH1I( "bmmagordouble", "Margherita Or double counting whitin the +- 10 ns; Number of Margherita or signal; Events", 21, -0.5, 20.5);
    AddHistogram(fpRawMagorDouble);
  }

  fpRawbmTdcLessSTFITTrigger=new TH1F( "bmTdcLessSTFITTrigger", "Tdc Trigger Less STFITT time; Trigger time [ns]; Events", 200, 0, 0);
  AddHistogram(fpRawbmTdcLessSTFITTrigger);
  fpRawbmTdcPlusSTFITTrigger=new TH1F( "bmTdcPlusSTFITTrigger", "Tdc Trigger Plus STFITT time; Trigger time [ns]; Events", 200, 0, 0);
  AddHistogram(fpRawbmTdcPlusSTFITTrigger);

  fpRawCh1NoTrig=new TH1F( "RawCh1NoTrig", "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
  AddHistogram(fpRawCh1NoTrig);
  fpRawCh1LessTdcTr=new TH1F( "RawCh1LessTdcTr", "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
  AddHistogram(fpRawCh1LessTdcTr);
  fpRawCh1LessSTFit=new TH1F( "RawCh1LessSTFit", "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
  AddHistogram(fpRawCh1LessSTFit);
  fpRawCh1PlusSTFit=new TH1F( "RawCh1PlusSTFit", "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
  AddHistogram(fpRawCh1PlusSTFit);
  if(p_parmap->GetDaqTrefCh()>=0){
    fpRawCh1LessMagorTr=new TH1F( "RawCh1LessMagorTr", "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
    AddHistogram(fpRawCh1LessMagorTr);
    fpRawCh1LessSTFitLessMagor=new TH1F( "RawCh1LessSTFitLessMagor", "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
    AddHistogram(fpRawCh1LessSTFitLessMagor);
    fpRawCh1PlusSTFitLessMagor=new TH1F( "RawCh1PlusSTFitLessMagor", "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
    AddHistogram(fpRawCh1PlusSTFitLessMagor);
  }

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

  if (ValidHistogram())
    fpRawSTFitTrigger->Fill(p_timraw->GetTriggerTime());

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
//! Decode the BM hits from the FOOT DAQ and the start counter trigger time
//!
//! \param[in] evt FOOT DAQ TDCevent object with the tdc measurements
//! \param[in] sttrigger trigger time provided by the start counter
Bool_t TABMactNtuRaw::DecodeHits(const TDCEvent* evt, const double sttrigger) {

   TABMntuRaw*    p_datraw = (TABMntuRaw*)    fpDatRaw->Object();
   TABMparMap*    p_parmap = (TABMparMap*)    fpParMap->Object();
   TABMparCal*    p_parcal = (TABMparCal*)    fpParCal->Object();
   TABMparGeo*    p_pargeo = (TABMparGeo*)    fpParGeo->Object();

   //From there we get the Mapping of the wires into the Chamber to the TDC channels
  Int_t view,plane,cell, channel,up, hitnum=0, discharged=0, bmcellid;
  Double_t used_trigger=0., measurement, tdctrigger=-99999, magortrigger=-99999;
  vector<Double_t>magorvec;
  for(Int_t i = 0; i < ((int)evt->measurement.size());++i) {
    if(((evt->measurement.at(i)>>19) & 0x7f) == p_parmap->GetBmTrefCh()){
      tdctrigger=(evt->measurement.at(i) & 0x7ffff)/10.;
      // used_trigger+=tdctrigger - sttrigger;
      if(ValidHistogram())
        fpRawTdcTrigger->Fill(tdctrigger);
      if(p_parmap->GetDaqTrefCh()<0)
        break;
    }
    if(p_parmap->GetDaqTrefCh()>=0){
      if(((evt->measurement.at(i)>>19) & 0x7f) == p_parmap->GetDaqTrefCh()){
        magorvec.push_back((evt->measurement.at(i) & 0x7ffff)/10.);
        // used_trigger+=magortrigger;
        if(ValidHistogram())
          fpRawMagorTrigger->Fill(magorvec.back());
      }
    }
  }

  // check on trigger status
  Int_t magorcounter=0;
  if(p_parmap->GetDaqTrefCh()>=0){
    if(tdctrigger!=99999 && magorvec.size()!=0){
      Double_t min=999999999;
      for(Int_t i=0;i<magorvec.size();i++){
        if(fabs(tdctrigger-magorvec.at(i))<min){
          magortrigger=magorvec.at(i);
          magorcounter++;
        }
      }
    }
    if(magortrigger!=-99999)
      used_trigger=magortrigger+tdctrigger+sttrigger;
  }else
    used_trigger=tdctrigger+sttrigger;

  //redifinition of used
  used_trigger=tdctrigger+sttrigger;

  p_datraw->SetTrigtime(used_trigger);
  if (ValidHistogram()){
    fpRawTrigTrigger->Fill(used_trigger);
    if(p_parmap->GetDaqTrefCh()>=0){
      fpRawMagorDouble->Fill(magorcounter);
      fpRawbmTdcLessSTFITLessMagorTrigger->Fill(tdctrigger-sttrigger-magortrigger);
      fpRawbmTdcLessSTFITPlusMagorTrigger->Fill(tdctrigger-sttrigger+magortrigger);
      fpRawbmTdcPlusSTFITPlusMagorTrigger->Fill(tdctrigger+sttrigger+magortrigger);
      fpRawbmTdcPlusSTFITLessMagorTrigger->Fill(tdctrigger+sttrigger-magortrigger);
    }
    fpRawbmTdcPlusSTFITTrigger->Fill(tdctrigger+sttrigger);
    fpRawbmTdcLessSTFITTrigger->Fill(tdctrigger-sttrigger);
  }


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
      if(channel==10){
        fpRawCh1NoTrig->Fill(measurement);
        if(tdctrigger!=-99999){
          fpRawCh1LessSTFit->Fill(measurement-tdctrigger-sttrigger);
          fpRawCh1PlusSTFit->Fill(measurement-tdctrigger+sttrigger);
          fpRawCh1LessTdcTr->Fill(measurement-tdctrigger);
          if(magortrigger!=-99999){
            fpRawCh1LessMagorTr->Fill(measurement-tdctrigger-magortrigger);
            fpRawCh1LessSTFitLessMagor->Fill(measurement-tdctrigger-magortrigger-sttrigger);
            fpRawCh1PlusSTFitLessMagor->Fill(measurement-tdctrigger-magortrigger+sttrigger);
          }
        }
      }
    }
    if(bmcellid!=-1 && bmcellid!=-1000 && bmcellid!=-1001){//-1000=syncTime, -1=not set
      p_pargeo->GetBMNlvc(bmcellid,plane,view,cell);
      p_datraw->NewHit(bmcellid, plane,view,cell,measurement);
      hitnum++;
      if(FootDebugLevel(3))
        cout<<"BM hit charged : channel="<<channel<<"  tdc2cell="<<bmcellid<<"  measurement="<<measurement<<"  T0="<<p_parcal->GetT0(bmcellid)<<"  triggertime="<<used_trigger<<"  hittime="<<measurement - p_parcal->GetT0(bmcellid)-used_trigger<<endl;
    }else if(bmcellid!=-1000 && bmcellid!=-1001){
      p_datraw->AddDischarged();
      if(FootDebugLevel(3))
        cout<<"BM hit DIScharged: tdc channel="<<channel<<"  measurement="<<measurement<<endl;
    }
  }


   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Evaluate the T0s of each channel from the histograms created in CreateHistogram() and print the results on the terminal
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
