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
   TVector3    fPosition;      // position in FOOT framework
   TVector3    fPosError;      // position error in FOOT framework
   TVector3    fMomentum;      // momentum in FOOT framework
   TVector3    fMomError;      // momentum error in FOOT framework
   Int_t       fChargeZ;       // Charge Z

public:
   TAGpoint();
   TAGpoint(TVector3 pos, TVector3 posErr);
   TAGpoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ = 0);
   TAGpoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ = 0);
   ~TAGpoint() {};
  
   // return detector name
   const Char_t*    GetDevName()     const  { return fDevName.Data(); }
  
   // All the Get methods
   const TVector3&  GetPosition()    const  { return fPosition;   }
   const TVector3&  GetPosError()    const  { return fPosError;   }
   
   // return in the same framework of FOOT
   const TVector3&  GetPositionG()   const  { return fPosition;   }
   const TVector3&  GetPosErrorG()   const  { return fPosError;   }

   TVector3         GetMomentum()    const  { return fMomentum;   }
   TVector3         GetMomError()    const  { return fMomError;   }
   Int_t            GetChargeZ()     const  { return fChargeZ;    }
      
   void        SetDevName(TString name )    { fDevName = name;    }
   void        SetPosition(TVector3 pos)    { fPosition = pos;    }
   void        SetPosError(TVector3 pos)    { fPosError = pos;    }
   void        SetMomentum(TVector3 mom)    { fMomentum = mom;    }
   void        SetMomError(TVector3 mom)    { fMomError = mom;    }
   void        SetChargeZ(Int_t z)          { fChargeZ = z;       }
   void        Clear(Option_t* opt);
  
   ClassDef(TAGpoint,5)
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



















