#ifndef _TABMactNtuTrack_HXX
#define _TABMactNtuTrack_HXX
/*!
  \file
  \version $Id: TABMactNtuTrack.hxx,v 1.3 2003/06/29 21:52:35 mueller Exp $
  \brief   Declaration of TABMactNtuTrack.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TAGaction.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TABMntuTrack.hxx"

class TABMactNtuTrack : public TAGaction {
public:
  explicit        TABMactNtuTrack(const char* name=0,
				  TAGdataDsc* p_ntutrk=0,
				  TAGdataDsc* p_ntuhit=0,
				  TAGparaDsc* p_bmgeo=0,
				  TAGparaDsc* p_bmcon=0);
  virtual         ~TABMactNtuTrack();
  virtual Bool_t  Action();
  
  void sortDoubleHits(TABMntuRaw *hp, TABMntuTrack* pntr, TABMparGeo* ppg, TABMntuTrackTr & aRTr, TABMntuTrackTr & RefTr, int add[36], int ndou);
  void refitImprovedTrackSide(TABMntuRaw *hp, TABMntuTrack* pntr, TABMparGeo* ppg, int mulT, int mulS, double trchi2, TABMntuTrackTr & aRTr, int j);
  void refitImprovedTrackTop(TABMntuRaw *hp, TABMntuTrack* pntr, TABMparGeo* ppg, int mulT, int mulS, double trchi2, TABMntuTrackTr & aRTr, int j);
  
  ClassDef(TABMactNtuTrack,0)
    
  private:
  TAGdataDsc*     fpNtuTrk;		    // output data dsc
  TAGdataDsc*     fpNtuHit;		    // input data dsc
  TAGparaDsc*     fpBMGeo;		    // input data dsc
  TAGparaDsc*     fpBMCon;		    // input data dsc
};

#endif
