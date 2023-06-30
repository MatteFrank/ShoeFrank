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
const  Double_t TADItrackPropagator::fgkDefStepValue = 0.01;

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
//! \param[in] v initial position
//! \param[in] p initial momentum
//! \param[in] posZ Z position
//! \param[out] vOut final position
//! \param[out] pOut final momentum
Bool_t TADItrackPropagator::ExtrapoleZ(TVector3& pos, TVector3& beta0, Double_t p, Double_t posZ, TVector3& vOut, TVector3& pOut)
{
   // Propagate particle with momentum p to pos Z with an initial position v.
   fNormP       = p;
   fPosition    = pos;
   fBeta        = beta0;
   fTrackLength = 0;
   fStep        = fgkDefStepValue;
   
   while(fPosition.Z() <= posZ) {
      RungeKutta4();
      fTrackLength += fStep;
   }
   
   // new position
   vOut = fPosition;
   
   // new momentum
   // pc = gamma*mv = gamma*mc^2*beta
   Double_t mass = TAGgeoTrafo::GetMassFactorMeV();
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
   
   TVector3 K1 = SolveLorentz(fBeta,               GetFieldB(fPosition) );
   TVector3 K2 = SolveLorentz(fBeta + fStep/2.*K1, GetFieldB(fPosition + fStep/2. * fBeta + K1 * (fStep*fStep/8.)));
   TVector3 K3 = SolveLorentz(fBeta + fStep/2.*K2, GetFieldB(fPosition + fStep/2. * fBeta + K1 * (fStep*fStep/4.)));
   TVector3 K4 = SolveLorentz(fBeta + fStep*K3,    GetFieldB(fPosition + fStep*fBeta + K2 * (fStep*fStep/2.)));
   
   TVector3 beta = fBeta + fStep/6.*(K1 + 2*K2 + 2*K3 + K4);
   TVector3 position   = fPosition + fStep * fBeta + (K1 + K2 + K3) * (fStep*fStep/6.);
   
   fBeta     = beta;
   fPosition = position;

      
   // divide step by 2
   Double_t step = fStep/2.;
   for (Int_t i = 0; i < 2; ++i) {
      TVector3 K1_1 = SolveLorentz(beta1,                GetFieldB(position1) );
      TVector3 K2_1 = SolveLorentz(beta1 + step/2.*K1_1, GetFieldB(position1 + step/2. * beta1 + K1_1 * (step*step/8.)));
      TVector3 K3_1 = SolveLorentz(beta1 + step/2.*K2_1, GetFieldB(position1 + step/2. * beta1 + K2_1 * (step*step/8.)));
      TVector3 K4_1 = SolveLorentz(beta1 + step*K3_1,    GetFieldB(position1 + step*beta1 + K3_1 * (step*step/2.)));
      
      beta1 = beta1 + step/6.*(K1_1 + 2*K2_1 + 2*K3_1 + K4_1);
      position1   = position1 + step * beta1 + (K1_1 + K2_1 + K3_1) * (step*step/6.);
   }

   Double_t err = TMath::Sqrt( (fPosition[0]-position1[0])*(fPosition[0]-position1[0]) + (fPosition[1]-position1[1])*(fPosition[1]-position1[1]) );
   
   if (err > fToterance)
      fStep *=  0.9*TMath::Power(fToterance/err, 1./5.);
   
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

