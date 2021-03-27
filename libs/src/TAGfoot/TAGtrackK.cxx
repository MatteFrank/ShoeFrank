
#include "TAGtrackK.hxx"

ClassImp(TAGtrackK);

//Default constructor
TAGtrackK::TAGtrackK()
{
  m_track = new Track();
  m_lenght = 1.;
}

//Alternative constructor
TAGtrackK::TAGtrackK( string name, Track* track, long evNum, int stateID,
				      TVector3* mom, TVector3* pos,
				      TVector3* mom_MC, TVector3* pos_MC, 
				      TMatrixD* mom_cov 
				      ) {
  
  m_name = name;
  
  m_evNum = evNum;
  m_stateID = stateID;
  m_mom = *mom;
  m_pos = *pos;
  *m_track = *track;
  
  //		m_lenght;
  //		m_pdgID;
  //		m_chi2;
  //		m_pVal;
  //
  //		m_sigmaMom;
  //		m_sigmaPos;
}


TAGtrackK::TAGtrackK(Track* track)
{
  *m_track = *track;
}
