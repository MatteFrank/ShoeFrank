#ifndef _TAITparCal_HXX
#define _TAITparCal_HXX
/*!
  \file TAITparCal.hxx
  \brief   Declaration of TAITparCal.
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TObject.h"
#include "TArrayF.h"
#include "TF1.h"

#include "TAVTbaseParCal.hxx"

//##############################################################################

class TAITparCal : public TAVTbaseParCal {

public:

   TAITparCal();
    virtual ~TAITparCal();

   ClassDef(TAITparCal,1)
};

#endif
