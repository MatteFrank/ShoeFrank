/*!
  \file
  \version $Id: TASTactNtuRaw.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TASTactNtuRaw.
*/

#include "TASTparMap.hxx"
#include "TASTactNtuRaw.hxx"
#include <TCanvas.h>
#include <unistd.h>
#include "TGraphErrors.h"
#include <stdint.h>

/*!
  \class TASTactNtuRaw TASTactNtuRaw.hxx "TASTactNtuRaw.hxx"
  \brief Get Beam Monitor raw data from WD. **
*/

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTactNtuRaw::TASTactNtuRaw(const char* name,
			     TAGdataDsc* p_datraw, 
			     TAGdataDsc* p_nturaw,
			     TAGparaDsc* p_parmap)
  : TAGaction(name, "TASTactNtuRaw - Unpack ST raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw),
    fpParMap(p_parmap)
{
  AddDataIn(p_datraw, "TASTdatRaw");
  AddDataOut(p_nturaw, "TASTntuRaw");

  m_debug = GetDebugLevel();
  

}

//------------------------------------------+-----------------------------------
//! Destructor.

TASTactNtuRaw::~TASTactNtuRaw()
{}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TASTactNtuRaw::Action() {

   TASTdatRaw*    p_datraw = (TASTdatRaw*)   fpDatRaw->Object();
   TASTntuRaw*   p_nturaw = (TASTntuRaw*)  fpNtuRaw->Object();
   TASTparMap*   p_parmap = (TASTparMap*)  fpParMap->Object();

   p_nturaw->SetupClones();

   clktime_map.clear();
 
   int nHit = p_datraw->GetHitsN();
   double TrigTime, Charge, time, q, clktime=0;
   int nvalid(0), ch_num, bo_num;
   for(int ih = 0; ih< nHit; ih++) {
     //Find out the deltas
     ch_num = p_datraw->GetHit(ih)->ChID();
     bo_num = p_datraw->GetHit(ih)->BoardId();
     time = p_datraw->GetHit(ih)->Time();
     if(ch_num == 16 || ch_num == 17) {
       clktime_map[make_pair(bo_num, ch_num)] = p_datraw->GetHit(ih)->Clocktime();
     }
   }


   
   double clktime_ref =0;
   if(clocktimeIsSet(REF_CLK.first, REF_CLK.second)){
     clktime_ref=find_clocktime(REF_CLK.first, REF_CLK.second);
   }else{
     printf("warning::reference clock not found for the ST\n");
   }


   double deltaclk=0;
   double norm=0;
   for(int ih = 0; ih< nHit; ih++) {
     ch_num = p_datraw->GetHit(ih)->ChID();
     bo_num = p_datraw->GetHit(ih)->BoardId();
     q =  p_datraw->GetHit(ih)->Charge();
     time = p_datraw->GetHit(ih)->Time();

     if(ch_num != 16 && ch_num != 17) {

       pair<int,int> clk_channel_bo = p_parmap->GetClockChannel(ch_num, bo_num);
       if(clocktimeIsSet(clk_channel_bo.first, clk_channel_bo.second)){
	 clktime=find_clocktime(clk_channel_bo.first, clk_channel_bo.second);
	 deltaclk = clktime-clktime_ref;
       }else{
	 printf("warning::clk not found for wavedream channel %d, board::%df\n", ch_num, bo_num);
       }
       time -= clktime + deltaclk;
       
       //here I select only waveforms with signals above a threshold, and with a correct chisquare
       if(p_datraw->GetHit(ih)->GetChiSquare()< CHISQUARE_THRESHOLD && p_datraw->GetHit(ih)->Amplitude() > AMPLITUDE_THRESHOLD){

	 double weight = p_parmap->GetChannelWeight(ch_num, bo_num);
	 TrigTime+= time*weight;
	 nvalid++;
	 norm+=weight;
       }
       Charge+=q;
       
       p_nturaw->NewHit(ch_num, q, time);

     }
   }
   
   //   if(nvalid) TrigTime = TrigTime/(double)nvalid;
   if(nvalid) TrigTime = TrigTime/norm;

   p_nturaw->SetTriggerTime(TrigTime);
   p_nturaw->SetDeltaClk(deltaclk);
   if(nHit>0) p_nturaw->SetTrigType(p_datraw->GetHit(0)->TriggerType());
   p_nturaw->SetCharge(Charge);


   if(ValidHistogram()){
     hTrigTime->Fill(TrigTime);
     hTotCharge->Fill(Charge);
     for(int iHit=0;iHit<nHit;iHit++){
       ch_num = p_datraw->GetHit(iHit)->ChID();
       if(ch_num>=0 && ch_num<8) hArrivalTime[ch_num]->Fill(TrigTime-p_datraw->GetHit(iHit)->Time());
       if(ch_num>=0 && ch_num<8) hAmplitude[ch_num]->Fill(p_datraw->GetHit(iHit)->Amplitude());
       if(ch_num>=0 && ch_num<8) hCharge[ch_num]->Fill(p_datraw->GetHit(iHit)->Charge());
     }
   }

   
   
   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}

void TASTactNtuRaw::SavePlot(TGraph WaveGraph, TF1 fun1, TF1 fun2, TASTrawHit *myHit){


  TCanvas c("c","",600,600);
  c.cd();

  WaveGraph.Draw("APL");
  WaveGraph.SetMarkerSize(0.5);
  WaveGraph.SetMarkerStyle(22);
  WaveGraph.SetMarkerColor(kBlue);
  WaveGraph.GetXaxis()->SetRangeUser(0,100);

  fun1.SetLineColor(kRed);
  fun2.SetLineColor(kGreen);

  fun1.SetNpx(10000);
  fun2.SetNpx(10000);
  
  fun1.Draw("same");
  fun2.Draw("same");

  c.Print(Form("waveform_ch%d_nev%d.png", myHit->ChID(), m_nev));
  
}


double TASTactNtuRaw::find_clocktime(int ch_num, int bo_num) {


  if(m_debug)printf("looking for clocktime for board::%d   channel::%d\n", bo_num, ch_num);
  return clktime_map.find(make_pair(bo_num, ch_num))->second;

}



bool TASTactNtuRaw::clocktimeIsSet(int ch_num, int bo_num) {

  if(m_debug)printf("looking for clocktime definition for board::%d   channel::%d   count::%d\n", bo_num, ch_num, clktime_map.count(make_pair(bo_num, ch_num)));

  return (bool)clktime_map.count(make_pair(bo_num, ch_num));

}




void TASTactNtuRaw::CreateHistogram(){

  DeleteHistogram();

  char histoname[100]="";
  if (fDebugLevel > 0)
     cout<<"I have created the ST histo. "<<endl;

  sprintf(histoname,"stEvtTime");
  hEventTime = new TH1F(histoname, histoname, 6000, 0., 60.);
  AddHistogram(hEventTime);

  sprintf(histoname,"stTrigTime");
  hTrigTime = new TH1F(histoname, histoname, 256, 0., 256.);
  AddHistogram(hTrigTime);

  sprintf(histoname,"stTotCharge");
  hTotCharge = new TH1F(histoname, histoname, 400, -0.1, 3.9);
  AddHistogram(hTotCharge);
  
  for(int iCh=0;iCh<8;iCh++){
    sprintf(histoname,"stDeltaTime_ch%d", iCh);
    hArrivalTime[iCh]= new TH1F(histoname, histoname, 100, -5., 5.);
    AddHistogram(hArrivalTime[iCh]);

    sprintf(histoname,"stCharge_ch%d", iCh);
    hCharge[iCh]= new TH1F(histoname, histoname, 200, -0.1, 1.9);
    AddHistogram(hCharge[iCh]);

    sprintf(histoname,"stMaxAmp_ch%d", iCh);
    hAmplitude[iCh]= new TH1F(histoname, histoname, 120, -0.1, 1.1);
    AddHistogram(hAmplitude[iCh]);
  }

  SetValidHistogram(kTRUE);
  
}
