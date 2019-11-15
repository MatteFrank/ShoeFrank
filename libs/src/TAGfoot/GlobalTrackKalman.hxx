#ifndef GLOBALTRACKKALMAN_H
#define GLOBALTRACKKALMAN_H

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


#define build_string(expr)						\
  (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


using namespace std;
using namespace genfit;

class GlobalTrackKalman {
  
public:
  
  GlobalTrackKalman( string name, Track* track, long evNum, int stateID, 
		     TVector3* mom, TVector3* pos,
		     TVector3* mom_MC, TVector3* pos_MC, 
		     TMatrixD* mom_cov 
		     );
  
  ~GlobalTrackKalman() {};
  
  
  /***	variables	***/
  
  string m_name;
  
  long m_evNum;
  int m_stateID;
  TVector3 m_mom;
  TVector3 m_pos;
  
  double m_lenght;
  int m_pdgID;
  double m_chi2;
  double m_pVal;
  
  TVector3 m_sigmaMom;
  TVector3 m_sigmaPos;
  
private:
  
  
};

#endif















