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

//! Class Imp
ClassImp(TASTactNtuHit);



//------------------------------------------+-----------------------------------
//! Default constructor.

TASTactNtuHit::TASTactNtuHit(const char* name,
			     TAGdataDsc* p_datraw, 
			     TAGdataDsc* p_nturaw,
			     TAGparaDsc* p_parmap,
			     TAGparaDsc* p_parconf)
  : TAGaction(name, "TASTactNtuHit - Unpack ST raw data"),
    fpDatRaw(p_datraw),
    fpNtuRaw(p_nturaw),
    fpParMap(p_parmap),
    fpParConf(p_parconf)
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
Bool_t TASTactNtuHit::Action()
{
   TASTntuRaw*   p_datraw = (TASTntuRaw*) fpDatRaw->Object();
   TASTntuHit*   p_nturaw = (TASTntuHit*)  fpNtuRaw->Object();
   TASTparMap*   p_parmap = (TASTparMap*)  fpParMap->Object();
   TASTparConf*  p_parconf = (TASTparConf*)  fpParConf->Object();

   p_nturaw->SetupClones();

   double timestamp=0, charge_dep=0, de_dep=0, amp_tot=-10000 , ped=-10000, bas=-1000, ris=-100; 
  
   double thr[8]= {0.030,0.028,0.029,0.028,0.025,0.020,0.025,0.020};

   int neff=6;
   vector<double> vampot;
   
   if(p_datraw->GetSuperHit()){
      vector<double> noi =  p_datraw->GetSuperHit()->GetNoise();
      if(FootDebugLevel(1))printf("got super hit\n");

      timestamp = p_datraw->GetSuperHit()->GetTime();
      charge_dep =  p_datraw->GetSuperHit()->GetCharge();
      amp_tot= p_datraw->GetSuperHit()->GetAmplitude();
      ped = p_datraw->GetSuperHit()->GetPedestal();
      bas = p_datraw->GetSuperHit()->GetBaseline();
      ris = p_datraw->GetSuperHit()->GetRiseTime();
      de_dep = -1000.; //calibration missing



      if(p_datraw->GetSuperHit()->IsFFTValid()){
	for(int i=0;i<p_datraw->GetSuperHit()->GetFFT("MAG").size();i++){
	
	  double old = hFFT->GetBinContent(i+1);
	  hFFT->SetBinContent(i+1,old+p_datraw->GetSuperHit()->GetFFT("MAG").at(i));

	  double oldre = hFFTre->GetBinContent(i+1);
	  hFFTre->SetBinContent(i+1,oldre+p_datraw->GetSuperHit()->GetFFT("RE").at(i));

	  double oldim = hFFTim->GetBinContent(i+1);
	  hFFTim->SetBinContent(i+1,oldim+p_datraw->GetSuperHit()->GetFFT("IM").at(i));

	  if(p_parconf->ApplyFFT()){
	    double oldf = hFFTfilter->GetBinContent(i+1);
	    hFFTfilter->SetBinContent(i+1,oldf+p_datraw->GetSuperHit()->GetFFTfilter("MAG").at(i));
	  }
	}
      }

      
      int nHit =  p_datraw->GetHitsN();

      if(nHit<8)cout << "Warning, ST channels < 8, probably zero suppression disabled" << endl;
	
      double timeST_oth=0,norm=0;
      for(int iHit=0;iHit<nHit;iHit++){
	Double_t amplitude = p_datraw->GetHit(iHit)->GetAmplitude();
	Double_t charge= p_datraw->GetHit(iHit)->GetCharge();
	Double_t ped= p_datraw->GetHit(iHit)->GetPedestal();
	Double_t time = p_datraw->GetHit(iHit)->GetTime();
	Double_t baseline= p_datraw->GetHit(iHit)->GetBaseline();
	vector<Double_t> sidevalues = p_datraw->GetHit(iHit)->GetNoise();
	Int_t iCh  = p_datraw->GetHit(iHit)->GetChID();
	Double_t timeoth =0; //for resolution 
	Int_t noth=0;
	Double_t risetime =  p_datraw->GetHit(iHit)->GetRiseTime();
	timeST_oth+=(time*(amplitude/risetime));
	norm+=(amplitude/risetime);
	for(Int_t jHit=0;jHit<nHit;jHit++){
	  Int_t jCh  = p_datraw->GetHit(jHit)->GetChID();
	  if(iCh==jCh)continue;
	  timeoth+=p_datraw->GetHit(jHit)->GetTime();
	  noth++;
	}
	timeoth/=(Double_t)noth;
	if(iCh<8){
	  if(ValidHistogram()){
	    hArrivalTime[iCh]->Fill(time-timeoth);
	    hAmplitude[iCh]->Fill(amplitude);
	    hCharge[iCh]->Fill(charge);
	    hChargePedestal[iCh]->Fill(ped);
	    hBaseline[iCh]->Fill(baseline);
	    hRiseTime[iCh]->Fill(risetime);
	    for(int isa=0;isa<sidevalues.size();isa++){
	      hNoise[iCh]->Fill(baseline-sidevalues.at(isa));
	    }
	  }
	}
      }

      //      cout <<"time1::"<< timestamp;
      // timestamp = timeST_oth/norm;
      // cout <<"time2::"<< timestamp << endl ;
      p_nturaw->NewHit(charge_dep, de_dep, timestamp, p_datraw->GetSuperHit()->GetPileUp() );
      p_nturaw->SetTriggerTime(timestamp);
      p_nturaw->SetTriggerTimeOth(p_datraw->GetSuperHit()->GetTimeOth());
      p_nturaw->SetCharge(charge_dep);
      p_nturaw->SetTrigType(p_datraw->GetSuperHit()->GetTriggerType());
      p_nturaw->SetRiseTime(p_datraw->GetSuperHit()->GetRiseTime());


      if(ValidHistogram()){
	hTotTime->Fill(timestamp);
	hTotCharge->Fill(charge_dep);
	hTotAmplitude->Fill(amp_tot);
	hTotPedestal->Fill(ped);
	hTotBaseline->Fill(bas);
	hTotRiseTime->Fill(p_datraw->GetSuperHit()->GetRiseTime());
	for(int isa=0;isa<noi.size();isa++){
	  hTotNoise->Fill(bas-noi.at(isa));
	  hTotTimeNoise->Fill(timestamp - isa*0.2,fabs(noi.at(isa)));
	}
	hChargevsTime[0]-> Fill(timestamp,charge_dep ); //info di superhit

	//info di superhit - no pileup derivative mode
	if ( p_nturaw->GetHit(0)->GetPileUp() == false ) hChargevsTime[1]-> Fill(timestamp,charge_dep ); 
       
	//info di superhit - no pileup charge mode
	if ( charge_dep < 2. ) hChargevsTime[2]-> Fill(timestamp,charge_dep ); 
	
	hPileUp->Fill(p_nturaw->GetHit(0)->GetPileUp());
      }
   }else{
      if(FootDebugLevel(1))printf("super hit missing\n");
   }
     
   fpNtuRaw->SetBit(kValid);
   return kTRUE;
}

//------------------------------------------+-----------------------------------
void TASTactNtuHit::SavePlot(TGraph WaveGraph, TF1 fun1, TF1 fun2, TASTrawHit *myHit)
{
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

//------------------------------------------+-----------------------------------
void TASTactNtuHit::CreateHistogram()
{
  DeleteHistogram();


  TASTntuRaw*   p_datraw = (TASTntuRaw*) fpDatRaw->Object();
  
  char histoname[100]="";
  if(FootDebugLevel(1))
     cout<<"I have created the ST histo. "<<endl;

  
  strcpy(histoname,"stTotTime");
  hTotTime = new TH1F(histoname, histoname, 256, 0., 256.);
  AddHistogram(hTotTime);

  strcpy(histoname,"stTotCharge");
  hTotCharge = new TH1F(histoname, histoname, 1100, -0.1, 40.);
  AddHistogram(hTotCharge);

  strcpy(histoname,"stTotAmplitude");
  hTotAmplitude = new TH1F(histoname, histoname, 100, -0.1, 10.9);
  AddHistogram(hTotAmplitude);

  strcpy (histoname,"stTotPedestal");
  hTotPedestal = new TH1F(histoname, histoname, 200, -5, 5);
  AddHistogram(hTotPedestal);

  strcpy (histoname,"stTotBaseline");
  hTotBaseline = new TH1F(histoname, histoname, 1000, -0.5, 0.5);
  AddHistogram(hTotBaseline);

  strcpy (histoname,"stTotNoise");
  hTotNoise = new TH1F(histoname, histoname, 400, -0.4, 0.4);
  AddHistogram(hTotNoise);

  strcpy (histoname,"stTotTimeNoise");
  hTotTimeNoise = new TH1F(histoname, histoname, 10000, 0, 1000);
  AddHistogram(hTotTimeNoise);

  strcpy (histoname,"stTotRiseTime");
  hTotRiseTime = new TH1F(histoname, histoname, 1000, 0, 10);
  AddHistogram(hTotRiseTime);


  strcpy (histoname,"stFFT");
  hFFT = new TH1F(histoname, histoname, NSAMPLES, 0, 3.0);
  AddHistogram(hFFT);

  strcpy (histoname,"stFFTre");
  hFFTre = new TH1F(histoname, histoname, NSAMPLES, 0, 3.0);
  AddHistogram(hFFTre);

  strcpy (histoname,"stFFTim");
  hFFTim = new TH1F(histoname, histoname, NSAMPLES, 0, 3.0);
  AddHistogram(hFFTim);

  strcpy (histoname,"stFFTfiltered");
  hFFTfilter = new TH1F(histoname, histoname, NSAMPLES, 0, 3.0);
  AddHistogram(hFFTfilter);

  strcpy(histoname,"stEff");
  hEff = new TH1F(histoname, histoname, 11, -0.5, 10.5);
  AddHistogram(hEff);


  strcpy(histoname,"stTotChargevsTotTime");
  hChargevsTime[0] = new TH2F(histoname, histoname,  256, 0., 256., 1100, -0.1, 40.);
  AddHistogram(hChargevsTime[0]);

  strcpy(histoname,"stTotChargevsTotTime-NOPILEUP-DerivativeMode");
  hChargevsTime[1] = new TH2F(histoname, histoname,  256, 0., 256., 1100, -0.1, 40.);
  AddHistogram(hChargevsTime[1]);

  strcpy(histoname,"stTotChargevsTotTime-NOPILEUP-ChargeMode");
  hChargevsTime[2] = new TH2F(histoname, histoname,  256, 0., 256., 1100, -0.1, 40.);
  AddHistogram(hChargevsTime[2]);

  strcpy(histoname,"pileup");
  hPileUp = new TH1F(histoname, histoname, 2, 0., 2.);
  AddHistogram(hPileUp);


  for(int iCh=0;iCh<8;iCh++){
    hArrivalTime[iCh]= new TH1F(Form("stTime_ch%d", iCh), Form("stTime_ch%d", iCh), 100, -2., 2.);
    AddHistogram(hArrivalTime[iCh]);

    hCharge[iCh]= new TH1F(Form("stCharge_ch%d", iCh), Form("stCharge_ch%d", iCh), 1000, -9.999, 10.001);
    AddHistogram(hCharge[iCh]);

    hAmplitude[iCh]= new TH1F(Form("stAmp_ch%d", iCh), Form("stAmp_ch%d", iCh), 120, -0.1, 1.1);
    AddHistogram(hAmplitude[iCh]);

    hBaseline[iCh]= new TH1F(Form("stBaseline_ch%d", iCh), Form("stBaseline_ch%d", iCh), 1000, -0.5, -0.5);
    AddHistogram(hBaseline[iCh]);

    hChargePedestal[iCh]= new TH1F(Form("stPedestal_ch%d", iCh), Form("stPedestal_ch%d", iCh), 1000, -9.999, 10.001);
    AddHistogram(hChargePedestal[iCh]);

    hNoise[iCh]= new TH1F(Form("stNoise_ch%d", iCh), Form("stNoise_ch%d", iCh), 100, -0.1, 0.1);
    AddHistogram(hNoise[iCh]);

    hRiseTime[iCh]= new TH1F(Form("stRiseTime_ch%d", iCh), Form("stRiseTime_ch%d", iCh), 1000, 0, 10);
    AddHistogram(hRiseTime[iCh]);
  }

  
  SetValidHistogram(kTRUE);
}
