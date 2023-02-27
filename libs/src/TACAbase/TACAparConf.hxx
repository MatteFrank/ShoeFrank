#ifndef _TACAparConf_HXX
#define _TACAparConf_HXX
/*!
  \file TACAparConf.hxx
  \brief   Declaration of TACAparConf.
*/
/*------------------------------------------+---------------------------------*/

#include "TString.h"

#include "TAGparTools.hxx"

//##############################################################################

class TACAparConf : public TAGparTools {
   
public:
   TACAparConf();
   virtual ~TACAparConf();
   
private:
   /*!
    \struct AnalysisParameter_t
    \brief  Analysis parameters
    */
   struct AnalysisParameter_t : public  TObject {
      TString    ClusteringAlgo;           ///< Tracking algorithm
   };
   
   AnalysisParameter_t fAnalysisParameter;   ///< analysis parameters

public:
   // From file
   Bool_t               FromFile(const TString& name = "");
      
   //! Get analysis parameters
   AnalysisParameter_t& GetAnalysisPar()             { return fAnalysisParameter;           }

   ClassDef(TACAparConf,1)
};

#endif
