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
      
      Int_t viewId  = ( fpEvtStr->SCNiview[i] == -1 ? 0 : 1 );
      Int_t trackId = fpEvtStr->SCNid[i] - 1;
      Int_t barId   = fpEvtStr->SCNibar[i];
      Int_t view    = fpEvtStr->SCNiview[i];

      TVector3 ipos( fpEvtStr->SCNxin[i], fpEvtStr->SCNyin[i], fpEvtStr->SCNzin[i]);
      TVector3 fpos( fpEvtStr->SCNxout[i], fpEvtStr->SCNyout[i], fpEvtStr->SCNzout[i]);
      TVector3 imom( fpEvtStr->SCNpxin[i], fpEvtStr->SCNpyin[i], fpEvtStr->SCNpzin[i]);
      TVector3 fmom( fpEvtStr->SCNpxout[i], fpEvtStr->SCNpyout[i], fpEvtStr->SCNpzout[i]);

      p_nturaw->NewHit(viewId, barId, view, -99, ipos, fpos, imom, fmom, fpEvtStr->SCNde[i], fpEvtStr->SCNtim[i], trackId);
   }
   
   fpNtuMC->SetBit(kValid);
   
   return kTRUE;
}

