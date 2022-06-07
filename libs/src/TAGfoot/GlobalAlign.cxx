
/*!
 \file GlobalAlign.cxx
 \brief Implementation of GlobalAlign
 */
/*------------------------------------------+---------------------------------*/


#include "GlobalAlign.hxx"



//! Class Imp
ClassImp(GlobalAlign)

/*!
 \class GlobalAlign
 \brief Global alignment class from raw data
 */
/*------------------------------------------+---------------------------------*/


//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] fileNameIn data input file name
//! \param[in] fileNameout data output root file name
GlobalAlign::GlobalAlign(TString expName, Int_t runNumber, TString fileNameIn, TString fileNameout)
 : LocalReco(expName, runNumber, fileNameIn, fileNameout)
{
   EnableMsdTracking();
   
   TAGrecoManager::GetPar()->IncludeStraight(false);
}

//__________________________________________________________
//! default destructor
GlobalAlign::~GlobalAlign()
{
}

//__________________________________________________________
//! Loop over events
//!
//! \param[in] nEvents number of events to process
void GlobalAlign::LoopEvent(Int_t nEvents)
{
   Int_t frequency = 1;
   
   if (nEvents >= 100000)      frequency = 10000;
   else if (nEvents >= 10000)  frequency = 1000;
   else if (nEvents >= 1000)   frequency = 100;
   else if (nEvents >= 100)    frequency = 10;
   else if (nEvents >= 10)     frequency = 1;
   
   
   if (fSkipEventsN > 0)
      printf(" Skipped Event: %d\n", fSkipEventsN);
   
   for (Int_t ientry = 0; ientry < nEvents; ientry++) {
      
      if(ientry % frequency == 0)
         printf(" Loaded Event: %d\n", ientry+fSkipEventsN);
      
      if (!fTAGroot->NextEvent()) break;
      
   }
}

//__________________________________________________________
//! Actions after loop event
void GlobalAlign::AfterEventLoop()
{
   fTAGroot->EndEventLoop();
   
   if (fFlagOut)
      CloseFileOut();
   
   CloseFileIn();
}
