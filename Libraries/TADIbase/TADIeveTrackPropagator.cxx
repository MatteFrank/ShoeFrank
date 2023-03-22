/*!
 \file TADIeveTrackPropagator.cxx
 \brief Implementation of TADIeveTrackPropagator
 */

#include "TADIeveTrackPropagator.hxx"

/*!
  \class TADIeveTrackPropagator
  \brief TEve Propagator in Mag field **
 
   units: B (Testla), pos (cm), momentum (GeV/c)
*/

//______________________________________________________________________________
//! Constructor
TADIeveTrackPropagator::TADIeveTrackPropagator()
: TEveTrackPropagator()
{
}

// __________________________________________________________________________
//! Propagate in Z charged particle with momentum p to vertex v.
//!
//! \param[in] v initial position
//! \param[in] p initial momentum
//! \param[out] vOut final position
//! \param[out] pOut final momentum
Bool_t TADIeveTrackPropagator::Extrapole(TEveVectorD& v, TEveVectorD& p, TEveVectorD& vOut, TEveVectorD& pOut)
{
   TEveVector4D currV(fV);
   TEveVector4D forwV(fV);
   TEveVectorD  currP(p);
   TEveVectorD  forwP(p);
   
   Float_t origStep = fH.fRKStep;
   
   fH.fRKStep = 0.01;
   
   do {
      Step(currV, currP, forwV, forwP);
      
      currV = forwV;
      currP = forwP;
      
   } while (currV[2] < v[2]);
   
   
   fH.fRKStep = origStep;
   
   pOut = currP;
   vOut = currV;
   
   return kTRUE;
}


// __________________________________________________________________________
//!  Intersect helix with a plane. Current position and argument p define
//!
//! \param[in] p initial momentum
//! \param[in] point initial position
//! \param[out] pOut final momentum
//! \param[out] itsect intersection position
Bool_t TADIeveTrackPropagator::IntersectPlaneZ(const TEveVectorD& p,  const TEveVectorD& point, TEveVectorD& pOut, TEveVectorD& itsect)
{
   // the helix.
   TEveVectorD pos(fV);
   TEveVectorD mom(p);
   if (fMagFieldObj->IsConst())
      fH.UpdateHelix(mom, fMagFieldObj->GetFieldD(pos), kFALSE, kFALSE);
   
   TEveVectorD n(0,0,1);
   TEveVectorD delta = pos - point;
   Double_t d = delta.Dot(n);
   if (d > 0) {
      n.NegateXYZ(); // Turn normal around so that we approach from negative side of the plane
      d = -d;
   }
   
   TEveVector4D forwV;
   TEveVectorD  forwP;
   TEveVector4D pos4(pos);
   
   while (kTRUE)
   {
      Update(pos4, mom);
      Step(pos4, mom, forwV , forwP);
      Double_t new_d = (forwV - point).Dot(n);
      if (new_d < d)
      {
         // We are going further away ... fail intersect.
         Warning("HelixIntersectPlane", "going away from the plane.");
         return kFALSE;
      }
      if (new_d > 0)
      {
         delta = forwV - pos;
         itsect = pos + delta * (d / (d - new_d));
         pOut = forwP;
         return kTRUE;
      }
      pos4 = forwV;
      mom  = forwP;
   }
}
