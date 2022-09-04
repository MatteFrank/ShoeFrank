/*!
  \file TASTactNtuHit.cxx
  \brief   Implementation of TASTactNtuHit.
*/

#include "TASTparMap.hxx"
#include "TASTactNtuHit.hxx"
#include <TCanvas.h>
#include <unistd.h>
#include "TGraphErrors.h"
#include <stdint.h>

/*!
  \class TASTactNtuHit
  \brief Get ST raw data from WD. **
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
  AddDataIn(p_datraw, "TASTntuRaw");
  AddDataOut(p_nturaw, "TASTntuHit");

}

//------------------------------------------+-----------------------------------
//! Destructor.

TASTactNtuHit::~TASTactNtuHit()
{}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TASTactNtuHit::Action() {

  
   TASTntuRaw*   p_datraw = (TASTntuRaw*) fpDatRaw->Object();
   TASTntuHit*   p_nturaw = (TASTntuHit*)  fpNtuRaw->Object();
   TASTparMap*   p_parmap = (TASTparMap*)  fpParMap->Object();

   p_nturaw->SetupClones();

   double timestamp=0, charge_dep=0, de_dep=0, amp_tot=-10000 , ped=-10000; 

   double thr[8]= {0.030,0.028,0.029,0.028,0.025,0.020,0.025,0.020};

   int neff=6;
   vector<double> vampot;
   
   if(p_datraw->GetSuperHit()){
     if(FootDebugLevel(1))printf("got super hit\n");
     timestamp = p_datraw->GetSuperHit()->GetTime();
     charge_dep =  p_datraw->GetSuperHit()->GetCharge();
     amp_tot= p_datraw->GetSuperHit()->GetAmplitude(); 
     ped= p_datraw->GetSuperHit()->GetPedestal();
     de_dep = -1000.; //calibration missing

     p_nturaw->NewHit(charge_dep, de_dep, timestamp);
     
     p_nturaw->SetTriggerTime(timestamp);
     p_nturaw->SetTriggerTimeOth(p_datraw->GetSuperHit()->GetTimeOth());

     p_nturaw->SetCharge(charge_dep);
     p_nturaw->SetTrigType(p_datraw->GetSuperHit()->GetTriggerType());
     int nHit =  p_datraw->GetHitsN(); 
     if(ValidHistogram()){
       hTime->Fill(timestamp);
       hTotCharge->Fill(charge_dep);
       hTotAmplitude->Fill(amp_tot);
       hPedestal->Fill(ped);
       for(int iHit=0;iHit<nHit;iHit++){
	 Double_t amplitude = p_datraw->GetHit(iHit)->GetAmplitude();
	 Double_t charge= p_datraw->GetHit(iHit)->GetCharge();
	 Double_t time = p_datraw->GetHit(iHit)->GetTime();
	 int iCh  = p_datraw->GetHit(iHit)->GetChID();
	 if(amplitude>thr[iCh])vampot.push_back(iCh);
	 if(iCh<8){
	   hArrivalTime[iCh]->Fill(time-timestamp);
	   hAmplitude[iCh]->Fill(amplitude);
	   hCharge[iCh]->Fill(charge);
	 }
       }
     }   
   }else{
     if(FootDebugLevel(1))printf("super hit missing\n");
   }

   hEff->Fill(8);
   if(vampot.size()==7)hEff->Fill(9);
   if(vampot.size()==8)hEff->Fill(10);
   if(vampot.size()>=neff){
     for(int ich=0;ich<8;ich++){
       if(find(vampot.begin(), vampot.end(), ich) != vampot.end()){
	 hEff->Fill(ich);
       }
     }
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

  c.Print(Form("waveform_ch%d_nev%d.png", myHit->GetChID(), fEventsN));
  
}






void TASTactNtuHit::CreateHistogram(){

  DeleteHistogram();

  char histoname[100]="";
  if(FootDebugLevel(1))
     cout<<"I have created the ST histo. "<<endl;

  
  sprintf(histoname,"stTime");
  hTime = new TH1F(histoname, histoname, 256, 0., 256.);
  AddHistogram(hTime);

  sprintf(histoname,"stTotCharge");
  hTotCharge = new TH1F(histoname, histoname, 1100, -0.1, 10.9);
  AddHistogram(hTotCharge);

  sprintf(histoname,"stTotAmplitude"); 
  hTotAmplitude = new TH1F(histoname, histoname, 100, -0.1, 10.9);
  AddHistogram(hTotAmplitude);

  sprintf(histoname,"stTotPedestal"); 
  hPedestal = new TH1F(histoname, histoname, 200, -5, 5);
  AddHistogram(hPedestal);


  sprintf(histoname,"stEff"); 
  hEff = new TH1F(histoname, histoname, 11, -0.5, 10.5);
  AddHistogram(hEff);


  for(int iCh=0;iCh<8;iCh++){
    sprintf(histoname,"stTime_ch%d", iCh);
    hArrivalTime[iCh]= new TH1F(histoname, histoname, 100, -2., 2.);
    AddHistogram(hArrivalTime[iCh]);

    sprintf(histoname,"stCharge_ch%d", iCh);
    hCharge[iCh]= new TH1F(histoname, histoname, 200, -0.1, 4.9);
    AddHistogram(hCharge[iCh]);

    sprintf(histoname,"stAmp_ch%d", iCh);
    hAmplitude[iCh]= new TH1F(histoname, histoname, 120, -0.1, 1.1);
    AddHistogram(hAmplitude[iCh]);
  }





  
  SetValidHistogram(kTRUE);
  
}
