#ifndef _TACAdigitizer_HXX
#define _TACAdigitizer_HXX

#include <map>
#include "TAGbaseDigitizer.hxx"

#include "TACAntuRaw.hxx"

using namespace std;

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

   Float_t        GetResEnergy(Float_t edep);
   Double_t       ResEnergy(Double_t* x, Double_t* par);
 
   TF1*           GetFuncBirks() const { return fFuncBirks; }
   
   void           SetGain(Float_t g)   { fGain = g;          }

   TACAntuHit*    GetCurrentHit()      { return fCurrentHit; }
   void           ClearMap()           { fMap.clear();       }
  
private:
   TACAntuRaw*   fpNtuRaw;
   TACAntuHit*   fCurrentHit;

   map<int, TACAntuHit*> fMap; //! map for pilepup
  
   TF1*          fFuncBirks;

   TF1*          fDeResE;
   Float_t       fResPar0;
   Float_t       fResErrPar0;
   Float_t       fResPar1;
   Float_t       fResErrPar1;
   Float_t       fResPar2;
   Float_t       fResErrPar2;

   Float_t       fGain;
   
private:

   
};
#endif

