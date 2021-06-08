
#include "TAGtrackKalman.hxx"

ClassImp(TAGtrackKalman);

//Default constructor
TAGtrackKalman::TAGtrackKalman()
 : TAGobject(),
    m_name(""),
    m_evNum(-999),
    m_pdgID(-999),
    m_pdgCh(-999),
    m_measCh(-999),
    m_mass(-999),
    m_length(-999),
    m_tof(-999),
    m_chi2(-999),
    m_ndof(-999),
    m_pVal(-999),
    // m_pos(0,0,0),
    // m_mom(0,0,0),
	m_track(0x0),
    m_pointContainer(0x0)
{
	SetupClones();
	m_track = new Track();
}

//Alternative constructor
TAGtrackKalman::TAGtrackKalman( string name, Track* track, long evNum, 
									int pdgID, int pdgCh, int measCh, float mass, 
									float length, float tof, 
									float chi2, int ndof, float pVal, 
									TVector3* pos, TVector3* mom,
									// TVector3* mom_MC, TVector3* pos_MC, 
									TMatrixD* pos_cov, TMatrixD* mom_cov,
									vector<TAGshoeTrackPoint*>* shoeTrackPointRepo 
					) 
	: TAGobject(),
	m_track(0x0),
	m_pointContainer(0x0)
{

	SetupClones();

	m_name = name;
	m_evNum = evNum;
	m_pdgID = pdgID;
	m_pdgCh = pdgCh;
	m_measCh = measCh;
	m_mass = mass;
	m_length = length;
	m_tof = tof;
	m_chi2 = chi2;
	m_ndof = ndof;
	m_pVal = pVal;
	// m_stateID = stateID;
	m_target_mom = *mom;
	m_target_pos = *pos;
	
	if(!m_track){ m_track = new Track(*track);}

	TClonesArray &pointArray = *m_pointContainer;
	for(int i=0; i < shoeTrackPointRepo->size(); ++i)
	{
		new (pointArray[pointArray.GetEntriesFast()]) TAGshoeTrackPoint( *(shoeTrackPointRepo->at(i)) );
	}

	// m_pos_cov = *pos_cov;
	// m_mom_cov = *mom_cov;
}

TAGtrackKalman::TAGtrackKalman(Track* track)
: TAGobject(),
	m_track(0x0)
{
	if(!m_track) m_track = new Track(*track);
}



TAGtrackKalman::~TAGtrackKalman()
{
	delete m_track;
	delete m_pointContainer;
}


void TAGtrackKalman::SetupClones()
{
	if(!m_pointContainer)
	{
		m_pointContainer = new TClonesArray("TAGshoeTrackPoint");
		m_pointContainer->SetOwner(true);
	}
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAGtrackKalman::Clear(Option_t*)
{
   m_pointContainer->Delete();
}
