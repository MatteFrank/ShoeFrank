#ifndef _TATWntuHit_HXX
#define _TATWntuHit_HXX

/*!
 \File TATWntuHit.hxx
 \brief   Declaration of TATWntuHit.
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
#include "TATWntuRaw.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdata.hxx"
#include "TATWparameters.hxx"

/*!
 \class TAThHit
 \brief class for TW  hit **
 */
class TATWhit : public TAGobject {
   
private:
    Int_t    fLayer;
    Int_t    fBar;
    Double_t fDe;                     // energy loss in the scintillator bar
    Double_t fTime;                   // timestamp
    Double_t fTimeofflight;           // time of flight
    Double_t fCoordinate;             // x or y coordinate in the local detector frame, depending on the layer
    Double_t fZ;                      // z coordinate in the local detector frame
    Int_t    fChargeZ;                // atomic charge Z (tmp solution)
    TArrayI  fMCindex;                // Id of the hit created in the simulation
    TArrayI  fMCtrackId;              // Id of the track created in the simulation
    Double_t fChargeA;
    Double_t fChargeB;
    Double_t fAmplitudeA;
    Double_t fAmplitudeB;
    Double_t fTimeA;
    Double_t fTimeB;
    Bool_t   fIsValid;
    Int_t    fTrigType;
  
public:
  TATWhit();
  TATWhit( TATWrawHit* hit );
  TATWhit(const TATWhit& aHit);
  TATWhit (Int_t aView, Int_t aBar, Double_t aDe, Double_t aTime,
           Double_t pos, Double_t ChargeA,
           Double_t ChargeB,Double_t AmplitudeA,Double_t AmplitudeB,Double_t TimeA,Double_t TimeB,
           Int_t trigType);
  ~TATWhit() {};
   
   void   Clear(Option_t* option = "C");
   bool IsColumn() { return ( fLayer == (Int_t)LayerY ? true : false ); };
   bool IsRow()    { return ( fLayer == (Int_t)LayerX ? true : false ); };
   
   //    All the Get methods
   Int_t     GetBar()                  const   { return fBar;               }
   Int_t     GetLayer()                const   { return fLayer;             }
   Double_t  GetEnergyLoss()           const   { return fDe;                }
   Double_t  GetTime()                 const   { return fTime;              }
   Double_t  GetToF()                  const   { return fTimeofflight;      }
   Double_t  GetPosition()             const   { return fCoordinate;        }
   Int_t     GetChargeZ()              const   { return fChargeZ;           }
   Double_t  GetChargeChA()            const   { return fChargeA;           }
   Double_t  GetChargeChB()            const   { return fChargeB;           }
   Double_t  GetAmplitudeChA()         const   { return fAmplitudeA;        }
   Double_t  GetAmplitudeChB()         const   { return fAmplitudeB;        }
   Double_t  GetTimeChA()              const   { return fTimeA;             }
   Double_t  GetTimeChB()              const   { return fTimeB;             }
   Bool_t    IsValid()                 const   { return fIsValid;           }
   Float_t   GetHitCoordinate_detectorFrame() const   { return fCoordinate; }
   Float_t   GetHitZ_detectorFrame()          const   { return fZ;          }
   Int_t     GetTrigType()             const {return fTrigType;}
  
   // MC track id
   Int_t     GetMcIndex(Int_t index)    const   { return fMCindex[index];      }
   Int_t     GetMcTrackIdx(Int_t index) const   { return fMCtrackId[index];    }
   Int_t     GetMcTracksN()             const   { return fMCtrackId.GetSize(); }
   // Add MC track Id
   void      AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);
   
   //    All the Set Methods
   void      SetEnergyLoss(Double_t e)          { fDe = e;                  }
   void      SetTime(Double_t t)                { fTime = t;                }
   void      SetToF(Double_t time)              { fTimeofflight = time;     }
   void      SetPosition(Double_t p)            { fCoordinate = p;          }
   void      SetChargeZ(Int_t z)                { fChargeZ = z;             }
   void      SetChargeChA(Double_t chg)         { fChargeA = chg;           }
   void      SetChargeChB(Double_t chg)         { fChargeB = chg;           }
   void      SetAmplitudeChA(Double_t amp)      { fAmplitudeA = amp;        }
   void      SetAmplitudeChB(Double_t amp)      { fAmplitudeB = amp;        }
   void      SetTimeChA(Double_t t)             { fTimeA   = t;             }
   void      SetTimeChB(Double_t t)             { fTimeB   = t;             }
   void      SetValid(Bool_t t)                 { fIsValid   = t;           }

  ClassDef(TATWhit,4)                            // Pixel or Pixel of a Detector Plane
};

//##############################################################################

/*!
 \class TATWntuHit
 \brief Container class for TW ntu hit **
 */

class TATWntuHit : public TAGdata {
   
private:
    TClonesArray* fListOfHits;
    int           fHitlayX;
    int           fHitlayY;


public:
    TATWntuHit();
    virtual          ~TATWntuHit();
  TATWhit* Hit(Int_t i_ind);


   TATWhit*           NewHit(Int_t aView, Int_t aBar, Double_t aDe, Double_t aTime,
                             Double_t pos, Double_t ChargeA, Double_t ChargeB, Double_t AmplitudeA, Double_t AmplitudeB,
                             Double_t TimeA,Double_t TimeB, Int_t trigType);
  
    int               GetHitN(int layer); 
    int 			       GetHitN();
    TATWhit*          GetHit( int hitID , int layer);
    TATWhit*          GetHit( int hitID); //Return the hitID hit without looking for expl layer
    
    
    virtual void      SetupClones();
    TClonesArray*     GetListOfHits();

    virtual void      Clear(Option_t* opt="");

    // delete?
    virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
private:
   static TString     fgkBranchName;    // Branch name in TTree
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data(); }
   
   ClassDef(TATWntuHit,2)
};

#endif
