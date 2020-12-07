#ifndef _TABMdigitizer_HXX
#define _TABMdigitizer_HXX

#include <map>

#include "TAGbaseDigitizer.hxx"

#include "TABMparGeo.hxx"
#include "TABMparConf.hxx"
#include "TABMntuRaw.hxx"
#include "TABMntuHit.hxx"

using namespace std;
// --------------------------------------------------------------------------------------

class TF1;
class TABMdigitizer : public TAGbaseDigitizer
 {

public:
  TABMdigitizer(TABMntuRaw* p_datraw, TABMparGeo* parGeo, TABMparConf* parCon);
  ~TABMdigitizer();
  void           ClearMap()            { fMap.clear(); }

  //getters
  TF1*           GetEffDist()          { return fpEffDist;   }
  Int_t          GetMapSize()          { return fMap.size(); }
  TABMntuHit*    GetCurrentHit()       { return fCurrentHit; }

  //others
  Bool_t         Process(Double_t edep, Double_t x0, Double_t y0, Double_t z0, Double_t zout, Double_t time,
                         Int_t wireid, Int_t Z, Double_t px0, Double_t py0, Double_t pz0);
  Double_t       EffFunc(Double_t* x, Double_t* par);           //BM efficiency as a function of drift distance
  Double_t       SmearRdrift(const Double_t , const Double_t ); //to smear the drift distance

private:

  TABMntuRaw*   fpNtuRaw;
  TABMparGeo*   fpParGeo;
  TABMparConf*  fpParCon;

  Float_t       fTimeMinDiff;            //minimum time difference between two hits in the same cell to be detected

  TF1*          fpEffDist;               //efficiency as a function of drift distance, measured from data taken @ Trento 2019
  TABMntuHit*   fCurrentHit;

  multimap<Int_t, TABMntuHit*> fMap;     //multimap with cellid and the bm hits

  ClassDef(TAGobject,0)

};
#endif
