#ifndef _TAMSDdigitizer_HXX
#define _TAMSDdigitizer_HXX


#include <map>

#include "TAGbaseDigitizer.hxx"

// --------------------------------------------------------------------------------------
class TAMSDparGeo;
class TF1;
class TAMSDdigitizer : public TAGbaseDigitizer {
   
   // Class to digitize the energy into pixel based on given patterns
public:
   TAMSDdigitizer(TAMSDparGeo* parGeo);
   virtual ~TAMSDdigitizer();

   void  FillMap(Int_t strip, Double_t value);
   Bool_t Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin = 0, Double_t zout = 0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
   
   Int_t GetStrip(Float_t pos) const;

   Double_t       EtaX0(Double_t* x, Double_t* par);
   Double_t       GetEta(Double_t pos);

   Double_t       ResEnergy(Double_t* x, Double_t* par);
   Double_t       GetResEnergy(Float_t edep);
   
   void           SetFunctions();
   void           SetParFunction();

   
private:
   TAMSDparGeo*    fpParGeo;
   TF1*            fFuncEta;
   Int_t           fStripsN;         // number of strips for a given eloss
   Float_t         fPitch;
   Int_t           fView;
   
   Double_t        fEtaLimLo;
   Double_t        fEtaLimUp;
   Float_t         fEtaCst;
   Float_t         fEtaErrCst;
   
   TF1*            fDeResE;
   Float_t         fResPar0;
   Float_t         fResErrPar0;
   Float_t         fResPar1;
   Float_t         fResErrPar1;
   Float_t         fResPar2;
   Float_t         fResErrPar2;
   
   std::map<int, double> fMap;      // map of found strips

private:
   static Float_t  fgChargeGain;      // gain factor for despoted charge
   static Float_t  fgDefSmearPos;
   static Bool_t   fgSmearFlag;

public:
   Int_t                  GetStripsN() const { return fStripsN; }
   Int_t                  GetPitch()   const { return fPitch;   }
   Int_t                  GetView()    const { return fView;    }
   
   std::map<int, double>  GetMap()     const { return fMap;     }

public:
   static Float_t GetChargeGain()                   { return fgChargeGain;       }
   static void    SetChargeGain(Float_t gain)       { fgChargeGain = gain;       }
   static Bool_t  GetSmearFlag()                    { return fgSmearFlag;        }
   static void    SetSmearFlag(Bool_t flag)         { fgSmearFlag = flag;        }
   
   static Float_t GetDefSmearPos()                  { return fgDefSmearPos;      }
   static void    SetDefSmearPos(Float_t pos)       { fgDefSmearPos = pos;       }
   
};
        

#endif

