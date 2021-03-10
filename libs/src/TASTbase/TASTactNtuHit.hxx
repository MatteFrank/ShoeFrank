#ifndef _TASTactNtuHit_HXX
#define _TASTactNtuHit_HXX
/*!
  \file
  \version $Id: TASTactNtuHit.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TASTactNtuHit.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TASTparGeo.hxx"
#include "TASTdatRaw.hxx"
#include "TASTntuHit.hxx"

#define REF_CLK make_pair(16,27)
#define CHISQUARE_THRESHOLD 1.0
#define AMPLITUDE_THRESHOLD 0.04

class TASTactNtuHit : public TAGaction {

public:

  explicit        TASTactNtuHit(const char* name=0,
                                TAGdataDsc* p_datraw=0,
                                TAGdataDsc* p_datdaq=0,
                                TAGparaDsc* p_parmap=0);
  virtual         ~TASTactNtuHit();

  virtual Bool_t  Action();

  virtual void CreateHistogram();

  ClassDef(TASTactNtuHit,0)
    
  private:
    
  TAGdataDsc*     fpDatRaw;		    // input data dsc
  TAGdataDsc*     fpNtuRaw;		    // output data dsc
  TAGparaDsc*     fpParMap;		    // output data dsc
  
  TH1F *hArrivalTime[8];
  TH1F *hCharge[8];
  TH1F *hAmplitude[8];
  TH1F *hTrigTime;
  TH1F *hTotCharge;
  TH1F *hEventTime;

  int m_nev;
  

private:
 
  void   SavePlot(TGraph g,TF1 f1, TF1 f2, TASTrawHit *hit);

};

#endif
