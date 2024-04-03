/*!
  \file
  \brief   Implementation of TASTparConf.
*/

#include <string.h>

#include <fstream>
#include <math.h>
#include <vector>

using namespace std;
#include "TSystem.h"
#include "TString.h"
#include "TFile.h"

#include "TASTparConf.hxx"
#include "TAGroot.hxx"

//##############################################################################

/*!
  \file TASTparConf.hxx
  \brief Configuration file for the st
*/

ClassImp(TASTparConf);

//------------------------------------------+-----------------------------------
//! Default constructor.
TASTparConf::TASTparConf()
 : TAGparTools(),
   fkDefaultParName(""),
   fFFTflag(false),
   fApplyLowPassFilter(false),
   fApplySFFilter(false),
   fApplySFSmoothFilter(false),
   fLPFreqCutoff(0.0)
{
  fkDefaultParName = "./config/TASTfft.cfg";
}

//------------------------------------------+-----------------------------------
//! Default Destructor.
TASTparConf::~TASTparConf()
{
   
}

//------------------------------------------+-----------------------------------
//! Read config input file
//!
//! \param[in] name input file name
Bool_t TASTparConf::FromFile(const TString& name)
{
   TString nameExp;
   
   if (name.IsNull()){
      Warning("FromFile()","The input filename is Null, I will load the default par file: %s", fkDefaultParName.Data());
      nameExp = fkDefaultParName;
   }else
      nameExp = name;
   
   if (!Open(nameExp)) return false;
   
   Info("FromFile()", "Open file %s for configuration\n", name.Data());

   int size;
   
   ReadItem(fApplyLowPassFilter);
   if(FootDebugLevel(1)){
     cout<< "ST apply low pass filter " << fApplyLowPassFilter <<endl;
   }
   ReadItem(fLPFreqCutoff);
   if(FootDebugLevel(1) && fApplyLowPassFilter){
     cout<< "ST low pass freq cutoff" << fLPFreqCutoff  <<endl;
   }
   
   ReadItem(fApplySFFilter);
   if(FootDebugLevel(1)){
     cout<< "ST apply noise single frequency sharp filter " << fApplySFFilter <<endl;
   }
   ReadItem(size);
   fSFFreqCutoff.resize(size);
   ReadItem(&fSFFreqCutoff[0],size,',');
   if(FootDebugLevel(1) && fApplySFFilter){
     for(int i=0;i<size;i++){
       cout<< "ST single freq cutoff" << fSFFreqCutoff.at(i)  <<endl;
     }
   }

   
   ReadItem(fApplySFSmoothFilter);
   if(FootDebugLevel(1)){
     cout<< "ST apply noise single frequency smooth filter " << fApplySFSmoothFilter <<endl;
   }
   ReadItem(size);
   fSFSmoothFreqCutoff.resize(size);
   ReadItem(&fSFSmoothFreqCutoff[0],size,',');
   if(FootDebugLevel(1) && fApplySFSmoothFilter){
     for(int i=0;i<size;i++){
       cout<< "ST single freq cutoff" << fSFSmoothFreqCutoff.at(i)  <<endl;
     }
   }
   
   fFFTflag =  fApplySFSmoothFilter || fApplySFFilter || fApplyLowPassFilter;
   
   Close();
   
   return true;
}

//------------------------------------------+-----------------------------------
//! Clear config info
void TASTparConf::Clear(Option_t*)
{
  fFFTflag = false;
  fApplyLowPassFilter = false;
  fApplySFFilter = false;
  fApplySFSmoothFilter = false;
  fLPFreqCutoff = 0.0;
  fSFFreqCutoff.clear();
  fSFSmoothFreqCutoff.clear();
}

/*------------------------------------------+---------------------------------*/
//! ostream output
void TASTparConf::ToStream(ostream& os, Option_t*) const
{

  os << "TASTparConf " << GetName() << endl;
  return;
}

