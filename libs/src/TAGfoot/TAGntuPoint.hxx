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
#include "TATWntuRaw.hxx"


/** TAGpoint class is the global point for global reconstruction
 
 */
/*------------------------------------------+---------------------------------*/

class TAGpoint : public TAGobject {
   
private:
   TVector3    fPosition;      // position in FOOT framework
   Double32_t  fTime;          // Time information
   Double32_t  fChargeZ;       // Charge Z
   Double32_t  fChargeProbaZ;  // Probability of charge Z
   
public:
   
   TAGpoint();
   TAGpoint(TVector3 pos, Double_t time, Double_t chargeZ, Double_t probaZ);
   ~TAGpoint() {};
   
   //    All the Get methods
   TVector3    GetPosition()         const  { return fPosition;   }
   Double32_t  GetTime()             const  { return fTime;      }
   Int_t       GetChargeZ()          const  { return fChargeZ;   }
   Double32_t  GetChargeProbaZ()     const  { return fChargeZ;   }
   
   void        SetPosition(TVector3 pos)    { fPosition = pos;    }
   void        SetChargeZ(Int_t z)          { fChargeZ = z;       }
   void        SetChargeProbaZ(Double_t z)  { fChargeProbaZ = z;  }
   void        Clear(Option_t* opt);
   
   ClassDef(TAGpoint,1)
};

//##############################################################################

class TAGntuPoint : public TAGdata {
   
private:
   
   TClonesArray*     fListOfPoints;
   
public:

	TAGntuPoint();
	virtual ~TAGntuPoint();
	
	TAGpoint*         NewPoint(TVector3 pos, Double_t time, Double_t chargeZ, Double_t probaZ);

	Int_t             GetPointsN();
	TAGpoint*         GetPoint(Int_t iPoint );


	virtual void      Clear(Option_t* opt="");

	// delete?
	virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
 
   virtual void      SetupClones();

public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:
   static TString fgkBranchName;    // Branch name in TTree
   
   ClassDef(TAGntuPoint,1)
};

#endif



















