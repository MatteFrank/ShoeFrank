#ifndef _TAMSDntuPoint_HXX
#define _TAMSDntuPoint_HXX
/*!
 \file
 \version $Id: TAMSDntuPoint.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAMSDntuPoint.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGcluster.hxx"
#include <map>

// ROOT classes
#include "TVector3.h"

#include "TAGobject.hxx"
#include "TAMSDntuCluster.hxx"

class TAMSDparGeo;


/** TAMSDpoint class is the MSD hit reconstructed by the intersection of 2 clusters on a
	column and a row cluster respectively

	All the coordinates are in cm and in the detector reference frame, i.e. the center
	is the center of the detector.

 */
/*------------------------------------------+---------------------------------*/

class TAMSDpoint : public TAGcluster {

private:
   int              fStation;    // number of MSD tracking station
   TAMSDcluster*    fColClus;    // cluster column
   TAMSDcluster*    fRowClus;    // cluster col

   //generic methods
   Double32_t  fDe1;           // energy loss in 1st strip
   Double32_t  fDe2;           // energy loss in 2nd strip
   Double32_t  fTime;

   int         fChargeZ;       // raw guess of charge Z
   Double32_t  fChargeZProba;  // raw guess of charge Z probability

public:
   TAMSDpoint();
   TAMSDpoint( int layer, double x, double y, TVector3 position);
   TAMSDpoint(Int_t layer, Double_t x, Double_t dx, TAMSDcluster* clusX, Double_t y, Double_t dy, TAMSDcluster* clusY);

   virtual ~TAMSDpoint() {};

   //    All the Get methods
   int       GetColClusId()      const  { return fColClus->GetClusterIdx(); }
   int       GetRowClusId()      const  { return fRowClus->GetClusterIdx(); }

   int       GetLayer()          const  { return fStation;      }
   int       GetStation()        const  { return fStation;      }
   
   TAMSDcluster*  GetColClus()   const  { return fColClus;      }
   TAMSDcluster*  GetRowClus()   const  { return fRowClus;      }

   double    GetEnergyLoss1()    const  { return fDe1;          }
   double    GetEnergyLoss2()    const  { return fDe2;          }
   double    GetEnergyLoss()     const  { return fDe1+fDe2;     }
   double    GetTime()           const  { return fTime;         }
   int       GetChargeZ()        const  { return fChargeZ;      }
   double    GetChargeZProba()   const  { return fChargeZProba; }

   void      SetChargeZ(int z)          { fChargeZ = z;         }
   void      SetChargeZProba(double p)  { fChargeZProba = p;    }
   void      Clear(Option_t* opt);

   ClassDef(TAMSDpoint,5)
};

//##############################################################################

class TAMSDntuPoint : public TAGdata {

private:
  TAMSDparGeo*         fGeometry; //!
  TObjArray*           fListOfPoints;

public:
  TAMSDntuPoint();
  virtual ~TAMSDntuPoint();

  TAMSDpoint*         NewPoint( int iStation, double x, double y, TVector3 position);
  TAMSDpoint*         NewPoint(Int_t iStation, Double_t x, Double_t dx, TAMSDcluster* clusX, Double_t y, Double_t dy, TAMSDcluster* clusY);

  int                 GetPointN(int iStation) const;
  int                 GetPointN_includingGhosts();

  TClonesArray*       GetListOfPoints(int iStation) const;

  TAMSDpoint*         GetPoint( int iStation, int iPoint ) const ;


  virtual void        Clear(Option_t* opt="");

  // delete?
  virtual void        ToStream(ostream& os=cout, Option_t* option="") const;

  virtual void        SetupClones();

public:
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

private:
  static TString fgkBranchName;    // Branch name in TTree

  ClassDef(TAMSDntuPoint,2)
};

#endif
