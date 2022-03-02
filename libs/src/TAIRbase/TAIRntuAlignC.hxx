#ifndef _TAIRntuAlignC_HXX
#define _TAIRntuAlignC_HXX
/*!
 \file TAIRntuAlignC.hxx
 \brief   Declaration of TAIRntuAlignC.
 
 \author C.A. Reidel & Ch. Finck
 */
/*------------------------------------------+---------------------------------*/
#include "TAGdata.hxx"
#include "TAGparaDsc.hxx"

#include "Riostream.h"
#include "TString.h"
#include "TArrayI.h"
#include "TMatrixD.h"

class TAIRntuAlignC : public TAGdata {
   
public:
   TAIRntuAlignC(TArrayI* arraySensor, Int_t* pConf = 0);
   ~TAIRntuAlignC();
   
public:
   // Reset
   void   Reset();
   // Define constants
   void   DefineConstant(Double_t* weightQ, Double_t* position);
   // Accumulate
   Bool_t Accumulate(Double_t* Ucluster, Double_t* Vcluster);
   // Sum
   void   Sum();
   // Constraint
   void   Constraint(Int_t planeRef1, Int_t planeRef2);
   // Minimize
   void   Minimize();
   
   //! Return offset U
   Float_t*     GetOffsetU() const { return fAlignOffsetU; }
   //! Return offset V
   Float_t*     GetOffsetV() const { return fAlignOffsetV; }
   //! Return tilt angle W
   Float_t*     GetTiltW()   const { return fAlignTiltW;   }
   
private:
   Int_t*      fpDevStatus;      ///< device status array
   TArrayI*    fSensorArray;     ///< contains the sensor number to be aligned
   
   Float_t*    fAlignTiltW;      ///< Tilt angle array
   Float_t*    fAlignOffsetU;    ///< Alignment in U
   Float_t*    fAlignOffsetV;    ///< Alignment in V
   
   Double_t    fSumZiWiQ;        ///< Sum of weighted Q
   Double_t    fSumZiQWiQ;       ///< Sum Q
   
   Int_t       fColumnY;         ///< Column Y
   Int_t       fColumnPhi;       ///< Column phi
   
   TMatrixD    fQ;               ///< Q matrices
   TMatrixD    fCxInit;          ///< = Q . Px
   TMatrixD    fCyInit;          ///< = Q . Py
   TMatrixD    fCx;              ///< base matrix in X
   TMatrixD    fCy;              ///< base matrix in Y
   TMatrixD    fCxtCx;           ///< Product matrices
   TMatrixD    fCytCy;           ///< Product matrices
   TMatrixD    fCxtCxT;          ///< Product matrices
   TMatrixD    fCytCyT;          ///< Product matrices
   
   TMatrixD    fDxInit;          ///< vector = Q . Xvector
   TMatrixD    fDyInit;          ///< vector = Q . Yvector
   TMatrixD    fDx;              ///< base vector in X
   TMatrixD    fDy;              ///< base vector in Y
   
   TMatrixD    fTermeL;          ///< termL
   TMatrixD    fTermeCst;        ///< term constant
   
   TMatrixD    fCm;              ///< tmp matrix
   TMatrixD    fDm;              ///< tmp matrix
   TMatrixD    fCmf;             ///< tmp matrix
   TMatrixD    fDmf;             ///< tmp matrix
   TMatrixD    fAv;              ///< tmp matrix
   
   ClassDef(TAIRntuAlignC,0)
};

#endif
