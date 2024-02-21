/*!
  \file TAGanaManager.cxx
   \brief Class of configuration analysis manager
*/

#include <Riostream.h>
#include "TAGrecoManager.hxx"

#include "TAGanaManager.hxx"

//##############################################################################

/*!
  \class TAGanaManager
  \brief Base class of configuration parameters for VTX
*/

//! Class Imp
ClassImp(TAGanaManager);

//______________________________________________________________________________
//! Standard constructor
TAGanaManager::TAGanaManager()
: TAGparTools()
{
   fkDefaultName = "TANAdetector.cfg";
}

//______________________________________________________________________________
//! Destructor
TAGanaManager::~TAGanaManager()
{
}

//------------------------------------------+-----------------------------------
//! Read from file
//!
//! \param[in] name file name
Bool_t TAGanaManager::FromFile(const TString& name)
{   
   // simple file reading, waiting for real config file
   TString nameExp;
   
   if (name.IsNull())
     nameExp = fkDefaultName;
   else 
     nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   Info("FromFile()", "Open file %s for analysis configuration\n", name.Data());

   Int_t tmp;
   ReadItem(tmp);
   fAnalysisParameter.MassResoFlag = tmp;
   if(FootDebugLevel(1))
      cout << "Mass resolution flag: "<< fAnalysisParameter.MassResoFlag << endl;

   ReadItem(tmp);
   fAnalysisParameter.PtResoFlag = tmp;
   if(FootDebugLevel(1))
      cout << " Momentum resolution flag: "<< fAnalysisParameter.PtResoFlag << endl;
   
   ReadItem(tmp);
   fAnalysisParameter.EfficiencyFlag = tmp;
   if(FootDebugLevel(1))
     cout << "Efficiency studies flag: "<< fAnalysisParameter.EfficiencyFlag << endl;
   
   
   return kFALSE;
}



//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TAGanaManager::Clear(Option_t*)
{
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
//!
//! \param[out] os stream output
//! \param[in] option option for printout
void TAGanaManager::ToStream(ostream& os, Option_t*) const
{
//  os << "TAGanaManager " << GetName() << endl;
//  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
//     << "   ver_x   ver_y   ver_z  width" << endl;

  return;
}

