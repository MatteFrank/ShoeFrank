#ifndef TAGFTRACKKALMAN_H
#define TAGFTRACKKALMAN_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <array>
#include <sys/stat.h>

#include "TVector3.h"

#include <Track.h>
#include "TAGobject.hxx"
#include "TAGshoeTrackPoint.hxx"

#define build_string(expr)						\
  (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


using namespace std;
using namespace genfit;

class TAGtrackKalman : public TAGobject {
  
public:
  
  TAGtrackKalman();
  TAGtrackKalman( string name, Track* track, long evNum, 
					int pdgID, int pdgCh, int measCh, float mass, 
					float length, float tof, 
					float chi2, int ndof, float pVal, 
					TVector3* pos, TVector3* mom,
					// TVector3* mom_MC, TVector3* pos_MC, 
					TMatrixD* pos_cov, TMatrixD* mom_cov,
					vector<TAGshoeTrackPoint*>* shoeTrackPointRepo 
			);
	TAGtrackKalman(Track* track);
	
	virtual ~TAGtrackKalman();
	

	//! Get list of measured points
	TClonesArray*	GetListOfMeasPoints()	const { return m_pointContainer; }
	
	//! Get number of measured points
	Int_t			GetMeasPointsN()	const { return m_pointContainer->GetEntries(); }
	
	//! Get measured point
	TAGshoeTrackPoint*	GetMeasPoint(Int_t index)	{ return (TAGshoeTrackPoint*)m_pointContainer->At(index); }
  
  /***	variables	***/

private:

	void Clear(Option_t* opt="");
	void SetupClones();

	string m_name;
	
	long m_evNum;
	int m_pdgID;
	int m_pdgCh;
	int m_measCh;

	double m_length;
	double m_tof;
	double m_mass;
	// double m_eLoss;
	// double m_dedx_tof;

	double m_chi2;
	int m_ndof;
	double m_pVal;

	// int m_mcParticleID;
	// vector<int> m_mcParticleID;

	Track* m_track;

	TVector3 m_target_mom;
	TVector3 m_target_pos;
	
	TVector3 m_target_mom_err;
	TVector3 m_target_pos_err;

	TMatrixD m_pos_cov;
	TMatrixD m_mom_cov;

	TClonesArray*   m_pointContainer;

	ClassDef(TAGtrackKalman, 2)
};

#endif
