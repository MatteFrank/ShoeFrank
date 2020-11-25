/*!
  \file
  \version $Id: TACAparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TACAparMap.
*/

#include <string.h>

#include <fstream>

#include "TSystem.h"
#include "TString.h"

#include "TACAparMap.hxx"

//##############################################################################

/*!
  \class TACAparMap TACAparMap.hxx "TACAparMap.hxx"
  \brief Map parameters for calorimeter. **
*/

ClassImp(TACAparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TACAparMap::TACAparMap()
: TAGparTools()
{
   fCrysId.clear();
}

//------------------------------------------+-----------------------------------
//! Destructor.

TACAparMap::~TACAparMap()
{
   
}

//------------------------------------------+-----------------------------------
//! Read mapping data from file \a name .

Bool_t TACAparMap::FromFile(const TString& name)
{
  Clear();
  
  if (Open(name)) return false;
  
  // read for parameter
  Double_t* para = new Double_t[3];
  
  // read parameters (boardId chId, crysId)
  ReadItem(para, 3, ' ');
  
  // fill map
  for (Int_t p = 0; p < 3; p++) { // Loop over parameters
    Int_t boardId   = TMath::Nint(para[0]);
    Int_t channelId = TMath::Nint(para[1]);
    Int_t crysId    = TMath::Nint(para[2]);
    
    pair<int, int> idx(boardId, channelId);
    fCrysId[idx] = crysId;
  }
  
  
  delete [] para;

  return true;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TACAparMap::Clear(Option_t*)
{
  fCrysId.clear();
}

//------------------------------------------+-----------------------------------
Int_t TACAparMap::GetCrystalId(Int_t boardId, Int_t channelId)
{
  pair<int, int> idx(boardId, channelId);
  
  return fCrysId[idx];
}
