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
class TABMdigitizer : public TAGobject {

public:
  TABMdigitizer(TABMntuRaw* p_datraw, TABMparConf* parCon);
  ~TABMdigitizer();
  void           ClearMap()            { fMap.clear(); }

  //getters
  TF1*  GetEffDist(){return fpEffDist;};
  Int_t GetMapSize(){return fMap.size();};

  //others
  Bool_t         Process(Double_t rdrift, Int_t cellid, Int_t plane, Int_t view, Int_t cell,Int_t inhitindex,Int_t inipoint);
  Double_t       EffFunc(Double_t* x, Double_t* par);           //BM efficiency as a function of drift distance
  Double_t       SmearRdrift(const Double_t , const Double_t ); //to smear the drift distance

private:

  TABMntuRaw*   fpNtuRaw;
  TABMparConf*   fpParCon;

  Float_t       fTimeMinDiff;            //minimum time difference between two hits in the same cell to be detected

  TF1*          fpEffDist;               //efficiency as a function of drift distance, measured from data taken @ Trento 2019
  multimap<Int_t, TABMntuHit*> fMap;     //multimap with cellid and the bm hits

  ClassDef(TAGobject,0)

};
#endif
