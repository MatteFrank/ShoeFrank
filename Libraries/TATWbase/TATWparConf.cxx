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
   fEnableEnergyThr(false),
   fEnableRateSmearMc(false),   
   fBarsN(0),
   fGain(0.)
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
   if(fEnableZmc==1)
     Warning("FromFile()","TW ZID set to MC true one...set the EnableZmc value to 0 in TATWdetector.cfg file if it is not the wanted behaviour");
   if(FootDebugLevel(1))
     Info("FromFile()", "Enable MC true Z::%d\n",fEnableZmc);

   ReadItem(tmp);
   fEnableNoPileUp = tmp;
   if(fEnableNoPileUp==1)
     Warning("FromFile()","Multiple hits in the same TW bar considered as separate hits in MCinstead of a single one...set the EnableNoPileUp value to 0 in the TATWdetector.cfg file if it is not the wanted behaviour");
   if(FootDebugLevel(1))
     Info("FromFile()","Enable no pileup::%d\n",fEnableNoPileUp);
   
   ReadItem(tmp);
   fEnableZmatching = tmp;
   if(fEnableZmatching==0)
     Warning("FromFile()","TW Z matching is disabled...set the EnableZmatching value to 1 in the TATWdetector.cfg file if it is not the wanted behaviour");
   if(FootDebugLevel(1))
     Info("FromFile()","Enable Z matching::%d\n",fEnableZmatching);
   
   ReadItem(tmp);
   fEnableCalibBar = tmp;
   if(fEnableCalibBar==1)
     Warning("FromFile()","Enabled TW bar calibration ...set the EnableCalibBar value to 0 in the TATWdetector.cfg file if you want a TW calibration for each bar cross");
   if(FootDebugLevel(1))
     Info("FromFile()","Enable TW bar calibration::%d\n",fEnableCalibBar);

   ReadItem(tmp);
   fEnableRateSmearMc = tmp;
   if(fEnableRateSmearMc==1)
     Warning("FromFile()","Enabled TW Rate Smear for MC ...set the EnableRateSmearMc value to 0 in the TATWdetector.cfg file if it is not the wanted behaviour");
   if(FootDebugLevel(1))
     Info("FromFile()","Enable rate smearing in MC::%d\n",fEnableRateSmearMc);

   ReadItem(fBarsN);
   if(FootDebugLevel(1))
     Info("FromFile()","Number of bars::%d\n",fBarsN);
   
   ReadItem(fGain);
   if(fGain==0)
     Warning("FromFile()","WD gain in TW set to %.1f in TATWdetector.cfg file...set the correct value",fGain);
   if(fGain>1)
     Warning("FromFile()","WD gain in TW set to %.1f in TATWdetector.cfg file...check if this is the correct value",fGain);
   if(FootDebugLevel(1))
     Info("FromFile()","Gain value set in the WD for TW::%f.1\n",fGain);
   
   ReadItem(tmp);
   fEnableEnergyThr = tmp;
   if(fEnableEnergyThr == 0)
     Warning("FromFile()","Threshold over deposited energy has been disabled in TW...set the EnableEnergyThr value to 1 in TATWdetector.cfg file if it is not the wanted behaviour");
   if(FootDebugLevel(1))
     Info("FromFile()","Enable energy threshold in TW::%d\n",fEnableEnergyThr);

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
   fEnableEnergyThr   = false;
   fGain              = 0.;
   fBarsN             = 0;

   return;
}

/*------------------------------------------+---------------------------------*/
//! ostream output
void TATWparConf::ToStream(ostream& os, Option_t*) const
{
  os << "TATWparConf " << GetName() << endl;
  return;
}
