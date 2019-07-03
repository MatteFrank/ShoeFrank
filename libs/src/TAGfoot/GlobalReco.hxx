
#ifndef _GlobalReco_HXX_
#define _GlobalReco_HXX_

#include "TString.h"

#include "TAGactNtuGlbTrack.hxx"
#include "LocalReco.hxx"


class GlobalReco : public LocalReco 
{
public:
   //! default constructor
   GlobalReco(TString expName, TString fileNameIn, TString fileNameout);
   
   virtual ~GlobalReco();
   
   //! Create glb action
   virtual void CreateRecAction();
   
   //! Add required items
   virtual void AddRequiredItem();
   
   //! Set rec histogram directory
   virtual void SetHistogramDir();
   

protected:
   TAGdataDsc*           fpNtuGlbTrack;     // input data dsc
   TAGactNtuGlbTrack*    fActGlbTrack;    // Global tracking action
   
   ClassDef(GlobalReco, 1); // Base class for event display
};


#endif
