
/*!
 \file TAMCactNtuRegion.cxx
 \brief NTuplizer for MC events. **
 */

#include "TAMCntuRegion.hxx"
#include "TAMCactNtuRegion.hxx"

#include "TAGroot.hxx"

/*!
  \class TAMCactNtuRegion
  \brief NTuplizer for MC events. **
*/

//! Class Imp
ClassImp(TAMCactNtuRegion);

//------------------------------------------+-----------------------------------
//! Default constructor.
//! \param[in] name action name
//! \param[in] p_nturaw MC region container descriptor
//! \param[in] evStr Fluka structure
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

  TAMCflukaParser::GetRegions(fpEvtStr, fpNtuMC);

  fpNtuMC->SetBit(kValid);
  
  return kTRUE;
}
