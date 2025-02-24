#ifndef _TATWntuHit_HXX
#define _TATWntuHit_HXX

/*!
 \File TATWntuHit.hxx
 \brief   Declaration of TATWntuHit, the class for the TW hits.
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
 \class TATWhit
 \brief class for TW  hit **
 */
class TATWhit : public TAGobject {
   
private:
    Int_t    fLayer;                  ///< layerId (0 for rear LayerY, 1 for front LayerX)
    Int_t    fBar;                    ///< barId [0-19] for each Layer
    Double_t fDe;                     ///< energy loss in the bar [MeV]
    Double_t fTime;                   ///< timestamp [ns]
    Double_t fTimeofflight;           ///< time of flight [ns]
    Double_t fCoordinate;             ///< position along the bar [cm]
    Double_t fZ;                      ///< z coordinate of the TW layer
    Int_t    fChargeZ;                ///< Nuclear Charge Number Z 
    TArrayI  fMCindex;                ///< Id of the hit created in the simulation
    TArrayI  fMCtrackId;              ///< Id of the track created in the simulation
    Double_t fChargeA;                ///< Charge in Channel A in the bar
    Double_t fChargeB;                ///< Charge in Channel B in the bar
    Double_t fAmplitudeA;             ///< Amplitude in Channel A in the bar
    Double_t fAmplitudeB;             ///< Amplitude in Channel B in the bar
    Double_t fTimeA;                  ///< Time in Channel A in the bar
    Double_t fTimeB;                  ///< Time in Channel B in the bar
    Bool_t   fIsValid;                ///< TW hit validity check
    Int_t    fTrigType;               ///< Trigger Type ID
  
public:
  TATWhit();
  TATWhit( TATWrawHit* hit );
  TATWhit(const TATWhit& aHit);
  TATWhit (Int_t aView, Int_t aBar, Double_t aDe, Double_t aTime,
           Double_t pos, Double_t ChargeA,
           Double_t ChargeB,Double_t AmplitudeA,Double_t AmplitudeB,Double_t TimeA,Double_t TimeB,
           Int_t trigType);
  ~TATWhit() {};
   
   void      Clear(Option_t* option = "C");
  
   //! Validity check if bar is part of LayerY (a vertical bar of rear layer)
   Bool_t    IsColumn() { return ( fLayer == (Int_t)LayerY ? true : false ); };
   //! Validity check if bar is part of LayerX (a horizontal bar of front layer)
   Bool_t    IsRow()    { return ( fLayer == (Int_t)LayerX ? true : false ); };
   //! Check if TW hit is valid (In DATA: both q_A and B >0. In MC: properly reconstructed and over threshold)
   Bool_t    IsValid()                 const   { return fIsValid;           }

   
   //    All the Get methods
   //! Get BarId [0-19] for a given layer
   Int_t     GetBar()                  const   { return fBar;               }
   //! Get LayerId [0 -> rear LayerY, 1->front LayerX]
   Int_t     GetLayer()                const   { return fLayer;             }
   //! Get deposited energy in MeV
   Double_t  GetEnergyLoss()           const   { return fDe;                }
   //! Get time ns
   Double_t  GetTime()                 const   { return fTime;              }
   //! Get ToF in ns
   Double_t  GetToF()                  const   { return fTimeofflight;      }
   //! Get position along the bar in cm
   Double_t  GetPosition()             const   { return fCoordinate;        }
   //! Get nuclear charge number Z
   Int_t     GetChargeZ()              const   { return fChargeZ;           }
   //! Get the charge at the bar channel A (for MC it provides the true MC position  along the bar smeared by the experimental resolution of the position computed by time difference)
   Double_t  GetChargeChA()            const   { return fChargeA;           }
   //! Get the charge at the bar channel B (for MC it provides the true MC position along the bar)
   Double_t  GetChargeChB()            const   { return fChargeB;           }
   //! Get the amplitude at the bar channel A (for MC it provides the true energy loss)
   Double_t  GetAmplitudeChA()         const   { return fAmplitudeA;        }
   //! Get the amplitude at the bar channel B (for MC it provides the true TW time)
   Double_t  GetAmplitudeChB()         const   { return fAmplitudeB;        }
   //! Get the time at the bar channel A (for MC true Z)
   Double_t  GetTimeChA()              const   { return fTimeA;             }
   //! Get the time at the bar channel B (for MC true Z)
   Double_t  GetTimeChB()              const   { return fTimeB;             }
   //! Get the trigger type Id for the decoded run
   Int_t     GetTrigType()             const {return fTrigType;}
   Double_t  GetHitZ_detectorFrame() const   { return fZ;             }

       // MC track id
       Int_t GetMcIndex(Int_t index) const
   {
     return fMCindex[index];
   }
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
   
   ClassDef(TATWntuHit,2)
};

#endif
