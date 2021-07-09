	#ifndef TAGglobalTrack_H
	#define TAGglobalTrack_H

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

	#include "TClonesArray.h"
	#include "TAGobject.hxx"
	#include "TAGshoeTrackPoint.hxx"
	#include "TAGntuGlbTrack.hxx"

	#define build_string(expr)						\
	(static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


	using namespace std;

	class TAGglobalTrack : public TAGobject {
	
	public:
	
	TAGglobalTrack();
	TAGglobalTrack( string name, long evNum, 
					int pdgID, int pdgCh, int measCh, float mass, 
					float length, float tof, 
					float chi2, int ndof, float pVal, 
					TVector3* pos, TVector3* mom,
					// TVector3* mom_MC, TVector3* pos_MC, 
					TMatrixD* pos_cov, TMatrixD* mom_cov,
					vector<TAGshoeTrackPoint*>* shoeTrackPointRepo 
			);
	
	virtual ~TAGglobalTrack();
	
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
	int m_pdgID;							// PDG ID used in the fit
	int m_pdgCh;							// charge used in the fit by the cardinal rep
	int m_measCh;							// charge measured at state 0 (VTX 1st ly)

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

	TVector3 m_target_mom; 
	TVector3 m_target_pos;
	
	TVector3 m_target_mom_err;
	TVector3 m_target_pos_err;

	TMatrixD m_pos_cov;
	TMatrixD m_mom_cov;

	/// ONLY MC QUANTITIES  //////////

	int m_MCparticle_id;					//	ID in the shoe particle container ( TAMCpart )
	float m_trackQuality;					//	ratio of (N of most frequent particle measure)/(N all measurements)

	/////////////////////////////////

	TClonesArray*   m_pointContainer;

	ClassDef(TAGglobalTrack, 2)
};

#endif








