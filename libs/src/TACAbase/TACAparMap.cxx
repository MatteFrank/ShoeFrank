/*!
  \file
  \version $Id: TACAparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TACAparMap.
*/


#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
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
   fParGeo = (TACAparGeo*)gTAGroot->FindParaDsc(TACAparGeo::GetDefParaName(), "TACAparGeo")->Object();
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
  
  if (!Open(name)) {
    Error("FromFile()", "Cannot open file %s", name.Data());
    return false;
  }
  
  // read for parameter
  Double_t* para = new Double_t[3];

  for (Int_t i = 0; i < fParGeo->GetCrystalsN(); ++i) { // Loop over crystal

  // read parameters (boardId chId, crysId)
  ReadItem(para, 3, ' ', false);
  
  // fill map
    Int_t boardId   = TMath::Nint(para[0]);
    Int_t channelId = TMath::Nint(para[1]);
    Int_t crysId    = TMath::Nint(para[2]);
    
    pair<int, int> idx(boardId, channelId);
    fCrysId[idx] = crysId;
    if (FootDebugLevel(1))
      printf("%d %d %d\n", boardId, channelId, crysId);
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
