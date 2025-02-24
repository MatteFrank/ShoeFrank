#ifndef _TABMdigitizer_HXX
#define _TABMdigitizer_HXX

#include <map>

#include "TAGbaseDigitizer.hxx"

#include "TABMparGeo.hxx"
#include "TABMparConf.hxx"
#include "TABMparCal.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"

using namespace std;
// --------------------------------------------------------------------------------------

class TF1;
class TABMdigitizer : public TAGbaseDigitizer
 {

public:
  TABMdigitizer(TABMntuHit* p_datraw, TABMparGeo* parGeo, TABMparConf* parCon, TABMparCal* parCal);
  ~TABMdigitizer();
  void           ClearMap()            { fMap.clear(); }

  //getters
  Int_t          GetMapSize()          { return fMap.size(); }
  TABMhit*    GetCurrentHit()       { return fCurrentHit; }

  //others
  Bool_t         Process(Double_t edep, Double_t x0, Double_t y0, Double_t z0, Double_t zout, Double_t time,
                         Int_t wireid, Int_t Z, Double_t px0, Double_t py0, Double_t pz0);
  Double_t       EffFunc(Double_t* x, Double_t* par);           //BM efficiency as a function of drift distance
  Double_t       SmearRdrift(const Double_t , const Double_t ); //to smear the drift distance

private:

  TABMntuHit*   fpNtuRaw;
  TABMparGeo*   fpParGeo;
  TABMparConf*  fpParCon;
  TABMparCal*   fpParCal;

  Float_t       fTimeMinDiff;            //minimum time difference between two hits in the same cell to be detected

  TABMhit*   fCurrentHit;

  multimap<Int_t, TABMhit*> fMap;     //multimap with cellid and the bm hits

  ClassDef(TAGobject,0)

};
#endif
