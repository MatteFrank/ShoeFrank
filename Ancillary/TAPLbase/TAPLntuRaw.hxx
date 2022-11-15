#ifndef _TAPLntuRaw_HXX
#define _TAPLntuRaw_HXX
/*!
  \file
  \version $Id: TAPLntuRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TAPLntuRaw.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;

#include "TObject.h"
#include "TClonesArray.h"
#include "TAGdata.hxx"
#include "TAGbaseWC.hxx"
#include "TAGwaveCatcher.hxx"
//

/**
 * This class stores the params of a single channel waveform
 */
class TAPLrawHit : public TAGwaveCatcher
{
   
public:
  TAPLrawHit();
  TAPLrawHit(const TAGwaveCatcher& rawHit);
  virtual         ~TAPLrawHit();
  
  ClassDef(TAPLrawHit,1);
  //
};

//##############################################################################

class TAPLntuRaw : public TAGdata {
public:

  TAPLntuRaw();
  virtual         ~TAPLntuRaw();


  TAPLrawHit*       GetHit();
  const TAPLrawHit* GetHit() const;
  
  TAPLrawHit*       NewHit(const TAGwaveCatcher& w);
  void              SetupClones();

   const Char_t*    GetSoftwareVersion() const { return fSoftwareVersion.Data(); }
   Int_t            GetSamplesN()        const { return fSamplesN;               }
   Int_t            GetChannelsN()       const { return fChannelsN;              }
   Float_t          GetPeriod()          const { return fPeriod;                 }
   ULong_t          GetTimeStamp()       const { return fTimeStamp;              }
   
   void             SetSoftwareVersion(TString s) { fSoftwareVersion = s;        }
   void             SetSamplesN(Int_t n)          { fSamplesN = n;               }
   void             SetChannelsN(Int_t n)         { fChannelsN = n;              }
   void             SetPeriod(Float_t p)          { fPeriod = p;                 }
   void             SetTimeStamp(ULong_t t)       { fTimeStamp = t;              }

  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
   TAPLrawHit*     fHit;         // hits
   
   TString         fSoftwareVersion;
   Int_t           fSamplesN;
   Int_t           fChannelsN;
   Float_t         fPeriod;
   ULong_t         fTimeStamp;

private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TAPLntuRaw,1);
};

#endif
