/*!
  \file
  \version $Id: TACAparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TACAparMap.
*/


#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
#include "TACAparMap.hxx"
#include "TAGrecoManager.hxx"

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
  
  if (!Open(name))
    return false;
  
  // read for parameter
  Double_t* para = new Double_t[5];
  // Int_t nCrys = 0;

  // number of crystal
  ReadItem(nCrys);

  if (FootDebugLevel(1)) {
    printf("CrystalsN: %d\n", nCrys);
    printf("CrystalId ModuleId ChannelId BoardId ActiveCrystal \n");
  }
  
  // nCrystals = nCrys;
  // cout << "n crys: " << nCrys << endl;

  for (Int_t i = 0; i < nCrys; ++i) { // Loop over crystal

    // read parameters (boardId chId, crysId)
    ReadItem(para, 5, ' ', false);
  
    // fill map
    Int_t crysId      = TMath::Nint(para[0]);
    Int_t moduleId    = TMath::Nint(para[1]);
    Int_t channelId   = TMath::Nint(para[2]);
    Int_t boardId     = TMath::Nint(para[3]);
    Int_t activeCrys  = TMath::Nint(para[4]);
    
    pair<int, int> idx(boardId, channelId);
    fCrysId[idx] = crysId;
    
    fModuleId.push_back(moduleId);
    fBoardId.push_back(boardId);
    fChannelId.push_back(channelId);

    if (FootDebugLevel(1))
      printf("%2d %2d %2d B%2d %d\n", crysId, moduleId, channelId, boardId, activeCrys);
      
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
  
  auto itr = fCrysId.find(idx);
  if (itr == fCrysId.end())
    return -1;
  
  return fCrysId[idx];
}
