#ifndef _TAMPparConf_HXX
#define _TAMPparConf_HXX
/*!
  \file TAMPparConf.hxx
  \brief   Declaration of TAMPparConf.
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "TAVTbaseParConf.hxx"

//##############################################################################

class TAMPparConf : public TAVTbaseParConf {
   
public:

   TAMPparConf();
    virtual ~TAMPparConf();
   
   ClassDef(TAMPparConf,1)
};

#endif
