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

const  Double_t TADItrackPropagator::fgkConvFactor   = 0.299792458e-3;// (MeV/c)/G/cm
const  Double_t TADItrackPropagator::fgkDefStepValue = 0.1;

//______________________________________________________________________________
//! Constructor
TADItrackPropagator::TADItrackPropagator(TADIgeoField* field)
 : TAGobject(),
   fField(field),
   fTrackLength(0.),
   fNormP(0.),
   fBeta(0,0,0),
   fPosition(0,0,0),
   fZ(-99),
   fA(-1),
   fStep(fgkDefStepValue),
   fToterance(1.e-5)
{
}

// __________________________________________________________________________
//! Propagate in Z-direction charged particle with momentum p to vertex v.
//!
//! \param[in] pos initial position
//! \param[in] beta0 nitial beta
//! \param[in] posZ Z position
//! \param[out] vOut final position
//! \param[out] pOut final momentum
Bool_t TADItrackPropagator::ExtrapoleZ(TVector3& pos, TVector3& beta0, Double_t posZ, TVector3& vOut, TVector3& pOut)
{
   Double_t mass = TAGgeoTrafo::GetMassFactorMeV();

   // Propagate particle with momentum p to posZ with an initial position pos and velocity beta0.
   Double_t gamma0 = 1./TMath::Sqrt(1-beta0.Mag2());
   fNormP       = gamma0*fA*mass*beta0.Mag();
   fPosition    = pos;
   fBeta        = beta0;
   fTrackLength = 0;
   fStep        = fgkDefStepValue;
   
   while(fPosition.Z() <= posZ)
      RungeKutta4();
   
   // new position
   vOut = fPosition;
   
   // new momentum
   // pc = gamma*mv = gamma*mc^2*beta
   TVector3 beta = fBeta;
   Double_t gamma = 1./TMath::Sqrt(1-beta.Mag2());
   pOut = gamma*fA*mass*beta;
   
   return kTRUE;
}

//______________________________________________________________________________
//! Runge Kutta propagation
void TADItrackPropagator::RungeKutta4()
{
   TVector3 beta1     = fBeta;
   TVector3 position1 = fPosition;
   
   TVector3 beta2     = fBeta;
   TVector3 position2 = fPosition;
   
   // mormnal step
   RungeKutta4(fPosition, fBeta, fStep);
   
   // divide step by 2
   Double_t step = fStep/2.;
   for (Int_t i = 0; i < 2; ++i)
      RungeKutta4(position1, beta1, step);

   Double_t err = TMath::Sqrt( (fPosition[0]-position1[0])*(fPosition[0]-position1[0]) + (fPosition[1]-position1[1])*(fPosition[1]-position1[1]) );
   
   Bool_t toMuch = false;
   if (err > fToterance) {
      fStep *= 0.9*TMath::Power(fToterance/err, 1./5.);
      toMuch = true;
   }
   
   // multiply step by 2
   step = fStep*2.;
   RungeKutta4(position2, beta2, step);

   Double_t err2 = TMath::Sqrt( (fPosition[0]-position2[0])*(fPosition[0]-position2[0]) + (fPosition[1]-position2[1])*(fPosition[1]-position2[1]) );

   if (err2 < fToterance && toMuch == false)
      fStep  = step;
      
   fTrackLength += fStep;
}

//______________________________________________________________________________
//! Solver for Lorentz equation
//!
//! \param[in] beta  position vector
//! \param[in] field field vector
//! \return new position
TVector3 TADItrackPropagator::SolveLorentz(TVector3 beta, TVector3 field)
{
   return (fZ/fNormP)*beta.Cross(field)*fgkConvFactor;
}

//______________________________________________________________________________
//! Runge Kutta propagation
//!
//! \param[in] position initial position
//! \param[in] beta nitial beta
//! \param[in] step step  value
void TADItrackPropagator::RungeKutta4(TVector3& position, TVector3& beta, Double_t step)
{
   TVector3 K1 = SolveLorentz(beta,                GetFieldB(position) );
   TVector3 K2 = SolveLorentz(beta + step/2. * K1, GetFieldB(position + step/2. * beta + K1 * (step*step/8.)));
   TVector3 K3 = SolveLorentz(beta + step/2. * K2, GetFieldB(position + step/2. * beta + K1 * (step*step/4.)));
   TVector3 K4 = SolveLorentz(beta + step    * K3, GetFieldB(position + step    * beta + K2 * (step*step/2.)));
   
   beta     = beta     + step/6. * (K1 + 2*K2 + 2*K3 + K4);
   position = position + step    * beta + (K1 + K2 + K3) * (step*step/6.);
}
