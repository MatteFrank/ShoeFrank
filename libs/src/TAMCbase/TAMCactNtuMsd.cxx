/*!
 \file
 \version $Id: TAMCactNtuMsd.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMCactNtuMsd.
 */

#include "TAMCntuHit.hxx"
#include "TAMCactNtuMsd.hxx"

//FOOT
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TAMCactNtuMsd TAMCactNtuMsd.hxx "TAMCactNtuMsd.hxx"
 \brief NTuplizer for MC events. **
 */

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuMsd::TAMCactNtuMsd(const char* name, TAGdataDsc* p_nturaw, EVENT_STRUCT* evStr)
 : TAGaction(name, "TAMCactNtuMsd - NTuplize MC evt raw data"),
   fpNtuMC(p_nturaw),
   fpEvtStr(evStr)
{
   AddDataOut(p_nturaw, "TAMCntuHit");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuMsd::~TAMCactNtuMsd()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuMsd::Action() {
   
   TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMC->Object();
   
   p_nturaw->Clear();
   
   for (Int_t i = 0; i < fpEvtStr->MSDn; i++) {
      
      Int_t sensorId = fpEvtStr->MSDilay[i];
      Int_t trackId  = fpEvtStr->MSDid[i] - 1;
       Int_t layer = fpEvtStr->MSDilay[i];
       Int_t stripx = fpEvtStr->MSDistripx[i];
       Int_t stripy = fpEvtStr->MSDistripy[i];

      TVector3 ipos( fpEvtStr->MSDxin[i], fpEvtStr->MSDyin[i], fpEvtStr->MSDzin[i]);
      TVector3 fpos( fpEvtStr->MSDxout[i], fpEvtStr->MSDyout[i], fpEvtStr->MSDzout[i]);
      TVector3 imom( fpEvtStr->MSDpxin[i], fpEvtStr->MSDpyin[i], fpEvtStr->MSDpzin[i]);
      TVector3 fmom( fpEvtStr->MSDpxout[i], fpEvtStr->MSDpyout[i], fpEvtStr->MSDpzout[i]);

      p_nturaw->NewHit(sensorId, layer, stripx, stripy, ipos, fpos, imom, fmom, fpEvtStr->MSDde[i], fpEvtStr->MSDtim[i], trackId);
   }
    
   fpNtuMC->SetBit(kValid);
   return kTRUE;
}

