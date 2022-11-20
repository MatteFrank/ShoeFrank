/*!
  \file TANEparMap.cxx
  \brief   Implementation of TANEparMap.
*/


#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
#include "TANEparMap.hxx"
#include "TAGrecoManager.hxx"

//##############################################################################

/*!
  \class TANEparMap
  \brief Map parameters for calorimeter.
*/

//! Class Imp
ClassImp(TANEparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.
TANEparMap::TANEparMap()
: TAGparTools()
{
   fModId.clear();
}

//------------------------------------------+-----------------------------------
//! Destructor.
TANEparMap::~TANEparMap()
{
   
}

//------------------------------------------+-----------------------------------
//! Read mapping data from file name
//!
//! \param[in] name file name
Bool_t TANEparMap::FromFile(const TString& name)
{
   Clear();
   
   if (!Open(name))
      return false;
   
   Info("FromFile()", "Open file %s for Module status map\n", name.Data());

   // read for parameter
   Double_t* para = new Double_t[5];

   // number of Module
   ReadItem(fModulesN);

   if (FootDebugLevel(1)) {
      printf("ModulesN: %d\n", fModulesN);
      printf("ModuleId ModuleId ChannelId BoardId ActiveModule \n");
   }

   //To read header
   ReadItem(para, 5, ' ', false);

   for (Int_t i = 0; i < fModulesN; ++i) { // Loop over Module

      // read parameters (boardId chId, crysId)
      ReadItem(para, 5, ' ', false);
   
      // fill map
      Int_t crysId      = TMath::Nint(para[0]);
      Int_t moduleId    = TMath::Nint(para[1]);
      Int_t channelId   = TMath::Nint(para[2]);
      Int_t boardId     = TMath::Nint(para[3]);
      Int_t activeCrys  = TMath::Nint(para[4]);
      
      pair<int, int> idx(boardId, channelId);
      fModId[idx] = crysId;
      
      fModuleId.push_back(moduleId);
      fBoardId.push_back(boardId);
      fChannelId.push_back(channelId);

      if (FootDebugLevel(1))
         printf("%2d %2d %2d B%2d %d\n", crysId, moduleId, channelId, boardId, activeCrys);

   }

   // Read Arduino/temp parameters
   if ( !Eof()) {

      ReadItem(para, 5, ' ', false);  // To read header
      ReadItem(para, 4, ' ', false);  // To read header
      for (Int_t i = 0; i < fModulesN && !Eof(); ++i) { // Loop over Module
         // read parameters (cryId, boardId, mux, chId)
         ReadItem(para, 4, ' ', false);

         Int_t crysIdA      = TMath::Nint(para[0]);
         Int_t boardIdA     = TMath::Nint(para[1]);
         Int_t muxnum       = TMath::Nint(para[2]);
         Int_t channelIdA   = TMath::Nint(para[3]);

         if (boardIdA < 1 || boardIdA > 4 
            || muxnum < 0 || muxnum > 4
            || channelIdA < 0 || channelIdA > 15) {
            Error("FromFile", "Arduino parameter out of range. board %d mux %d channel %d", boardIdA, muxnum, channelIdA);
            continue;
         }
      }
   }

   delete [] para;

   return true;
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TANEparMap::Clear(Option_t*)
{
   fModId.clear();
}

//------------------------------------------+-----------------------------------
//! Get Module id connected to WD board, channel
//!
//! \param[in] boardId board id
//! \param[in] channelId channel id
Int_t TANEparMap::GetModuleId(Int_t boardId, Int_t channelId)
{
   pair<int, int> idx(boardId, channelId);
  
   if (fModId.count(idx)) {
      //auto itr = fModId.find(idx);
      return fModId[idx];
   }

   return -1;
}
