#ifndef _TAGntuPoint_HXX
#define _TAGntuPoint_HXX
/*!
 \file
 \version $Id: TAGntuPoint.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAGntuPoint.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include <string>
#include <vector>

// ROOT classes
#include "TVector3.h"
#include <TMatrixD.h>
#include "TArrayI.h"

#include "TAGdata.hxx"
#include "TAGcluster.hxx"

using namespace std;

/** TAGpoint class is the global point for global reconstruction
 
 */
/*------------------------------------------+---------------------------------*/
class TClonesArray;
class TAGpoint : public TAGcluster {
   
protected:
   TString     fDevName;       // Device name (VT,IT, MSD, TW, CA)
   TVector3    fMomentum;      // momentum in FOOT framework
   TVector3    fMomError;      // momentum error in FOOT framework
   Int_t       fChargeZ;       // Charge Z
   Double32_t  fEnergyLoss;    // Energy loss in sensor

   //genfit
	int m_planeID;
	int m_clusterID; 
	vector<int> m_iPart;
	TVector3 m_measPos;

	TMatrixD m_recoPos_cov;
	TMatrixD m_recoMom_cov;

public:
   TAGpoint();
   TAGpoint(TVector3 pos, TVector3 posErr);
   TAGpoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ = 0);
   TAGpoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ = 0);
   TAGpoint(TString name, TVector3 pos, TVector3 posErr);
   TAGpoint(string trackDetID, int iPlane, int iClus, vector<int> iPart, TVector3* measPos);
   ~TAGpoint() {};

   void SetRecoInfo( TVector3* recoPos, TVector3* recoMom, TMatrixD* recoPos_cov, TMatrixD* recoMom_cov );

   double EvalError( TVector3 mom, TMatrixD cov );
   TVector3 EvalError( TMatrixD cov );
  
   // return detector name
   const Char_t*    GetDevName()     const  { return fDevName.Data(); }		 //(also Genfit)
  
   // return in the same framework of FOOT
   const TVector3&  GetPositionG()   const  { return fPosition;   }		 //(also Genfit)
   const TVector3&  GetPosErrorG()   const  { return fPosError;   }		 //(also Genfit)

   TVector3         GetMomentum()    const  { return fMomentum;   }		 //(also Genfit)
   TVector3         GetMomError()    const  { return fMomError;   }		 //(also Genfit)
   Int_t            GetChargeZ()     const  { return fChargeZ;    }
   Double_t         GetEnergyLoss()  const  { return fEnergyLoss; }

   // genfit
   int 				GetPlaneID()				 { return m_planeID; 	}
   int 				GetClusterID()				 { return m_clusterID; 	}
   vector<int> 		GetGeneratedPartID()		 { return m_iPart; 		}
   TVector3         GetMeasurePos()				 { return m_measPos;   	}
   TMatrixD         GetRecoPos_Covariance()      { return m_recoPos_cov;}
   TMatrixD         GetRecoMom_Covariance()      { return m_recoMom_cov;}

   void        SetDevName(TString name )    { fDevName = name;    }
   void        SetMomentum(TVector3 mom)    { fMomentum = mom;    }
   void        SetMomError(TVector3 mom)    { fMomError = mom;    }
   void        SetChargeZ(Int_t z)          { fChargeZ = z;       }
   void        SetEnergyLoss(Double_t e)    { fEnergyLoss = e;    }
   void        Clear(Option_t* opt);
  
   ClassDef(TAGpoint,9)
};

//##############################################################################

class TAGntuPoint : public TAGdata {
   
private:
   TClonesArray*     fListOfPoints;
   
public:
	TAGntuPoint();
	virtual ~TAGntuPoint();
	
   TAGpoint*         NewPoint(TVector3 pos, TVector3 posErr);
   TAGpoint*         NewPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ = 0);

	Int_t             GetPointsN();
	TAGpoint*         GetPoint(Int_t iPoint );

	virtual void      Clear(Option_t* opt="");

	virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
 
   virtual void      SetupClones();

public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TAGntuPoint,1)
};

#endif



















