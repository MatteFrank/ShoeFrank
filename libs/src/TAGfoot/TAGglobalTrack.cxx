
#include "TAGglobalTrack.hxx"

// ClassImp(TAGglobalTrack);

//Default constructor
TAGglobalTrack::TAGglobalTrack()
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
    m_pointContainer(0x0)
{
	SetupClones();
}

//Alternative constructor
TAGglobalTrack::TAGglobalTrack( string name, long evNum, 
								int pdgID, int pdgCh, int measCh, float mass, 
								float length, float tof, 
								float chi2, int ndof, float pVal, 
								TVector3* pos, TVector3* mom,
								// TVector3* mom_MC, TVector3* pos_MC, 
								TMatrixD* pos_cov, TMatrixD* mom_cov,
								vector<TAGshoeTrackPoint*>* shoeTrackPointRepo 
					) 
	: TAGobject(),
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

	TClonesArray &pointArray = *m_pointContainer;
	for(int i=0; i < shoeTrackPointRepo->size(); ++i)
	{
		new (pointArray[pointArray.GetEntriesFast()]) TAGshoeTrackPoint( *(shoeTrackPointRepo->at(i)) );
	}

	// m_pos_cov = *pos_cov;
	// m_mom_cov = *mom_cov;
}


TAGglobalTrack::~TAGglobalTrack()
{
	delete m_pointContainer;
}


void TAGglobalTrack::SetupClones()
{
	if(!m_pointContainer)
	{
		m_pointContainer = new TClonesArray("TAGshoeTrackPoint");
		m_pointContainer->SetOwner(true);
	}
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TAGglobalTrack::Clear(Option_t*)
{
   m_pointContainer->Delete();
}
