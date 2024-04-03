#ifndef _TAGbaseWD_HXX
#define _TAGbaseWD_HXX
/*!
  \file TAGbaseWD.hxx
  \brief   Declaration of TAGbaseWD.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;
#include "TObject.h"
#include "TClonesArray.h"
#include "TAGroot.hxx"
#include "TAGdata.hxx"
#include "TAGdataDsc.hxx"
#include "TWaveformContainer.hxx"
#include "TComplex.h"
#include "TSpectrum.h"

#define NNOISEFFT 500
#define NSUPERHIT 1000
#define FIRSTVALIDSAMPLE 5
#define NSIDESAMPLES 30

class TAGbaseWD : public TAGobject {

public:
  TAGbaseWD();
  TAGbaseWD(TWaveformContainer *w);
  virtual         ~TAGbaseWD();
  
  virtual double ComputeTime(TWaveformContainer *w, double frac, double del, double tleft, double tright);
  virtual double ComputeTimeSimpleCFD(TWaveformContainer *w, double frac);
  virtual double ComputeTimeTangentCFD(TWaveformContainer *w, double frac);
  virtual double ComputePedestal( TWaveformContainer *w, double th);
  virtual double ComputeBaseline( TWaveformContainer *w);
  virtual double ComputeCharge(TWaveformContainer *w,double th);
  virtual double ComputeAmplitude(TWaveformContainer *w);
  virtual double ComputeRiseTime(TWaveformContainer *w);
  
  static  void ComputeFFT(vector<double> &amp, vector<double> &re, vector<double> &im, vector<double> &mag);
  static  void ComputeInverseFFT(vector<double> &amp, vector<double> re, vector<double> im);
  static  void FFTnotchFilter(vector<double> FreqCut, vector<double> &re, vector<double> &im);
  static  void FFTnotchFilterSmooth(vector<double> FreqCut, vector<double> &re, vector<double> &im);
  static  void LowPassFilterFFT(vector<double>& re, vector<double>& im, double cutoffFrequency, int order);


   Int_t    GetChID()          const {  return fChId;          }
   Int_t    GetBoardId()       const { return fBoardId;        }
   Int_t    GetTriggerType()   const { return fTriggerTypeId;  }
   Int_t    GetTriggerCellID() const { return fTriggerCellId;  }
   Double_t GetTime()          const { return fTime;           }
   Double_t GetTimeOth()       const { return fTimeOth;        }
   Double_t GetCharge()        const { return fChg;            }
   Double_t GetFreqminFFT()    const { return fFreqminFFT;      }
   Double_t GetFreqmaxFFT()    const { return fFreqmaxFFT;      }
   Bool_t    IsFFTValid()         { return (!fFFT.empty()) ? true : false; }
   vector<double>&  GetFFT(string type)         { return fFFT.find(type)->second; }
   vector<double>&  GetFFTfilter(string type)         { return fFFTfilter.find(type)->second; }
   Double_t GetAmplitude()     const { return fAmplitude;      }
   Double_t GetPedestal()      const { return fPedestal;       }
   Double_t GetRiseTime()      const { return fRiseTime;       }
   Double_t GetBaseline()      const { return fBaseline;       }
   Double_t GetDeltaClk()      const { return fDeltaClk;       }
   vector<double>&  GetNoise()   { return fSidebandValues; }
   Int_t    GetIDMC()          const { return fMcId;           }
   
   void SetChID(int id)              {  fChId = id;            }
   void SetBoardId(Int_t boardid)    { fBoardId=boardid;       }
   void SetTriggerType(int value)    { fTriggerTypeId = value; }
   void SetTriggerCellID(int value)  { fTriggerCellId = value; }
   void SetTime(double atime)        { fTime = atime;          }
   void SetTimeOth(double atime)     { fTimeOth = atime;       }
   void SetCharge(double achg)       { fChg = achg;            }
   void SetFreqminFFT(double value)  { fFreqminFFT = value;    }
   void SetFreqmaxFFT(double value)  { fFreqmaxFFT = value;    }
   void SetMCID(int id)              { fMcId = id;             }
   void SetAmplitude(double value)   { fAmplitude=value;       }
   void SetPedestal(double value)    { fPedestal=value;        }
   void SetRiseTime(double value)    { fRiseTime=value;        }
   void SetFFT(string type, vector<double> values){ fFFT[type]=values;        }
   void SetFFTfilter(string type, vector<double> values){ fFFTfilter[type]=values;        }


  ClassDef(TAGbaseWD,4);
    //
  protected:
  Double32_t fTime;
  Double32_t fTimeOth;
  Double32_t fChg;
  Double32_t fAmplitude;
  Double32_t fBaseline;
  Double32_t fPedestal;
  Double32_t fRiseTime;
  vector<double> fSidebandValues;
  map<string,vector<double>> fFFT;
  map<string,vector<double>> fFFTfilter;
  Double32_t fFreqminFFT;
  Double32_t fFreqmaxFFT;
  Int_t      fChId;
  Int_t      fBoardId;
  Int_t      fMcId;
  Int_t      fTriggerTypeId;
  Int_t      fTriggerCellId;
  Double32_t fDeltaClk;

};

#endif
