#ifndef _TAITparConf_HXX
#define _TAITparConf_HXX
/*!
  \file
  \version $Id: TAITparConf.hxx
  \brief   Declaration of TAITparConf.
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "TAVTbaseParConf.hxx"

//##############################################################################

class TAITparConf : public TAVTbaseParConf {
   
public:

   TAITparConf();
    virtual ~TAITparConf();
   
   ClassDef(TAITparConf,1)
};

#endif
