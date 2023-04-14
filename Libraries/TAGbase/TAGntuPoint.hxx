#ifndef _TAGntuPoint_HXX
#define _TAGntuPoint_HXX

/*!
 \file TAGntuPoint.hxx
 \brief   Declaration of TAGntuPoint & TAGpoint.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include <string>
#include <vector>

///< ROOT classes
#include "TVector3.h"
#include <TMatrixD.h>
#include "TArrayI.h"

#include "TAGdata.hxx"
#include "TAGcluster.hxx"

class TClonesArray;
class TAGpoint : public TAGcluster {
   
private:
   TString     fDevName;    ///< Device name (VT,IT, MSD, TW, CA)
   TVector3    fMomentum;   ///< fitted momentum in FOOT framework
   TVector3    fMomError;   ///< fitted momentum error in FOOT framework
   Double32_t  fEnergyLoss; ///< Energy loss in sensor

public:
   TAGpoint();
   TAGpoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr);
   TAGpoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr);
   TAGpoint(TString name, TVector3 measPos, TVector3 measPosErr);
   TAGpoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr);
   TAGpoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr);
   TAGpoint(string trackDetID, int iPlane, int iClus, vector<int>* iPart, TVector3* measPos, TVector3* measPosErr);
   ~TAGpoint() {};

   void SetRecoInfo( TVector3* recoPos, TVector3* recoMom, TMatrixD* recoPos_cov, TMatrixD* recoMom_cov );

   double EvalError( TVector3 mom, TMatrixD cov );
   TVector3 EvalError( TMatrixD cov );
  
   //! return detector name
   const Char_t*    GetDevName()     const  { return fDevName.Data(); }
  
   //! return position in framework of FOOT
   const TVector3&  GetPositionG()    const  { return fPosition2;  }
   //! return position error in framework of FOOT
   const TVector3&  GetPosErrorG()    const  { return fPosError2;  }

   //! Get momentum
   TVector3         GetMomentum()     const  { return fMomentum;   }
   //! Get momentum error
   TVector3         GetMomError()     const  { return fMomError;   }
   //! Get energy loss
   Double_t         GetEnergyLoss()   const  { return fEnergyLoss; }

   //!Set device name
   void        SetDevName(TString name )     { fDevName = name;    }
   //!Set momentum
   void        SetMomentum(TVector3 mom)     { fMomentum = mom;    }
   //!Set momentum name
   void        SetMomError(TVector3 mom)     { fMomError = mom;    }
   //!Set energy loss
   void        SetEnergyLoss(Double_t e)     { fEnergyLoss = e;    }
   
   // Clear
   void        Clear(Option_t* opt);
  
   ClassDef(TAGpoint,10)
};

//##############################################################################

class TAGntuPoint : public TAGdata {
   
private:
   TClonesArray*     fListOfPoints;
   
public:
	TAGntuPoint();
	virtual ~TAGntuPoint();
	
   // New point from measured/fitted position/error
   TAGpoint*         NewPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr);
   // New point from measured/fitted position/error and momentum/error
   TAGpoint*         NewPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr);

   //! Get number of points
	Int_t             GetPointsN();
   //! Get point
	TAGpoint*         GetPoint(Int_t iPoint );

   // Clear
	virtual void      Clear(Option_t* opt="");
   // To stream
	virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   // Set up clones
   virtual void      SetupClones();

public:
   //! Get branch name
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
   static TString fgkBranchName;    ///< Branch name in TTree
   
   ClassDef(TAGntuPoint,1)
};

#endif



















