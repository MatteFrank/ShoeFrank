#ifndef _TACAdigitizer_HXX
#define _TACAdigitizer_HXX

#include <map>
#include "TAGbaseDigitizer.hxx"

#include "TACAntuHit.hxx"

class TF1;
class TACAntuHit;
class TACAhit;
using namespace std;
// --------------------------------------------------------------------------------------
class TACAdigitizer : public TAGbaseDigitizer {
   
public:
   TACAdigitizer(TACAntuHit* p_datraw);
   ~TACAdigitizer();
   
   void           SetFunctions();
   void           SetParFunction();
   
   Bool_t         Process(Double_t edep, Double_t x0, Double_t y0, Double_t zin=0, Double_t zout=0, Double_t time = 0, Int_t sensorId = 0, Int_t Z =-99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
 
   Float_t        GetResEnergy(Float_t edep);
   Double_t       ResEnergy(Double_t* x, Double_t* par);
   TF1*           GetFuncResE()    const  { return fDeResE;     }
 
   TACAhit*       GetCurrentHit()         { return fCurrentHit; }
   void           ClearMap()              { fMap.clear();       }

public:
   static Float_t GetThreshold()          { return fgThreshold; }
   static void    SetThreshold(Float_t t) { fgThreshold = t;    }

private:
   TACAntuHit*   fpNtuRaw;
   TF1*          fDeResE;
   
   Float_t       fResPar0;
   Float_t       fResErrPar0;
   Float_t       fResPar1;
   Float_t       fResErrPar1;
   Float_t       fResPar2;
   Float_t       fResErrPar2;
   
   TACAhit*   fCurrentHit;
   map<int, TACAhit*> fMap; //! map for pilepup

private:
   static Float_t fgThreshold;

};
#endif

