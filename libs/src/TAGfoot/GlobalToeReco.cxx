
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
GlobalToeReco::GlobalToeReco(TString expName, TString fileNameIn, TString fileNameout)
 : TAGobject()
{
   TAGactNtuGlbTrack::EnableStdAlone();
   fReco = new LocalRecoMC(expName, fileNameIn, fileNameout); // using MC framework to read back in Tree
}

//__________________________________________________________
GlobalToeReco::~GlobalToeReco()
{
   delete fReco;
}
