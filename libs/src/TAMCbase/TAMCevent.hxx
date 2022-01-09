#ifndef ROOT_TAMCevent
#define ROOT_TAMCevent

/*!
 \file TAMCevent.hxx
 \brief   Declaration of TAMCevent.
 */
/*------------------------------------------+---------------------------------*/


#include <iostream>
#include <fstream>
#include "TObject.h"
#include "TTree.h"
#include "TVector3.h"

class TAMCntuHit ;
class TAMCntuEvent ;
class TAMCntuPart ;
class TAMCntuRegion;
class TAMCntuEvent ;
using namespace std;

class TAMCevent : public TObject {

 public:
  TAMCevent();
  virtual ~TAMCevent();

  //! Clean
  Int_t Clean();

  //! Add event number
  void AddEvent(Int_t nb);
  //! Add particle
  void AddPart(Int_t aTRpaid, Int_t aTRgen, Int_t aTRcharge, Int_t aTRreg, Int_t aTRbaryon,
                Int_t aTRdead, Int_t aTRflukid, TVector3 aTRipos, TVector3 aTRfpos,
                TVector3 aTRip, TVector3 aTRfp, Double_t aTRmass, Double_t aTRtof,
                Double_t aTRtime, Double_t aTRtrlen);
  //! Add STC hits
  void AddSTC(Int_t aSTCid, TVector3 aSTCinpos, TVector3 aSTCoutpos,
               TVector3 aSTCpin, TVector3 aSTCpout, Double_t aSTCde,
               Double_t aSTCal, Double_t aSTCtof, Int_t atrId=-99);

  //! Add BM hits
  void AddBMN(Int_t aBMNid, Int_t aBMNilay, Int_t aBMNiview, Int_t aBMNicell,
               TVector3 aBMNinpos, TVector3 aBMNoutpos, TVector3 aBMNpin, TVector3 aBMNpout,
               Double_t aBMNde, Double_t aBMNal, Double_t aBMNtof, Int_t atrId=-99);
  //! Add VTX hits
  void AddVTX(Int_t aVTXid, Int_t aVTXilay, TVector3 aVTXinpos, TVector3 aVTXoutpos,
               TVector3 aVTXpin, TVector3 aVTXpout, Double_t aVTXde, Double_t aVTXal,
               Double_t aVTXtof, Int_t atrId=-99);
  //! Add ITR hits
  void AddITR(Int_t aITRid, Int_t aITRilay,
               TVector3 aITRinpos, TVector3 aITRoutpos, TVector3 aITRpin,
               TVector3 aITRpout, Double_t aITRde, Double_t aITRal, Double_t aITRtof, Int_t atrId=-99);
  //! Add MSD hits
  void AddMSD(Int_t aMSDid, Int_t aMSDilay,
               TVector3 aMSDinpos, TVector3 aMSDout, TVector3 aMSDpin,
               TVector3 aMSDpout, Double_t aMSDde, Double_t aMSDal, Double_t aMSDtof, Int_t atrId=-99);
  //! Add TW hits
  void AddTW(Int_t aTWid, Int_t aTWibar, Int_t aTWiview, TVector3 aTWinpos,
              TVector3 aTWoutpos, TVector3 aTWpin, TVector3 aTWpout, Double_t aTWde,
              Double_t aTWal, Double_t aTWtof, Int_t atrId=-99 );
  //! Add CAL hits
  void AddCAL(Int_t aCALid, Int_t aCALicry, TVector3 aCALinpos, TVector3 aCALoutpos,
               TVector3 aCALpin,TVector3 aCALpout, Double_t aCALde, Double_t aCALal,
               Double_t aCALtof, Int_t atrId=-99);
  //! Add crossing regions
  void AddCROSS(Int_t aCROSSid, Int_t aCROSSnreg, Int_t aCROSSnregold, TVector3 aCROSSpos,
                 TVector3 aCROSSp, Double_t aCROSSm, Double_t aCROSSch, Double_t aCROSSt);

  //! Find branches in MC tree
  void FindBranches(TTree *RootTree);
  //! Set branches in MC tree
  void SetBranches(TTree *RootTree);

  //! Dump
  void Dump() const;

public:
   // Getters
   //! Get Event container
   TAMCntuEvent*  GetNtuEvent() const { return fEvent;  }
   //! Get particle container
   TAMCntuPart*   GetNtuTrack() const { return fTrack;  }
   //! Get region container
   TAMCntuRegion* GetNtuReg()   const { return fRegion; }
   //! Get STC hits container
   TAMCntuHit*    GetHitSTC()   const { return fHitSTC; }
   //! Get BM hits container
   TAMCntuHit*    GetHitBMN()   const { return fHitBMN; }
   //! Get VTX hits container
   TAMCntuHit*    GetHitVTX()   const { return fHitVTX; }
   //! Get ITR hits container
   TAMCntuHit*    GetHitITR()   const { return fHitITR; }
   //! Get MSD hits container
   TAMCntuHit*    GetHitMSD()   const { return fHitMSD; }
   //! Get TW hits container
   TAMCntuHit*    GetHitTW()    const { return fHitTW;  }
   //! Get CAL hits container
   TAMCntuHit*    GetHitCAL()   const { return fHitCAL; }

 private:
   TAMCntuEvent*  fEvent;   ///< Event container
   TAMCntuPart*   fTrack;   ///< Particle container
   TAMCntuRegion* fRegion;  ///< Region container
   TAMCntuHit*    fHitSTC;  ///< STC hits container
   TAMCntuHit*    fHitBMN;  ///< BM hits container
   TAMCntuHit*    fHitVTX;  ///< VTX hits container
   TAMCntuHit*    fHitITR;  ///< ITR hits container
   TAMCntuHit*    fHitMSD;  ///< MSD hits container
   TAMCntuHit*    fHitTW;   ///< TW hits container
   TAMCntuHit*    fHitCAL;  ///< CAL hits container

  ClassDef(TAMCevent,1);

};

#endif
