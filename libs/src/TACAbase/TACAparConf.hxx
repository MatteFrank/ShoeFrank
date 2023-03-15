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
    TString    fkDefaultConfName; ///< default detector configuration file

   /*!
    \struct AnalysisParameter_t
    \brief  Analysis parameters
    */
   struct AnalysisParameter_t : public  TObject {
      Bool_t     EnableArduinoTemp;        ///< Enable arduino reading
      TString    ClusteringAlgo;           ///< Clustetring  algorithm
   };
   
   AnalysisParameter_t fAnalysisParameter;   ///< analysis parameters

public:
   // From file
   Bool_t               FromFile(const TString& name = "");
      
   //! Get analysis parameters
   AnalysisParameter_t& GetAnalysisPar() { return fAnalysisParameter; }

   ClassDef(TACAparConf,1)
};

#endif
