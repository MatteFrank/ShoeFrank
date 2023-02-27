/*!
  \file TACAparConf.cxx
  \brief   Implementation of TACAparConf.
*/

#include "TAGrecoManager.hxx"

#include "TACAparConf.hxx"

//##############################################################################

/*!
  \class TACAparConf
  \brief Map and Geometry parameters for CAL **
*/

//! Class imp
ClassImp(TACAparConf);


//______________________________________________________________________________
//! Standard constructor
TACAparConf::TACAparConf()
: TAGparTools()
{
   fkDefaultConfName = "./config/TACAdetector.cfg";
}

//______________________________________________________________________________
//! Destructor
TACAparConf::~TACAparConf()
{
}

//_____________________________________________________________________
//! From file
//!
//! \param[in] name input file
Bool_t TACAparConf::FromFile(const TString& name)
{
   // simple file reading, waiting for real config file
   TString nameExp;
   
   if (name.IsNull())
      nameExp = fkDefaultConfName;
   else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
  
   
   ReadItem(fAnalysisParameter.ClusteringAlgo);
   if(FootDebugLevel(1))
      cout << "Clustering algorithm: "<< fAnalysisParameter.ClusteringAlgo << endl;
   
   
   return kFALSE;
}

