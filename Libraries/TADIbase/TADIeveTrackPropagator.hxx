#ifndef _TADIeveTrackPropagator_HXX
#define _TADIeveTrackPropagator_HXX
/*!
  \file TADIeveTrackPropagator.hxx
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
   
   //! Set new vertex
   void     SetNewVertex(TEveVectorD& vertex) { fV = vertex; }
   // Extrapolate to new vertex
   Bool_t	Extrapole(TEveVectorD& v, TEveVectorD& p, TEveVectorD& vOut, TEveVectorD& pOut);
   // Compute momentum and position at a given Z
   Bool_t   IntersectPlaneZ(const TEveVectorD& p,  const TEveVectorD& point, TEveVectorD& pOut, TEveVectorD& itsect);
   
};

#endif
