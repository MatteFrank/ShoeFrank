#ifndef _TACAdigitizer_HXX
#define _TACAdigitizer_HXX

#include "TAGbaseDigitizer.hxx"

#include "TACAntuRaw.hxx"

class TF1;
class TACAntuRaw;
class TACAntuHit;

// --------------------------------------------------------------------------------------
class TACAdigitizer : public TAGbaseDigitizer {
   
public:
   TACAdigitizer(TACAntuRaw* p_datraw);
   ~TACAdigitizer();
   
   void           SetFunctions();
   void           SetParFunction();
   
   Bool_t         Process(Double_t edep, Double_t x0, Double_t y0, Double_t zin=0, Double_t zout=0, Double_t time = 0, Int_t sensorId = 0, Int_t Z =-99);

   Float_t        GetPhotonsN(Float_t X, Float_t Y, Float_t edep);
   Double_t       RecPhotonsN(Double_t* x, Double_t* par);
 
   TF1*           GetFuncBirks() const { return fFuncBirks; }
   
   void           SetGain(Float_t g)   { fGain = g;          }

   TACAntuHit*    GetCurrentHit()      { return fCurrentHit; }
   
private:
   TACAntuRaw*   fpNtuRaw;
   TF1*          fFuncBirks;
   Float_t       fGain;
   TACAntuHit*   fCurrentHit;

};
#endif

