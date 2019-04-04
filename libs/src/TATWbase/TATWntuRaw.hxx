#ifndef _TATWNtuRaw_HXX
#define _TATWNtuRaw_HXX

/*!
 \File
 \version $Id: TATWntuRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TATWntuRaw.
 */
/*------------------------------------------+---------------------------------*/

// ROOT classes
#include "TArrayC.h"
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
    Int_t m_layer;
    Int_t m_bar;
    Double_t m_de;                     // energy loss in the scintillator bar
    Double_t m_time;                   // timestamp
    Double_t m_coordinate;             // x or y coordinate in the local detector frame, depending on the layer
    Double_t m_z;                         // z coordinate in the local detector frame
    Double_t m_chargeCOM;                  // Center of Mass evaluated with the charge
    TArrayC   m_MCindex;                  // Id of the hit created in the simulation
    TArrayC   m_McTrackId;                // Id of the track created in the simulation
   
public:
   TATWntuHit();
   TATWntuHit( TATWrawHit* hit );
   TATWntuHit ( Int_t aView, Int_t aBar, Double_t aDe, Double_t aTime, Double_t pos,Double_t chargeCOM);
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
   Double_t  GetPosition()             const   { return m_coordinate;        }
   
   Float_t    GetHitCoordinate_detectorFrame() const   { return m_coordinate; }
   Float_t    GetHitZ_detectorFrame()          const   { return m_z;          }

   // MC track id
   Int_t      GetMcIndex(Int_t index)   const   { return m_MCindex[index];      }
   Int_t      GetMcTrackId(Int_t index) const   { return m_McTrackId[index];    }
   Int_t      GetMcTrackCount()         const   { return m_McTrackId.GetSize(); }
   
   // Add MC track Id
   void       AddMcTrackId(Int_t trackId, Int_t mcId = -1);

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

    TATWntuHit*         NewHit( int layer, int bar, double energyLoss, double time, double pos,double m_chargeCOM);
    int               GetHitN(int layer); 
    int 			  GetHitN();
    TATWntuHit*         GetHit( int hitID , int layer);
    
    
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
