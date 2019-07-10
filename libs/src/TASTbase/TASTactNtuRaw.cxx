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

ClassImp(TASTactNtuRaw);

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTactNtuRaw::TASTactNtuRaw(const char* name,
			     TAGdataDsc* p_datraw, 
			     TAGdataDsc* p_nturaw)
  : TAGaction(name, "TASTactNtuRaw - Unpack ST raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw)
{
  AddDataIn(p_datraw, "TASTdatRaw");
  AddDataOut(p_nturaw, "TASTntuRaw");
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

   p_nturaw->SetupClones();

   int nHit = p_datraw->nirhit;
   double TrigTime, Charge, time, q, deltaclk;
   int nvalid(0), ch_num, bo_num;
   delta_clk.clear();
   delta_clk_bo.clear();
   delta_clk_ch.clear();
   for(int ih = 0; ih< nHit; ih++) {
     //Find out the deltas
     ch_num = p_datraw->Hit(ih)->ChID();
     bo_num = p_datraw->Hit(ih)->BoardId();
     time = p_datraw->Hit(ih)->Time();
     if(ch_num == 16 || ch_num == 17) {
       delta_clk.push_back(p_datraw->Hit(ih)->Clocktime());
       delta_clk_bo.push_back(bo_num);
       delta_clk_ch.push_back(ch_num);
     }
   }
   
   for(int ih = 0; ih< nHit; ih++) {
     ch_num = p_datraw->Hit(ih)->ChID();
     bo_num = p_datraw->Hit(ih)->BoardId();
     q =  p_datraw->Hit(ih)->Charge();
     time = p_datraw->Hit(ih)->Time();
     if(delta_clk.size() != 1) {
       deltaclk = find_deltaclock(ch_num,bo_num);
       time -= deltaclk;
     }
     TrigTime+= time;     
     Charge+=q;
     nvalid++;
     p_nturaw->NewHit(ch_num, q, time);
   }

   if(nvalid) TrigTime = TrigTime/(double)nvalid;
   p_nturaw->SetTriggerTime(TrigTime);
   if(delta_clk.size())
     p_nturaw->SetDeltaClk(delta_clk.at(0));
   else {
     p_nturaw->SetDeltaClk(0);
     cout<<" Missing reference clock time:: ERROR!!! CHK CONFIG!! "<<endl;
   } 
   if(nHit>0) p_nturaw->SetTrigType(p_datraw->Hit(0)->TriggerType());
   p_nturaw->SetCharge(Charge);


   if(ValidHistogram()){
     hTrigTime->Fill(TrigTime);
     hTotCharge->Fill(Charge);
     for(int iHit=0;iHit<nHit;iHit++){
       ch_num = p_datraw->Hit(iHit)->ChID();
       if(ch_num>=0 && ch_num<8) hArrivalTime[ch_num]->Fill(TrigTime-p_datraw->Hit(iHit)->Time());
       if(ch_num>=0 && ch_num<8) hAmplitude[ch_num]->Fill(p_datraw->Hit(iHit)->Amplitude());
       if(ch_num>=0 && ch_num<8) hCharge[ch_num]->Fill(p_datraw->Hit(iHit)->Charge());
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

double TASTactNtuRaw::find_deltaclock(int ch_num, int bo_num) {

  double deltaclock = 0;
  return deltaclock;

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
