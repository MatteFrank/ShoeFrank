#ifndef ROOT_TAMCevent
#define ROOT_TAMCevent

#include <iostream>
#include <fstream>
#include "TObject.h"
#include "TTree.h"
#include "TVector3.h"

class TAMCntuHit ;
class TAMCntuEve ;

using namespace std;

class TAMCevent : public TObject {

 public:

  TAMCevent();
  virtual ~TAMCevent();

  Int_t Clean();

  void SetEvent(Int_t aEventNumber){ fEventNumber = aEventNumber ; }

  Int_t AddPart(Int_t aTRpaid, Int_t aTRgen, Int_t aTRcharge, Int_t aTRreg, Int_t aTRbaryon,
                Int_t aTRdead, Int_t aTRflukid, TVector3 aTRipos, TVector3 aTRfpos,
                TVector3 aTRip, TVector3 aTRfp, Double_t aTRmass, Double_t aTRtof,
                Double_t aTRtime, Double_t aTRtrlen);

  Int_t AddSTC(Int_t aSTCid, TVector3 aSTCinpos, TVector3 aSTCoutpos,
               TVector3 aSTCpin, TVector3 aSTCpout, Double_t aSTCde,
               Double_t aSTCal, Double_t aSTCtof);


  Int_t AddBMN(Int_t aBMNid, Int_t aBMNilay, Int_t aBMNiview, Int_t aBMNicell,
               TVector3 aBMNinpos, TVector3 aBMNoutpos, TVector3 aBMNpin, TVector3 aBMNpout,
               Double_t aBMNde, Double_t aBMNal, Double_t aBMNtof);

  Int_t AddVTX(Int_t aVTXid, Int_t aVTXilay, TVector3 aVTXinpos, TVector3 aVTXoutpos,
               TVector3 aVTXpin, TVector3 aVTXpout, Double_t aVTXde, Double_t aVTXal,
               Double_t aVTXtof);

  Int_t AddITR(Int_t aITRid, Int_t aITRilay, Int_t aITRiplume, Int_t aITRimimo,
               TVector3 aITRinpos, TVector3 aITRoutpos, TVector3 aITRpin,
               TVector3 aITRpout, Double_t aITRde, Double_t aITRal, Double_t aITRtof);

  Int_t AddMSD(Int_t aMSDid, Int_t aMSDilay, Int_t aMSDistripx, Int_t aMSDistripy,
               TVector3 aMSDinpos, TVector3 aMSDout, TVector3 aMSDpin,
               TVector3 aMSDpout, Double_t aMSDde, Double_t aMSDal, Double_t aMSDtof);
 
  Int_t AddTW(Int_t aTWid, Int_t aTWibar, Int_t aTWiview, TVector3 aTWinpos,
              TVector3 aTWoutpos, TVector3 aTWpin, TVector3 aTWpout, Double_t aTWde,
              Double_t aTWal, Double_t aTWtof );

  Int_t AddCAL(Int_t aCALid, Int_t aCALicry, TVector3 aCALinpos, TVector3 aCALoutpos,
               TVector3 aCALpin,TVector3 aCALpout, Double_t aCALde, Double_t aCALal,
               Double_t aCALtof);
  
  Int_t AddCROSS(Int_t aCROSSid, Int_t aCROSSnreg, Int_t aCROSSnregold, TVector3 aCROSSpos,
                 TVector3 aCROSSp, Double_t aCROSSm, Double_t aCROSSch, Double_t aCROSSt);

  void FindBranches(TTree *RootTree);
  void SetBranches(TTree *RootTree);

  void Dump() const;

 private:

    Int_t fEventNumber;

    TAMCntuEve* fTrack;
    TAMCntuHit *fHitSTC;
    TAMCntuHit *fHitBMN;
    TAMCntuHit *fHitVTX;
    TAMCntuHit *fHitITR;
    TAMCntuHit *fHitMSD;
    TAMCntuHit *fHitTW;
    TAMCntuHit *fHitCAL;

 protected:

  ClassDef(TAMCevent,1);

};

#endif
