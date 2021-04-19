#ifndef _TAMSDntuPoint_HXX
#define _TAMSDntuPoint_HXX
/*!
 \file
 \version $Id: TAMSDntuPoint.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAMSDntuPoint.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGdata.hxx"
#include <map>

// ROOT classes
#include "TVector3.h"

#include "TAGobject.hxx"
#include "TAMSDhit.hxx"

class TAMSDparGeo;


/** TAMSDpoint class is the MSD hit reconstructed by the intersection of 2 hits on a
	column and a row strip respectively

	All the coordinates are in cm and in the detector reference frame, i.e. the center
	is the center of the detector.

 */
/*------------------------------------------+---------------------------------*/

class TAMSDpoint : public TAGobject {

private:
   TVector3    m_position;      // position in detector framework
   int         m_layer;         // number of MSD tracking station
   int         m_column;        // column number
   int         m_row;           // row number

   int         m_columnHitID;
   int         m_rowHitID;

   //specific methods for MSD
   int         m_columnParticleID;
   int         m_rowParticleID;
   int         m_ParticleID;

   int         m_columnMCHitID;
   int         m_rowMCHitID;

   bool        m_isMC;
   bool        m_isTrueGhost;

   TVector3    m_posMC;
   TVector3    m_momMC;

   void TrueGhostWarning();

   //generic methods
   Double32_t  m_de1;           // energy loss in 1st strip
   Double32_t  m_de2;           // energy loss in 2nd strip
   Double32_t  m_time;

   int         m_chargeZ;       // raw guess of charge Z
   Double32_t  m_chargeZProba;  // raw guess of charge Z probability

public:

   TAMSDpoint();
   TAMSDpoint( int layer, double x, double y, TVector3 position);
   virtual ~TAMSDpoint() {};


   //    All the Get methods
   TVector3  GetPosition()    const  { return m_position;      }

   int       GetColumnID()    const  { return m_column;        }
   int       GetRowID()       const  { return m_row;           }

   int       GetColumnHitID()    const  { return m_columnHitID;}
   int       GetRowHitID()       const  { return m_rowHitID;   }

   int       GetLayer()  const  {return m_layer;}

   bool      IsMC()             { return m_isMC; };
   bool      IsTrueGhost()      { return m_isTrueGhost; };

   int       GetGenPartID()     {return m_ParticleID;};
   int       GetColumnMCHitID() {return m_columnMCHitID;};
   int       GetRowMCHitID()    {return m_rowMCHitID;};

   double    GetEnergyLoss1()  const  { return m_de1;           }
   double    GetEnergyLoss2()  const  { return m_de2;           }
   double    GetEnergyLoss()   const  { return m_de1+m_de2;     }
   double    GetTime()         const  { return m_time;          }
   int       GetChargeZ()      const  { return m_chargeZ;       }
   double    GetChargeZProba() const  { return m_chargeZProba; }

   void      SetGeneratedParticle ( int colGenPart, int rowGenPart, int colMCHitID, int rowMCHitID );


   void      SetChargeZ(int z)       { m_chargeZ = z;          }
   void      SetChargeZProba(double p){ m_chargeZProba = p;    }
   void      Clear(Option_t* opt);

   ClassDef(TAMSDpoint,4)
};

//##############################################################################

class TAMSDntuPoint : public TAGdata {

private:

  TAMSDparGeo*         m_geometry;
  TObjArray*           m_listOfPoints;

public:

  TAMSDntuPoint();
  virtual ~TAMSDntuPoint();

  TAMSDpoint*         NewPoint( int iStation, double x, double y, TVector3 position);

  int                 GetPointN(int iStation) const;
  int                 GetPointN_includingGhosts();

  TClonesArray*       GetListOfPoints(int iStation) const;

  TAMSDpoint*         GetPoint( int iStation, int iPoint );


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
