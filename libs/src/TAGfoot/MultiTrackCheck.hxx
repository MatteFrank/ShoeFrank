
#ifndef MultiTrackCheck_hxx
#define MultiTrackCheck_hxx


#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#include "TGraphErrors.h"
#include "TF1.h"

//MC block
#include "ControlPlotsRepository.hxx"
#include "TAMCntuEve.hxx"
#include "TAMCactNtuEve.hxx"

#include "TAVTparMap.hxx"
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTparCal.hxx"
#include "TAVTntuRaw.hxx"
#include "TAVTntuCluster.hxx"

#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTactNtuVertex.hxx"
#include "TAVTactNtuVertexPD.hxx"
#include "TAVTactNtuMC.hxx"
#include "TAVTactNtuClusterF.hxx"
#include "TAVTactNtuTrack.hxx"
#include "TAVTactNtuTrackH.hxx"
#include "TAVTactNtuTrackF.hxx"

#include "TAMSDntuRaw.hxx"
#include "TAITntuRaw.hxx"
#include "TAITparGeo.hxx"
#include "TAITntuCluster.hxx"

class MultiTrackCheck{

public :
  
  MultiTrackCheck();
  ~MultiTrackCheck() {};
  
  void Initialize( EVENT_STRUCT* evStr );
  void Process( Long64_t jentry );
  void Finalize();

private:
  
  #define UMA 0.9310986964

  int debug_multi;
  bool doMinDist;
  bool doVertex;
  
  void EfficiencyChi2Fit();
  void EfficiencyMinimumDistance();
  //void FindVertexCoordinate();
  void fint(TF1 *f1, TF1 *f2);

  double VertexZ( vector<double> m_p0,  vector<double> m_p1 ); 
  
  // //MC block
  TAGdataDsc* myn_mceve;
  TAGaction* mya_mceve;

  // //VT block
  TAVTntuRaw* myn_vtraw;
  TAGparaDsc* myp_vtgeo;
  shared_ptr<TAVTparGeo> m_VT_geo;
  
  // //IT block
  TAITntuRaw* myn_itraw;
  TAGparaDsc* myp_itgeo;
  shared_ptr<TAITparGeo> m_IT_geo;
  
  // //MSD block
  TAMSDntuRaw* myn_msdraw;


  
  vector<TAVTntuHit*> m_VT_hitCollection;
  vector<TAITntuHit*> m_IT_hitCollection;
  vector<TAMSDntuHit*> m_MSD_hitCollection;
  vector<TAMCeveTrack*> m_Frag_hitCollection;
  vector<TAMCeveTrack*> m_Trcks9hits;
  
  vector<int> m_Frag_indexCollection;
  vector< int > m_VT_partIDColl;
  
  vector<int> m_FragIdxs;
  vector<int> m_Trcks9index;
  map< string, vector<TVector3>> m_DetTrackpos;

  vector<TVector3> m_VT_Trackpos;
  vector<TVector3> m_IT_Trackpos;
  vector<TVector3> m_MSD_Trackpos;

  vector <TVector3> firstVThitlist;
  vector <TVector3> secondVThitlist;
  vector <TVector3> thirdVThitlist;
  vector <TVector3> lastVThitlist;

  vector <TVector3> firstIThitlist;
  vector <TVector3> secondIThitlist;

  vector <TVector3> firstMSDhitlist;
  vector <TVector3> secondMSDhitlist;
  vector <TVector3> thirdMSDhitlist;
  
  int tmpVT_partID, VT_partID, VT_charge;
  int tmpIT_partID, IT_partID, IT_charge;
  int tmpMSD_partID, MSD_partID, MSD_charge;
  
  int distanceok;
  int distancetot;
  int pos1, pos2, pos3, pos4;
  
  int  badchi2truetrack;
  int  tottrackwith9hits;
  int  tottrackwithatleastonehitperdet;
  int  tottrack;
  int  Chi2oftruetrack;
  int  Chi2ofallgoodtrack;
  int  Chi2goodwithtruetrack;
  int  goodchi2truetrack;
  
  bool Truetrack;
  bool TwoTrackEvent;
  
  vector<double> m_p0_zx;
  vector<double> m_p0_zy;

  vector<double> m_p1_zx;
  vector<double> m_p1_zy;
  
  
  ControlPlotsRepository* m_Plotter;
  };
#endif
