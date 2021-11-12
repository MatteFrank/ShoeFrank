#ifndef _TAGntuPoint_HXX
#define _TAGntuPoint_HXX
/*!
 \file
 \version $Id: TAGntuPoint.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAGntuPoint.
 */
/*------------------------------------------+---------------------------------*/

#include <map>

// ROOT classes
#include "TVector3.h"
#include "TArrayI.h"

#include "TAGdata.hxx"
#include "TAGcluster.hxx"

/** TAGpoint class is the global point for global reconstruction
 
 */
/*------------------------------------------+---------------------------------*/
class TClonesArray;
class TAGpoint : public TAGcluster {
   
private:
   TString     fDevName;       // Device name (VT,IT, MSD, TW, CA)
   TVector3    fMeasMom;       // measured momentum in FOOT framework
   TVector3    fMeasMomError;  // measured momentum error in FOOT framework
   TVector3    fFitMom;        // fitted momentum in FOOT framework
   TVector3    fFitMomError;   // fitted momentum error in FOOT framework
   Double32_t  fEnergyLoss;    // Energy loss in sensor

public:
   TAGpoint();
   TAGpoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr);
   TAGpoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 measMom, TVector3 measMomErr, TVector3 fitMom, TVector3 fitMomErr);
   TAGpoint(TString name, TVector3 measPos, TVector3 measPosErr);
   TAGpoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr);
   TAGpoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 measMom, TVector3 measMomErr, TVector3 fitMom, TVector3 fitMomErr);
   ~TAGpoint() {};
  
   // return detector name
   const Char_t*    GetDevName()     const  { return fDevName.Data(); }
  
   // return in the same framework of FOOT
   const TVector3&  GetPositionG()    const  { return fPosition;   }
   const TVector3&  GetPosErrorG()    const  { return fPosError;   }

   TVector3         GetMeasMomentum() const  { return fMeasMom;      }
   TVector3         GetMeasMomError() const  { return fMeasMomError; }
   TVector3         GetFitMomentum()  const  { return fFitMom;       }
   TVector3         GetFitMomError()  const  { return fFitMomError;  }
   Double_t         GetEnergyLoss()   const  { return fEnergyLoss;   }

   void        SetDevName(TString name )     { fDevName = name;     }
   void        SetMeasMomentum(TVector3 mom) { fMeasMom = mom;      }
   void        SetMeasMomError(TVector3 mom) { fMeasMomError = mom; }
   void        SetFitMomentum(TVector3 mom)  { fFitMom = mom;       }
   void        SetFitMomError(TVector3 mom)  { fFitMomError = mom;  }
   void        SetEnergyLoss(Double_t e)     { fEnergyLoss = e;     }
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
	
   TAGpoint*         NewPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr);
   TAGpoint*         NewPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 measMom, TVector3 measMomErr, TVector3 fitMom, TVector3 fitMomErr);

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



















