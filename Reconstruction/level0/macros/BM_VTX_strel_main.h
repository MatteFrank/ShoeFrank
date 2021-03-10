#if !defined(__CINT__) || defined(__MAKECINT__)

#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TVector3.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TProfile.h>
#include <TBrowser.h>


#include "TAGparGeo.hxx"

#include "TABMparGeo.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"
#include "TABMntuTrack.hxx"

#include "TAVTparGeo.hxx"
#include "TAVTntuHit.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuTrack.hxx"
#include "TAVTntuVertex.hxx"

#include "TAGgeoTrafo.hxx"

#endif

//fixed par
#define DEG2RAD	 0.01745329251994329577
#define RAD2DEG	 57.2957795130823208768

//general par
#define debug            1000
#define bmmcstudy        0    //<0: real data study; ==0 mc study with vtx events; >0 study with only bm and bm mc events, 
#define MCSTREL          8    //8=first strel*0.8/0.78, 1=first strel*1./0.78 otherwise=garfield strel       
#define STBIN          100    //strel binning on rdrift/tdrift
#define BMNCUT          10.   //cut on bm track chi2 
#define MAXBMHITNUM     30    //number of maximum bm hit 
#define NUMEVTSHIFT      1    //shift of the MSD evnum with respect to BM evnum (to be checked carefully!!)     
#define NUMEVT2SHIFT     0    //shift of the MSD evnum with respect to BM evnum after changeshift 
#define CHANGESHIFT      999999999 //event number in which numevtshift change to numevt2shift      
#define CORRMINIMUM      0.9 //minimum value of the correlation factor between the pvers of the BM and the vtx to evaluate the new strel      

//~ #include "../../../Simulation/foot_geo.h"

//bm theorical geo par
#define BMISOZ         -14.   //position of the BM with respect to ISO 
#define BMISOX         0.             
#define BMISOY         0.            
#define BMISOXZANGLE   0.     //rotation of the BM with respect to ISO        
#define BMISOYZANGLE   0.             

//bm residual calculated shift & rotation
//~ #define BMSHIFTX       0.
//~ #define BMSHIFTY       0.
//~ #define BMSHIFTZ       0.
//~ #define BMYZANGLE      0.     //bm rotation around X axis in deg (provv)        
//~ #define BMXZANGLE      0.    //bm rotation around Y axis in deg (provv)        

//VTX residual calculated shift & rotation (a tentative)
#define VTXLAYERNUM       4
#define VTXISOZ       1.5
#define VTXISOX       0.
#define VTXISOY       0.
//~ #define VTXSHIFTX       0.
//~ #define VTXSHIFTY       0.
//~ #define VTXSHIFTZ       0.
#define VTXYZANGLE      0.     //VTX rotation around X axis in deg (provv)        
#define VTXXZANGLE      0.     //VTX rotation around Y axis in deg (provv)  

//bm FIXED par
#define BMN_MYLAR1Z	      -10.50125 //mylar1 position with respecto to bm center
#define BMN_MYLAR2Z	      10.50125

//provv: ISO system: where everything is alligned 

typedef struct BM_evstruct {
  int hitnum;//not from ttree
  double bm_track_chi2;
  TVector3 bm_track_pvers;//in the bm system of reference
  TVector3 bm_track_r0pos;//in the bm system of reference
  double bm_hit_rdrift[MAXBMHITNUM];
  double bm_hit_residual[MAXBMHITNUM];
  double bm_hit_time[MAXBMHITNUM];
  int bm_hit_cellid[MAXBMHITNUM];
  double bm_hit_realrdrift[MAXBMHITNUM];
  int status;

} BM_evstruct;

typedef struct vtx_evstruct {
  double vtx_track_chi2tot;
  double vtx_track_chi2uview;
  double vtx_track_chi2vview;
  TVector3 vtx_track_pvers;
  TVector3 vtx_track_r0pos;
  int status;
} vtx_evstruct;

//general functions
void readallMSDfile();
void BookingBMVTX(TFile* f_out, bool onlyVTX, bool merging);
void Printoutput(TFile* f_out, vector<BM_evstruct> &allbmeventin, vector<vtx_evstruct> &allvtxeventin, vector<vector<double>> &space_residual, vector<vector<double>> &time_residual, vector<vector<int>> &selected_index, bool onlyVTX);
void EvaluateSpaceResidual(vector<vector<double>> &space_residual,vector<vector<double>> &time_residual, BM_evstruct &bmevent, vtx_evstruct &vtxevent, vector<TVector3> &wire_pos, vector<TVector3> &wire_dir);
void Allign_estimate();
void fitPositionResidual();
void fitStrel(TFile *f_out, const int index, TF1* first_strel_tf1_1, TF1* first_strel_tf1_08 ,TF1* garfield_strel_tf1);
void merge_graphics(TFile* infile, TFile* f_out);

//bm functions
TVector3 BMlocaltoiso(TVector3 local);//from local to labo syst of ref., including the shift
void clean_bmevstruct(BM_evstruct &bmevstruct, bool forced);
void bmreadevent(TABMparGeo* geomap, TABMntuHit*  bmRawHit, TABMntuTrackTr* bmtrack, BM_evstruct &bmevent, vector<BM_evstruct> &allbmeventin);
TVector3 ExtrapolateZ(TVector3 pvers, TVector3 r0pos, double proposz, bool global,  bool beammonitor);
void print_bmevstruct(BM_evstruct &bmevstruct);

  
//vtx functions
void clean_vtxevstruct(vtx_evstruct &vtxevstruct);
//~ bool vtxreadevent(vtx_evstruct &vtxevent,TTreeReader &vtxReader, TTreeReaderValue<int> &evnum, TTreeReaderValue<int> &tracknum, TTreeReaderValue<double> &chi2tot, TTreeReaderValue<double> &chi2uview, TTreeReaderValue<double> &chi2vview, TTreeReaderValue<double> &r0x, TTreeReaderValue<double> &r0y, TTreeReaderValue<double> &pversx, TTreeReaderValue<double> &pversy);
void print_vtxevstruct(vtx_evstruct &vtxevstruct);
void fitVTXgraph();
void vtxpversr02global(const TVector3 vtxpvers, const TVector3 vtxr0pos, TVector3 &pversout, TVector3 &r0posout );

