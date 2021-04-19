/*!
  \file
  \version $Id: TAMCactNtuEvent.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAMCactNtuEvent.
*/

#include "TAMCntuPart.hxx"
#include "TAMCactNtuEvent.hxx"

//First
#include "TAGroot.hxx"

#include "math.h"

/*!
  \class TAMCactNtuEvent TAMCactNtuEvent.hxx "TAMCactNtuEvent.hxx"
  \brief NTuplizer for MC events. **
*/

ClassImp(TAMCactNtuEvent);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuEvent::TAMCactNtuEvent(const char* name,
			       TAGdataDsc* p_nturaw, 
			       EVENT_STRUCT* evStr)
  : TAGaction(name, "TAMCactNtuEvent - NTuplize MC event"),
    fpNtuMC(p_nturaw),
    fpEvtStr(evStr)
{
    if(FootDebugLevel(1))
      cout<<" Entering TAMCactNtuEvent::TAMCactNtuEvent()"<<endl;
   
  AddDataOut(p_nturaw, "TAMCntuEvent");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuEvent::~TAMCactNtuEvent()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuEvent::Action()
{
  if(FootDebugLevel(2))
    cout << " Entering TAMCactNtuEvent" << endl;

  TAMCflukaParser::GetEvents(fpEvtStr, fpNtuMC);

  fpNtuMC->SetBit(kValid);
  
  return kTRUE;
}
