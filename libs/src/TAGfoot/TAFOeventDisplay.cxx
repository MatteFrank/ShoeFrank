

#include "TAFOeventDisplay.hxx"

// root include
#include "Riostream.h"
#include "TGComboBox.h"
#include "TRootEmbeddedCanvas.h"
#include "TEveManager.h"
#include "TEveWindow.h"
#include "TEveBrowser.h"
#include "TEveBrowser.h"

#include "GlobalPar.hxx"

#include "TASTdatRaw.hxx"
#include "TASTntuRaw.hxx"
#include "TABMntuRaw.hxx"
#include "TAVTntuHit.hxx"
#include "TAITntuHit.hxx"
#include "TAMSDntuHit.hxx"
#include "TATWntuRaw.hxx"
#include "TATWntuPoint.hxx"
#include "TACAntuRaw.hxx"

#include "TAVTntuCluster.hxx"
#include "TAITntuCluster.hxx"
#include "TAMSDntuCluster.hxx"

#include "TAVTtrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "LocalReco.hxx"

ClassImp(TAFOeventDisplay)

TAFOeventDisplay* TAFOeventDisplay::fgInstance = 0x0;

//__________________________________________________________
TAFOeventDisplay* TAFOeventDisplay::Instance(const TString name, Int_t runNumber, Int_t type)
{
   if (fgInstance == 0x0)
      fgInstance = new TAFOeventDisplay(name, runNumber, type);

   return fgInstance;
}

//__________________________________________________________
TAFOeventDisplay::TAFOeventDisplay(const TString expName, Int_t runNumber, Int_t type)
 : TAFObaseEventDisplay(expName, runNumber, type)
{
   // local reco
   SetLocalReco();  
}

//__________________________________________________________
TAFOeventDisplay::~TAFOeventDisplay()
{
}

//__________________________________________________________
void TAFOeventDisplay::SetLocalReco()
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
