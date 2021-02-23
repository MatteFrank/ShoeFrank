/*!
 \file
 \version $Id: TAMCactNtuTof.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMCactNtuTof.
 */

#include "TAMCntuHit.hxx"
#include "TAMCactNtuTof.hxx"

//FOOT
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TAMCactNtuTof TAMCactNtuTof.hxx "TAMCactNtuTof.hxx"
 \brief NTuplizer for MC events. **
 */

ClassImp(TAMCactNtuTof);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuTof::TAMCactNtuTof(const char* name, TAGdataDsc* p_nturaw, EVENT_STRUCT* evStr)
 : TAGaction(name, "TAMCactNtuTof - NTuplize MC evt raw data"),
   fpNtuMC(p_nturaw),
   fpEvtStr(evStr)
{
   AddDataOut(p_nturaw, "TAMCntuHit");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuTof::~TAMCactNtuTof()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuTof::Action() {
   
   TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMC->Object();
   
   p_nturaw->Clear();
   
   for (Int_t i = 0; i < fpEvtStr->SCNn; i++) {
      
      Int_t trackIdx = fpEvtStr->SCNid[i];
      Int_t barId    = fpEvtStr->SCNibar[i];
      Int_t view     = fpEvtStr->SCNiview[i];
     Float_t edep   = fpEvtStr->SCNde[i];
      Float_t time   = fpEvtStr->SCNtim[i];

      TVector3 ipos( fpEvtStr->SCNxin[i], fpEvtStr->SCNyin[i], fpEvtStr->SCNzin[i]);
      TVector3 fpos( fpEvtStr->SCNxout[i], fpEvtStr->SCNyout[i], fpEvtStr->SCNzout[i]);
      TVector3 imom( fpEvtStr->SCNpxin[i], fpEvtStr->SCNpyin[i], fpEvtStr->SCNpzin[i]);
      TVector3 fmom( fpEvtStr->SCNpxout[i], fpEvtStr->SCNpyout[i], fpEvtStr->SCNpzout[i]);

      p_nturaw->NewHit(trackIdx, barId, view, -99, ipos, fpos, imom, fmom, edep, time, 0);
   }
   
   fpNtuMC->SetBit(kValid);
   
   return kTRUE;
}

