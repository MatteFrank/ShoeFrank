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

#include "LocalReco.hxx"
#include "GlobalToeReco.hxx"

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
   SetLocalReco();  
}

//__________________________________________________________
//! Destructor
TAGeventDisplay::~TAGeventDisplay()
{
}

//__________________________________________________________
//! Set reconstruction
void TAGeventDisplay::SetLocalReco()
{
   Bool_t lrc = TAGrecoManager::GetPar()->IsLocalReco();
   
   if (fType == 0) {
      if (lrc) {
         fReco = new GlobalToeReco(fExpName, fRunNumber);
         fReco->EnableReadL0Hits();
      } else
         fReco = new LocalReco(fExpName, fRunNumber);
   } else
      Error("SetLocalReco()", "Unknown type %d", fType);

   SetRecoOptions();

   if (fgStdAloneFlag)
      LocalReco::EnableStdAlone();
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
