#ifndef _TAGntuGlbTrack_HXX
#define _TAGntuGlbTrack_HXX
/*!
 \file
 \version $Id: TAGntuGlbTrack.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAGntuGlbTrack.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include <array>
#include "TVector3.h"
#include "TH1.h"
#include "TH2.h"
using namespace std;


#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAGntuPoint.hxx"
//
class TAGtrack : public TAGobject {
public:
    struct polynomial_fit_parameters{
        std::array<double, 4> parameter_x;
        std::array<double, 2> parameter_y;
    };
    
public:
   
   TAGtrack();
   TAGtrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof);
   TAGtrack(const TAGtrack& aTrack);

   virtual         ~TAGtrack();
   
   void             SetMass(Double_t amass)       { fMass = amass;     }
   Double_t         GetMass()               const { return fMass;      }
   
   void             SetMomentum(Double_t amom)    { fMom = amom;       }
   Double_t         GetMomentum()           const { return fMom;       }
   
   void             SetCharge(Int_t acharge)      { fCharge = acharge; }
   Int_t            GetCharge()             const { return fCharge;    }
   
   void             SetTof(Double_t atoff)        { fTof = atoff;      }
   Double_t         GetTof()                const { return fTof;       }
   
   void             SetTrackId(Int_t trid)        { fTrkId = trid;     }
   Int_t            GetTrackId()            const { return fTrkId;     }
   
   void             SetEnergy(Double_t e)         { fEnergy = e;       }
   Double_t         GetEnergy()             const { return fEnergy;    }
   
   void             SetTgtDirection(TVector3 dir) { fTgtDir = dir;     }
   TVector3         GetTgtDirection()             { return fTgtDir;    }
   
   void             SetTgtPosition(TVector3 pos)  { fTgtPos = pos;     }
   TVector3         GetTgtPosition()              { return fTgtPos;    }
   
   void             SetTofPosition(TVector3 pos)  { fTofPos = pos;     }
   TVector3         GetTofPosition()              { return fTofPos;    }
   
   void             SetTofDirection(TVector3 dir) { fTofDir = dir;     }
   TVector3         GetTofDirection()             { return fTofDir;    }

   //! Get theta angle at target
   Double_t         GetTgtTheta()           const;
   
   //! Get theta angle at Tof
   Double_t         GetTofTheta()           const;
   
   //! Get phi angle at target
   Double_t         GetTgtPhi()             const;
   
   //! Get phi angle at Tof
   Double_t         GetTofPhi()             const;

   //! Intersection near target
   TVector3         Intersection(Double_t posZ) const;
   
   //! Distance to a track near target
   Double_t         Distance(TAGtrack* track, Float_t z) const;

   
   //! Get list of measured points
   TClonesArray*    GetListOfMeasPoints()   const { return fListOfMeasPoints;                       }
   
   //! Get list of corrected points
   TClonesArray*    GetListOfCorrPoints()   const { return fListOfCorrPoints;                       }
   
   //! Get number of measured points
   Int_t            GetMeasPointsN()        const { return fListOfMeasPoints->GetEntries();         }
   
   //! Get number of corrected points
   Int_t            GetCorrPointsN()        const { return fListOfCorrPoints->GetEntries();         }
   
   //! Get measured point
   TAGpoint*        GetMeasPoint(Int_t index)     { return (TAGpoint*)fListOfMeasPoints->At(index); }
   
   //! Get corrected point
   TAGpoint*        GetCorrPoint(Int_t index)     { return (TAGpoint*)fListOfCorrPoints->At(index); }
   
   //! Add measured point
   TAGpoint*        AddMeasPoint(TAGpoint* point);
   TAGpoint*        AddMeasPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr);
   TAGpoint*        AddMeasPoint(TString name, TVector3 pos, TVector3 posErr);
   TAGpoint*        AddMeasPoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr);

   //! Add corrected point
   TAGpoint*        AddCorrPoint(TAGpoint* point);
   TAGpoint*        AddCorrPoint(TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr);
   TAGpoint*        AddCorrPoint(TString name, TVector3 pos, TVector3 posErr, TVector3 mom, TVector3 momErr);

   void             Clear(Option_t* opt="");
   
   void             SetupClones();
    
    void             SetParameters( polynomial_fit_parameters parameters ){ fParameters = std::move( parameters ); }
   TVector3         GetPosition( double z );
   
private:
   Double32_t       fMass;
   Double32_t       fMom;
   Int_t            fCharge;
   Double32_t       fTof;
   Double32_t       fEnergy;
   Int_t            fTrkId;

   //Particle directions and positions computed on target middle
   TVector3         fTgtDir;
   TVector3         fTgtPos;
   
   //Particle directions and positions computed on ToF Wall
   TVector3         fTofPos;
   TVector3         fTofDir;
   
   TClonesArray*    fListOfMeasPoints;          // Attached measured points
   TClonesArray*    fListOfCorrPoints;          // Attached corrected points

    polynomial_fit_parameters fParameters;
    
   ClassDef(TAGtrack,2)
   
};

//##############################################################################

class TAGntuGlbTrack : public TAGdata {
   
private:
   TClonesArray*    fListOfTracks;		// tracks
    
private:
   static TString fgkBranchName;    // Branch name in TTree
   
public:
   TAGntuGlbTrack();
   virtual         ~TAGntuGlbTrack();
   
   TAGtrack*        GetTrack(Int_t i);
   const TAGtrack*  GetTrack(Int_t i) const;
   Int_t            GetTracksN()      const;
   
   TClonesArray*    GetListOfTracks() { return fListOfTracks; }
   
   TAGtrack*        NewTrack();
   TAGtrack*        NewTrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof);
   TAGtrack*        NewTrack(TAGtrack& track);

    
   virtual void     SetupClones();
   virtual void     Clear(Option_t* opt="");
   
   
   virtual void     ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   ClassDef(TAGntuGlbTrack,2)
};


#endif
