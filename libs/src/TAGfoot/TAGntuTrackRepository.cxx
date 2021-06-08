
#include "TAGntuTrackRepository.hxx"


ClassImp(TAGntuTrackRepository);

TString TAGntuTrackRepository::fgkBranchName = "glbtrackNEW.";

//Default constructor
TAGntuTrackRepository::TAGntuTrackRepository() : TAGdata(), fListOfTracks(new TClonesArray("TAGglobalTrack"))
{
    m_kalmanOutputDir = (string)getenv("FOOTRES")+"/Kalman_new";
    // m_debug = TAGrecoManager::GetPar()->Debug();
    m_debug = 0;
}

//Default destructor
TAGntuTrackRepository::~TAGntuTrackRepository()
{
	delete fListOfTracks;
}


/*Get number of tracks in the repo
*
*/
Int_t TAGntuTrackRepository::GetTracksN()
{
	return fListOfTracks->GetEntries();
}


/*Get track by index
*
*/
TAGglobalTrack* TAGntuTrackRepository::GetTrack(Int_t trId)
{
	if(trId >=0 && trId < GetTracksN())
		return (TAGglobalTrack*) fListOfTracks->At(trId);
	else
		return 0x0;
}

/* Setup clones
*
*/
void TAGntuTrackRepository::SetupClones()
{
   if (!fListOfTracks) {
      fListOfTracks = new TClonesArray("TAGglobalTrack");
      fListOfTracks->SetOwner(true);
   }
}


/* Add a new track to the repo
*
*/
TAGglobalTrack* TAGntuTrackRepository::NewTrack(	string name, long evNum, 
									int pdgID, int pdgCh, int measCh, float mass, 
									float length, float tof, 
									float chi2, int ndof, float pVal, 
									TVector3* recoPos_target, TVector3* recoMom_target,
									TMatrixD* recoPos_target_cov, TMatrixD* recoMom_target_cov,
									vector<TAGshoeTrackPoint*>* shoeTrackPointRepo	
									)
{
	TClonesArray &trackArray = *fListOfTracks;
	TAGglobalTrack* track = new (trackArray[trackArray.GetEntriesFast()]) TAGglobalTrack(
													name, evNum,
													pdgID, pdgCh, measCh, mass, length, tof, chi2, ndof, pVal, 
													recoPos_target, recoMom_target, recoPos_target_cov, recoMom_target_cov,
													shoeTrackPointRepo
												);
	return track;
}


/* Add a new track to the repo
*
*/
TAGglobalTrack* TAGntuTrackRepository::NewTrack()
{
	TClonesArray &trackArray = *fListOfTracks;
	TAGglobalTrack* track = new (trackArray[trackArray.GetEntriesFast()]) TAGglobalTrack();
	return track;
}


/* Add a new track to the repo
* @param -> Kalman Track
*/
TAGglobalTrack* TAGntuTrackRepository::NewTrack(TAGglobalTrack& trk)
{
	TClonesArray &trackArray = *fListOfTracks;
	TAGglobalTrack* track = new (trackArray[trackArray.GetEntriesFast()]) TAGglobalTrack(trk);
	return track;
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//! ostream insertion.
void TAGntuTrackRepository::ToStream(ostream& os, Option_t* option) const
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
