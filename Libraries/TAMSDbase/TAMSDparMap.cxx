/*!
 \file TAMSDparMap.cxx
 \brief   Implementation of TAMSDparMap.
 */

#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
#include "TAMSDparMap.hxx"
#include "TAGrecoManager.hxx"

//##############################################################################

/*!
 \class TAMSDparMap
 \brief Mapping for MSD.
 */

//! Class Imp
ClassImp(TAMSDparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.
TAMSDparMap::TAMSDparMap()
 : TAGparTools(),
   fSensorsN(0)
{
   fkDefaultMapName = "./config/TAMSDdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMSDparMap::~TAMSDparMap()
{
}

//------------------------------------------+-----------------------------------
//! Read from file
//!
//! \param[in] name file name
Bool_t TAMSDparMap::FromFile(const TString& name)
{
  Clear();
  
  if (!Open(name))
    return false;
  
  Info("FromFile()", "Open file %s for mapping\n", name.Data());

  // read for parameter
  Double_t* para = new Double_t[3];

  // number of sensors
  ReadItem(fSensorsN);

  if (FootDebugLevel(1)) {
    printf("SensorsN: %d\n", fSensorsN);
    printf("SensorId BoardId View \n");
  }

  //To read header
  for (Int_t i = 0; i < fSensorsN; ++i) { // Loop over sensors

    // read parameters (sensId, boardId, view)
    ReadItem(para, 3, ' ', false);
  
    // fill map
    Int_t sensId      = TMath::Nint(para[0]);
    Int_t boardId     = TMath::Nint(para[1]);
    Int_t view        = TMath::Nint(para[2]);
    
    fSensId.push_back(sensId);
    fBoardId.push_back(boardId);
    fViewId.push_back(view);

    if (FootDebugLevel(1))
      printf("%2d %2d %d\n", sensId, boardId, view);
  }

  delete [] para;

  return true;
}

//------------------------------------------+-----------------------------------
//! Get sensor id from board and view id
//!
//! \param[in] boardId board Id
//! \param[in] viewId view Id
Int_t TAMSDparMap::GetSensorId(Int_t boardId, Int_t viewId)
{

  for(int i=0; i<fSensId.size(); i++) {
    if(boardId == fBoardId.at(i) && viewId == fViewId.at(i))
      return fSensId.at(i);
  }

  return -1;
}
