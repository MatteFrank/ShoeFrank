#ifndef _TASTdigitizer_HXX
#define _TASTdigitizer_HXX

/*!
 \file TASTdigitizer.hxx
 \brief   Declaration of TASTdigitizer.
 */
/*------------------------------------------+---------------------------------*/


#include <map>

#include "TAGbaseDigitizer.hxx"

#include "TASTparGeo.hxx"
#include "TASTntuHit.hxx"

class TF1;
using namespace std;
// --------------------------------------------------------------------------------------
class TASTdigitizer : public TAGbaseDigitizer {
   
public:
   TASTdigitizer(TASTntuHit* p_datraw);
   ~TASTdigitizer();
   
   void           SetFunctions();
   void           SetInitParFunction();
   
   void           SmearTimeST(Double_t energy,Double_t &time);

   Bool_t         Process(Double_t edep, Double_t x0, Double_t y0, Double_t zin=0, Double_t zout=0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
  

   Bool_t         IsOverEnergyThreshold(double ethr, double ene);  
   Double_t       GetEnergyThreshold() {return fEnergyThreshold;}

   Double_t       GetResTimeST(Double_t edep);

   Double_t       GetPhotonsN(Double_t X, Double_t Y, Double_t edep);
   Double_t       RecPhotonsN(Double_t* x, Double_t* par);
   Double_t       ResTimeFormula(Double_t* x, Double_t* par);
 
   TF1*           GetFuncBirks() const  { return fFuncBirks;  }
   TF1*           GetTimeResST() const  { return fTimeSTResE; }
   
   void           SetGain(Double_t g)    { fGain = g;          }
  
   TASThit*       GetCurrentHit()       { return fCurrentHit; }
   void           ClearMap()            { fMap.clear();       }
  
private:
   TASTntuHit*   fpNtuRaw;
   TASThit*      fCurrentHit;

   // N photons-->fake
   TF1*          fFuncBirks;
   Double_t      fGain;

   // Time Res function and parameters
   TF1*          fTimeSTResE;
   Double_t      fTimeST_A;
   Double_t      fTimeST_p0;
   Double_t      fTimeSTErr_p0;
   Double_t      fTimeST_p1;
   Double_t      fTimeSTErr_p1;
  
   Double_t      fEnergyThreshold;
   Double_t      fEnergyMax;

   map<int, TASThit*> fMap; //! map for pilepup

};
#endif

