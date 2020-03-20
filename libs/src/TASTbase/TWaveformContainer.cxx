#include "TWaveformContainer.hxx"
#include "TAxis.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include <vector>

TWaveformContainer::TWaveformContainer(){
  fVectT.clear();
  fVectRawT.clear();
  fVectA.clear();
  fIsEmptyFlag = true;
  fHighlevelDebug = false; //it can be set only hard-coding this variable
  Clear();
  fNEvent=0;
}


void TWaveformContainer::Clear(){
  fChannelId=-1000;
  fBoardId=-1000;
  fTriggerCellId=-1000;
  fTrigType=-1000;
  fVectT.clear();
  fVectRawT.clear();
  fVectA.clear();
  fIsEmptyFlag = true;
  fNEvent=0;
}


TWaveformContainer::TWaveformContainer(const TWaveformContainer &other){

  fChannelId=other.fChannelId;
  fBoardId=other.fBoardId;
  fTriggerCellId = other.fTriggerCellId;
  fTrigType = other.fTrigType;
  fVectT = other.fVectT;
  fVectRawT = other.fVectRawT;
  fVectA = other.fVectA;
  fIsEmptyFlag = false;
}



// DEBUG ONLY
void TWaveformContainer::PlotWaveForm(int i){

  Double_t max=fVectA.at(0);
  Double_t min=fVectA.at(0);;
  for(int bin=0;bin<WAVEFORMBINS;++bin){
    if (fVectA.at(bin)>max){
      max=fVectA.at(bin);
    }
    if (fVectA.at(bin)<min){
      min=fVectA.at(bin);
    }
  }
  std::cout << " max " << max << " min "<< min <<std::endl;
  TCanvas *c = new TCanvas;
  c->Range(fVectT.at(0),min,fVectA.at(WAVEFORMBINS-1),max);
  TGraph *g=new TGraph(WAVEFORMBINS,&fVectT[0],&fVectA[0]);
  g->GetXaxis()->SetTitle("t (ns)");
  g->GetYaxis()->SetTitle("Amplitude (V)");
  g->SetTitle(TString::Format(" Board %d Channel %d",fBoardId,fChannelId));
  g->Draw("AC");
  c->SaveAs(TString::Format("Waveform_Event%d_Board%d_Channel%d.pdf",i, fBoardId,fChannelId));
  c->Close();
  gSystem->ProcessEvents();
}



// DEBUG ONLY
void TWaveformContainer::GraphWaveForm(TH1F *wv0){
  double min = TMath::MinElement(WAVEFORMBINS,&fVectT[0]);
  double max = TMath::MaxElement(WAVEFORMBINS,&fVectT[0]);
  wv0->SetBins(WAVEFORMBINS,min,max);
  for (int bin=0;bin<WAVEFORMBINS;++bin){
    wv0->SetBinContent(bin+1,fVectT.at(bin));
  }
  wv0->GetXaxis()->SetTitle("t (s)");
  wv0->GetYaxis()->SetTitle("Amplitude (V)");
  wv0->SetTitle(TString::Format(" Board %d Channel %d",fBoardId,fChannelId));
  return ; 
}




void TWaveformContainer::SanitizeWaveform(){
  Double_t old=fVectA.at(WAVEFORMBINS-1);
  if (old<-0.8){
    old+=1;
    fVectA.at(WAVEFORMBINS-1)+=1;
  }
  for (int bin=WAVEFORMBINS-2;bin>0;--bin){
    Double_t derivative=(fVectA.at(bin)-old);
    if (fabs(derivative)>VOLTAGE_TS){
      fVectA.at(bin)-=TMath::Sign(1,derivative);
    }
    old=fVectA.at(bin);
  }
}



bool TWaveformContainer::IsAClock(){
  if (fChannelId==16 || fChannelId==17){
    return true;
  }
  return false;
}


TWaveformContainer::~TWaveformContainer(){
  fVectT.clear();
  fVectRawT.clear();
  fVectA.clear();

}



