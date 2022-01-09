#ifndef _TADItrackPropagator_HXX
#define _TADItrackPropagator_HXX
/*!
  \file TADItrackPropagator.hxx
  \brief   Declaration of TADItrackPropagator.
  Inspired from the macro of A. Sécher
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TAGobject.hxx"
#include "TVector3.h"


//##############################################################################

class TADIgeoField;
class TADItrackPropagator : public TAGobject {
   
public:
   TADItrackPropagator(TADIgeoField* field);
   
   //! Extrapole vertex and momentum to a given Z
   Bool_t	ExtrapoleZ(TVector3& v, TVector3& p, Double_t posZ, TVector3& vOut, TVector3& pOut);
   
   //! Get field
   TVector3 GetFieldB(TVector3 vertex) const { return fField->GetField(vertex); }

   // Getter
   //! Get track length
   Double_t GetTrackLength()           const { return fTrackLength;        }
   //! Get Z of particle
   Int_t    GetPartZ()                 const { return fZ;                  }
   //! Get A of particle
   Int_t    GetPartA()                 const { return fA;                  }
   //! Get step
   Double_t GetStep()                  const { return fStep;               }

   // Setter
   //! Set A for particle
   void SetPartA(Double_t A)                 { fA = A;                     }
   //! Set Z for particle
   void SetPartZ(Double_t Z)                 { fZ = Z;                     }
   //! Set step
   void SetStep(Double_t step)               { fStep = step;               }
   
private:
   TADIgeoField* fField;     ///< magnetic field
   Double_t   fTrackLength;  ///< track length
   Double_t   fNormP;        ///< Norm of momentum
   TVector3   fDerivative;   ///< First derivative
   TVector3   fPosition;     ///< Position
   Int_t      fZ;            ///< Z of particle
   Int_t      fA;            ///< A of particle
   Double_t   fStep;         ///< Step size

private:
   //! Solving Lorentz equation
   TVector3 SolveLorentz(TVector3 u, TVector3 field);
   //! Runge-Kutta fourth computing
   void RungeKutta4();
   
private:
   //! Conversion factor for light velocity
   static const Double_t fgkConvFactor;
   
};

#endif
