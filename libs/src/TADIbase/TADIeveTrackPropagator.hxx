#ifndef _TADIeveTrackPropagator_HXX
#define _TADIeveTrackPropagator_HXX
/*!
  \file
  \brief   Declaration of TADIeveTrackPropagator.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TEveTrackPropagator.h"
#include "TEveVector.h"


//##############################################################################

class TADIeveTrackPropagator : public TEveTrackPropagator {
   
public:
   TADIeveTrackPropagator();
   
   void     SetNewVertex(TEveVectorD& vertex) { fV = vertex; }
   
   Bool_t	Extrapole(TEveVectorD& v, TEveVectorD& p, TEveVectorD& vOut, TEveVectorD& pOut);
   
   Bool_t   IntersectPlaneZ(const TEveVectorD& p,  const TEveVectorD& point, TEveVectorD& pOut, TEveVectorD& itsect);
   
   ClassDef(TADIeveTrackPropagator,1)
};

#endif
