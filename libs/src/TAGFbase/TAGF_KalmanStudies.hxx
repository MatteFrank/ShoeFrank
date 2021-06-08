#ifndef TAGF_KalmanStudies_HXX
#define TAGF_KalmanStudies_HXX

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

#include <TROOT.h>
#include <TStyle.h>
#include "TCanvas.h"
#include "TH1F.h"
#include <TF1.h>
#include <TVector3.h>
#include <vector>
#include <TMatrixD.h>

// #include "TVector3.h"
#include "TAGrecoManager.hxx"


#define build_string(expr) \
    (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


using namespace std;

class TAGF_KalmanStudies {

public:

	TAGF_KalmanStudies();
	~TAGF_KalmanStudies();

	double EvalError( TVector3 mom, TVector3 err );
	double EvalError( TVector3 mom, TMatrixD cov );

	void FillMomentumInfo( TVector3 meas, TVector3 expected, TMatrixD cov, string hitSampleName, 
							map<string, TH1F*> *h_deltaP );

	void Fill_MomentumResidual( TVector3 meas, TVector3 expected, TMatrixD cov, string hitSampleName, 
							map<string, map<float, TH1F*> > * h_dPOverP_x_bin );

	void EvaluateAndFill_MomentumResolution( map<string, map<float, TH1F*> > *h_dPOverP_x_bin, 
							map<string, TH1F*>* h_resoP_over_Pkf, map<string, TH1F*> *h_biasP_over_Pkf );


private:


	int m_debug;
	float m_resoP_step;
	string m_kalmanOutputDir;





ClassDef(TAGF_KalmanStudies,2)
};

#endif















