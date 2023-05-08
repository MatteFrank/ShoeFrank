/*!
 \file TADItrackPropagator.cxx
 \brief Implementation of TADItrackPropagator.cxx
 */

#include "TMath.h"

#include "TADIgeoField.hxx"
#include "TAGgeoTrafo.hxx"

#include "TADItrackPropagator.hxx"

/*!
  \class TADItrackPropagator 
  \brief Track Propagator in Mag field base on RK4 **
   units: B (Gaus), pos (cm), momentum (MeV/c)
*/

const  Double_t TADItrackPropagator::fgkConvFactor = 0.299792458;

//______________________________________________________________________________
//! Constructor
TADItrackPropagator::TADItrackPropagator(TADIgeoField* field)
 : TAGobject(),
   fField(field),
   fTrackLength(0.),
   fNormP(0.),
   fDerivative(0,0,0),
   fPosition(0,0,0),
   fZ(-99),
   fA(-1),
   fStep(0.1)
{
}

// __________________________________________________________________________
//! Propagate in Z-direction charged particle with momentum p to vertex v.
//!
//! \param[in] v initial position
//! \param[in] p initial momentum
//! \param[in] posZ Z position
//! \param[out] vOut final position
//! \param[out] pOut final momentum
Bool_t TADItrackPropagator::ExtrapoleZ(TVector3& v, TVector3& p, Double_t posZ, TVector3& vOut, TVector3& pOut)
{
   // Propagate particle with momentum p to pos Z with an initial position v.
   fNormP       = p.Mag();
   fPosition    = v;
   fDerivative  = TVector3(0, 0, 1);
   fTrackLength = 0;
   
   while(fPosition.Z() <= posZ && fPosition.Z() >= 0){
      RungeKutta4();
      fTrackLength += fStep;
   }
   
   // new position
   vOut = fPosition;
   
   // new momentum
   // p = gamma*mv = gamma*mc^2*(beta)/c
   Double_t mass = TAGgeoTrafo::GetMassFactorMeV();
   TVector3 beta  = fDerivative*(1./TMath::Ccgs());
   Double_t gamma = 1./TMath::Sqrt(1-beta.Mag2());
   pOut = gamma*fA*mass*beta;
   
   return kTRUE;
}

//______________________________________________________________________________
//! Runge Kutta propagation
void TADItrackPropagator::RungeKutta4()
{
   TVector3 K1 = SolveLorentz(fDerivative,               GetFieldB(fPosition) );
   TVector3 K2 = SolveLorentz(fDerivative + fStep/2.*K1, GetFieldB(fPosition + fStep/2. * fDerivative));
   TVector3 K3 = SolveLorentz(fDerivative + fStep/2.*K2, GetFieldB(fPosition + fStep/2. * fDerivative + K1 * (fStep*fStep/4.)));
   TVector3 K4 = SolveLorentz(fDerivative + fStep*K3,    GetFieldB(fPosition + fStep*fDerivative + K2 * (fStep*fStep/2.)));
   
   TVector3 derivative = fDerivative + fStep/6.*(K1 + 2*K2 + 2*K3 + K4);
   TVector3 position   = fPosition + fStep * fDerivative + (K1 + K2 + K3) * (fStep*fStep/6.);
   
   fDerivative = derivative;
   fPosition   = position;
}

//______________________________________________________________________________
//! Solver for Lorentz equation
//!
//! \param[in] v  position vector
//! \param[in] field field vector
//! \return new position
TVector3 TADItrackPropagator::SolveLorentz(TVector3 u, TVector3 field)
{
   TVector3 temp = (fZ/fNormP)*u.Cross(field)*fgkConvFactor;
   
   return temp;
}
