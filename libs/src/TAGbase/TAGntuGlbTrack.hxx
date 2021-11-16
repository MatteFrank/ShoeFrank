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
#include <vector>
#include "TVector3.h"
#include <TMatrixD.h>
#include "TH1.h"
#include "TH2.h"



#include "TClonesArray.h"

#include "TAGnamed.hxx"
#include "TAGdata.hxx"
#include "TAGntuPoint.hxx"


#define build_string(expr) \
    (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())

using namespace std;

//
class TAGtrack : public TAGnamed {
public:
    struct polynomial_fit_parameters{
        std::array<double, 4> parameter_x;
        std::array<double, 2> parameter_y;
    };
    
public:
   TAGtrack();
   TAGtrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof);
   TAGtrack(const TAGtrack& aTrack);

   // Genfit

   TAGtrack(string name, long evNum, 
   			int pdgID, int pdgCh, int measCh, float mass, 
   			float length, float tof, 
   			float chi2, int ndof, float pVal, 
   			TVector3* pos, TVector3* mom, 
   			TMatrixD* pos_cov, TMatrixD* mom_cov, 
   			vector<TAGpoint*>* shoeTrackPointRepo
		);   

   void SetMCInfo( int MCparticle_id, float trackQuality );
   void SetExtrapInfoTW( TVector3* pos, TVector3* mom, TMatrixD* pos_cov, TMatrixD* mom_cov );

   virtual         ~TAGtrack();
   
   void             SetEvtNumber(Long64_t evt)    { fEvtNumber = evt;  }
   Long64_t         GetEvtNumber()          const { return fEvtNumber; }

   void             SetPdgID(Int_t id)            { fPdgId = id;       }
   Int_t            GetPdgID()              const { return fPdgId;     }

   void             SetLength(Int_t len)          { fLength = len;     }
   Double32_t       GetLength()             const { return fLength;    }
   
   void             SetChi2(Int_t chi)            { fChi2 =  chi;      }
   Double32_t       GetChi2()               const { return fChi2;      }
   
   void             SetNdof(Int_t n)              { fNdof = n;         }
   Int_t            GetNdof()               const { return fNdof;      }
   
   void             SetPval(Int_t p)              { fPval =  p;        }
   Double32_t       GetPval()               const { return fPval;      }

   void             SetQuality(Int_t q)           { fQuality =  q;     }
   Double32_t       GetQuality()            const { return fQuality;   }

   void             SetMass(Double_t amass)       { fMass = amass;     }
   Double_t         GetMass()               const { return fMass;      }  //(also Genfit)
   
   void             SetMomentum(Double_t amom)    { fMomModule = amom; }
   Double_t         GetMomentum()           const { return fMomModule; }
   
   void             SetTwChargeZ(Int_t chg)       { fTwChargeZ = chg;  }
   Int_t            GetTwChargeZ()          const { return fTwChargeZ; }
   
   void             SetTwTof(Double_t atoff)      { fTwTof = atoff;    }
   Double_t         GetTwTof()              const { return fTwTof;     }
   
   void             SetTrackId(Int_t trid)        { fTrkId = trid;     }
   Int_t            GetTrackId()            const { return fTrkId;     }
   
   void             SetFitMass(Double_t amass)    { fFitMass = amass;  }
   Double_t         GetFitMass()            const { return fFitMass;   }
   
   void             SetFitChargeZ(Int_t chg)      { fFitChargeZ = chg;  }
   Int_t            GetFitChargeZ()         const { return fFitChargeZ; }
   
   void             SetFitTof(Double_t atoff)     { fFitTof = atoff;    }
   Double_t         GetFitTof()             const { return fFitTof;     }
   
   void             SetFitEnergyLoss(Double_t e)  { fFitEnergyLoss = e; }
   Double_t         GetFitEnergyLoss()      const { return fFitEnergyLoss; }

   void             SetFitEnergy(Double_t e)      { fFitEnergy = e;    }
   Double_t         GetFitEnergy()          const { return fFitEnergy; }

   void             SetCalEnergy(Double_t e)      { fCalEnergy = e;    }
   Double_t         GetCalEnergy()          const { return fCalEnergy; }
   
   void             SetTgtDirection(TVector3 dir) { fTgtDir = dir;     }
   TVector3         GetTgtDirection()             { return fTgtDir;    }
   
   void             SetTgtPosition(TVector3 pos)  { fTgtPos = pos;     }
   TVector3         GetTgtPosition()              { return fTgtPos;    }  //(also Genfit)
   
   void             SetTgtMomentum(TVector3 pos)  { fTgtMom = pos;     }
   TVector3         GetTgtMomentum()              { return fTgtMom;    }
   
   void             SetTwPosition(TVector3 pos)   { fTwPos = pos;      }
   TVector3         GetTwPosition()               { return fTwPos;     }
   
   void             SetTwDirection(TVector3 dir)  { fTwDir = dir;      }
   TVector3         GetTwDirection()              { return fTwDir;     }
   
   void             SettWMomentum(TVector3 pos)   { fTwMom = pos;      }
   TVector3         GetTWMomentum()               { return fTwMom;     }

   //! Distance to a track near target
   Double_t         Distance(TAGtrack* track, Float_t z) const;
   
   //! Get list of points
   TClonesArray*    GetListOfPoints()       const { return fListOfPoints;                       }
   
   //! Get number of points
   Int_t            GetPointsN()            const { return fListOfPoints->GetEntries();         }
   
   //! Get  point
   const TAGpoint*  GetPoint(Int_t index)   const { return (TAGpoint*)fListOfPoints->At(index); }

   //! Get  point
   TAGpoint*        GetPoint(Int_t index)         { return (TAGpoint*)fListOfPoints->At(index); }
   
   //! Get MC info
   TArrayI          GetMcTrackIdx();
   
   //! Get theta angle at target
   Double_t         GetTgtTheta()           const;
   
   //! Get theta angle at Tw
   Double_t         GetTwTheta()            const;
   
   //! Get phi angle at target
   Double_t         GetTgtPhi()             const;
   
   //! Get phi angle at Tw
   Double_t         GetTwPhi()              const;

   //! Intersection near target
   TVector3         Intersection(Double_t posZ) const;
   
   //! Get Totol Energy Loss (MSD+TW+CAL)
   Double_t         GetTotEnergyLoss()      const;
   
   //! Get Totol Energy Loss (MSD+TW+CAL)
   Double_t         GetMsdEnergyLoss()      const;
   
   //! Get Totol Energy Loss (MSD+TW+CAL)
   Double_t         GetTwEnergyLoss()       const;
   

   //! Add measured point
   TAGpoint*        AddPoint(TAGpoint* point);
   TAGpoint*        AddPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr);
   TAGpoint*        AddPoint(TString name, TVector3 measPos, TVector3 measPosErr);
   TAGpoint*        AddPoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr);
   TAGpoint*        AddPoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr);

   void             Clear(Option_t* opt="");
   
   void             SetupClones();
    
   void             SetParameters( polynomial_fit_parameters parameters ){ fParameters = std::move( parameters ); }
   polynomial_fit_parameters const&    GetParameters( ) const{ return fParameters; }
   TVector3         GetPosition( double z );
   
private:
   TString          fName;      // particleName_mass_id (using TNamed data)
   Long64_t         fEvtNumber;      // event number
   Int_t            fPdgId;          // PDG Id used in the fit
   Double32_t       fLength;         // track length from target to TW
   Double32_t       fChi2;           // Chi2 of the fitted track
   Int_t            fNdof;           // number of freedom of the fitted track
   Double32_t       fPval;           // p-Value of the fitted track
   Double32_t       fQuality;        // quality factor of the fitted track

   Double32_t       fMass;           // Initial mass
   Double32_t       fMomModule;      // Momentum module
   Int_t            fTwChargeZ;      // TW atomic charge Z
   Double32_t       fTwTof;          // TW time of flight
   Double32_t       fCalEnergy;      // CAL energy (loss)
   Int_t            fTrkId;          // track absolute Id
   
   Double32_t       fFitMass;         // fitted mass
   Int_t            fFitChargeZ;      // fitted charge Z
   Double32_t       fFitTof;          // fitted time of flight
   Double32_t       fFitEnergyLoss;   // fitted energy loss
   Double32_t       fFitEnergy;       // fitted energy

   //Particle directions and positions computed on target middle
   TVector3         fTgtDir;
   TVector3         fTgtPos;
   TVector3         fTgtMom;

   //Particle directions and positions computed on TW Wall -> Tof to Tw avoid confusion btw time of flight and TW detector
   TVector3         fTwPos;
   TVector3         fTwDir;
   TVector3         fTwMom;

   TClonesArray*    fListOfPoints;          // Attached measured points
   
   TArrayI           fMcTrackIdx;          //! Idx of the track created in the simulation
   map<int, int>     fMcTrackMap;          //! Map of MC track Id


    polynomial_fit_parameters fParameters;
    
   ClassDef(TAGtrack,3)
   
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
   TAGtrack* 		NewTrack(string name, long evNum, int pdgID, int pdgCh, int measCh, float mass, float length, float tof, float chi2, int ndof, float pVal, TVector3* pos, TVector3* mom, TMatrixD* pos_cov, TMatrixD* mom_cov, vector<TAGpoint*>* shoeTrackPointRepo);   

   TAGtrack*        NewTrack(TAGtrack& track);

    
   virtual void     SetupClones();
   virtual void     Clear(Option_t* opt="");
   
   
   virtual void     ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   int m_debug;
	string m_kalmanOutputDir;

   ClassDef(TAGntuGlbTrack,2)
};


#endif

















