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
  m_vectT.clear();
  m_vectRawT.clear();
  m_vectA.clear();
  IsEmptyFlag = true;
  high_level_debug = false; //it can be set only hard-coding this variable
  Clear();
  m_nEvent=0;
}


void TWaveformContainer::Clear(){
  ChannelId=-1000;
  BoardId=-1000;
  TriggerCellId=-1000;
  TrigType=-1000;
  m_vectT.clear();
  m_vectRawT.clear();
  m_vectA.clear();
  IsEmptyFlag = true;
  m_nEvent=0;
}


TWaveformContainer::TWaveformContainer(const TWaveformContainer &other){

  ChannelId=other.ChannelId;
  BoardId=other.BoardId;
  TriggerCellId = other.TriggerCellId;
  TrigType = other.TrigType;
  m_vectT = other.m_vectT;
  m_vectRawT = other.m_vectRawT;
  m_vectA = other.m_vectA;
  IsEmptyFlag = false;
}



// DEBUG ONLY
void TWaveformContainer::PlotWaveForm(int i){

  Double_t max=m_vectA.at(0);
  Double_t min=m_vectA.at(0);;
  for(int bin=0;bin<WAVEFORMBINS;++bin){
    if (m_vectA.at(bin)>max){
      max=m_vectA.at(bin);
    }
    if (m_vectA.at(bin)<min){
      min=m_vectA.at(bin);
    }
  }
  std::cout << " max " << max << " min "<< min <<std::endl;
  TCanvas *c = new TCanvas;
  c->Range(m_vectT.at(0),min,m_vectA.at(WAVEFORMBINS-1),max);
  TGraph *g=new TGraph(WAVEFORMBINS,&m_vectT[0],&m_vectA[0]);
  g->GetXaxis()->SetTitle("t (ns)");
  g->GetYaxis()->SetTitle("Amplitude (V)");
  g->SetTitle(TString::Format(" Board %d Channel %d",BoardId,ChannelId));
  g->Draw("AC");
  c->SaveAs(TString::Format("Waveform_Event%d_Board%d_Channel%d.pdf",i, BoardId,ChannelId));
  c->Close();
  gSystem->ProcessEvents();
}



// DEBUG ONLY
void TWaveformContainer::GraphWaveForm(TH1F *wv0){
  double min = TMath::MinElement(WAVEFORMBINS,&m_vectT[0]);
  double max = TMath::MaxElement(WAVEFORMBINS,&m_vectT[0]);
  wv0->SetBins(WAVEFORMBINS,min,max);
  for (int bin=0;bin<WAVEFORMBINS;++bin){
    wv0->SetBinContent(bin+1,m_vectT.at(bin));
  }
  wv0->GetXaxis()->SetTitle("t (s)");
  wv0->GetYaxis()->SetTitle("Amplitude (V)");
  wv0->SetTitle(TString::Format(" Board %d Channel %d",BoardId,ChannelId));
  return ; 
}




void TWaveformContainer::SanitizeWaveform(){
  Double_t old=m_vectA.at(WAVEFORMBINS-1);
  if (old<-0.8){
    old+=1;
    m_vectA.at(WAVEFORMBINS-1)+=1;
  }
  for (int bin=WAVEFORMBINS-2;bin>0;--bin){
    Double_t derivative=(m_vectA.at(bin)-old);
    if (fabs(derivative)>VOLTAGE_TS){
      m_vectA.at(bin)-=TMath::Sign(1,derivative);
    }
    old=m_vectA.at(bin);
  }
}



bool TWaveformContainer::IsAClock(){
  if (ChannelId==16 || ChannelId==17){
    return true;
  }
  return false;
}


TWaveformContainer::~TWaveformContainer(){
  m_vectT.clear();
  m_vectRawT.clear();
  m_vectA.clear();

}



