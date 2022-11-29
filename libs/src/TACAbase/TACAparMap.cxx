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
         printf("%2d\t %2d\t %2d\t B%3d\t %d\n", crysId, moduleId, channelId, boardId, activeCrys);
   }

   // Read Arduino/temp parameters
   if ( !Eof()) {
      if (FootDebugLevel(1)) {
         printf("CrystalsN: %d\n", fCrystalsN);
         printf("CrystalIdA BoardIdA Muxmum ChannelidA\n");
      }
      
      for (Int_t i = 0; i < fCrystalsN && !Eof(); ++i) { // Loop over crystal
         
         // read parameters (cryId, boardId, mux, chId)
         ReadItem(para, 4, ' ', false);

         Int_t crysIdA      = TMath::Nint(para[0]);
         Int_t boardIdA     = TMath::Nint(para[1]);
         Int_t muxnum       = TMath::Nint(para[2]);
         Int_t channelIdA   = TMath::Nint(para[3]);

         if (FootDebugLevel(1))
            printf("%2d\t B%3d\t %2d\t %2d\n", crysIdA, boardIdA, muxnum, channelIdA);
         
         if (boardIdA < 1 || boardIdA > 4 
            || muxnum < 0 || muxnum > 4
            || channelIdA < 0 || channelIdA > 15) {
            Error("FromFile", "Arduino parameter out of range. board %d mux %d channel %d", boardIdA, muxnum, channelIdA);
            continue;
         }

         // add if it is not found
         if ( GetArduinoCrystalId(boardIdA, muxnum,channelIdA ) == -1) {
            pair<int, int> idx(muxnum, channelIdA);
            fCrysIdArduino[boardIdA-1][idx] = crysIdA;
         } else {
            Error("FromFile", "Arduino parameter duplicated. board %d mux %d channel %d", boardIdA, muxnum, channelIdA);
            continue;
         }
      }
   }

   delete [] para;

   return true;
}

//------------------------------------------+-----------------------------------
//! Clear event.
void TACAparMap::Clear(Option_t*)
{
   fCrysId.clear();
   fCrysIdArduino[0].clear();
   fCrysIdArduino[1].clear();
   fCrysIdArduino[2].clear();
   fCrysIdArduino[3].clear();
}

//------------------------------------------+-----------------------------------
//! Get crystal id connected to WD board, channel
//!
//! \param[in] boardId board id
//! \param[in] channelId channel id
Int_t TACAparMap::GetCrystalId(Int_t boardId, Int_t channelId)
{
   pair<int, int> idx(boardId, channelId);
  
   if (fCrysId.count(idx)) {
      //auto itr = fCrysId.find(idx);
      return fCrysId[idx];
   }

   return -1;
}

//------------------------------------------+-----------------------------------
//! Get crystal id connected to Arduino board
//!
//! \param[in] boardId board id
//! \param[in] channelId channel id
Int_t TACAparMap::GetArduinoCrystalId(Int_t boardID, Int_t muxnum, Int_t ch)
{
   if (boardID < 1 || boardID > 4 
       || muxnum < 0 || muxnum > 4
       || ch < 0 || ch > 15) return -1;

   pair<int, int> idx(muxnum, ch);
   if (fCrysIdArduino[boardID-1].count(idx)) {
      return fCrysIdArduino[boardID-1][idx];
   }

   return -1;
}
