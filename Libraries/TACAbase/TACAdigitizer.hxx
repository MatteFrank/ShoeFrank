#ifndef _TACAdigitizer_HXX
#define _TACAdigitizer_HXX

/*!
 \file TACAdigitizer.hxx
 \brief   Declaration of TACAdigitizer.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include "TAGbaseDigitizer.hxx"
#include "TACAparCal.hxx"

#include "TACAntuHit.hxx"

class TF1;
class TACAntuHit;
class TACAhit;
using namespace std;
// --------------------------------------------------------------------------------------
class TACAdigitizer : public TAGbaseDigitizer {
   
public:
   TACAdigitizer(TACAntuHit* p_datraw,  TACAparCal* p_parcal);
   ~TACAdigitizer();
   
   void           SetFunctions();
   void           SetParFunction();
   
   Bool_t         Process(Double_t edep, Double_t x0, Double_t y0, Double_t zin=0, Double_t zout=0, Double_t time = 0, Int_t sensorId = 0, Int_t Z =-99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
 
   void           SmearEnergy(TACAparCal* parCal);
  
   Float_t        GetResEnergy(Float_t edep);
   Double_t       ResEnergy(Double_t* x, Double_t* par);
   //! Get energy resolution function
   TF1*           GetFuncResE()    const  { return fDeResE;     }
   //! Get current hit
   TACAhit*       GetCurrentHit()         { return fCurrentHit; }
   //! Clear map
   void           ClearMap()              { fMap.clear();       }

private:
   TACAntuHit*   fpNtuRaw;      ///< input raw data container
   TACAparCal*   fpParCal;      ///< calibration parameter
   TF1*          fDeResE;       ///< histogram of resolution energy
   
   Float_t       fResPar0;      ///< resolution energy parameter 0
   Float_t       fResErrPar0;   ///< resolution energy parameter error 0
   Float_t       fResPar1;      ///< resolution energy parameter 1
   Float_t       fResErrPar1;   ///< resolution energy parameter error 1
   Float_t       fResPar2;      ///< resolution energy parameter 2
   Float_t       fResErrPar2;   ///< resolution energy parameter error 2
   
   TACAhit*      fCurrentHit;   ///< current hit
   map<int, TACAhit*> fMap;     //! map for pilepup

private:
   static Float_t emin_res;  ///< default threshold value  

};
#endif

