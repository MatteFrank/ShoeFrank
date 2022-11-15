#ifndef _TACEdigitizer_HXX
#define _TACEdigitizer_HXX

#include <map>
#include "TAGobject.hxx"

#include "TACEntuHit.hxx"

class TF1;
using namespace std;
// --------------------------------------------------------------------------------------
class TACEdigitizer : public TAGobject {
   
public:
   TACEdigitizer(TACEntuHit* p_datraw);
   ~TACEdigitizer();
   
   void           SetFunctions();
   void           SetParFunction();
   
   Bool_t         Process(Double_t edep, Double_t x0, Double_t y0, Double_t zin=0, Double_t zout=0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
  
   
   Float_t        GetEnergyRes(Float_t X, Float_t Y, Float_t edep);
   Double_t       RecEnergyRes(Double_t* x, Double_t* par);
 
   TF1*           GetFuncEnergyRes() const  { return fFuncEnergyRes;  }
   
   void           SetGain(Float_t g)        { fGain = g;              }
   void           SetResTime(Float_t r)     { fResTime = r;           }

   TACEhit*       GetCurrentHit()           { return fCurrentHit;     }
   void           ClearMap()                { fMap.clear();           }
  
private:
   TACEntuHit*   fpNtuRaw;
   TF1*          fFuncEnergyRes;
   Float_t       fGain;
   Float_t       fEnergyResCst;
   Float_t       fEnergyResSqrt;
   Float_t       fResTime;
   TACEhit*      fCurrentHit;

   map<int, TACEhit*> fMap; //! map for pilepup

};
#endif

