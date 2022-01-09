/*!
  \file TAMCactNtuPart.cxx
  \brief   Implementation of TAMCactNtuPart.
*/

#include "TAMCntuPart.hxx"
#include "TAMCactNtuPart.hxx"

//First
#include "TAGroot.hxx"

#include "math.h"

/*!
  \class TAMCactNtuPart 
  \brief NTuplizer for MC events. **
*/

ClassImp(TAMCactNtuPart);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuPart::TAMCactNtuPart(const char* name,
			       TAGdataDsc* p_nturaw, 
			       EVENT_STRUCT* evStr)
  : TAGaction(name, "TAMCactNtuPart - NTuplize MC evt raw data"),
    fpNtuMC(p_nturaw),
    fpEvtStr(evStr)
{
    if(FootDebugLevel(1))
      cout<<" Entering TAMCactNtuPart::TAMCactNtuPart()"<<endl;
   
  AddDataOut(p_nturaw, "TAMCntuPart");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuPart::~TAMCactNtuPart()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuPart::Action()
{
  if(FootDebugLevel(2))
    cout << " Entering TAMCactNtuPart" << endl;

  TAMCflukaParser::GetTracks(fpEvtStr, fpNtuMC);

  fpNtuMC->SetBit(kValid);
  
  return kTRUE;
}
