#include "TF1.h"
#include "TH1D.h"
#include "TGraph.h"
#include <vector>

#ifndef TWAVEFORM_CONTAINER
#define TWAVEFORM_CONTAINER

using namespace std;

// this class holds the waveform, channel id of the WD and the BoardId
class TWaveformContainer
{

public:
  TWaveformContainer();
  ~TWaveformContainer();
  TWaveformContainer(const TWaveformContainer &other);
  Double_t ComputeCharge();
  Double_t ComputeChargeST();
  Double_t ComputePedestal();
  Double_t ComputeAmplitude();
  Double_t ComputeTimeStamp();
  void Clear();
  void PlotWaveForm(int i);
  void GraphWaveForm(TH1F *wv0);
  void SanitizeWaveform();
  bool IsAClock();
  Double_t FindFirstRaisingEdgeTime();
  bool ComputeArrivalTime();
  inline void SetDel(double d)  {m_del = d; return;}
  inline void SetFrac(double f) {m_frac = f; return;}
  inline void SetBinMin(int bm) {m_binmin = bm; return;}
  inline void SetBinMax(int bM) {m_binmax = bM; return;}

  inline double GetDel()  {return m_del;}
  inline double GetFrac() {return m_frac;}
  inline int GetBinMin()  {return m_binmin;}
  inline int GetBinMax()  {return m_binmax;}

  double m_del, m_frac;
  int m_binmin, m_binmax;
  int TrigType;
  Int_t ChannelId;
  Int_t BoardId;
  Double_t ir_time;    
  Double_t ir_chg;
  Double_t ir_pedestal;
  Double_t ir_pedestalstd;
  Double_t ir_amplitude;
  Double_t *T;
  Double_t *W;
  vector<double> m_vectT;
  vector<double> m_vectW;

};

#endif
