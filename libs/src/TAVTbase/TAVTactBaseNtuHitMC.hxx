#ifndef _TAVTactBaseNtuHitMC_HXX
#define _TAVTactBaseNtuHitMC_HXX
/*!
 \file TAVTactBaseNtuHitMC.hxx
 \brief   Declaration of TAVTactBaseNtuHitMC.
 */
/*------------------------------------------+---------------------------------*/

#include "TString.h"

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"

#include "TAVTbaseDigitizer.hxx"
#include "TAVTdigitizerE.hxx"

#include "TAVThit.hxx"

class TH2F;
class TH1F;
class TAVTactBaseNtuHitMC : public TAGaction {

protected:
   struct RawMcHit_t : public  TObject {
      RawMcHit_t() {id = 0; de = x = y = 0.;}
      Int_t  id;   ///< sensor id
      Float_t de;  ///< enerhy loss
      Float_t x;   ///< hit in X
      Float_t y;   ///< hit in Y
      Float_t zi;  ///< hit in Zin
      Float_t zo;  ///< hit in Zout
   };
   

public:
   explicit TAVTactBaseNtuHitMC(const char* name=0, TAGparaDsc* p_geomap = 0);
   //! Destructor 
   virtual ~TAVTactBaseNtuHitMC() {};
   
   //! Base action 
   virtual bool  Action()      { return true; }

   // Fill noise over sensors
   virtual void  FillNoise()   { return;      }
   
   // Base creation of histogram
   void          CreateHistogram();

public:
   //! Set refit flag
   static void   SetPileup(Bool_t flag)      { fgPileup = flag;         }
   
   //! Get refit flag
   static Bool_t GetPileup()                 { return fgPileup;         }
   
   //! Set number of pileup evt
   static void   SetPileupEventsN(Bool_t n)  { fgPileupEventsN = n;     }
   
   //! Get number of pileup evt
   static Int_t  GetPileupEventsN()          { return fgPileupEventsN;  }
   
   //! Set Poisson parameter
   static void   SetPoissonPar(Float_t par)  { fgPoissonPar = par;      }
   
   //! Get Poisson parameter
   static Float_t GetPoissonPar()            { return fgPoissonPar;     }

   //! Get Sigma noise level
   static Float_t GetSigmaNoiseLevel()              { return fgSigmaNoiseLevel;  }
   //! Set Sigma noise level
   static void    SetSigmaNoiseLevel(Float_t level) { fgSigmaNoiseLevel = level; }
   
   //! Get MC noise track id
   static Int_t   GetMcNoiseId()                    { return fgMcNoiseId;        }
   //! Set MC noise track id
   static void    SetMcNoiseId(Int_t id)            { fgMcNoiseId = id;          }
      
protected:
   TAGparaDsc*     fpGeoMap;		      ///< geometry para dsc

   TAVTbaseDigitizer*  fDigitizer;     ///< cluster size digitizer
   Int_t           fNoisyPixelsN;
   
   TString         fPrefix;            ///< prefix of histogram
   TString         fTitleDev;          ///< device name for histogram title

   TH2F*           fpHisPixelMap[32];  ///< pixel map per sensor
   TH2F*           fpHisPosMap[32];    ///< pixel map per sensor
   TH1F*           fpHisPixel[32];     ///< number pixels per cluster MC
   TH1F*           fpHisPixelTot;      ///< total number pixels per cluster MC
   TH1F*           fpHisDeTot;         ///< Total energy loss
   TH1F*           fpHisDeSensor[32];  ///< Energy loss per sensor
   TH1F*           fpHisPoisson;       ///< Poisson distribution for pileup

   std::vector<std::vector<RawMcHit_t>> fStoredEvents; ///< list of hits used for pilepup

protected:
   // Generated pileup events
   void            GeneratePileup();
   // Compute noise level
   void            ComputeNoiseLevel();
   
   //! Fill noisy pixels
   virtual void    FillNoise(Int_t /*sensorId*/)                    { return; }
   //! Creat digitizer
   virtual void    CreateDigitizer()                                { return; }
   //! Fill pixels
   virtual void    FillPixels( Int_t /*sensorId*/, Int_t /*mcId*/, Int_t /*trackId*/ ) { return; }
   

protected:
   static Bool_t   fgPileup;           ///< flag to generated pileup events
   static Int_t    fgPileupEventsN;    ///< number of pileup events to be stored
   static Float_t  fgPoissonPar;       ///< Poisson parameter for pileup simulation
   static Float_t  fgSigmaNoiseLevel;  ///< Sigma noise level
   static Int_t    fgMcNoiseId;        ///< MC noise track Id

   ClassDef(TAVTactBaseNtuHitMC,0)
};

#endif
























