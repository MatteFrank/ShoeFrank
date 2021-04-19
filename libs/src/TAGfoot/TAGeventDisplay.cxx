

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

ClassImp(TAGeventDisplay)

TAGeventDisplay* TAGeventDisplay::fgInstance = 0x0;

//__________________________________________________________
TAGeventDisplay* TAGeventDisplay::Instance(const TString name, Int_t runNumber, Int_t type)
{
   if (fgInstance == 0x0)
      fgInstance = new TAGeventDisplay(name, runNumber, type);

   return fgInstance;
}

//__________________________________________________________
TAGeventDisplay::TAGeventDisplay(const TString expName, Int_t runNumber, Int_t type)
 : TAGbaseEventDisplay(expName, runNumber, type)
{
   // local reco
   SetLocalReco();  
}

//__________________________________________________________
TAGeventDisplay::~TAGeventDisplay()
{
}

//__________________________________________________________
void TAGeventDisplay::SetLocalReco()
{
   if (fType == 0)
      fReco = new LocalReco(fExpName);
   else
      Error("SetLocalReco()", "Unknown type %d", fType);

   fReco->DisableTree();
   fReco->DisableSaveHits();
   fReco->EnableHisto();

   if (fgTrackFlag) {
      fReco->SetVtxTrackingAlgo(fgVtxTrackingAlgo[0]);
      fReco->EnableTracking();
   }

   if (fgStdAloneFlag)
      LocalReco::EnableStdAlone();

   fpFootGeo = fReco->GetGeoTrafo();
  
   gTAGroot->SetRunNumber(fRunNumber);
}
