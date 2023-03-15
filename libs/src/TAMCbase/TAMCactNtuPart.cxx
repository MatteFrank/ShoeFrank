/*!
  \file TAMCactNtuPart.cxx
  \brief   Implementation of TAMCactNtuPart.
*/

#include "TAMCntuPart.hxx"
#include "TAMCactNtuPart.hxx"

//First
#include "TAGroot.hxx"

#include "math.h"

/*!
  \class TAMCactNtuPart 
  \brief NTuplizer for MC events. **
*/

//! Class Imp
ClassImp(TAMCactNtuPart);

//------------------------------------------+-----------------------------------
//! Default constructor.
//! \param[in] name action name
//! \param[in] p_nturaw MC particle container descriptor
//! \param[in] evStr Fluka structure
TAMCactNtuPart::TAMCactNtuPart(const char* name,
			       TAGdataDsc* p_nturaw, 
			       EVENT_STRUCT* evStr)
  : TAGaction(name, "TAMCactNtuPart - NTuplize MC evt raw data"),
    fpNtuMC(p_nturaw),
    fpEvtStr(evStr)
{
    if(FootDebugLevel(1))
      cout<<" Entering TAMCactNtuPart::TAMCactNtuPart()"<<endl;
   
  AddDataOut(p_nturaw, "TAMCntuPart");
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAMCactNtuPart::~TAMCactNtuPart()
{
}

//------------------------------------------+-----------------------------------
//! Action.
Bool_t TAMCactNtuPart::Action()
{
  if(FootDebugLevel(2))
    cout << " Entering TAMCactNtuPart" << endl;

  TAMCflukaParser::GetTracks(fpEvtStr, fpNtuMC);

  // if (ValidHistogram()) {

  // }

  // TAMCntuPart* ntu = (TAMCntuPart*) fpNtuMC->Object();
  // cout << "MC NTU tracks BEFORE" << ntu->GetTracksN() << "   " << ntu << endl;
  // for(int i=0; i<ntu->GetTracksN(); ++i)
  // {
  //   TAMCpart* part = ntu->GetTrack(i);
  //   cout << "part::" << i << "    " << part << endl;
  //   cout << "GetInitPos::"; part->GetInitPos().Print();
  //   cout << "GetInitP::"; part->GetInitP().Print();
  //   cout << "GetFinalPos::"; part->GetFinalPos().Print();
  //   cout << "GetFinalP::"; part->GetFinalP().Print();
  //   cout << "GetMass::" << part->GetMass() << endl;
  //   cout << "GetType::" << part->GetType() << endl;
  //   cout << "GetRegion::" << part->GetRegion() << endl;
  //   cout << "GetBaryon::" << part->GetBaryon() << endl;
  //   cout << "GetFlukaID::" << part->GetFlukaID() << endl;
  //   cout << "GetMotherID::" << part->GetMotherID() << endl;
  //   cout << "GetCharge::" << part->GetCharge() << endl;
  //   cout << "GetDead::" << part->GetDead() << endl;
  //   cout << "GetTime::" << part->GetTime() << endl;
  //   cout << "GetTrkLength::" << part->GetTrkLength() << endl;
  //   cout << "GetTof::" << part->GetTof() << endl;
  // }

  fpNtuMC->SetBit(kValid);
  
  return kTRUE;
}
