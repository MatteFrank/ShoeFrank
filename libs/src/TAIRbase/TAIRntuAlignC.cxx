/*!
 \file TAIRntuAlignC.cxx
 \brief   Implementation of TAIRntuAlignC.
 */
#include "TMath.h"
#include "TVector3.h"
#include "TMatrixD.h"
#include "TArrayD.h"

#include "TAVTparConf.hxx"

#include "TAIRntuAlignC.hxx"


/*!
 \class TAIRntuAlignC
 \brief Alignment class. **
 */

//! Class Imp
ClassImp(TAIRntuAlignC);


//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] arraySensor sensor index array
//! \param[in] pDevStatus sensor status array
TAIRntuAlignC::TAIRntuAlignC(TArrayI* arraySensor, Int_t* pDevStatus)
: TAGdata(),
  fSumZiWiQ(0),
  fSumZiQWiQ(0),
  fColumnY(-1),
  fColumnPhi(-1),
  fpDevStatus(pDevStatus)
{
   
   fSensorArray = arraySensor;
   
   fColumnY = fSensorArray->GetSize();
   fColumnPhi = fSensorArray->GetSize()*2;
   
   fQ.ResizeTo(fSensorArray->GetSize(),fSensorArray->GetSize());

   fCxInit.ResizeTo(fSensorArray->GetSize(),   3*fSensorArray->GetSize());
   fCyInit.ResizeTo(fSensorArray->GetSize(),   3*fSensorArray->GetSize());
   fCx.ResizeTo(fSensorArray->GetSize(),       3*fSensorArray->GetSize());
   fCy.ResizeTo(fSensorArray->GetSize(),       3*fSensorArray->GetSize());
   fCxtCx.ResizeTo(3*fSensorArray->GetSize(),  3*fSensorArray->GetSize());
   fCytCy.ResizeTo(3*fSensorArray->GetSize(),  3*fSensorArray->GetSize());
   fCxtCxT.ResizeTo(3*fSensorArray->GetSize(), 3*fSensorArray->GetSize());
   fCytCyT.ResizeTo(3*fSensorArray->GetSize(), 3*fSensorArray->GetSize());
   
   fDxInit.ResizeTo(fSensorArray->GetSize(), 1);
   fDyInit.ResizeTo(fSensorArray->GetSize(), 1);
   fDx.ResizeTo(fSensorArray->GetSize(), 1);
   fDy.ResizeTo(fSensorArray->GetSize(), 1);
   
   fCm.ResizeTo(3*fSensorArray->GetSize(), 3*fSensorArray->GetSize());
   fDm.ResizeTo(1,3*fSensorArray->GetSize());
   fCmf.ResizeTo((3*fSensorArray->GetSize()-5),(3*fSensorArray->GetSize()-5));
   fDmf.ResizeTo((3*fSensorArray->GetSize()-5),1);
   
   fTermeL.ResizeTo(3*fSensorArray->GetSize(), 3*fSensorArray->GetSize());
   fTermeCst.ResizeTo(1,3*fSensorArray->GetSize());
   
   fAv.ResizeTo((3*fSensorArray->GetSize()-5),1);
   
   fAlignTiltW   = new Float_t[fSensorArray->GetSize()];
   fAlignOffsetU = new Float_t[fSensorArray->GetSize()];
   fAlignOffsetV = new Float_t[fSensorArray->GetSize()];
   
   for(Int_t i = 0; i < fSensorArray->GetSize(); i++){
      fAlignTiltW[i]   = 0;
      fAlignOffsetU[i] = 0;
      fAlignOffsetV[i] = 0;
   }
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAIRntuAlignC::~TAIRntuAlignC()
{
   delete[] fAlignTiltW;
   delete[] fAlignOffsetU;
   delete[] fAlignOffsetV;
   
}
//______________________________________________________________________________
//! Accumulation of the coordinates of the cluster for each plane with the constans defined before
//!
//! \param[in] Ucluster cluster position in U-direction
//! \param[in] Vcluster cluster position in V-direction
Bool_t TAIRntuAlignC::Accumulate(Double_t* Ucluster, Double_t* Vcluster)
{
   fCx = fCxInit;
   fCy = fCyInit;
   fDx = fDxInit;
   fDy = fDyInit;
   
   for (Int_t iSensor = 0; iSensor < fSensorArray->GetSize(); iSensor++){
      if (Ucluster[iSensor] == 999999 || Vcluster[iSensor] == 999999) return false;
      
      TMatrixDColumn(fCx,iSensor+fColumnPhi) *= Vcluster[iSensor];
      TMatrixDColumn(fCy,iSensor+fColumnPhi) *= (-Ucluster[iSensor]);
      
      for (Int_t iElement = 0; iElement < fSensorArray->GetSize(); iElement++){
         fDx(iSensor,0) += ( fQ(iSensor,iElement) * Ucluster[iElement]);
         fDy(iSensor,0) += ( fQ(iSensor,iElement) * Vcluster[iElement]);
      }
   }
   
   return true;
}

//______________________________________________________________________________
//! Addition of the new matrice and vector with the previous one
void TAIRntuAlignC::Sum()
{
   fCxtCx = TMatrixD(fCx,TMatrixD::kTransposeMult,fCx);
   fCxtCxT = fCxtCx.T();
   
   fCytCy = TMatrixD(fCy,TMatrixD::kTransposeMult,fCy);
   fCytCyT = fCytCy.T();
   
   fTermeL = fCxtCx + fCxtCxT + fCytCy + fCytCyT;
   
   fTermeCst = TMatrixD(fDx,TMatrixD::kTransposeMult,fCx) + TMatrixD(fDy,TMatrixD::kTransposeMult,fCy);
   
   fCm += fTermeL;
   fDm += fTermeCst;
   
   return;
}
//______________________________________________________________________________
//! The final vector containing the alignment parameters are calculated
void TAIRntuAlignC::Minimize()
{
   TMatrixD CMatrixFinal = fCmf;
   TMatrix dVectorFinal  = fDmf;
   
   TMatrixD invCMatrixFinal = CMatrixFinal.Invert();
   fAv.Mult(invCMatrixFinal,dVectorFinal);
   
   Int_t counterUV = 0;
   Int_t counterW  = 0;
      
   for (Int_t i = 0; i < fSensorArray->GetSize(); i++){
      Int_t iSensor = fSensorArray->At(i);
      
      if (fpDevStatus[i] == 0){
         fAlignOffsetU[i] = 0;
         fAlignOffsetV[i] = 0;
         fAlignTiltW[i] = 0;
      }
      else if (fpDevStatus[i] == 1){
         fAlignOffsetU[i] = 0;
         fAlignOffsetV[i] = 0;
         fAlignTiltW[i]   = fAv(2*(fSensorArray->GetSize()-2)+counterW, 0);
         counterW++;
      }
      else {
         fAlignOffsetU[i] = fAv(counterUV, 0);
         fAlignOffsetV[i] = fAv(fSensorArray->GetSize()-2+counterUV, 0);
         fAlignTiltW[i]   = fAv(2*(fSensorArray->GetSize()-2)+counterW, 0);
         counterUV++;
         counterW++;
      }
   }
   
   return;
}
//______________________________________________________________________________
//! Delete the lines and columns of the final matrice and vector corresponding to the alignment parameters.
//! In this step we constraint the tracker in a global frame
//!
//! \param[in] planeRef1 plane index for reference 1
//! \param[in] planeRef2 plane index for reference 2
void TAIRntuAlignC::Constraint(Int_t planeRef1, Int_t planeRef2)
{
   TMatrixD CMatrix = fCm;
   TMatrix dVector  = fDm;
   TArrayD arrMatrix;
   TArrayD arrVector;
   
   for (Int_t line = 0; line < (3*fSensorArray->GetSize()); line++) {
      if ((line != planeRef1) && (line != planeRef2) && (line != (fSensorArray->GetSize() + planeRef1)) &&
          (line != (fSensorArray->GetSize() + planeRef2)) && (line != (2*fSensorArray->GetSize() + planeRef1))){
         
         for (Int_t column = 0; column < (3*fSensorArray->GetSize()); column++) {
            if ((column != planeRef1) && (column != planeRef2) && (column != (fSensorArray->GetSize() + planeRef1)) &&
                (column != (fSensorArray->GetSize() + planeRef2)) && (column != (2*fSensorArray->GetSize() + planeRef1))){
               arrMatrix.Set(arrMatrix.GetSize()+1);
               arrMatrix.AddAt(CMatrix(line,column), arrMatrix.GetSize()-1);
            }
         }
         arrVector.Set(arrVector.GetSize()+1);
         arrVector.AddAt((2)*dVector(0,line), arrVector.GetSize()-1);
         
      }
   }
   
   fCmf.SetMatrixArray(arrMatrix.GetArray());
   fDmf.SetMatrixArray(arrVector.GetArray());
   
   return;
}
//______________________________________________________________________________
//! All the constants used later are initialized in this function
//!
//! \param[in] weightQ weight position
//! \param[in] position position
void TAIRntuAlignC::DefineConstant(Double_t* weightQ, Double_t* position)
{
   TMatrixD    A;
   TMatrixD    B;
   TMatrixD    L;
   TMatrixD    K;
   TMatrixD    In;
   
   A.ResizeTo(2,2);
   B.ResizeTo(2,fSensorArray->GetSize());
   L.ResizeTo(2,fSensorArray->GetSize());
   K.ResizeTo(fSensorArray->GetSize(),2);
   In.ResizeTo(fSensorArray->GetSize(),fSensorArray->GetSize());

   for (Int_t i = 0; i < fSensorArray->GetSize(); i++){
      fSumZiWiQ += (position[i] * weightQ[i]);
      fSumZiQWiQ += (position[i] * position[i] * weightQ[i]);
   }
   
   for (Int_t iSensor = 0; iSensor < fSensorArray->GetSize(); iSensor++){
      B(0,iSensor) = weightQ[iSensor];
      B(1,iSensor) = weightQ[iSensor] * position[iSensor];
      K(iSensor,0) = 1;
      K(iSensor,1) = position[iSensor];
      A(0,0) += weightQ[iSensor];
   }
   
   A(0,1) = fSumZiWiQ;
   A(1,0) = fSumZiWiQ;
   A(1,1) = fSumZiQWiQ;
   
   TMatrixD invA = A.Invert();
   L.Mult(invA,B);
   In.UnitMatrix();
   fQ = TMatrixD(K,TMatrixD::kMult,L) - In;
   
   for (Int_t iSensor = 0; iSensor < fSensorArray->GetSize(); iSensor++){
      TMatrixDRow(fQ,iSensor) *= sqrt(weightQ[iSensor]);
   }
   
   for (Int_t iSensor = 0; iSensor < fSensorArray->GetSize(); iSensor++){
      TMatrixDColumn(fCxInit,iSensor)              = TMatrixDColumn(fQ,iSensor);
      TMatrixDColumn(fCxInit,(iSensor+fColumnPhi)) = TMatrixDColumn(fQ,iSensor);
      TMatrixDColumn(fCyInit,(iSensor+fColumnY))   = TMatrixDColumn(fQ,iSensor);
      TMatrixDColumn(fCyInit,(iSensor+fColumnPhi)) = TMatrixDColumn(fQ,iSensor);
   }
   
   return;
}
//______________________________________________________________________________
//! Reset function
void TAIRntuAlignC::Reset()
{
   fCm.Zero();
   fDm.Zero();
   
   return;
}
