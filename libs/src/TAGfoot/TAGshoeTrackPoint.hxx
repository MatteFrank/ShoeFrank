#ifndef _TAGshoeTrackPoint_HXX
#define _TAGshoeTrackPoint_HXX
/*!
 \file
 \version $Id: TAGshoeTrackPoint.hxx,v 1.0 2011/04/01 18:11:59 Franchini Exp $
 \brief   Declaration of TAGshoeTrackPoint.
 */
/*------------------------------------------+---------------------------------*/

#include <map>

// ROOT classes
#include "TVector3.h"
#include <TMatrixD.h>
#include "TArrayI.h"

#include "TAGdata.hxx"
#include "TAGcluster.hxx"

/** TAGshoeTrackPoint class is the global point for global reconstruction
 
 */
/*------------------------------------------+---------------------------------*/
// class TClonesArray;
class TAGshoeTrackPoint : public TAGcluster {
   
private:
   TString     fDevName;       // Device name (VT,IT, MSD, TW, CA)
   TVector3    fPosition;      // position in FOOT framework
   TVector3    fPosError;      // position error in FOOT framework
   TVector3    fMomentum;      // momentum in FOOT framework
   TVector3    fMomError;      // momentum error in FOOT framework
   Int_t       fChargeZ;       // Charge Z

public:
   TAGshoeTrackPoint();
   // TAGshoeTrackPoint(TVector3 pos, TVector3 posErr);
   // TAGshoeTrackPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ = 0);
   // TAGshoeTrackPoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr, Int_t chargeZ = 0);
   
   // Genfit
   TAGshoeTrackPoint( int trackDetID, int iPlane, int iClus, vector<int> iPart, TVector3* measPos );
   // TAGshoeTrackPoint( int trackDetID, int iPlane, int iClus, int iPart, TVector3 measPos, TVector3 momErr );
   ~TAGshoeTrackPoint() {};
  
   void SetRecoInfo( TVector3* recoPos, TVector3* recoMom, TMatrixD* recoPos_cov, TMatrixD* recoMom_cov );

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
  
   ClassDef(TAGshoeTrackPoint,5)
};

#endif



