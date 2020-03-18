#include "TF1.h"
#include "TH1D.h"
#include "TGraph.h"
#include <vector>
#include "Parameters.h"


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

  void Clear();
  void PlotWaveForm(int i);
  void GraphWaveForm(TH1F *wv0);
  void SanitizeWaveform();
  pair<double,double> ComputeClockParameters();
  bool IsAClock();

  inline bool IsEmpty(){return IsEmptyFlag;}
  inline void SetEmptyFlag(bool value){IsEmptyFlag = value;}
  // inline void SetNEvent(int value){ m_nEvent = value;}
  
  int TrigType;
  int TriggerCellId;
  Int_t ChannelId;
  Int_t BoardId;
  vector<double> m_vectT;
  vector<double> m_vectRawT;
  vector<double> m_vectA;
  Int_t m_nEvent;
  bool high_level_debug;
  bool IsEmptyFlag;
  
};

#endif
