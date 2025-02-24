/*!
 \file TAGeventDisplay.cxx
 \brief FOOT class to work on event display
 */
/*------------------------------------------+---------------------------------*/


#include "TAGeventDisplay.hxx"

// root include
#include "Riostream.h"
#include "TGComboBox.h"
#include "TRootEmbeddedCanvas.h"
#include "TEveManager.h"
#include "TEveWindow.h"
#include "TEveBrowser.h"
#include "TEveBrowser.h"

#include "TAGrecoManager.hxx"

#include "TASTntuRaw.hxx"
#include "TASTntuHit.hxx"
#include "TABMntuHit.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWntuHit.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuHit.hxx"

#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"

#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "RecoRaw.hxx"
#include "GlobalToeReco.hxx"
#include "GlobalReco.hxx"

//! Class Imp
ClassImp(TAGeventDisplay)

/*!
 \class TAGeventDisplay
 \brief FOOT class to work on event display
 */
/*------------------------------------------+---------------------------------*/

TAGeventDisplay* TAGeventDisplay::fgInstance = 0x0;

//__________________________________________________________
//! Instance
//!
//! \param[in] name experiment name
//! \param[in] runNumber run number
//! \param[in] type toggle for raw or MC data
TAGeventDisplay* TAGeventDisplay::Instance(const TString name, Int_t runNumber, Int_t type)
{
   if (fgInstance == 0x0)
      fgInstance = new TAGeventDisplay(name, runNumber, type);

   return fgInstance;
}

//__________________________________________________________
//! Constructor
//!
//! \param[in] expName experiment name
//! \param[in] runNumber run number
//! \param[in] type toggle for raw or MC data
TAGeventDisplay::TAGeventDisplay(const TString expName, Int_t runNumber, Int_t type)
 : TAGbaseEventDisplay(expName, runNumber, type)
{
   // local reco
   SetReco();  
}

//__________________________________________________________
//! Destructor
TAGeventDisplay::~TAGeventDisplay()
{
}

//__________________________________________________________
//! Set reconstruction
void TAGeventDisplay::SetReco()
{
   Bool_t lrc = TAGrecoManager::GetPar()->IsFromLocalReco();
   Bool_t toe = TAGrecoManager::GetPar()->IncludeTOE();
   Bool_t gf  = TAGrecoManager::GetPar()->IncludeKalman();

   if (fType == 0) {
      if (lrc) {
         if (toe)
            fReco = new GlobalToeReco(fExpName, fRunNumber);
         if (gf)
            fReco = new GlobalReco(fExpName, fRunNumber);

         fReco->EnableReadL0Hits();
      } else
         fReco = new RecoRaw(fExpName, fRunNumber);
   } else
      Error("SetRecoRaw()", "Unknown type %d", fType);

   SetRecoOptions();

   if (fgStdAloneFlag)
      RecoRaw::EnableStdAlone();
}

//__________________________________________________________
//! Skip events
Bool_t TAGeventDisplay::SkipEvent()
{
   Int_t nEvts = fNumberEvent->GetIntNumber();
   fCurrentEventId += nEvts;
   gTAGroot->SetEventNumber(fCurrentEventId-1);
   
  return fReco->GoEvent(nEvts);
}
