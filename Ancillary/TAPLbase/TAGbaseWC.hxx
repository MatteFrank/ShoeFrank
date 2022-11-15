#ifndef _TAGbaseWC_HXX
#define _TAGbaseWC_HXX
/*!
 \file
 \version $Id: TAGbaseWC.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAGbaseWC.
 */
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;
#include "TObject.h"
#include "TClonesArray.h"

#include "TAGroot.hxx"
#include "TAGdata.hxx"
#include "TAGdataDsc.hxx"
#include "TAGwaveCatcher.hxx"
//

/**
 * This class stores the params of a single channel waveform
 */

class TAGbaseWC : public TObject {
   
public:
   TAGbaseWC();
   TAGbaseWC(TAGwaveCatcher *w, Bool_t pwFlag = false);
   virtual         ~TAGbaseWC();
   
   Int_t    GetChannelId()     const {  return fChId;          }
   Double_t GetAmplitude()     const { return fAmplitude;      }
   Double_t GetCharge()        const { return fCharge;         }
   Double_t GetChargeFast()    const { return fChargeFast;     }
   Double_t GetChargeSlow()    const { return fChargeSlow;     }
   Double_t GetBaseline()      const { return fBaseline;       }
   Double_t GetRiseTime()      const { return fRiseTime;       }
   Double_t GetFallTime()      const { return fFallTime;       }
   Double_t GetTimeLE()        const { return fTimeLE;         }
   Double_t GetTimeCFD()       const { return fTimeCFD;        }
   Int_t    GetTriggerType()   const { return fTriggerTypeId;  }
   Double_t GetGate(Int_t idx) const { return fGates[idx];     }


   void SetChannelId(int id)         {  fChId = id;            }
   void SetAmplitude(double value)   { fAmplitude=value;       }
   void SetCharge(double achg)       { fCharge = achg;         }
   void SetChargeFast(double achg)   { fChargeFast = achg;     }
   void SetChargeSlow(double achg)   { fChargeSlow = achg;     }
   void SetRiseTime(double atime)    { fRiseTime = atime;      }
   void SetFallTime(double atime)    { fFallTime = atime;      }
   void SetTimeLE(double atime)      { fTimeLE = atime;        }
   void SetTimeCFD(double atime)     { fTimeCFD = atime;       }
   void SetTriggerType(int value)    { fTriggerTypeId = value; }

   void Clear(Option_t* opt="");
   void SetHit(TAGwaveCatcher *w, Bool_t pwFlag = false);

   //! Add the id of the simulated track
   void              AddMcTrackId(Int_t trackId);
   //! Set the id of the simulated track
   void              SetMcTrackId(Int_t index, Int_t trackId) { fMcTrackId[index] = trackId; }
   //! Get the id of the simulated track
   Int_t              GetTrackIdSim(Int_t index) const   { return fMcTrackId[index];  }
   //! Index of array of track that touched the actual pixel
   Int_t              GetMcTrackCount()           const   { return fMcTrackCount;  }
public:
   enum {kT10, kT90, kTfast, kTfall};
   
protected:
   static const Int_t fMAXTRACK = 10;

   Int_t      fChId;
   Double32_t fAmplitude;
   Double32_t fCharge;
   Double32_t fChargeFast;
   Double32_t fChargeSlow;
   Double32_t fBaseline;
   Double32_t fRiseTime;
   Double32_t fFallTime;
   Double32_t fTimeLE;
   Double32_t fTimeCFD;
   Int_t      fTriggerTypeId;
   Bool_t     fIdPhoswich;
   Double32_t fGates[4];       //!
   
   Int_t              fMcTrackCount;                 // Variable that count the number of times a pixel is touched
   Int_t              fMcTrackId[fMAXTRACK];         // Id of the track created in the simulation

private:
   static Int_t   fgNoiseBinMax;
   static Float_t fgNoiseSigmaLevel;
   static Float_t fgCFDfac;
   static Float_t fgCFDdelay;
   static Float_t fgPWfast;

public:
   static void SetNoiseBinMax(Int_t n)       { fgNoiseBinMax = n;     }
   static void SetNoiseSigmaLevel(Float_t l) { fgNoiseSigmaLevel = l; }
   static void SetCFDfac(Float_t f)          { fgCFDfac = f;          }
   static void SetCFDdelay(Float_t d)        { fgCFDdelay = d;        }
   static void SetPwfast(Float_t d)          { fgPWfast = d;          }

protected:
   void ComputeInfo(TAGwaveCatcher *w);
   vector<double> GetLinearFit(const vector<double>& z, const vector<double>& x, const vector<double>& dx);

   
   ClassDef(TAGbaseWC,6);
};

#endif
