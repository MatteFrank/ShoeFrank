/*!
 \file
 \version $Id: TAMCactNtuStc.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMCactNtuStc.
 */

#include "TAMCntuHit.hxx"
#include "TAMCactNtuStc.hxx"

//FOOT
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TAMCactNtuStc TAMCactNtuStc.hxx "TAMCactNtuStc.hxx"
 \brief NTuplizer for MC events. **
 */

ClassImp(TAMCactNtuStc);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuStc::TAMCactNtuStc(const char* name, TAGdataDsc* p_nturaw, EVENT_STRUCT* evStr)
 : TAGaction(name, "TAMCactNtuStc - NTuplize MC start counter evt"),
   fpNtuMC(p_nturaw),
   fpEvtStr(evStr)
{
   AddDataOut(p_nturaw, "TAMCntuHit");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuStc::~TAMCactNtuStc()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuStc::Action() {
   
   TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMC->Object();
   
   p_nturaw->Clear();
   
   for (Int_t i = 0; i < fpEvtStr->STCn; i++) {
      
      Int_t id       = 0;
      Int_t trackIdx = fpEvtStr->STCid[i];

      TVector3 ipos( fpEvtStr->STCxin[i], fpEvtStr->STCyin[i], fpEvtStr->STCzin[i]);
      TVector3 fpos( fpEvtStr->STCxout[i], fpEvtStr->STCyout[i], fpEvtStr->STCzout[i]);
      TVector3 imom( fpEvtStr->STCpxin[i], fpEvtStr->STCpyin[i], fpEvtStr->STCpzin[i]);
      TVector3 fmom( fpEvtStr->STCpxout[i], fpEvtStr->STCpyout[i], fpEvtStr->STCpzout[i]);

      p_nturaw->NewHit(trackIdx, id, -99, -99, ipos, fpos, imom, fmom, fpEvtStr->STCde[i], fpEvtStr->STCtim[i], 0);
   }

   fpNtuMC->SetBit(kValid);
   return kTRUE;
}

