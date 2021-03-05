/*!
  \file
  \version $Id: TAMCactNtuTrack.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAMCactNtuTrack.
*/

#include "TAMCntuTrack.hxx"
#include "TAMCactNtuTrack.hxx"

//First
#include "TAGroot.hxx"

#include "math.h"

/*!
  \class TAMCactNtuTrack TAMCactNtuTrack.hxx "TAMCactNtuTrack.hxx"
  \brief NTuplizer for MC events. **
*/

ClassImp(TAMCactNtuTrack);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuTrack::TAMCactNtuTrack(const char* name,
			       TAGdataDsc* p_nturaw, 
			       EVENT_STRUCT* evStr)
  : TAGaction(name, "TAMCactNtuTrack - NTuplize MC evt raw data"),
    fpNtuMC(p_nturaw),
    fpEvtStr(evStr)
{
    if(FootDebugLevel(1))
      cout<<" Entering TAMCactNtuTrack::TAMCactNtuTrack()"<<endl;
   
  AddDataOut(p_nturaw, "TAMCntuTrack");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuTrack::~TAMCactNtuTrack()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuTrack::Action()
{
  if(FootDebugLevel(2))
    cout << " Entering TAMCactNtuTrack" << endl;

  TAMCflukaParser::GetTracks(fpEvtStr, fpNtuMC);

  fpNtuMC->SetBit(kValid);
  
  return kTRUE;
}
