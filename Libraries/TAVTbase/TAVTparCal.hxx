#ifndef _TAVTparCal_HXX
#define _TAVTparCal_HXX
/*!
  \file TAVTparCal.hxx
  \brief   Declaration of TAVTparCal.
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TObject.h"
#include "TArrayF.h"
#include "TF1.h"

#include "TAVTbaseParCal.hxx"

//##############################################################################

class TAVTparCal : public TAVTbaseParCal {

public:

   TAVTparCal();
    virtual ~TAVTparCal();

   ClassDef(TAVTparCal,1)
};

#endif
