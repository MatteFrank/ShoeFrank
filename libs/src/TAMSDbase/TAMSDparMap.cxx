/*!
 \file
 \version $Id: TAMSDparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
 \brief   Implementation of TAMSDparMap.
 */

#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
#include "TAMSDparMap.hxx"
#include "TAGrecoManager.hxx"

//##############################################################################

/*!
 \class TAVTparMap TAVTparMap.hxx "TAVTparMap.hxx"
 \brief Mapping for MSD. **
 */

ClassImp(TAMSDparMap);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMSDparMap::TAMSDparMap()
: TAGparTools()
{
   fkDefaultMapName = "./config/TAMSDdetector.map";
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMSDparMap::~TAMSDparMap()
{
}

//------------------------------------------+-----------------------------------
//! Read mapping data from file \a name .
Bool_t TAMSDparMap::FromFile(const TString& name)
{
  Clear();
  
  if (!Open(name))
    return false;
  
  Info("FromFile()", "Open file %s for mapping\n", name.Data());

  // read for parameter
  Double_t* para = new Double_t[3];

  // number of sensors
  ReadItem(nSens);

  if (FootDebugLevel(1)) {
    printf("SensorsN: %d\n", nSens);
    printf("SensorId BoardId View \n");
  }

  //To read header
  ReadItem(para, 3, ' ', false);  
  for (Int_t i = 0; i < nSens; ++i) { // Loop over sensors

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
Int_t TAMSDparMap::GetSensorId(Int_t boardId, Int_t viewId)
{

  for(int i=0; i<fSensId.size(); i++) {
    if(boardId == fBoardId.at(i) && viewId == fViewId.at(i))
      return fSensId.at(i);
  }

  return -1;
}
