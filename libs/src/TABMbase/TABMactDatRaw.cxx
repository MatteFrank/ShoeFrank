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
#include "TASTdatRaw.hxx"

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
  AddDataOut(p_datraw, "TABMdatRaw");
  AddPara(p_parmap, "TABMparMap");
  AddPara(p_parcon, "TABMparCon");
  AddPara(p_pargeo, "TABMparGeo");
  AddDataIn(p_timraw, "TASTdatRaw");
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

   fpRawMapX=new TH2I( "BM_Dat_cell_raw_occupancy_2d_x", "Cell occupancy for raw hits; z; x", 11, -5.5, 5.5,7, -3.5,3.5);
   fpRawMapY=new TH2I( "BM_Dat_cell_raw_occupancy_2d_y", "Cell occupancy for raw hits; z; y", 11, -5.5, 5.5,7, -3.5,3.5);
   fpRawHitNum=new TH1I( "BM_Dat_Raw_hit_distribution", "Number of accepted hits x event; Number of hits; Events", 30, 0, 30);
   fpRawDiscAccept=new TH1I( "BM_Dat_Accepted and discharged hits", "Number of hits accepted/discharged in the time cut; -1=discharged 0=accepted; Events", 3, -1, 1);
   for(Int_t i=0;i<77;i++){//necessary to use addbincontent
     fpRawMapX->SetBinContent(i,1);
     fpRawMapY->SetBinContent(i,1);
   }
   AddHistogram(fpRawMapX);   
   AddHistogram(fpRawMapY);   
   AddHistogram(fpRawHitNum);   
   
   SetValidHistogram(kTRUE);
}



//! Action.
Bool_t TABMactDatRaw::Action() {

  if(GetDebugLevel()) { cout<<" Entering the TABMactDatRaw action "<<endl; }
    
   TAGdaqEvent*   p_datdaq = (TAGdaqEvent*)  fpDatDaq->Object();

   Int_t nFragments = p_datdaq->GetFragmentsN();
   
   for (Int_t i = 0; i < nFragments; ++i) {
      
       TString type = p_datdaq->GetClassType(i);
       if (type.Contains("TDCEvent")) {
         const TDCEvent* evt = static_cast<const TDCEvent*> (p_datdaq->GetFragment(i));
         DecodeHits(evt);
       }
   }
   
   fpDatRaw->SetBit(kValid);
   
   return kTRUE;
}
   
//------------------------------------------+-----------------------------------
//! Action.
Bool_t TABMactDatRaw::DecodeHits(const TDCEvent* evt) {
   
   // Fill BM_struct with TDCEvent output
   TABMdatRaw*    p_datraw = (TABMdatRaw*)    fpDatRaw->Object();

   //From there we get the Mapping of the wires into the Chamber to the TDC channels
   TABMparMap*    p_parmap = (TABMparMap*)    fpParMap->Object();
   TABMparCon*    p_parcon = (TABMparCon*)    fpParCon->Object();
   TABMparGeo*    p_pargeo = (TABMparGeo*)    fpParGeo->Object();
   TASTdatRaw*    p_timraw = (TASTdatRaw*)    fpTimRaw->Object();
   
  Int_t view,plane,cell, channel, measurement,up, hitnum=0, discharged=0;
   
   for(Int_t i = 0; i < ((int)evt->measurement.size());i++) {
      measurement=evt->measurement.at(i) & 0x7ffff;
      channel=(evt->measurement.at(i)>>19) & 0x7f;

      if(p_parmap->tdc2cell(channel)>=0 && ((((Double_t) measurement)/10.) - p_parcon->GetT0(p_parmap->tdc2cell(channel))-p_timraw->TrigTime())<p_parcon->GetHitTimecut()){//-1000=syncTime, -1=not set
        p_pargeo->GetBMNlvc(p_parmap->tdc2cell(channel),plane,view,cell);
        p_datraw->SetHitData(p_parmap->tdc2cell(channel), plane,view,cell,((Double_t) (measurement))/10.);
        hitnum++;
        if (ValidHistogram()){
          fpRawDiscAccept->Fill(1);    
          if(view==0){
            up=(plane%2==0) ? 1:0;
            fpRawMapY->AddBinContent(fpRawMapY->GetBin(plane*2+1,cell*2+up+1),1);
            fpRawMapY->AddBinContent(fpRawMapY->GetBin(plane*2+1,cell*2+up+2),1);
          }else{
            up=(plane%2==0) ? 0:1;
            fpRawMapX->AddBinContent(fpRawMapX->GetBin(plane*2+1,cell*2+up+1),1);
            fpRawMapX->AddBinContent(fpRawMapX->GetBin(plane*2+1,cell*2+up+2));
          }  
        }    
        if(p_parcon->GetBMdebug()>10)
          cout<<"BM hit charged: channel="<<channel<<"  tdc2cell="<<p_parmap->tdc2cell(channel)<<"  measurement/10.="<<measurement/10.<<"  T0="<<p_parcon->GetT0(p_parmap->tdc2cell(channel))<<"  triggertime="<<p_timraw->TrigTime()<<"  hittime="<<(((Double_t) measurement)/10.) - p_parcon->GetT0(p_parmap->tdc2cell(channel))-p_timraw->TrigTime()<<"  hittimecut="<<p_parcon->GetHitTimecut()<<endl;
      }else{
        if (ValidHistogram())
          fpRawDiscAccept->Fill(-1);    
        p_datraw->AddDischarged();    
        if(p_parcon->GetBMdebug()>10)
          cout<<"BM hit DIScharged: channel="<<channel<<"  tdc2cell="<<p_parmap->tdc2cell(channel)<<"  measurement/10.="<<measurement/10.<<"  T0="<<p_parcon->GetT0(p_parmap->tdc2cell(channel))<<"  triggertime="<<p_timraw->TrigTime()<<"  hittime="<<(((Double_t) measurement)/10.) - p_parcon->GetT0(p_parmap->tdc2cell(channel))-p_timraw->TrigTime()<<"  hittimecut="<<p_parcon->GetHitTimecut()<<endl;
        }
    }
    if (ValidHistogram())
      fpRawHitNum->Fill(hitnum);    
    
    
   return kTRUE;
}

