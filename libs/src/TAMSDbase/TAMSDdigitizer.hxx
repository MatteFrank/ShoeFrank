#ifndef _TAMSDdigitizer_HXX
#define _TAMSDdigitizer_HXX


#include <map>

#include "TAGbaseDigitizer.hxx"

// --------------------------------------------------------------------------------------
class TAMSDparGeo;
class TAMSDdigitizer : public TAGbaseDigitizer {
   
   // Class to digitize the energy into pixel based on given patterns
public:
   TAMSDdigitizer(TAMSDparGeo* parGeo);
   virtual ~TAMSDdigitizer();

   void  FillMap(Int_t strip, Double_t value);
   Bool_t Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin = 0, Double_t zout = 0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99);
   
   Int_t GetStrip(Float_t pos) const;

private:
   TAMSDparGeo*    fpParGeo;
   Int_t           fStripsN;         // number of strips for a given eloss
   Float_t         fPitch;
   Int_t           fView;
   
   std::map<int, double> fMap;      // map of found strips

private:
   static Float_t  fgChargeGain;      // gain factor for despoted charge
   static Float_t  fgChargeFac;       // sharing factor between strips
   
public:
   Int_t                  GetStripsN() const { return fStripsN; }
   Int_t                  GetPitch()   const { return fPitch;   }
   Int_t                  GetView()    const { return fView;    }
   
   std::map<int, double>  GetMap()     const { return fMap;     }

public:
   static Float_t GetChargeGain()                   { return fgChargeGain;       }
   static void    SetChargeGain(Float_t gain)       { fgChargeGain = gain;       }
   
   static Float_t GetChargeFac()                    { return fgChargeFac;        }
   static void    SetChargeFac(Float_t fac)         { fgChargeFac = fac;         }
   
   
   ClassDef(TAMSDdigitizer,0)
};
        

#endif

