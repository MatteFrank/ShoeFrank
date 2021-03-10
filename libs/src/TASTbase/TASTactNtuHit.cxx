/*!
  \file
  \version $Id: TASTactNtuHit.cxx,v 1.5 2003/06/22 10:35:47 mueller Exp $
  \brief   Implementation of TASTactNtuHit.
*/

#include "TASTparMap.hxx"
#include "TASTactNtuHit.hxx"
#include <TCanvas.h>
#include <unistd.h>
#include "TGraphErrors.h"
#include <stdint.h>

/*!
  \class TASTactNtuHit TASTactNtuHit.hxx "TASTactNtuHit.hxx"
  \brief Get Beam Monitor raw data from WD. **
*/

ClassImp(TASTactNtuHit);

//------------------------------------------+-----------------------------------
//! Default constructor.

TASTactNtuHit::TASTactNtuHit(const char* name,
			     TAGdataDsc* p_datraw, 
			     TAGdataDsc* p_nturaw,
			     TAGparaDsc* p_parmap)
  : TAGaction(name, "TASTactNtuHit - Unpack ST raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw),
    fpParMap(p_parmap)
{
  AddDataIn(p_datraw, "TASTdatRaw");
  AddDataOut(p_nturaw, "TASTntuHit");

}

//------------------------------------------+-----------------------------------
//! Destructor.

TASTactNtuHit::~TASTactNtuHit()
{}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TASTactNtuHit::Action() {

   TASTdatRaw*   p_datraw = (TASTdatRaw*) fpDatRaw->Object();
   TASTntuHit*   p_nturaw = (TASTntuHit*)  fpNtuRaw->Object();
   TASTparMap*   p_parmap = (TASTparMap*)  fpParMap->Object();

   p_nturaw->SetupClones();

   double timestamp=0, charge_dep=0, de_dep=0; 
   
   if(p_datraw->GetSuperHit()){

     timestamp = p_datraw->GetSuperHit()->GetTime();
     charge_dep =  p_datraw->GetSuperHit()->GetCharge();
     de_dep = -1000.; //calibration missing

     p_nturaw->NewHit(charge_dep, de_dep, timestamp);

     p_nturaw->SetTriggerTime(timestamp);
     p_nturaw->SetTriggerTimeOth(p_datraw->GetSuperHit()->GetTimeOth());
     p_nturaw->SetCharge(charge_dep);
     p_nturaw->SetTrigType(p_datraw->GetSuperHit()->GetTriggerType());
   }
   
   fpNtuRaw->SetBit(kValid);
   return kTRUE;



}

void TASTactNtuHit::SavePlot(TGraph WaveGraph, TF1 fun1, TF1 fun2, TASTrawHit *myHit){


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

  c.Print(Form("waveform_ch%d_nev%d.png", myHit->GetChID(), m_nev));
  
}






void TASTactNtuHit::CreateHistogram(){

  DeleteHistogram();

  char histoname[100]="";
  if(FootDebugLevel(1))
     cout<<"I have created the ST histo. "<<endl;

  // sprintf(histoname,"stEvtTime");
  // hEventTime = new TH1F(histoname, histoname, 6000, 0., 60.);
  // AddHistogram(hEventTime);

  // sprintf(histoname,"stTrigTime");
  // hTrigTime = new TH1F(histoname, histoname, 256, 0., 256.);
  // AddHistogram(hTrigTime);

  // sprintf(histoname,"stTotCharge");
  // hTotCharge = new TH1F(histoname, histoname, 400, -0.1, 3.9);
  // AddHistogram(hTotCharge);
  
  // for(int iCh=0;iCh<8;iCh++){
  //   sprintf(histoname,"stDeltaTime_ch%d", iCh);
  //   hArrivalTime[iCh]= new TH1F(histoname, histoname, 100, -5., 5.);
  //   AddHistogram(hArrivalTime[iCh]);

  //   sprintf(histoname,"stCharge_ch%d", iCh);
  //   hCharge[iCh]= new TH1F(histoname, histoname, 200, -0.1, 1.9);
  //   AddHistogram(hCharge[iCh]);

  //   sprintf(histoname,"stMaxAmp_ch%d", iCh);
  //   hAmplitude[iCh]= new TH1F(histoname, histoname, 120, -0.1, 1.1);
  //   AddHistogram(hAmplitude[iCh]);
  // }

  SetValidHistogram(kTRUE);
  
}
