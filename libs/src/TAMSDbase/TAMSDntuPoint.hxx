#ifndef _TAMSDntuPoint_HXX
#define _TAMSDntuPoint_HXX
/*!
 \file TAMSDntuPoint.hxx
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
class TAMSDtrack;

/*!
 \class TAMSDpoint
 \brief TAMSDpoint class is the MSD hit reconstructed by the intersection of 2 clusters on a
        column and a row cluster respectively
        All the coordinates are in cm and in the detector reference frame, i.e. the center
        is the center of the detector.
 */

class TAMSDpoint : public TAGcluster {

private:
   int         fStation;       ///< number of MSD tracking station
   Double32_t  fDe1;           ///< energy loss in 1st strip
   Double32_t  fDe2;           ///< energy loss in 2nd strip
   Double32_t  fTime;

   int         fChargeZ;       ///< raw guess of charge Z
   Double32_t  fChargeZProba;  ///< raw guess of charge Z probability

public:
   TAMSDpoint();
   TAMSDpoint(const TAMSDpoint& point);
   TAMSDpoint( int layer, double x, double y, TVector3 position);
   TAMSDpoint(Int_t layer, TAMSDcluster* clusX, TAMSDcluster* clusY);

   virtual ~TAMSDpoint() {};

   //    All the Get methods
   //! Get layer number
   int       GetLayer()          const  { return fStation;      }
   //! Get station number
   int       GetStation()        const  { return fStation;      }
   //! Get energy loss 1st plane
   double    GetEnergyLoss1()    const  { return fDe1;          }
   //! Get energy loss 1nd plane
   double    GetEnergyLoss2()    const  { return fDe2;          }
   //! Get total energy loss
   double    GetEnergyLoss()     const  { return fDe1+fDe2;     }
   //! Get time
   double    GetTime()           const  { return fTime;         }
   //! Get atomic charge
   int       GetChargeZ()        const  { return fChargeZ;      }
   //! Get atomic charge probablity
   double    GetChargeZProba()   const  { return fChargeZProba; }

   //! Set atomic charge
   void      SetChargeZ(int z)          { fChargeZ = z;         }
   //! Set atomic charge probablity
   void      SetChargeZProba(double p)  { fChargeZProba = p;    }
   
   void      Clear(Option_t* opt);
   
   ClassDef(TAMSDpoint,6)
};

//##############################################################################
/*!
 \class TAMSDntuPoint
 \brief point container class for MSD
 */
class TAMSDntuPoint : public TAGdata {

private:
  TAMSDparGeo*         fGeometry;     //! geometry parameter
  TObjArray*           fListOfPoints; ///< list of points

public:
  TAMSDntuPoint();
  virtual ~TAMSDntuPoint();

  TAMSDpoint*         NewPoint( int iStation, double x, double y, TVector3 position);
  TAMSDpoint*         NewPoint(Int_t iStation, TAMSDcluster* clusX, TAMSDcluster* clusY);
  TAMSDpoint*         NewPoint(TAMSDpoint* point, Int_t iStation);

  int                 GetPointsN(int iStation) const;

  TClonesArray*       GetListOfPoints(int iStation) const;

  TAMSDpoint*         GetPoint( int iStation, int iPoint ) const ;

  void                SetParGeo(TAMSDparGeo* par) { fGeometry = par; }

  virtual void        Clear(Option_t* opt="");

  virtual void        ToStream(ostream& os=cout, Option_t* option="") const;

  virtual void        SetupClones();

public:
   //! Get branch name
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }

private:
  static TString fgkBranchName;    // Branch name in TTree

  ClassDef(TAMSDntuPoint,2)
};

#endif
