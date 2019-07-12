
#ifndef _GlobalRecoMC_HXX_
#define _GlobalRecoMC_HXX_

#include "TString.h"

#include "TAGactNtuGlbTrack.hxx"
#include "LocalRecoMC.hxx"


class GlobalRecoMC : public LocalRecoMC
{
public:
   //! default constructor
   GlobalRecoMC(TString expName, TString fileNameIn, TString fileNameout);
   
   virtual ~GlobalRecoMC();
   
   //! Create glb action
   virtual void CreateRecAction();
   
   //! Add required items
   virtual void AddRecRequiredItem();
   
   //! Set rec histogram directory
   virtual void SetHistogramDir();
   

protected:
   TAGdataDsc*           fpNtuGlbTrack;     // input data dsc
   TAGactNtuGlbTrack*    fActGlbTrack;    // Global tracking action
   
   ClassDef(GlobalRecoMC, 1); // Base class for event display
};


#endif
