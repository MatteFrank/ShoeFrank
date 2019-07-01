#ifndef ROOT_Evento
#define ROOT_Evento

#include <iostream>
#include <fstream>
#include "TObject.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TH1.h"

#include "EventStruct.hxx"

using namespace std;

class Evento : public TObject {

 public:

  Evento();
  virtual ~Evento();

  Int_t Clean();

  Int_t SetEvent(Int_t fEventNumber);

  Int_t AddPart(Int_t fTRpaid, Int_t fTRgen,
		Int_t fTRcha, Int_t fTRreg, Int_t fTRbar,
		Int_t fTRdead, Int_t fTRfid,
		Double_t fTRix, Double_t fTRiy, Double_t fTRiz,
		Double_t fTRfx, Double_t fTRfy, Double_t fTRfz,
		Double_t fTRipx, Double_t fTRipy, Double_t fTRipz,
		Double_t fTRfpx, Double_t fTRfpy, Double_t fTRfpz,
		Double_t fTRmass, Double_t fTRtime,
		Double_t fTRtof, Double_t fTRtrlen);

  Int_t AddSTC(Int_t fSTCid, 
	       Double_t fSTCxin, Double_t fSTCyin, Double_t fSTCzin,
	       Double_t fSTCxout, Double_t fSTCyout, Double_t fSTCzout,
	       Double_t fSTCpxin, Double_t fSTCpyin, Double_t fSTCpzin,
	       Double_t fSTCpxout, Double_t fSTCpyout, Double_t fSTCpzout,
	       Double_t fSTCde, Double_t fSTCal, Double_t fSTCtim);

  Int_t AddBMN(Int_t fBMNid, Int_t fBMNilay, Int_t fBMNiview,
	       Int_t fBMNicell,
	       Double_t fBMNxin, Double_t fBMNyin, Double_t fBMNzin,
	       Double_t fBMNxout, Double_t fBMNyout, Double_t fBMNzout,
	       Double_t fBMNpxin, Double_t fBMNpyin, Double_t fBMNpzin,
	       Double_t fBMNpxout, Double_t fBMNpyout,
	       Double_t fBMNpzout,
	       Double_t fBMNde, Double_t fBMNal, Double_t fBMNtim);

  Int_t AddVTX(Int_t fVTXid, Int_t fVTXilay,
	       Double_t fVTXxin, Double_t fVTXyin, Double_t fVTXzin,
	       Double_t fVTXxout, Double_t fVTXyout, Double_t fVTXzout,
	       Double_t fVTXpxin, Double_t fVTXpyin, Double_t fVTXpzin,
	       Double_t fVTXpxout, Double_t fVTXpyout, Double_t fVTXpzout,
	       Double_t fVTXde, Double_t fVTXal, Double_t fVTXtim);

  Int_t AddITR(Int_t fITRid, Int_t fITRisens,
	       Double_t fITRxin, Double_t fITRyin, Double_t fITRzin,
	       Double_t fITRxout, Double_t fITRyout, Double_t fITRzout,
	       Double_t fITRpxin, Double_t fITRpyin, Double_t fITRpzin,
	       Double_t fITRpxout, Double_t fITRpyoutITR, Double_t fITRpzout,
	       Double_t fITRde, Double_t fITRal, Double_t fITRtim);

  Int_t AddMSD(Int_t fMSDid, Int_t fMSDilay, 
	       Double_t fMSDxin, Double_t fMSDyin, Double_t fMSDzin,
	       Double_t fMSDxout, Double_t fMSDyout, Double_t fMSDzout,
	       Double_t fMSDpxin, Double_t fpMSDyin, Double_t fMSDpzin,
	       Double_t fMSDpxout, Double_t fMSDpyout,
	       Double_t fMSDpzout,
	       Double_t fMSDde, Double_t fMSDal, Double_t fMSDtim);
 
  Int_t AddSCN(Int_t fSCNid, Int_t fSCNibar, Int_t fSCNiview,
	       Double_t fSCNxin, Double_t fSCNyin, Double_t fSCNzin,
	       Double_t fSCNxout, Double_t fyout, Double_t fSCNzout,
	       Double_t fSCNpxin, Double_t fpyin, Double_t fSCNpzin,
	       Double_t fSCNpxout,Double_t fSCNpyout,
	       Double_t fSCNpzout,Double_t fSCNde, Double_t fSCNal,
	       Double_t fSCNtim );

  Int_t AddCAL(Int_t fCALid, Int_t fCALicry,
	       Double_t fCALxin, Double_t fCALyin, Double_t fCALzin, 
	       Double_t fCALxout, Double_t fCALyout, Double_t fCALzout, 
	       Double_t fCALpxin, Double_t fCALpyin, Double_t fCALpzin, 
	       Double_t fCALpxout, Double_t fpyout, Double_t fCALpzout,
	       Double_t fCALde, Double_t fCALal, Double_t fCALtim);
  
  Int_t AddCROSS(Int_t fCROSSid, Int_t fCROSSnreg, Int_t fCROSSnregold,
		 Double_t fCROSSx, Double_t fCROSSy, Double_t fCROSSz,
		 Double_t fCROSSpx, Double_t fCROSSpy, Double_t fCROSSpz,
		 Double_t fCROSSm, Double_t fCROSSch,Double_t fCROSSt);

  int FindBranches(TTree *RootTree,EVENT_STRUCT *eve);
  int SetBranches(TTree *RootTree);

  void Dump() const;

  EVENT_STRUCT Output();

 private:

  TRandom3 *ar3;
  TH1D *myreso;
  EVENT_STRUCT eve;

  // 
  TRandom3 rnd;
  Int_t count_skip;

 protected:

  ClassDef(Evento,1);

};

#endif
