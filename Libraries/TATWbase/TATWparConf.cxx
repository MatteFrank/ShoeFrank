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
   fEnableZmc(false),
   fEnableNoPileUp(false),
   fEnableZmatching(false),
   fEnableCalibBar(false),
   fEnableRateSmearMc(false),   
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
      Warning("FromFile()","The input filename is Null, I will load the default par file: %s", fkDefaultParName.Data());
      nameExp = fkDefaultParName;
   }else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   Info("FromFile()", "Open file %s for configuration\n", name.Data());

   Int_t tmp;
   ReadItem(tmp);
   fEnableZmc = tmp;
   if(FootDebugLevel(1))
      cout<< "Enable MC true Z: " << fEnableZmc <<endl;

   ReadItem(tmp);
   fEnableNoPileUp = tmp;
   if(FootDebugLevel(1))
      cout<<"Enable no pileup: "<< fEnableNoPileUp <<endl;
   
   ReadItem(tmp);
   fEnableZmatching = tmp;
   if(FootDebugLevel(1))
      cout<<"Enable Z matching: "<< fEnableZmatching <<endl;
   
   ReadItem(tmp);
   fEnableCalibBar = tmp;
   if(FootDebugLevel(1))
      cout<<"Enable TW bar calibration: "<< fEnableCalibBar <<endl;

   ReadItem(tmp);
   fEnableRateSmearMc = tmp;
   if(FootDebugLevel(1))
      cout<<"Enable rate smearing in MC: "<< fEnableRateSmearMc <<endl;

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
   fEnableZmc         = false;
   fEnableNoPileUp    = false;
   fEnableZmatching   = false;
   fEnableCalibBar    = false;
   fEnableRateSmearMc = false;
   fBarsN             = 0;
}

/*------------------------------------------+---------------------------------*/
//! ostream output
void TATWparConf::ToStream(ostream& os, Option_t*) const
{
  os << "TATWparConf " << GetName() << endl;
  return;
}
