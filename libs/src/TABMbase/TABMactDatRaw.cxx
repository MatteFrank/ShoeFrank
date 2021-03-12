/*!
  \file
  \version $Id: TABMactDatRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TABMactDatRaw.
*/


#include "TABMactDatRaw.hxx"

/*!
  \class TABMactDatRaw TABMactDatRaw.hxx "TABMactDatRaw.hxx"
  \brief Get Beam Monitor raw data from DAQ. **
*/

ClassImp(TABMactDatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactDatRaw::TABMactDatRaw(const char* name,
                             TAGdataDsc* dscdatraw,
                             TAGdataDsc* dscdatdaq,
                             TAGparaDsc* dscparmap,
                             TAGparaDsc* dscparcal,
                             TAGparaDsc* dscpargeo,
                             TAGdataDsc* dsctimraw)
  : TAGaction(name, "TABMactDatRaw - Unpack BM raw data"),
    fpDatRaw(dscdatraw),
    fpDatDaq(dscdatdaq),
    fpParMap(dscparmap),
    fpParCal(dscparcal),
    fpParGeo(dscpargeo),
    fpTimRaw(dsctimraw)
{
  if (FootDebugLevel(1))
    cout<<"TABMactDatRaw::default constructor::Creating the Beam Monitor hit Ntuplizer"<<endl;
  AddDataOut(dscdatraw, "TABMntuRaw");
  AddPara(dscparmap, "TABMparMap");
  AddPara(dscparcal, "TABMparCal");
  AddPara(dscpargeo, "TABMparGeo");
  AddDataIn(dsctimraw, "TASTntuHit");
  AddDataIn(dscdatdaq, "TAGdaqEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TABMactDatRaw::~TABMactDatRaw()
{
}

//------------------------------------------+-----------------------------------

void TABMactDatRaw::CreateHistogram(){

  DeleteHistogram();
  TABMparMap*    p_parmap = (TABMparMap*)    fpParMap->Object();
  // fpRawMapX=new TH2I( "BM_Dat_cell_raw_occupancy_2d_x", "Cell occupancy for raw hits; z; x", 11, -5.5, 5.5,7, -3.5,3.5);
  // AddHistogram(fpRawMapX);
  // fpRawMapY=new TH2I( "BM_Dat_cell_raw_occupancy_2d_y", "Cell occupancy for raw hits; z; y", 11, -5.5, 5.5,7, -3.5,3.5);
  // AddHistogram(fpRawMapY);
  fpRawTdcChannel=new TH1I( "bmDatTdcHitDistribution", "Number of hits x channel (-1=error); TDC channel; Number of hits", p_parmap->GetTdcMaxcha()+2, -1.5, p_parmap->GetTdcMaxcha()+0.5);
  AddHistogram(fpRawTdcChannel);
  fpRawTrigTime=new TH1I( "bmDatTrigger", "Trigger time; Trigger time [ns]; Events", 200, 0, 0);
  AddHistogram(fpRawTrigTime);
  TH1I *RawTdcPlot;
  for(Int_t i=0;i<p_parmap->GetTdcMaxcha();i++){
    TString title="bmDatTdcCha_";
    title+=i;
    RawTdcPlot=new TH1I( title.Data(), "Time;Time [ns]; Events", 3001, -1000.5, 2000.5);
    AddHistogram(RawTdcPlot);
    fpRawTdcMeas.push_back(RawTdcPlot);
  }

  SetValidHistogram(kTRUE);
}



//! Action.
Bool_t TABMactDatRaw::Action() {


   TAGdaqEvent*   p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();
   TASTntuHit*    p_timraw = (TASTntuHit*)    fpTimRaw->Object();

   Int_t nFragments = p_datdaq->GetFragmentsN();

  if(FootDebugLevel(1))
    cout<<"TABMactDatRaw::Action():: I'm going to charge "<<nFragments<<" number of fragments"<<endl;

  if(p_timraw->GetTriggerTime()<0.0001){//No start counter trigger time! --> No BM hits
    if(FootDebugLevel(1))
      cout<<"TABMactDatRaw::Action():: No ST trigger time --> no BM measurements for this event"<<endl;
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
    cout<<"TABMactDatRaw::Action():: done"<<endl;

   return kTRUE;
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactDatRaw::DecodeHits(const TDCEvent* evt, const double sttrigger) {

   TABMntuRaw*    p_datraw = (TABMntuRaw*)    fpDatRaw->Object();
   TABMparMap*    p_parmap = (TABMparMap*)    fpParMap->Object();
   TABMparCal*    p_parcal = (TABMparCal*)    fpParCal->Object();
   TABMparGeo*    p_pargeo = (TABMparGeo*)    fpParGeo->Object();

   //From there we get the Mapping of the wires into the Chamber to the TDC channels
  Int_t view,plane,cell, channel,up, hitnum=0, discharged=0, bmcellid;
  Double_t used_trigger, measurement;
  for(Int_t i = 0; i < ((int)evt->measurement.size());++i) {
    if(((evt->measurement.at(i)>>19) & 0x7f) == p_parmap->GetTrefCh()){
      if(p_parcal->GetT0Choice()==0){
        used_trigger=(evt->measurement.at(i) & 0x7ffff)/10.;
      }else if (p_parcal->GetT0Choice()==1){
        used_trigger=sttrigger;
      }else if (p_parcal->GetT0Choice()==2){
        used_trigger=(evt->measurement.at(i) & 0x7ffff)/10. + sttrigger;
      }else if (p_parcal->GetT0Choice()==3){
        used_trigger=(evt->measurement.at(i) & 0x7ffff)/10. - sttrigger;
      }
      break;
    }
  }

  p_datraw->SetTrigtime(used_trigger);
  if (ValidHistogram())
    fpRawTrigTime->Fill(used_trigger);
  for(Int_t i = 0; i < ((int)evt->measurement.size());i++) {
    measurement=(Double_t) (evt->measurement.at(i) & 0x7ffff)/10.;
    channel=(evt->measurement.at(i)>>19) & 0x7f;
    if(channel>p_parmap->GetTdcMaxcha() || channel<0){
      cout<<"TABMactDatRaw::ERROR!!!!!!!   channel="<<channel<<"  TDC maximum number of channel="<<p_parmap->GetTdcMaxcha()<<endl;
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
    }
    if(bmcellid!=-1 && bmcellid!=-1000){//-1000=syncTime, -1=not set
      p_pargeo->GetBMNlvc(bmcellid,plane,view,cell);
      p_datraw->NewHit(bmcellid, plane,view,cell,measurement);
      hitnum++;
      if(FootDebugLevel(3))
        cout<<"BM hit charged : channel="<<channel<<"  tdc2cell="<<bmcellid<<"  measurement="<<measurement<<"  T0="<<p_parcal->GetT0(bmcellid)<<"  triggertime="<<used_trigger<<"  hittime="<<measurement - p_parcal->GetT0(bmcellid)-used_trigger<<endl;
    }else if(channel!=p_parmap->GetTrefCh()){
      p_datraw->AddDischarged();
      if(FootDebugLevel(3))
        cout<<"BM hit DIScharged: tdc channel="<<channel<<"  measurement="<<measurement<<endl;
    }
  }


   return kTRUE;
}
