#ifndef _TATWNtuRaw_HXX
#define _TATWNtuRaw_HXX

/*!
 \File
 \version $Id: TATWntuRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TATWntuRaw.
 */
/*------------------------------------------+---------------------------------*/

// ROOT classes
#include "TArrayI.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TVector3.h"

// all 3 needed to take from gTagROOT
#include "TAGobject.hxx"
#include "TAGroot.hxx"
#include "TATWparGeo.hxx"
#include "TATWdatRaw.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdata.hxx"

class TATWntuHit : public TAGobject {
   
private:
    Int_t    m_layer;
    Int_t    m_bar;
    Double_t m_de;                     // energy loss in the scintillator bar
    Double_t m_time;
    Double_t m_time_oth;                   // timestamp // timestamp
    Double_t m_timeofflight;           // time of flight
    Double_t m_coordinate;             // x or y coordinate in the local detector frame, depending on the layer
    Double_t m_z;                      // z coordinate in the local detector frame
    Int_t    m_chargeZ;                // atomic charge Z (tmp solution)
    Double_t m_chargeCOM;              // Center of Mass evaluated with the charge
    TArrayI  m_MCindex;                // Id of the hit created in the simulation
    TArrayI  m_McTrackId;              // Id of the track created in the simulation
    Double_t m_ChargeA;
    Double_t m_ChargeB;
    Double_t m_TimeA;
    Double_t m_TimeB;
   Double_t m_TimeA_oth;
    Double_t m_TimeB_oth;

public:
  TATWntuHit();
  TATWntuHit( TATWrawHit* hit );
  TATWntuHit ( Int_t aView, Int_t aBar, Double_t aDe, Double_t aTime, Double_t aTimeOth,
		  	   Double_t pos,Double_t chargeCOM,Double_t ChargeA,
	       Double_t ChargeB,Double_t TimeA,Double_t TimeB, Double_t TimeAOth,Double_t TimeBOth);
   TATWntuHit(const TATWntuHit& aHit);
  ~TATWntuHit() {};
   void   Clear(Option_t* option = "C");
   bool IsColumn() { return ( m_layer == 0 ? true : false ); };
   bool IsRow()    { return ( m_layer == 1 ? true : false ); };
   
   //    All the Get methods
   Int_t     GetBar()                  const   { return  m_bar;              }
   Int_t     GetLayer()                const   { return  m_layer;            }
   Double_t  GetEnergyLoss()           const   { return m_de;                }
   Double_t  GetTime()                 const   { return m_time;              }
  Double_t  GetTimeOth()                 const   { return m_time_oth;              }
   Double_t  GetToF()                  const   { return m_timeofflight;      }
   Double_t  GetPosition()             const   { return m_coordinate;        }
   Int_t     GetChargeZ()              const   { return m_chargeZ;           }
   Double_t  GetCOM()                  const   { return m_chargeCOM;         }
   Double_t  GetChargeChA()            const   { return m_ChargeA;           }
   Double_t  GetChargeChB()            const   { return m_ChargeB;           }
   Double_t  GetChargeTimeA()          const   { return m_TimeA;             }
   Double_t  GetChargeTimeB()          const   { return m_TimeB;             }
   Float_t   GetHitCoordinate_detectorFrame() const   { return m_coordinate; }
   Float_t   GetHitZ_detectorFrame()          const   { return m_z;          }
   
   // MC track id
   Int_t     GetMcIndex(Int_t index)    const   { return m_MCindex[index];      }
   Int_t     GetMcTrackIdx(Int_t index) const   { return m_McTrackId[index];    }
   Int_t     GetMcTracksN()             const   { return m_McTrackId.GetSize(); }
   // Add MC track Id
   void      AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);
   
   //    All the Set Methods
   void      SetEnergyLoss(Double_t e)          { m_de = e;                  }
   void      SetTime(Double_t t)                { m_time = t;                }
   void      SetToF(Double_t time)              { m_timeofflight = time;     }
   void      SetPosition(Double_t p)            { m_coordinate = p;          }
   void      SetChargeZ(Int_t z)                { m_chargeZ = z;             }
   void      SetCOM(Double_t c)                 { m_chargeCOM = c;           }
   void      SetChargeChA(Double_t chg)         { m_ChargeA = chg;           }
   void      SetChargeChB(Double_t chg)         { m_ChargeB = chg;           }
   void      SetChargeTimeA(Double_t t)         { m_TimeA   = t;             }
   void      SetChargeTimeB(Double_t t)         { m_TimeB   = t;             }

  ClassDef(TATWntuHit,1)                            // Pixel or Pixel of a Detector Plane
};

//##############################################################################

class TATWntuRaw : public TAGdata {
   
private:
    TClonesArray*        m_listOfHits;
    int m_hitlay1;
    int m_hitlay2;


public:
    TATWntuRaw();
    virtual          ~TATWntuRaw();
  TATWntuHit* Hit(Int_t i_ind);


   TATWntuHit*       NewHit( Int_t aView, Int_t aBar, Double_t aDe, Double_t aTime, Double_t aTime_oth,
		  	   	   	   	      Double_t pos,Double_t chargeCOM,Double_t ChargeA,
			     Double_t ChargeB,Double_t TimeA,Double_t TimeB, Double_t TimeA_oth,Double_t TimeB_oth);
  
    int               GetHitN(int layer); 
    int 			  GetHitN();
    TATWntuHit*       GetHit( int hitID , int layer);
  TATWntuHit* GetHit( int hitID); //Return the hitID hit without looking for expl layer
    
    
    virtual void      SetupClones();
    TClonesArray*     GetListOfHits();

    virtual void      Clear(Option_t* opt="");

    // delete?
    virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
private:
   static TString     fgkBranchName;    // Branch name in TTree
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data(); }
   
   ClassDef(TATWntuRaw,1)
};

#endif
