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
#include "TAGgeoTrafo.hxx"
#include "TAVTparameters.hxx"

#include "TAVThit.hxx"

class TH2F;
class TH1F;
class TAMChit;
class TAVTactBaseNtuHitMC : public TAGaction {

protected:
   //! Hit structure
   struct RawMcHit_t : public  TObject {
      RawMcHit_t() {id = tkid = htid = 0; de = x = y = zi = zo = 0.;}
      Int_t  id;   ///< sensor id
      Float_t de;  ///< enerhy loss
      Float_t x;   ///< hit in X
      Float_t y;   ///< hit in Y
      Float_t zi;  ///< hit in Zin
      Float_t zo;  ///< hit in Zout
      Int_t  tkid; ///< MC track index
      Int_t  htid; ///< MC hit index      
   };
   

public:
   explicit TAVTactBaseNtuHitMC(const char* name=0, TAGparaDsc* p_geomap = 0, TAGparaDsc* pConfig=0);
   //! Destructor 
   virtual ~TAVTactBaseNtuHitMC() {};
   
   //! Base action 
   virtual bool  Action()             { return true;     }

   //! Fill noise over sensors
   virtual void  FillNoise()          { return;          }
   
   //! Set refit flag
   void   SetPileup(Bool_t flag)      { fPileup = flag;  }
   
   //! Get refit flag
   Bool_t GetPileup()                 { return fPileup;  }

   // Base creation of histogram
   void          CreateHistogram();
   
public:
   //! Set number of pileup evt
   static void   SetPileupEventsN(Bool_t n)         { fgPileupEventsN = n;       }
   
   //! Get number of pileup evt
   static Int_t  GetPileupEventsN()                 { return fgPileupEventsN;    }
   
   //! Get Sigma noise level
   static Float_t GetSigmaNoiseLevel()              { return fgSigmaNoiseLevel;  }
   //! Set Sigma noise level
   static void    SetSigmaNoiseLevel(Float_t level) { fgSigmaNoiseLevel = level; }
   
   //! Get MC noise track id
   static Int_t   GetMcNoiseId()                    { return fgMcNoiseId;        }
   //! Set MC noise track id
   static void    SetMcNoiseId(Int_t id)            { fgMcNoiseId = id;          }
      
protected:
   TAGparaDsc*     fpGeoMap;           ///< geometry para dsc
   TAGparaDsc*     fpConfig;           ///< config para dsc
   TAGgeoTrafo*    fpGeoTrafo;         ///< Gobal transformation
   TAVTbaseDigitizer*  fDigitizer;     ///< cluster size digitizer
   Int_t           fNoisyPixelsN;      ///< number of noisy pixels
   Bool_t          fPileup;           ///< flag to generated pileup events

   
   TString         fPrefix;            ///< prefix of histogram
   TString         fTitleDev;          ///< device name for histogram title

   TH2F*           fpHisPixelMap[MaxSens];  ///< pixel map per sensor
   TH2F*           fpHisPosMap[MaxSens];    ///< pixel map per sensor
   TH1F*           fpHisPixel[MaxSens];     ///< number pixels per cluster MC
   TH1F*           fpHisPixelTot;           ///< total number pixels per cluster MC
   TH1F*           fpHisDeTot;              ///< Total energy loss
   TH1F*           fpHisDeSensor[MaxSens];  ///< Energy loss per sensor
   TH1F*           fpHisPoisson;            ///< Poisson distribution for pileup

   std::vector<std::vector<RawMcHit_t>> fStoredEvents; ///< list of hits used for pilepup

protected:
   // Generated pileup events
   void            GeneratePileup();
   // Fill pileup informations
   void            FillPileup(vector<RawMcHit_t>& /*storedEvtInfo*/, TAMChit* /*hit*/, Int_t /*hitIdx*/);
   // Compute noise level
   void            ComputeNoiseLevel();
   
   //! Fill noisy pixels
   virtual void    FillNoise(Int_t /*sensorId*/)                    { return; }
   //! Creat digitizer
   virtual void    CreateDigitizer()                                { return; }
   //! Fill pixels
   virtual void    FillPixels( Int_t /*sensorId*/, Int_t /*mcId*/, Int_t /*trackId*/, Bool_t /*pileup*/ ) { return; }

protected:
   static Int_t    fgPileupEventsN;    ///< number of pileup events to be stored
   static Float_t  fgSigmaNoiseLevel;  ///< Sigma noise level
   static Int_t    fgMcNoiseId;        ///< MC noise track Id

   ClassDef(TAVTactBaseNtuHitMC,0)
};

#endif
























