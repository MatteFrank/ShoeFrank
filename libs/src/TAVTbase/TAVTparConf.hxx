#ifndef _TAVTparConf_HXX
#define _TAVTparConf_HXX
/*!
  \file
  \version $Id: TAVTparConf.hxx
  \brief   Declaration of TAVTparConf.
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "TAVTbaseParConf.hxx"

//##############################################################################

class TAVTparConf : public TAVTbaseParConf {
   
public:

   TAVTparConf();
    virtual ~TAVTparConf();
   
   ClassDef(TAVTparConf,1)
};

#endif
