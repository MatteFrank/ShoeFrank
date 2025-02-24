#ifndef _TAGanaManager_HXX
#define _TAGanaManager_HXX
/*!
  \file TAGanaManager.hxx
  \brief   Declaration of TAGanaManager.
*/
/*------------------------------------------+---------------------------------*/

#include <map>
#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TArrayC.h"

#include "TAGparTools.hxx"

using namespace std;

//##############################################################################

class TAGanaManager : public TAGparTools {
      
protected:
   TString    fDefaultName; ///< default analysis file

   /*!
    \struct AnalysisParameter_t
    \brief  Analysis parameters
    */
   struct GlbAnaParameter_t : public  TObject {
      Bool_t     MassResoFlag;           ///< Mass resolution  flag
      Bool_t     PtResoFlag;              ///< Momentum resolution  flag
      Bool_t     EfficiencyFlag;          ///< Efficiency studies  flag

   };
   
   GlbAnaParameter_t fAnalysisParameter; ///< analysis parameters

private:
   static const TString fgkDefaultFileName; ///< Default analysis manager file name
   static const TString fgkDefaultFolder;   ///< Default analysis folder

public:
   TAGanaManager(TString expName);
    virtual ~TAGanaManager();

   // Reading from file
   Bool_t             FromFile(const TString& name = "");
   
   // Clear
   virtual void       Clear(Option_t* opt="");
   
   // To stream
   virtual void       ToStream(ostream& os = cout, Option_t* option = "") const;
   
   //! Get analysis parameters
   GlbAnaParameter_t& GetAnalysisPar()  { return fAnalysisParameter; }
   
   ClassDef(TAGanaManager,1)
};

#endif
