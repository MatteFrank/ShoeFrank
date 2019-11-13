#ifndef GLOBALTRACKINGREPOSITORY_H
#define GLOBALTRACKINGREPOSITORY_H

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

// #include "TVector3.h"
#include "GlobalPar.hxx"

// #include <Track.h>
#include <GlobalTrackKalman.hxx>


#define build_string(expr) \
    (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


using namespace std;
using namespace genfit;

class GlobalTrackRepostory {
  
public:

  GlobalTrackRepostory() {
    m_kalmanOutputDir = (string)getenv("FOOTRES")+"/Kalman";
    m_debug = GlobalPar::GetPar()->Debug();
    m_resoP_step = 0.2;
  };

  ~GlobalTrackRepostory() {};
  
  void AddTrack( string name, Track* track, long evNum, int stateID, 
		 TVector3* mom, TVector3* pos,
		 TVector3* mom_MC, TVector3* pos_MC, 
		 TMatrixD* mom_cov 
		 ) {
    
    m_fitTrackCollection.push_back( new GlobalTrackKalman( name, track, evNum, stateID, // trackID?
							   mom, pos,
							   mom_MC, pos_MC, 
							   mom_cov ) );
    
    PrintPositionResidual( *pos, *pos_MC, name );
    PrintMomentumResidual( *mom, *mom_MC, *mom_cov, name );
    
  };
  
  double EvalError( TVector3 mom, TVector3 err );
  double EvalError( TVector3 mom, TMatrixD cov );
  
  void PrintPositionResidual( TVector3 pos, TVector3 expectedPos, string hitSampleName );
  
  void PrintMomentumResidual( TVector3 meas, TVector3 expected, TVector3 cov, string hitSampleName );
  void PrintMomentumResidual( TVector3 meas, TVector3 expected, TMatrixD cov, string hitSampleName );
  void PrintMomentumResidual( TVector3 meas, TVector3 expected, double err, string hitSampleName );
  
  void EvaluateMomentumResolution();
  void Save();
  
  /***variables***/
  
  int m_debug;
  string m_kalmanOutputDir;
  
  vector<GlobalTrackKalman*> m_fitTrackCollection;
  
  double m_resoP_step;
  
  map<string, map<float, TH1F*> > h_dP_x_bin;
  map<string, map<float, TH1F*> > h_dPOverP_x_bin;
  map<string, map<float, TH1F*> > h_dPOverSigmaP_x_bin;
  
  map< string, TH1F* > h_resoP_over_Pkf;
  map< string, TH1F* > h_biasP_over_Pkf;
  
private:
  
  
};

#endif















