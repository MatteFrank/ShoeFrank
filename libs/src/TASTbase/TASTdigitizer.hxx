#ifndef _TASTdigitizer_HXX
#define _TASTdigitizer_HXX

#include <map>
#include "TAGbaseDigitizer.hxx"

#include "TASTntuHit.hxx"

class TF1;
using namespace std;
// --------------------------------------------------------------------------------------
class TASTdigitizer : public TAGbaseDigitizer {
   
public:
   TASTdigitizer(TASTntuHit* p_datraw);
   ~TASTdigitizer();
   
   void           SetFunctions();
   void           SetParFunction();
   
   Bool_t         Process(Double_t edep, Double_t x0, Double_t y0, Double_t zin=0, Double_t zout=0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
  
   
   Float_t        GetPhotonsN(Float_t X, Float_t Y, Float_t edep);
   Double_t       RecPhotonsN(Double_t* x, Double_t* par);
 
   TF1*           GetFuncBirks() const  { return fFuncBirks;  }
   
   void           SetGain(Float_t g)    { fGain = g;          }
   void           SetResTime(Float_t r) { fResTime = r;       }

   TASThit*    GetCurrentHit()       { return fCurrentHit; }
   void           ClearMap()            { fMap.clear();       }
  
private:
   TASTntuHit*   fpNtuRaw;
   TF1*          fFuncBirks;
   Float_t       fGain;
   Float_t       fResTime;
   TASThit*   fCurrentHit;

   map<int, TASThit*> fMap; //! map for pilepup

};
#endif

