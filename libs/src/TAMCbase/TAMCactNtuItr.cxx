/*!
 \file
 \version $Id: TAMCactNtuItr.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMCactNtuItr.
 */

#include "TAMCntuHit.hxx"
#include "TAMCactNtuItr.hxx"

//FOOT
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TAMCactNtuItr TAMCactNtuItr.hxx "TAMCactNtuItr.hxx"
 \brief NTuplizer for MC events. **
 */

ClassImp(TAMCactNtuItr);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuItr::TAMCactNtuItr(const char* name, TAGdataDsc* p_nturaw, EVENT_STRUCT* evStr)
 : TAGaction(name, "TAMCactNtuItr - NTuplize MC evt raw data"),
   fpNtuMC(p_nturaw),
   fpEvtStr(evStr)
{
   AddDataOut(p_nturaw, "TAMCntuHit");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuItr::~TAMCactNtuItr()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuItr::Action() {
   
   TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMC->Object();
   
   p_nturaw->Clear();
   
   for (Int_t i = 0; i < fpEvtStr->ITRn; i++) {
      
      Int_t sensorId = fpEvtStr->ITRisens[i];
      Int_t trackIdx = fpEvtStr->ITRid[i];

      TVector3 ipos( fpEvtStr->ITRxin[i], fpEvtStr->ITRyin[i], fpEvtStr->ITRzin[i]);
      TVector3 fpos( fpEvtStr->ITRxout[i], fpEvtStr->ITRyout[i], fpEvtStr->ITRzout[i]);
      TVector3 imom( fpEvtStr->ITRpxin[i], fpEvtStr->ITRpyin[i], fpEvtStr->ITRpzin[i]);
      TVector3 fmom( fpEvtStr->ITRpxout[i],fpEvtStr->ITRpyout[i],fpEvtStr->ITRpzout[i]);

      p_nturaw->NewHit(trackIdx, sensorId, -99, -99, ipos, fpos, imom, fmom, fpEvtStr->ITRde[i], fpEvtStr->ITRtim[i],0);
   }

   fpNtuMC->SetBit(kValid);
   return kTRUE;
}

