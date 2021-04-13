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
#include "TATWntuHit.hxx"


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
   TVector3    m_positionGlb;   // position in FOOT global framework
   TVector3    m_posErrGlb;     // position error

   int         m_row;           // row number
   int         m_column;        // column number
   
   TATWhit*   m_rowHit;      // hit col
   TATWhit*   m_columnHit;   // hit column
   
   Double32_t  m_de1;           // energy loss in the scintillator bars layer 1
   Double32_t  m_de2;           // energy loss in the scintillator bars layer 2

   Double32_t  m_tof1;          // ToF in the scintillator bars layer 1
   Double32_t  m_tof2;          // ToF in the scintillator bars layer 2
   Double32_t  m_time;          // for the moment I take the row time
   Int_t       m_matchCalIdx;   // Index of Calorimeter cluster matched (-1 if not)
  
   int         m_layer;         // layer with more hits in the event
   int         m_id;            // point ID
   int         m_chargeZ;       // point charge Z
   Double32_t  m_chargeZProba;  // raw guess of charge Z probability

public:
   
  TATWpoint();
  TATWpoint( double x, double dx, TATWhit* hitX, double y, double dy, TATWhit* hitY, Int_t mainLayer );
  ~TATWpoint() {};
  
  //    All the Get methods
  const TVector3&  GetPositionGlb() const  { return m_positionGlb;      }
  const TVector3&  GetPosErrorGlb() const  { return m_posErrGlb;        }

  int       GetRowID()    const  { return m_row;              }
  int       GetColumnID()       const  { return m_column;                 }
  
  int       GetRow()      const  { return m_rowHit->GetBar(); }
  int       GetColumn()         const  { return m_columnHit->GetBar();    }
  
  TATWhit* GetRowHit()	const	 { return m_rowHit;           }
  TATWhit* GetColumnHit()      const	 { return m_columnHit;              }
  
  double    GetEnergyLoss1() const  { return m_de1;                 }
  double    GetEnergyLoss2() const  { return m_de2;                 }
  double    GetEnergyLoss()  const  { return m_de1+m_de2;           }
  double    GetTof1()        const  { return m_tof1;                }
  double    GetTof2()        const  { return m_tof2;                }
  double    GetMeanTof()  const  { return (m_tof1+m_tof2)/2.;           }
  double    GetTime()        const  { return m_time;                }
  int       GetMatchCalIdx() const  { return m_matchCalIdx;         }
  int       GetMainLayer()     const  { return m_layer;             }
  int       GetPointMatchMCtrkID()     const  { return m_id;             }
  int       GetChargeZ()     const  { return m_chargeZ;             }

  double    GetChargeZProba() const  { return m_chargeZProba;       }
  bool      IsValid()         const;
  
  
  void      SetPosition(TVector3& pos);
  void      SetPositionG(TVector3& pos);
  void      SetPositionGlb(TVector3& pos);
  void      SetMatchCalIdx(int idx)   { m_matchCalIdx = idx;        }
  void      SetMainLayer(int main_lay)   { m_layer = main_lay;      }
  void      SetPointMatchMCtrkID(int id)       { m_id = id;                   }
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
	
  TATWpoint*          NewPoint( double x, double dx, TATWhit* hitX, double y, double dy, TATWhit* hitY, Int_t mainLayer );

  int                 GetPointsN() const;
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



















