/*!
  \file TACAparMap.cxx
  \brief   Implementation of TACAparMap.
*/


#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
#include "TACAparMap.hxx"
#include "TAGrecoManager.hxx"

//##############################################################################

/*!
  \class TACAparMap
  \brief Map parameters for calorimeter.
*/

//! Class Imp
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
//! Read mapping data from file name
//!
//! \param[in] name file name
Bool_t TACAparMap::FromFile(const TString& name)
{
  Clear();
  
  if (!Open(name))
    return false;
  
  Info("FromFile()", "Open file %s for crystal status map\n", name.Data());

  // read for parameter
  Double_t* para = new Double_t[5];

  // number of crystal
  ReadItem(fCrystalsN);

  if (FootDebugLevel(1)) {
    printf("CrystalsN: %d\n", fCrystalsN);
    printf("CrystalId ModuleId ChannelId BoardId ActiveCrystal \n");
  }

  //To read header
  ReadItem(para, 5, ' ', false);  

   for (Int_t i = 0; i < fCrystalsN; ++i) { // Loop over crystal

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
//! Get crystal id
//!
//! \param[in] boardId board id
//! \param[in] channelId channel id
Int_t TACAparMap::GetCrystalId(Int_t boardId, Int_t channelId)
{
  pair<int, int> idx(boardId, channelId);
  
  if (fCrysId.count(idx)) {
     auto itr = fCrysId.find(idx);
     return fCrysId[idx];
  }

   return -1;
}
