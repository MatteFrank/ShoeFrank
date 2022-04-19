#ifndef _TAGbaseWD_HXX
#define _TAGbaseWD_HXX
/*!
  \file
  \version $Id: TAGbaseWD.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
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
//

/**
 * This class stores the params of a single channel waveform
 */

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


   Int_t    GetChID()          const {  return fChId;          }
   Int_t    GetBoardId()       const { return fBoardId;        }
   Int_t    GetTriggerType()   const { return fTriggerTypeId;  }
   Int_t    GetTriggerCellID() const { return fTriggerCellId;  }
   Double_t GetTime()          const { return fTime;           }
   Double_t GetTimeOth()       const { return fTimeOth;        }
   Double_t GetCharge()        const { return fChg;            }
   Double_t GetAmplitude()     const { return fAmplitude;      }
   Double_t GetPedestal()      const { return fPedestal;       }
   Double_t GetBaseline()      const { return fBaseline;       }
   Int_t    GetIDMC()          const { return fMcId;           }
   
   void SetChID(int id)              {  fChId = id;            }
   void SetBoardId(Int_t boardid)    { fBoardId=boardid;       }
   void SetTriggerType(int value)    { fTriggerTypeId = value; }
   void SetTriggerCellID(int value)  { fTriggerCellId = value; }
   void SetTime(double atime)        { fTime = atime;          }
   void SetTimeOth(double atime)     { fTimeOth = atime;       }
   void SetCharge(double achg)       { fChg = achg;            }
   void SetMCID(int id)              { fMcId = id;             }
   void SetAmplitude(double value)   { fAmplitude=value;       }
   void SetPedestal(double value)    { fPedestal=value;        }

  ClassDef(TAGbaseWD,4);
    //
  protected:
  Double32_t fTime;
  Double32_t fTimeOth;
  Double32_t fChg;
  Double32_t fAmplitude;
  Double32_t fBaseline;
  Double32_t fPedestal;
  Int_t      fChId;
  Int_t      fBoardId;
  Int_t      fMcId;
  Int_t      fTriggerTypeId;
  Int_t      fTriggerCellId;

};

#endif
