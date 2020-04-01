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

  void SetTrigType(Int_t t)       { fTrigType = t;          }
  void SetTriggerCellId(Int_t t)  { fTriggerCellId = t;     }
  void SetChannelId(Int_t c)      { fChannelId = c;         }
  void SetBoardId(Int_t b)        { fBoardId = b;           }
  void SetNEvent(int value)       { fNEvent = value;        }
  void SetEmptyFlag(bool value)   { fIsEmptyFlag = value;   }
  void SetHighlevelDebug(bool b)  { fHighlevelDebug = b;    }

  Int_t GetTrigType()       const { return fTrigType;       }
  Int_t GetTriggerCellId()  const { return fTriggerCellId;  }
  Int_t GetChannelId()      const { return fChannelId;      }
  Int_t GetBoardId()        const { return fBoardId;        }
  Int_t GetNEvent()         const { return fNEvent;         }
  Bool_t IsEmpty()          const { return fIsEmptyFlag;    }
  Bool_t IsHighlevelDebug() const { return fHighlevelDebug; }

  vector<double>& GetVectT()      { return fVectT;          }
  vector<double>& GetVectRawT()   { return fVectRawT;       }
  vector<double>& GetVectA()      { return fVectA;          }

private:
  Int_t  fTrigType;
  Int_t  fTriggerCellId;
  Int_t  fChannelId;
  Int_t  fBoardId;
  Int_t  fNEvent;
  Bool_t fHighlevelDebug;
  Bool_t fIsEmptyFlag;
  
  vector<double> fVectT;
  vector<double> fVectRawT;
  vector<double> fVectA;
};

#endif
