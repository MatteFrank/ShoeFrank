/*!
 \file
 \version $Id: TAMCactNtuBm.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMCactNtuBm.
 */

#include "TAMCntuHit.hxx"
#include "TAMCactNtuBm.hxx"

//FOOT
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TAMCactNtuBm TAMCactNtuBm.hxx "TAMCactNtuBm.hxx"
 \brief NTuplizer for MC events. **
 */

ClassImp(TAMCactNtuBm);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuBm::TAMCactNtuBm(const char* name, TAGdataDsc* p_nturaw, EVENT_STRUCT* evStr)
 : TAGaction(name, "TAMCactNtuBm - NTuplize MC start counter evt"),
   fpNtuMC(p_nturaw),
   fpEvtStr(evStr)
{
   AddDataOut(p_nturaw, "TAMCntuHit");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuBm::~TAMCactNtuBm()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuBm::Action() {
   
   TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMC->Object();
   
   p_nturaw->Clear();
   
   for (Int_t i = 0; i < fpEvtStr->BMNn; i++) {
      
      Int_t layer    = fpEvtStr->BMNilay[i];
      Int_t view     = fpEvtStr->BMNicell[i];
      Int_t cellId   = fpEvtStr->BMNicell[i];
      Int_t trackIdx = fpEvtStr->BMNid[i] - 1;

      TVector3 ipos( fpEvtStr->BMNxin[i], fpEvtStr->BMNyin[i], fpEvtStr->BMNzin[i]);
      TVector3 fpos( fpEvtStr->BMNxout[i], fpEvtStr->BMNyout[i], fpEvtStr->BMNzout[i]);
      TVector3 imom( fpEvtStr->BMNpxin[i], fpEvtStr->BMNpyin[i], fpEvtStr->BMNpzin[i]);
      TVector3 fmom( fpEvtStr->BMNpxout[i],fpEvtStr->BMNpyout[i],fpEvtStr->BMNpzout[i]);

      p_nturaw->NewHit(trackIdx, layer, view, cellId, ipos, fpos, imom, fmom, fpEvtStr->BMNde[i], fpEvtStr->BMNtim[i], 0);
   }

   fpNtuMC->SetBit(kValid);
   return kTRUE;
}

