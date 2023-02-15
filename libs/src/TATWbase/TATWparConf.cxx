/*!
  \file
  \brief   Implementation of TATWparConf.
*/

#include <string.h>

#include <fstream>
#include <math.h>
#include <vector>

using namespace std;
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"

#include "TATWparConf.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \file TATWparConf.hxx
  \brief Configuration file for the Beam Monitor
*/

ClassImp(TATWparConf);

//------------------------------------------+-----------------------------------
//! Default constructor.
TATWparConf::TATWparConf()
 : TAGparTools(),
   fkDefaultParName(""),
   fEnableTWZmc(false),
   fEnableTWnoPU(false),
   fEnableTWZmatch(false),
   fEnableTWCalBar(false),
   fBarsN(0)
{
  fkDefaultParName = "./config/TATWdetector.cfg";
}

//------------------------------------------+-----------------------------------
//! Default Destructor.
TATWparConf::~TATWparConf()
{
   
}

//------------------------------------------+-----------------------------------
//! Read config input file
//!
//! \param[in] name input file name
Bool_t TATWparConf::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull()){
      cout<<"Warning in TATWparConf::FromFile:: the input filename is Null, I will load the default par file:"<<fkDefaultParName.Data()<<endl;
      nameExp = fkDefaultParName;
   }else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   
   
   //cuts
   Int_t tmp;
   ReadItem(tmp);
   fEnableTWZmc = tmp;
   if(FootDebugLevel(1))
      cout<< "Enable MC true Z: " << fEnableTWZmc <<endl;

   ReadItem(tmp);
   fEnableTWnoPU = tmp;
   if(FootDebugLevel(1))
      cout<<"Enable no pileup: "<< fEnableTWnoPU <<endl;
   
   ReadItem(tmp);
   fEnableTWZmatch = tmp;
   if(FootDebugLevel(1))
      cout<<"Enable Z matching: "<< fEnableTWZmatch <<endl;
   
   ReadItem(tmp);
   fEnableTWCalBar = tmp;
   if(FootDebugLevel(1))
      cout<<"Enable calibration by bar: "<< fEnableTWCalBar <<endl;
   
   ReadItem(fBarsN);
   if(FootDebugLevel(1))
      cout<<"Number of bars: "<< fBarsN <<endl;
   
   Close();
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Clear config info
void TATWparConf::Clear(Option_t*)
{
   fEnableTWZmc    = false;
   fEnableTWnoPU   = false;
   fEnableTWZmatch = false;
   fEnableTWCalBar = false;
   fBarsN          = 0;
}

/*------------------------------------------+---------------------------------*/
//! ostream output
void TATWparConf::ToStream(ostream& os, Option_t*) const
{
  os << "TATWparConf " << GetName() << endl;
  return;
}
