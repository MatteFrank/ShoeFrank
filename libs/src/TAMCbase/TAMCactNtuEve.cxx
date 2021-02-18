/*!
  \file
  \version $Id: TAMCactNtuEve.cxx,v 1.9 2003/06/22 10:35:48 mueller Exp $
  \brief   Implementation of TAMCactNtuEve.
*/

#include "TAMCntuEve.hxx"
#include "TAMCactNtuEve.hxx"
#include "TAMCflukaParser.hxx"

//First
#include "TAGroot.hxx"

#include "math.h"

/*!
  \class TAMCactNtuEve TAMCactNtuEve.hxx "TAMCactNtuEve.hxx"
  \brief NTuplizer for MC events. **
*/

ClassImp(TAMCactNtuEve);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuEve::TAMCactNtuEve(const char* name,
			       TAGdataDsc* p_nturaw, 
			       EVENT_STRUCT* evStr)
  : TAGaction(name, "TAMCactNtuEve - NTuplize MC evt raw data"),
    fpNtuMC(p_nturaw),
    fpEvtStr(evStr)
{
    if(FootDebugLevel(1))
      cout<<" Entering TAMCactNtuEve::TAMCactNtuEve()"<<endl;
   
  AddDataOut(p_nturaw, "TAMCntuEve");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuEve::~TAMCactNtuEve()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuEve::Action()
{
  if(FootDebugLevel(2))
    cout << " Entering TAMCactNtuEve" << endl;

  TAMCflukaParser::GetTracks(fpEvtStr, fpNtuMC);

  fpNtuMC->SetBit(kValid);
  
  return kTRUE;
}
