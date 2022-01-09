#ifndef _TAITdigitizerE_HXX
#define _TAITdigitizerE_HXX

/*!
 \file TAITdigitizerE.hxx
 \brief   Declaration of TAITdigitizerE
 */
/*------------------------------------------+---------------------------------*/


#include <map>

#include "Riostream.h"
#include "TF1.h"
#include "TH2F.h"
#include "TMath.h"
#include "TObject.h"
#include "TRandom3.h"
#include "TString.h"

#include "TAVTdigitizerE.hxx"

// --------------------------------------------------------------------------------------
class TAVTbaseParGeo;
class TAITdigitizerE : public TAVTdigitizerE {
   
   // Class to digitize the energy into pixel based on given patterns
public:
   TAITdigitizerE(TAVTbaseParGeo* parGeo);
   virtual ~TAITdigitizerE();
   
};
        

#endif

