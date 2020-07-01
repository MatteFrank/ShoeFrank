/*!
  \file
  \version $Id: TABMactDatRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TABMactDatRaw.
*/

#include "TDCEvent.hh"

#include "TABMparMap.hxx"
#include "TABMparCon.hxx"
#include "TAGdaqEvent.hxx"
#include "TABMdatRaw.hxx"
#include "TABMrawHit.hxx"
#include "TASTntuRaw.hxx"

#include "TABMactDatRaw.hxx"
#include <iomanip>

/*!
  \class TABMactDatRaw TABMactDatRaw.hxx "TABMactDatRaw.hxx"
  \brief Get Beam Monitor raw data from DAQ. **
*/

ClassImp(TABMactDatRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMactDatRaw::TABMactDatRaw(const char* name,
                             TAGdataDsc* p_datraw,
                             TAGdataDsc* p_datdaq,
                             TAGparaDsc* p_parmap,
                             TAGparaDsc* p_parcon,
                             TAGparaDsc* p_pargeo,
                             TAGdataDsc* p_timraw)
  : TAGaction(name, "TABMactDatRaw - Unpack BM raw data"),
    fpDatRaw(p_datraw),
    fpDatDaq(p_datdaq),
    fpParMap(p_parmap),
    fpParCon(p_parcon),
    fpParGeo(p_pargeo),
    fpTimRaw(p_timraw)
{
  if (FootDebugLevel(1))
    cout<<"TABMactDatRaw::default constructor::Creating the Beam Monitor hit Ntuplizer"<<endl;
  AddDataOut(p_datraw, "TABMdatRaw");
  AddPara(p_parmap, "TABMparMap");
  AddPara(p_parcon, "TABMparCon");
  AddPara(p_pargeo, "TABMparGeo");
  AddDataIn(p_timraw, "TASTntuRaw");
  AddDataIn(p_datdaq, "TAGdaqEvent");
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
  fpRawTdcChannel=new TH1I( "bmDatTdcHitDistribution", "Number of accepted hits x event; Number of hits; Events", p_parmap->GetTdcMaxcha()+1, -0.5, p_parmap->GetTdcMaxcha()+0.5);
  AddHistogram(fpRawTdcChannel);
  fpRawTrigTime=new TH1I( "bmDatTrigger", "Trigger time; Trigger time [ns]; Events", 200, 0, 0);
  AddHistogram(fpRawTrigTime);

  SetValidHistogram(kTRUE);
}



//! Action.
Bool_t TABMactDatRaw::Action() {


   TAGdaqEvent*   p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();
   TASTntuRaw*    p_timraw = (TASTntuRaw*)    fpTimRaw->Object();

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

   // Fill BM_struct with TDCEvent output
   TABMdatRaw*    p_datraw = (TABMdatRaw*)    fpDatRaw->Object();

   //From there we get the Mapping of the wires into the Chamber to the TDC channels
   TABMparMap*    p_parmap = (TABMparMap*)    fpParMap->Object();
   TABMparCon*    p_parcon = (TABMparCon*)    fpParCon->Object();
   TABMparGeo*    p_pargeo = (TABMparGeo*)    fpParGeo->Object();

  Int_t view,plane,cell, channel,up, hitnum=0, discharged=0, bmcellid;
  Double_t used_trigger, measurement;
  for(Int_t i = 0; i < ((int)evt->measurement.size());++i) {
    if(((evt->measurement.at(i)>>19) & 0x7f) == p_parmap->GetTrefCh()){
      if(p_parcon->GetT0Choice()==0){
        used_trigger=(evt->measurement.at(i) & 0x7ffff)/10.;
      }else if (p_parcon->GetT0Choice()==1){
        used_trigger=sttrigger;
      }else if (p_parcon->GetT0Choice()==2){
        used_trigger=(evt->measurement.at(i) & 0x7ffff)/10. + sttrigger;
      }else if (p_parcon->GetT0Choice()==3){
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
    bmcellid=p_parmap->tdc2cell(channel);
    if(ValidHistogram())
      fpRawTdcChannel->Fill(channel);
    if(bmcellid!=-1 && bmcellid!=-1000){//-1000=syncTime, -1=not set
      p_pargeo->GetBMNlvc(bmcellid,plane,view,cell);
      p_datraw->NewHit(bmcellid, plane,view,cell,measurement);
      hitnum++;
      if(FootDebugLevel(3))
        cout<<"BM hit charged : channel="<<channel<<"  tdc2cell="<<bmcellid<<"  measurement="<<measurement<<"  T0="<<p_parcon->GetT0(bmcellid)<<"  triggertime="<<used_trigger<<"  hittime="<<measurement - p_parcon->GetT0(bmcellid)-used_trigger<<"  hittimecut="<<p_parcon->GetHitTimeCut()<<endl;
    }else if(channel!=p_parmap->GetTrefCh()){
      p_datraw->AddDischarged();
      if(FootDebugLevel(3))
        cout<<"BM hit DIScharged: tdc channel="<<channel<<"  measurement="<<measurement<<endl;
    }
  }


   return kTRUE;
}
