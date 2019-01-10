/*!
 \file
 \version $Id: TAMCactNtuCal.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
 \brief   Implementation of TAMCactNtuCal.
 */

#include "TAMCntuHit.hxx"
#include "TAMCactNtuCal.hxx"

//FOOT
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

/*!
 \class TAMCactNtuCal TAMCactNtuCal.hxx "TAMCactNtuCal.hxx"
 \brief NTuplizer for MC events. **
 */

ClassImp(TAMCactNtuCal);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuCal::TAMCactNtuCal(const char* name, TAGdataDsc* p_nturaw, EVENT_STRUCT* evStr)
 : TAGaction(name, "TAMCactNtuCal - NTuplize MC evt raw data"),
   fpNtuMC(p_nturaw),
   fpEvtStr(evStr)
{
   AddDataOut(p_nturaw, "TAMCntuHit");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuCal::~TAMCactNtuCal()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuCal::Action() {
   
   TAMCntuHit* p_nturaw = (TAMCntuHit*) fpNtuMC->Object();
   
   p_nturaw->Clear();
   
   for (Int_t i = 0; i < fpEvtStr->CALn; i++) {
      
      Int_t crystalId      = fpEvtStr->CALicry[i];
      
      TVector3 pos( fpEvtStr->CALxin[i], fpEvtStr->CALyin[i], fpEvtStr->CALzin[i]);
      TVector3 mom( fpEvtStr->CALpxin[i], fpEvtStr->CALpyin[i], fpEvtStr->CALpzin[i]);
      
      p_nturaw->NewHit(crystalId, pos, mom, fpEvtStr->CALde[i], fpEvtStr->CALtim[i]);
   }
   
   fpNtuMC->SetBit(kValid);
   return kTRUE;
}

