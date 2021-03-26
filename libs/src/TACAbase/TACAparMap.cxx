/*!
  \file
  \version $Id: TACAparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
  \brief   Implementation of TACAparMap.
*/


#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
#include "TACAparMap.hxx"
#include "GlobalPar.hxx"

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
  printf("CrystalsN: %d\n", nCrys);
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
    
    printf("%2d %2d %2d B%2d %d\n", crysId, moduleId, channelId, boardId, activeCrys);
  
  }

  delete [] para;

  return true;
}


// void TACAparMap::FromFile(std::string FileName)
// {
//   Clear();

//   if (gSystem->AccessPathName(FileName.c_str()))
//   {
//   Error("TACAparMap::FromFile()","File %s doesn't exist",FileName.c_str());
//   } 

//   ifstream fin;
//   fin.open(FileName,std::ifstream::in);
  
//   if(fin.is_open()){
//     //skip the first line
//     fin.ignore(500,'\n');
    
//     Int_t nCrystals;
//     Int_t cnt(0);
    
//     char line[200];
//     //get the second line
//     fin.getline(line, 200, '\n');

//     sscanf(line, "%d", &nCrystals);

//     Int_t crysId[nCrystals];        // Id of crystal
//     Int_t moduleId[nCrystals];      // module Id
//     Int_t channelId[nCrystals];     // channel Id
//     Int_t boardId[nCrystals];       // board Id
//     Int_t activeCrys[nCrystals];    // active crystal

//     // loop over all the slat crosses ( nSlatCross*nLayers ) for two TW layers
//     while (fin.getline(line, 200, '\n')) {

//       if(strchr(line,'#')) {
//         if(FootDebugLevel(1))
//           Info("TACAparMap::FromFile()","Skip comment line:: %s\n",line);
//         continue;
//       }

//       sscanf(line, "%d %d %d %d %d",&crysId[cnt],&moduleId[cnt],&channelId[cnt],&boardId[cnt],&activeCrys[cnt]);
      
//       if(FootDebugLevel(1))
//         Info("TACAparMap::FromFile()","%d %d %d %d %d\n",crysId[cnt],moduleId[cnt],channelId[cnt],boardId[cnt],activeCrys[cnt]);
      
//       cnt++;
//     }

//   }
//   else
//     Info("TACAparMap::FromFile()","File Calibration Energy %s not open!!",FileName.data());

// }

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
