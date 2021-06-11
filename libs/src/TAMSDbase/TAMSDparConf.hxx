#ifndef _TAMSDparConf_HXX
#define _TAMSDparConf_HXX
/*!
  \file
  \version $Id: TAMSDparConf.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
  \brief   Declaration of TAMSDparConf.
*/
/*------------------------------------------+---------------------------------*/

#include "TString.h"

#include "TAVTbaseParConf.hxx"

//##############################################################################

class TAMSDparConf : public TAVTbaseParConf {
   
public:
   
   TAMSDparConf();
   virtual ~TAMSDparConf();
   
   
   Bool_t     FromFile(const TString& name = "");
   
   
   ClassDef(TAMSDparConf,1)
};

#endif
