
/*!
 \class GlobalToeReco
 \brief Global reconstruction from local reconstruction Tree **
 author: Ch. Finck
 */

#include "LocalReco.hxx"
#include "LocalRecoMC.hxx"
#include "TAGactNtuGlbTrack.hxx"

#include "GlobalToeReco.hxx"


ClassImp(GlobalToeReco)

//__________________________________________________________
GlobalToeReco::GlobalToeReco(TString expName, TString fileNameIn, TString fileNameout, Bool_t isMC)
 : TAGobject()
{
   TAGactNtuGlbTrack::EnableStdAlone();

   if (isMC)
      fReco = new LocalRecoMC(expName, fileNameIn, fileNameout);
   else
      fReco = new LocalReco(expName, fileNameIn, fileNameout);
}

//__________________________________________________________
GlobalToeReco::~GlobalToeReco()
{
}
