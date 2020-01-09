#ifndef _TAGntuPoint_HXX
#define _TAGntuPoint_HXX
/*!
 \file
 \version $Id: TAGntuPoint.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAGntuPoint.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGdata.hxx"
#include <map>

// ROOT classes
#include "TVector3.h"

#include "TAGobject.hxx"

/** TAGpoint class is the global point for global reconstruction
 
 */
/*------------------------------------------+---------------------------------*/
class TClonesArray;
class TAGpoint : public TAGobject {
   
private:
   TVector3    fPosition;      // position in FOOT framework
   TVector3    fPosError;      // position error in FOOT framework
   TVector3    fMomentum;      // momentum in FOOT framework
   TVector3    fMomError;      // momentum error in FOOT framework
   Double32_t  fTime;          // Time information
   Double32_t  fChargeZ;       // Charge Z
   Double32_t  fChargeProbaZ;  // Probability of charge Z
   
public:
   TAGpoint();
   TAGpoint(TVector3 pos, TVector3 posErr, Double_t time, Double_t chargeZ, Double_t probaZ);
   TAGpoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Double_t time, Double_t chargeZ, Double_t probaZ);
   ~TAGpoint() {};
   
   //    All the Get methods
   TVector3    GetPosition()         const  { return fPosition;   }
   TVector3    GetPosError()         const  { return fPosError;   }
   TVector3    GetMomentum()         const  { return fMomentum;   }
   TVector3    GetMomError()         const  { return fMomError;   }

   Double32_t  GetTime()             const  { return fTime;       }
   Int_t       GetChargeZ()          const  { return fChargeZ;    }
   Double32_t  GetChargeProbaZ()     const  { return fChargeZ;    }
   
   void        SetTime(Double_t time)       { fTime = time;       }
   void        SetPosition(TVector3 pos)    { fPosition = pos;    }
   void        SetPosError(TVector3 pos)    { fPosError = pos;    }
   void        SetMomentum(TVector3 mom)    { fMomentum = mom;    }
   void        SetMomError(TVector3 mom)    { fMomError = mom;    }
   void        SetChargeZ(Int_t z)          { fChargeZ = z;       }
   void        SetChargeProbaZ(Double_t z)  { fChargeProbaZ = z;  }
   
   void        Clear(Option_t* opt);
   
   ClassDef(TAGpoint,2)
};

//##############################################################################

class TAGntuPoint : public TAGdata {
   
private:
   TClonesArray*     fListOfPoints;
   
public:
	TAGntuPoint();
	virtual ~TAGntuPoint();
	
   TAGpoint*         NewPoint(TVector3 pos, TVector3 posErr, Double_t time, Double_t chargeZ, Double_t probaZ);
   TAGpoint*         NewPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Double_t time, Double_t chargeZ, Double_t probaZ);

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



















