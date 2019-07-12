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

#include "TAGobject.hxx"
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

class TATWpoint : public TAGobject {
   
private:
   TVector3    m_position;      // position in detector framework
   int         m_column;        // column number
   int         m_row;           // row number
   
   TATWntuHit*   m_columnHit;     // hit col
   TATWntuHit*   m_rowHit;        // hit row
   
   Double32_t  m_de1;           // energy loss in the scintillator bars layer 1
   Double32_t  m_de2;           // energy loss in the scintillator bars layer 2
   Double32_t  m_time;          // for the moment I take the column time
   
   int         m_chargeZ;       // raw guess of charge Z
   
public:
   
   TATWpoint();
   TATWpoint( double x, TATWntuHit* colHit, double y, TATWntuHit* rowHit );
   ~TATWpoint() {};
   
   //    All the Get methods
   TVector3  GetPosition()    const  { return m_position;            }
   
   int       GetColumnID()    const  { return m_column;              }
   int       GetRowID()       const  { return m_row;                 }
   
   int       GetColumn()      const  { return m_columnHit->GetBar(); }
   int       GetRow()         const  { return m_rowHit->GetBar();    }
   
   TATWntuHit* GetColumnHit()	const	 { return m_columnHit;           }
   TATWntuHit* GetRowHit()	   const	 { return m_rowHit;              }
   
   double    GetEnergyLoss1() const  { return m_de1;                 }
   double    GetEnergyLoss2() const  { return m_de2;                 }
   double    GetEnergyLoss()  const  { return m_de1+m_de2;           }
   double    GetTime()        const  { return m_time;                }
   int       GetChargeZ()     const  { return m_chargeZ;             }
   
   void      SetChargeZ(int z)       { m_chargeZ = z;                }
   
   void      Clear(Option_t* opt);
   
   ClassDef(TATWpoint,3)
};

//##############################################################################

class TATWntuPoint : public TAGdata {
   
private:
   
   TClonesArray*        m_listOfPoints;
   
public:

	TATWntuPoint();
	virtual ~TATWntuPoint();
	
	TATWpoint*         NewPoint( double x, TATWntuHit* colHit, double y, TATWntuHit* rowHit );

	int                 GetPointN();
	TATWpoint*         GetPoint( int iPoint );


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



















