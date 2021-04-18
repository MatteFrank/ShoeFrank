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
 
 */
/*------------------------------------------+---------------------------------*/

class TATWpoint : public TAGcluster {
   
private:
   TVector3    fPositionGlb;   // position in FOOT global framework
   TVector3    fPosErrGlb;     // position error

   Int_t       fRow;           // row number
   Int_t       fColumn;        // column number
   
   TATWhit*    fRowHit;        // hit col
   TATWhit*    fColumnHit;     // hit column
   
   Double32_t  fDe1;           // energy loss in the scintillator bars layer 1
   Double32_t  fDe2;           // energy loss in the scintillator bars layer 2

   Double32_t  fTof1;          // ToF in the scintillator bars layer 1
   Double32_t  fTof2;          // ToF in the scintillator bars layer 2
   Double32_t  fTime;          // for the moment I take the row time
   Int_t       fMatchCalIdx;   // Index of Calorimeter cluster matched (-1 if not)
  
   Int_t       fLayer;         // layer with more hits in the event
   Int_t       fId;            // point ID
   Int_t       fChargeZ;       // point charge Z
   Double32_t  fChargeZProba;  // raw guess of charge Z probability

public:
   
  TATWpoint();
  TATWpoint( Double_t x, Double_t dx, TATWhit* hitX, Double_t y, Double_t dy, TATWhit* hitY, Int_t mainLayer );
  ~TATWpoint() {};
  
  //    All the Get methods
  const TVector3&  GetPositionGlb() const  { return fPositionGlb;      }
  const TVector3&  GetPosErrorGlb() const  { return fPosErrGlb;        }

  Int_t     GetRowID()        const  { return fRow;                    }
  Int_t     GetColumnID()     const  { return fColumn;                 }
  
  Int_t     GetRow()          const  { return fRowHit->GetBar();       }
  Int_t     GetColumn()       const  { return fColumnHit->GetBar();    }
  
  TATWhit*  GetRowHit()	      const	 { return fRowHit;                 }
  TATWhit*  GetColumnHit()    const	 { return fColumnHit;              }
  
  Double_t  GetEnergyLoss1()  const  { return fDe1;                    }
  Double_t  GetEnergyLoss2()  const  { return fDe2;                    }
  Double_t  GetEnergyLoss()   const  { return fDe1+fDe2;               }
  Double_t  GetTof1()         const  { return fTof1;                   }
  Double_t  GetTof2()         const  { return fTof2;                   }
  Double_t  GetMeanTof()      const  { return (fTof1+fTof2)/2.;        }
  Double_t  GetTime()         const  { return fTime;                   }
  Int_t     GetMatchCalIdx()  const  { return fMatchCalIdx;            }
  Int_t     GetMainLayer()    const  { return fLayer;                  }
  Int_t     GetPointMatchMCtrkID()  const  { return fId;               }
  Int_t     GetChargeZ()      const  { return fChargeZ;                }
  Double_t  GetChargeZProba() const  { return fChargeZProba;           }
  Bool_t    IsValid()         const;
  
  void      SetPosition(TVector3& pos);
  void      SetPositionG(TVector3& pos);
  void      SetPositionGlb(TVector3& pos);
  void      SetMatchCalIdx(Int_t idx)      { fMatchCalIdx = idx;       }
  void      SetMainLayer(Int_t main_lay)   { fLayer = main_lay;        }
  void      SetPointMatchMCtrkID(Int_t id) { fId = id;                 }
  void      SetChargeZ(Int_t z)            { fChargeZ = z;             }
  void      SetChargeZProba(Double_t p)    { fChargeZProba = p;        }
  
  void      Clear(Option_t* opt);
  
  ClassDef(TATWpoint,5)

};

//##############################################################################

class TATWntuPoint : public TAGdata {
   
private:

  TClonesArray*        m_listOfPoints;
   
public:

  TATWntuPoint();
  virtual ~TATWntuPoint();
	
  TATWpoint*          NewPoint( Double_t x, Double_t dx, TATWhit* hitX, Double_t y, Double_t dy, TATWhit* hitY, Int_t mainLayer );

  Int_t               GetPointsN() const;
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



















