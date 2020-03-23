#ifndef Booter_hxx
#define Booter_hxx


#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

#include "Evento.hxx"

// #include "TAGpadGroup.hxx"
#include "TAGgeoTrafo.hxx"
#include "TADIparGeo.hxx"
// #include "TAGactTreeWriter.hxx"
// #include "TAGdataDsc.hxx"
#include "TAGroot.hxx"

#include "KFitter.hxx"
#include "GlobalPar.hxx"
#include "UpdatePDG.hxx"


#include <ctime>
#include <time.h>




 

class Booter {
  
 public :

 TH2F* pos2D;

   Booter(): m_wd(".") {};
  ~Booter() {};

  void Initialize( EVENT_STRUCT* evStr );
  void Process( Long64_t jentry );
  void Finalize();

  void MagFieldTest();
  void GeoPrint();


  //Actions for Tupling MC info for the various subdetectors
  void FillMCEvent(EVENT_STRUCT *myStr);
  void FillMCBeamMonitor(EVENT_STRUCT *myStr);
  void FillMCDriftChamber(EVENT_STRUCT *myStr);
  void FillMCVertex(EVENT_STRUCT *myStr);
  void FillMCInnerTracker(EVENT_STRUCT *myStr);
  void FillMCMSD(EVENT_STRUCT *myStr);
  void FillMCInteractionRegion(EVENT_STRUCT *myStr);
  void FillMCCalorimeter(EVENT_STRUCT *myStr);
  void FillMCTofWall(EVENT_STRUCT *myStr);

  //Monitors/Displays
  // void DisplayIRMonitor(TAGpadGroup* pg, EVENT_STRUCT *myStr);
  // void DisplayBeamMonitor(TAGpadGroup* pg);

  void initBMGeo(TAGparaDsc* p_bmgeo); //Beam Monitor Geometry
  void initBMCon(TAGparaDsc* beamcon);

  void bookHisto(TFile *f);
  void CalibBMVT();
  void MonitorBMVTMat();
  void MonitorBMNew(Long64_t jentry);
  void MonitorBM();
  void AssociateHitsToParticle();

  TAGgeoTrafo* fGeoTrafo;
  TADIparGeo *fDipole;
  TGeoVolume *top;

  KFitter* m_kFitter;

  fstream eventListFile;

  // time variables
 clock_t start_tot, end_tot;
 clock_t start_kal, end_kal;
 double m_tempo_kal;

  int m_nev;
  int m_debug;
  TFile* m_of;
  TFile* m_hf;
  TString m_oustr;
  TString m_instr;
  TString m_wd;

  //MC block
  TAGdataDsc* myn_mceve;
  TAGdataDsc* myn_mcmimo;

  // vector<string> my_files;

  //Parameters
  TAGparaDsc* myp_bmgeo;

  TAGparaDsc* myp_vtmap;
  TAGparaDsc* myp_vtcal;
  TAGparaDsc* myp_vtgeo;
  TAGparaDsc* myp_vtconf;

  TAGparaDsc* myp_itmap;
  TAGparaDsc* myp_itcal;
  TAGparaDsc* myp_itgeo;
  TAGparaDsc* myp_itconf;

  TAGparaDsc* myp_msdmap;
  TAGparaDsc* myp_msdcal;
  TAGparaDsc* myp_msdgeo;
  TAGparaDsc* myp_msdconf;

  //Data descriptions
 
  TAGdataDsc* myn_scraw;

  TAGparaDsc* myp_bmcon;
  TAGdataDsc* myn_bmraw;
  TAGdataDsc* myn_bmtrk;

  TAGparaDsc* myp_dccon;
  TAGdataDsc* myn_dcraw;
  TAGdataDsc* myn_dctrk;

  TAGdataDsc* myn_vtraw;
  TAGdataDsc* myn_vtclus;
  TAGdataDsc* myn_vtrk;
  TAGdataDsc* myn_vtvtx;

  TAGdataDsc* myn_itraw;
  TAGdataDsc* myn_itclus;
  TAGdataDsc* myn_itrk;

  TAGdataDsc* myn_msdraw;

  TAGdataDsc* myn_trtrk;
  TAGdataDsc* myn_trtin;

  TAGdataDsc* containerHit;
  TAGdataDsc* containerPoint;
  TAGdataDsc* myn_twraw;
  TAGparaDsc* myp_twgeo;
  TAGparaDsc* myp_cageo;
  TAGparaDsc* myp_parcal;
  TAGparaDsc* myp_pargeo;
  TAGdataDsc* myn_caraw;

   //Actions

  // TAGactTreeWriter* my_out;
  TAGaction* mya_vtraw;
  TAGaction* mya_vtclus;
  TAGaction* mya_vttrack;
  TAGaction* mya_vtvtx;

  TAGaction* mya_itraw;
  TAGaction* mya_itclus;
  TAGaction* mya_ittrack;

  TAGaction* mya_msdraw;
  TAGaction* mya_mceve;

};

#endif
