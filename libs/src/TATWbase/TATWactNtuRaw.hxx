#ifndef _TATWactNtuRaw_HXX
#define _TATWactNtuRaw_HXX
/*!
  \file
  \version $Id: TATWactNtuRaw.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
  \brief   Declaration of TATWactNtuRaw.
*/
/*------------------------------------------+---------------------------------*/

#include "TAGaction.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TATWparGeo.hxx"
#include "TATWdatRaw.hxx"
#include "TATWntuRaw.hxx"

class TATWactNtuRaw : public TAGaction {

public:

  explicit        TATWactNtuRaw(const char* name=0,
				TAGdataDsc* p_datraw=0,
				TAGdataDsc* p_datdaq=0,
				TAGparaDsc* p_pargeo=0,
				TAGparaDsc* p_parmap=0);
  virtual         ~TATWactNtuRaw();

  virtual Bool_t  Action();

  ClassDef(TATWactNtuRaw,0)
    
  private:
    
  TAGdataDsc*     fpDatRaw;		    // input data dsc
  TAGdataDsc*     fpNtuRaw;		    // output data dsc
  TAGparaDsc*     fpParGeo;		    // parameter dsc
  TAGparaDsc*     fpParMap;

  bool m_debug;
  

 private:
  Double_t GetEnergy(TATWrawHit*a,TATWrawHit*b);
  Double_t GetTime(TATWrawHit*a,TATWrawHit*b);

};

#endif
