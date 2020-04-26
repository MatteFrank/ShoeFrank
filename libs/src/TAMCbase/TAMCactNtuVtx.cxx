/*!
 \file
 \version $Id: TAMCactNtuVtx.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMCactNtuVtx.
 */

#include "TAMCntuHit.hxx"
#include "TAMCactNtuVtx.hxx"

//FOOT
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TAMCactNtuVtx TAMCactNtuVtx.hxx "TAMCactNtuVtx.hxx"
 \brief NTuplizer for MC events. **
 */

ClassImp(TAMCactNtuVtx);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuVtx::TAMCactNtuVtx(const char* name, TAGdataDsc* p_nturaw, EVENT_STRUCT* evStr)
 : TAGaction(name, "TAMCactNtuVtx - NTuplize MC evt raw data"),
   fpNtuMC(p_nturaw),
   fpEvtStr(evStr)
{
   AddDataOut(p_nturaw, "TAMCntuHit");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuVtx::~TAMCactNtuVtx()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuVtx::Action() {
   
   TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMC->Object();
   
   p_nturaw->Clear();
   
   for (Int_t i = 0; i < fpEvtStr->VTXn; i++) {
      
      Int_t sensorId = fpEvtStr->VTXilay[i];
      Int_t trackIdx = fpEvtStr->VTXid[i];

      TVector3 ipos( fpEvtStr->VTXxin[i], fpEvtStr->VTXyin[i], fpEvtStr->VTXzin[i]);
      TVector3 imom( fpEvtStr->VTXpxin[i], fpEvtStr->VTXpyin[i], fpEvtStr->VTXpzin[i]);
      TVector3 fpos( fpEvtStr->VTXxout[i], fpEvtStr->VTXyout[i], fpEvtStr->VTXzout[i]);
      TVector3 fmom( fpEvtStr->VTXpxout[i], fpEvtStr->VTXpyout[i], fpEvtStr->VTXpzout[i]);

//      p_nturaw->NewHit(sensorId, pos, mom, fpEvtStr->VTXde[i], fpEvtStr->VTXtim[i], trackId);
       p_nturaw->NewHit(trackIdx, sensorId, -99, -99,ipos, fpos, imom, fmom, fpEvtStr->VTXde[i], fpEvtStr->VTXtim[i], 0);
   }

   fpNtuMC->SetBit(kValid);
   return kTRUE;
}

