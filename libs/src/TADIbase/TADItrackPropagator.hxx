#ifndef _TADItrackPropagator_HXX
#define _TADItrackPropagator_HXX
/*!
  \file
  \brief   Declaration of TADItrackPropagator.
  Inspired from the macro of A. Sécher
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TAGobject.hxx"
#include "TVector3.h"


//##############################################################################

class FootField;
class TADItrackPropagator : public TAGobject {
   
public:
   TADItrackPropagator(FootField* field);
   
   Bool_t	ExtrapoleZ(TVector3& v, TVector3& p, Double_t posZ, TVector3& vOut, TVector3& pOut);
   
   TVector3 GetFieldB(TVector3 vertex)          { return fField->get(vertex); }
   
private:
   FootField* fField;
   Double_t   fTrackLength;
   Double_t   fNormP;
   TVector3   fDerivative;
   TVector3   fPosition;
   Int_t      fZ;
   Int_t      fA;
   
   Double_t   fStep;

private:
   
   TVector3 SolveLorentz(TVector3 u, TVector3 field);
   void RungeKutta4();
   
private:
   static Double_t fgkConvFactor;
   
   
   ClassDef(TADItrackPropagator,1)
};

#endif
