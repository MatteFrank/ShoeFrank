#ifndef _TATWntuPoint_HXX
#define _TATWntuPoint_HXX
/*!
 \file
 \version $Id: TATWntuPoint.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TATWntuPoint.
 */
/*------------------------------------------+---------------------------------*/

#include "TAGdata.hxx"
#include <map>

// ROOT classes
#include "TVector3.h"

#include "TAGcluster.hxx"
#include "TATWntuRaw.hxx"


/** TATWpoint class is the scintillator hit reconstructed by the intersection of 2 hits on a
	column and a row bar respectively
 
	Created in 2018 by Matteo Franchini franchinim@bo.infn.it
 
 Complete revised to be compliant with foot framework and raw data.
 Copy hits in point not just assigning pointers
 by Ch. Finck cfinck@iphc.cnrs.fr
 
	All the coordinates are in cm and in the detector reference frame, i.e. the center
	is the center of the detector.
 
 */
/*------------------------------------------+---------------------------------*/

class TATWpoint : public TAGcluster {
   
private:

   TVector3    m_position;      // position in local framework
   TVector3    m_posErr;        // position error in local framework
   TVector3    m_positionG;     // position in detector framework
   TVector3    m_posErrG;       // position error in detector framework

   int         m_row;        // row number
   int         m_column;           // column number
   
   TATWntuHit*   m_rowHit;     // hit col
   TATWntuHit*   m_columnHit;        // hit column
   
   Double32_t  m_de1;           // energy loss in the scintillator bars layer 1
   Double32_t  m_de2;           // energy loss in the scintillator bars layer 2
   Double32_t  m_time;          // for the moment I take the row time
   
   int         m_chargeZ;       // raw guess of charge Z
   Double32_t  m_chargeZProba;  // raw guess of charge Z probability

public:
   
  TATWpoint();
  TATWpoint( double x, double dx, TATWntuHit* hitX, double y, double dy, TATWntuHit* hitY );
  ~TATWpoint() {};
  
  //    All the Get methods
  const TVector3&  GetPosition()  const  { return m_position;       }
  const TVector3&  GetPosError()  const  { return m_posErr;         }
  
  const TVector3&  GetPositionG() const  { return m_positionG;      }
  const TVector3&  GetPosErrorG() const  { return m_posErrG;        }
  
  int       GetRowID()    const  { return m_row;              }
  int       GetColumnID()       const  { return m_column;                 }
  
  int       GetRow()      const  { return m_rowHit->GetBar(); }
  int       GetColumn()         const  { return m_columnHit->GetBar();    }
  
  TATWntuHit* GetRowHit()	const	 { return m_rowHit;           }
  TATWntuHit* GetColumnHit()      const	 { return m_columnHit;              }
  
  double    GetEnergyLoss1() const  { return m_de1;                 }
  double    GetEnergyLoss2() const  { return m_de2;                 }
  double    GetEnergyLoss()  const  { return m_de1+m_de2;           }
  double    GetTime()        const  { return m_time;                }
  int       GetChargeZ()     const  { return m_chargeZ;             }
  double    GetChargeZProba() const  { return m_chargeZProba;       }
  
  
  void      SetPositionG(TVector3& pos);
  void      SetChargeZ(int z)       { m_chargeZ = z;                }
  void      SetChargeZProba(double p){ m_chargeZProba = p;          }
  
  
  void      Clear(Option_t* opt);
  
  ClassDef(TATWpoint,4)

};

//##############################################################################

class TATWntuPoint : public TAGdata {
   
private:

  TClonesArray*        m_listOfPoints;
   
public:

  TATWntuPoint();
  virtual ~TATWntuPoint();
	
  TATWpoint*          NewPoint( double x, double dx, TATWntuHit* hitX, double y, double dy, TATWntuHit* hitY );

  int                 GetPointN() const;
  TATWpoint*          GetPoint( int iPoint ) const;
  
  
  virtual void        Clear(Option_t* opt="");
  
  // delete?
  virtual void        ToStream(ostream& os=cout, Option_t* option="") const;
  
  virtual void        SetupClones();
  
public:

  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
private:

  static TString fgkBranchName;    // Branch name in TTree
  
  ClassDef(TATWntuPoint,2)

};

#endif



















