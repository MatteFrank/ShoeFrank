
#ifndef _GlobalAlign_HXX_
#define _GlobalAlign_HXX_

/*!
 \file GlobalAlign.hxx
 \brief Reconstruction class from raw data
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "LocalReco.hxx"


class GlobalAlign : public LocalReco
{
public:
   // default constructor
   GlobalAlign(TString expName = "", Int_t runNumber = -1, TString fileNameIn = "", TString fileNameout = "");
   
   virtual ~GlobalAlign();
   
   // Loop events
   virtual void LoopEvent(Int_t nEvents);

   // End loop
   virtual void AfterEventLoop();

private:
 


   ClassDef(GlobalAlign, 1); ///< Base class for event display
};


#endif
