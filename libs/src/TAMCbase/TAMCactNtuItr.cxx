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
      
      Int_t sensorId = fpEvtStr->ITRilay[i]*16 +  fpEvtStr->ITRiplume[i]*4 + fpEvtStr->ITRimimo[i];
      Int_t trackId  = fpEvtStr->ITRid[i] - 1;
      Int_t layer = fpEvtStr->ITRilay[i];
      Int_t plume = fpEvtStr->ITRiplume[i];
      Int_t mimo  = fpEvtStr->ITRimimo[i];

      TVector3 ipos( fpEvtStr->ITRxin[i], fpEvtStr->ITRyin[i], fpEvtStr->ITRzin[i]);
      TVector3 fpos( fpEvtStr->ITRxout[i], fpEvtStr->ITRyout[i], fpEvtStr->ITRzout[i]);
      TVector3 imom( fpEvtStr->ITRpxin[i], fpEvtStr->ITRpyin[i], fpEvtStr->ITRpzin[i]);
      TVector3 fmom( fpEvtStr->ITRpxout[i],fpEvtStr->ITRpyout[i],fpEvtStr->ITRpzout[i]);

      p_nturaw->NewHit(sensorId, layer, plume, mimo, ipos, fpos, imom, fmom, fpEvtStr->ITRde[i], fpEvtStr->ITRtim[i], trackId);
   }

   fpNtuMC->SetBit(kValid);
   return kTRUE;
}

