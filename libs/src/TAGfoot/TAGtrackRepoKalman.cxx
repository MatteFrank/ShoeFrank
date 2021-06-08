
#include "TAGtrackRepoKalman.hxx"


ClassImp(TAGtrackRepoKalman);

TString TAGtrackRepoKalman::fgkBranchName = "glbtrackGF.";

//Default constructor
TAGtrackRepoKalman::TAGtrackRepoKalman() : TAGdata(), fListOfTracks(new TClonesArray("TAGtrackKalman"))
{
    m_kalmanOutputDir = (string)getenv("FOOTRES")+"/Kalman";
    // m_debug = TAGrecoManager::GetPar()->Debug();
    m_debug = 0;
}

//Default destructor
TAGtrackRepoKalman::~TAGtrackRepoKalman()
{
	delete fListOfTracks;
}


/*Get number of tracks in the repo
*
*/
Int_t TAGtrackRepoKalman::GetTracksN()
{
	return fListOfTracks->GetEntries();
}


/*Get track by index
*
*/
TAGtrackKalman* TAGtrackRepoKalman::GetTrack(Int_t trId)
{
	if(trId >=0 && trId < GetTracksN())
		return (TAGtrackKalman*) fListOfTracks->At(trId);
	else
		return 0x0;
}

/* Setup clones
*
*/
void TAGtrackRepoKalman::SetupClones()
{
   if (!fListOfTracks) {
      fListOfTracks = new TClonesArray("TAGtrackKalman");
      fListOfTracks->SetOwner(true);
   }
}


/* Add a new track to the repo
*
*/
TAGtrackKalman* TAGtrackRepoKalman::NewTrack(	string name, Track* trackGF, long evNum, 
									int pdgID, int pdgCh, int measCh, float mass, 
									float length, float tof, 
									float chi2, int ndof, float pVal, 
									TVector3* recoPos_target, TVector3* recoMom_target,
									TMatrixD* recoPos_target_cov, TMatrixD* recoMom_target_cov,
									vector<TAGshoeTrackPoint*>* shoeTrackPointRepo	
									)
{
	TClonesArray &trackArray = *fListOfTracks;
	TAGtrackKalman* track = new (trackArray[trackArray.GetEntriesFast()]) TAGtrackKalman(
													name, trackGF, evNum,
													pdgID, pdgCh, measCh, mass, length, tof, chi2, ndof, pVal, 
													recoPos_target, recoMom_target, recoPos_target_cov, recoMom_target_cov,
													shoeTrackPointRepo
												);
	return track;
}


/* Add a new track to the repo
*
*/
TAGtrackKalman* TAGtrackRepoKalman::NewTrack(Track* trk)
{
	TClonesArray &trackArray = *fListOfTracks;
	TAGtrackKalman* track = new (trackArray[trackArray.GetEntriesFast()]) TAGtrackKalman(trk);
	return track;
}



/* Add a new track to the repo
*
*/
TAGtrackKalman* TAGtrackRepoKalman::NewTrack()
{
	TClonesArray &trackArray = *fListOfTracks;
	TAGtrackKalman* track = new (trackArray[trackArray.GetEntriesFast()]) TAGtrackKalman();
	return track;
}


/* Add a new track to the repo
* @param -> Kalman Track
*/
TAGtrackKalman* TAGtrackRepoKalman::NewTrack(TAGtrackKalman& trk)
{
	TClonesArray &trackArray = *fListOfTracks;
	TAGtrackKalman* track = new (trackArray[trackArray.GetEntriesFast()]) TAGtrackKalman(trk);
	return track;
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! ostream insertion.
void TAGtrackRepoKalman::ToStream(ostream& os, Option_t* option) const
{
   //TODO properly
   os << "globTrackRepo " << endl; //GetName()
//    << Form("  nPixels=%3d", GetTracksN())
//    << endl;
   
//    for (Int_t j = 0; j < GetTracksN(); j++) {
//       const GlobalTrack*  track = GetTrack(j);
//       if (track)
//          os << Form("%4d", track->GetTrackId());
//       os << endl;
//    }
}
