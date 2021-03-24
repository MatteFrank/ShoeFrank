
#include "TAMCntuRegion.hxx"
#include "TAMCactNtuRegion.hxx"

#include "TAGroot.hxx"

/*!
  \class TAMCactNtuRegion TAMCactNtuRegion.hxx "TAMCactNtuRegion.hxx"
  \brief NTuplizer for MC events. **
*/

ClassImp(TAMCactNtuRegion);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAMCactNtuRegion::TAMCactNtuRegion(const char* name,
			       TAGdataDsc* p_nturaw, 
			       EVENT_STRUCT* evStr)
  : TAGaction(name, "TAMCactNtuRegion - NTuplize MC region"),
    fpNtuMC(p_nturaw),
    fpEvtStr(evStr)
{
    if(FootDebugLevel(1))
      cout<<" Entering TAMCactNtuRegion::TAMCactNtuRegion()"<<endl;
   
  AddDataOut(p_nturaw, "TAMCntuRegion");
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMCactNtuRegion::~TAMCactNtuRegion()
{
}

//------------------------------------------+-----------------------------------
//! Action.

Bool_t TAMCactNtuRegion::Action()
{
  if(FootDebugLevel(2))
    cout << " Entering TAMCactNtuRegion" << endl;

  TAMCflukaParser::GetEvents(fpEvtStr, fpNtuMC);

  fpNtuMC->SetBit(kValid);
  
  return kTRUE;
}
