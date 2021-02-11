#ifndef _TAMntuHit_HXX
#define _TAMntuHit_HXX
/*!
 \file
 \version $Id: TAMntuHit.hxx,v 1.7 2003/06/22 11:48:38 mueller Exp $
 \brief   Declaration of TAMntuHit.
 */
/*------------------------------------------+---------------------------------*/

#include "TObject.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include "TAGdata.hxx"

class TAMChit : public TObject {
public:
   TAMChit();
   TAMChit(Int_t id, Int_t layer, Int_t view, Int_t cell, TVector3 inpos, TVector3 outpos,TVector3 inmom, TVector3 outmom, Double_t de, Double_t tof, Int_t trackId);
   virtual      ~TAMChit();
   
   Int_t         GetID()            const  { return fID;       }
   Int_t         GetTrackIdx()      const  { return fID;       }
   Int_t         GetSensorId()      const  { return fLayer;    }
   Int_t         GetBarId()         const  { return fLayer;    }
   Int_t         GetCrystalId()     const  { return fLayer;    }
   Int_t         GetLayer()         const  { return fLayer;    }
   Int_t         GetView()          const  { return fView;     }
   Int_t         GetCell()          const  { return fCell;     }
   Int_t         GetTrackId()       const  { return fTrackId;  }
   TVector3      GetInPosition()    const  { return fInPosition;  }
   TVector3      GetOutPosition()   const  { return fOutPosition; }
   TVector3      GetInMomentum()    const  { return fInMomentum;  }
   TVector3      GetOutMomentum()   const  { return fOutMomentum; }
   Double_t      GetDeltaE()        const  { return fDeltaE;   }
   Double_t      GetTof()           const  { return fTof;      }

   
   void          SetID(int aid)            { fID = aid;        }
   void          SetLayer(int aLayer)      { fLayer = aLayer;  }
   void          SetView(int aView)        { fView = aView;    }
   void          SetCell(int aCell)        { fCell = aCell;    }
   void          SetInPosition(TVector3 pos) { fInPosition = pos;  }
   void          SetOutPosition(TVector3 pos){ fOutPosition = pos; }
   void          SetInMomentum(TVector3 mom) { fInMomentum = mom;  }
   void          SetOutMomentum(TVector3 mom){ fOutMomentum = mom; }
   void          SetDeltaE(Double_t e)     { fDeltaE   = e;    }
   void          SetTof(Double_t tof)      { fTof      = tof;  }
   void          SetTrackId(int aid)       { fTrackId = aid;   }

   Int_t         fID;           // identity
   Int_t         fLayer;        // layer number
   Int_t         fView;         // view number (for BM)
   Int_t         fCell;         // cell number (for BM)
   TVector3      fInPosition;   // initial position
   TVector3      fOutPosition;  // final position
   TVector3      fInMomentum;   // initial momentum
   TVector3      fOutMomentum;  // final momentum
   Double32_t    fDeltaE;       // deposited energy
   Double32_t    fTof;          // time fo flight
   Double32_t    fTrackId;      // MC track Id
   
   ClassDef(TAMChit,3)
};

//##############################################################################

class TAMCntuHit : public TAGdata {
public:
   TAMCntuHit();
   virtual          ~TAMCntuHit();
   
   Int_t             GetHitsN() const;

   TAMChit*          GetHit(Int_t i);
   const TAMChit*    GetHit(Int_t i) const;
   
   TAMChit*          NewHit(Int_t id, Int_t layer, Int_t view, Int_t cell, TVector3 inpos, TVector3 outpos, TVector3 inmom, TVector3 outmom, Double_t de, Double_t tof, Int_t trackId);

   virtual void      SetupClones();
   virtual void      Clear(Option_t* opt="");
   
   virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetStcBranchName()   { return fgkStcBranchName.Data();  }
   static const Char_t* GetBmBranchName()    { return fgkBmBranchName.Data();   }
   static const Char_t* GetVtxBranchName()   { return fgkVtxBranchName.Data();  }
   static const Char_t* GetItrBranchName()   { return fgkItrBranchName.Data();  }
   static const Char_t* GetMsdBranchName()   { return fgkMsdBranchName.Data();  }
   static const Char_t* GetTofBranchName()   { return fgkTofBranchName.Data();  }
   static const Char_t* GetCalBranchName()   { return fgkCalBranchName.Data();  }

private:
   TClonesArray*   fListOfHits; // hits
  
private:
   static TString    fgkStcBranchName;
   static TString    fgkBmBranchName;
   static TString    fgkVtxBranchName;
   static TString    fgkItrBranchName;
   static TString    fgkMsdBranchName;
   static TString    fgkTofBranchName;
   static TString    fgkCalBranchName;

   ClassDef(TAMCntuHit,1)
   
};

#endif
