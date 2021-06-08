#ifndef TAGFTRACKREPOKALMAN_H
#define TAGFTRACKREPOKALMAN_H

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

#include "TAGrecoManager.hxx"

#include <TAGtrackKalman.hxx>
#include <TAGshoeTrackPoint.hxx>
#include "TAGdata.hxx"


#define build_string(expr) \
    (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


using namespace std;
using namespace genfit;

class TAGtrackRepoKalman : public TAGdata {

private:
  TClonesArray*     fListOfTracks;

  static TString fgkBranchName;

public:

  TAGtrackRepoKalman();

  virtual ~TAGtrackRepoKalman();

  TAGtrackKalman* GetTrack(Int_t trId);
  Int_t              GetTracksN();

  TClonesArray*      GetListOfTracks() {return fListOfTracks;}

  TAGtrackKalman* NewTrack();
  TAGtrackKalman* NewTrack( 	string name, Track* track, long evNum, 
									int pdgID, int pdgCh, int measCh, float mass, 
									float length, float tof, 
									float chi2, int ndof, float pVal, 
									TVector3* pos, TVector3* mom,
									// TVector3* mom_MC, TVector3* pos_MC, 
									TMatrixD* pos_cov, TMatrixD* mom_cov,
									vector<TAGshoeTrackPoint*>* shoeTrackPointRepo
							);
  TAGtrackKalman* NewTrack(TAGtrackKalman& track);
  TAGtrackKalman* NewTrack(Track* track);

  virtual void      SetupClones();
  virtual void      Clear(Option_t* opt="") { fListOfTracks->Delete(); }

  virtual void    ToStream(ostream& os, Option_t* option) const;

  static const      Char_t* GetBranchName() { return fgkBranchName.Data(); }

  // void AddTrack(  string name, Track* track, long evNum, 
		// 					int pdgID, int pdgCh, int measCh, float mass, 
		// 					float length, float tof, 
		// 					float chi2, int ndof, float pVal, 
		// 					TVector3* pos, TVector3* mom,
		// 					// TVector3* mom_MC, TVector3* pos_MC, 
		// 					TMatrixD* pos_cov, TMatrixD* mom_cov,
		// 					vector<TAGshoeTrackPoint*> shoeTrackPointRepo
		// 		);
	
  // //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // void AddTrack( string name, Track* track, long evNum, int stateID,
  //   const TVectorD state, const TMatrixDSym covariance, TVector3* mom_MC, TVector3* pos_MC, int charge
  // );
  // //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



  /***variables***/

  int m_debug;
  string m_kalmanOutputDir;

  ClassDef(TAGtrackRepoKalman,2)
};

#endif
