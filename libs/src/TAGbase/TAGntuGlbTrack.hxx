#ifndef _TAGntuGlbTrack_HXX
#define _TAGntuGlbTrack_HXX
/*!
 \file TAGntuGlbTrack.hxx
 \brief   Declaration of TAGntuGlbTrack.
 \author all together
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

using namespace std;

//
class TAGtrack : public TAGnamed {
public:
   /*!
    \struct polynomial_fit_parameters
    \brief  polynonial fit  parameters
    */
    struct polynomial_fit_parameters{
        std::array<double, 4> parameter_x; ///< parameter x
        std::array<double, 2> parameter_y; ///< parameter y
    };
    
public:
   TAGtrack();
   TAGtrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof);
   TAGtrack(const TAGtrack& aTrack);

   // Genfit
   TAGtrack(string name, long evNum, 
   			int pdgID, float startMass, int fitCh, float fitMass, 
   			float length, float tof, 
   			float chi2, int ndof, float pVal, 
   			TVector3* TgtPos, TVector3* TgtMom, 
   			TMatrixD* TgtPos_cov, TMatrixD* TgtMom_cov, 
   			TVector3* TwPos, TVector3* TwMom, 
   			TMatrixD* TwPos_cov, TMatrixD* TwMom_cov, 
   			vector<TAGpoint*>* shoeTrackPointRepo);

   virtual         ~TAGtrack();
   
   //! Set event number
   void             SetEvtNumber(Long64_t evt)    { fEvtNumber = evt;  }
   //! Get event number
   Long64_t         GetEvtNumber()          const { return fEvtNumber; }

   //! Get PDG Id
   void             SetPdgID(Int_t id)            { fPdgId = id;       }
   //! Set PDG Id
   Int_t            GetPdgID()              const { return fPdgId;     }

   //! Set track length
   void             SetLength(Int_t len)          { fLength = len;     }
   //! Get track length
   Double32_t       GetLength()             const { return fLength;    }
   
   //! Set Chi2 of fit
   void             SetChi2(Int_t chi)            { fChi2 =  chi;      }
   //! Get Chi2 of fit
   Double32_t       GetChi2()               const { return fChi2;      }
   
   //! Set nof
   void             SetNdof(Int_t n)              { fNdof = n;         }
   //! Get nof
   Int_t            GetNdof()               const { return fNdof;      }
   
   //! Set p-value
   void             SetPval(Int_t p)              { fPval =  p;        }
   //! Get p-value
   Double32_t       GetPval()               const { return fPval;      }

   //! Set quality track
   void             SetQuality(Int_t q)           { fQuality =  q;     }
   //! Get quality track
   Double32_t       GetQuality()            const { return fQuality;   }

   //! Set mass
   void             SetMass(Double_t amass)       { fMass = amass;     }
   //! Get mass
   Double_t         GetMass()               const { return fMass;      }
   
   //! Set momentum
   void             SetMomentum(Double_t amom)    { fMomModule = amom; }
   //! Get momentum
   Double_t         GetMomentum()           const { return fMomModule; }
   
   //! Set TW charge Z
   void             SetTwChargeZ(Int_t chg)       { fTwChargeZ = chg;   }
   //! Get TW charge Z
   Int_t            GetTwChargeZ()          const { return fTwChargeZ;  }
   
   //! Set TW Time of flight
   void             SetTwTof(Double_t atoff)      { fTwTof = atoff;     }
   //! Get TW Time of flight
   Double_t         GetTwTof()              const { return fTwTof;      }
   
   //! Set Track id
   void             SetTrackId(Int_t trid)        { fTrkId = trid;      }
   //! Get Track id
   Int_t            GetTrackId()            const { return fTrkId;      }
   
   //! Set fiited mass
   void             SetFitMass(Double_t amass)    { fFitMass = amass;   }
   //! Get fiited mass
   Double_t         GetFitMass()            const { return fFitMass;    }
   
   //! Set fiited charge Z
   void             SetFitChargeZ(Int_t chg)      { fFitChargeZ = chg;  }
   //! Get fiited charge Z
   Int_t            GetFitChargeZ()         const { return fFitChargeZ; }
   
   //! Set fiited time of flight
   void             SetFitTof(Double_t atoff)     { fFitTof = atoff;    }
   //! Get fiited time of flight
   Double_t         GetFitTof()             const { return fFitTof;     }
   
   //! Set fiited energy loss
   void             SetFitEnergyLoss(Double_t e)  { fFitEnergyLoss = e;    }
   //! Get fiited energy loss
   Double_t         GetFitEnergyLoss()      const { return fFitEnergyLoss; }

   //! Set fiited energy
   void             SetFitEnergy(Double_t e)      { fFitEnergy = e;     }
   //! Get fiited energy
   Double_t         GetFitEnergy()          const { return fFitEnergy;  }

   //! Set CAL energy
   void             SetCalEnergy(Double_t e)      { fCalEnergy = e;     }
   //! Get CAL energy
   Double_t         GetCalEnergy()          const { return fCalEnergy;  }
   
   //! Set Target track direction
   void             SetTgtDirection(TVector3 dir) { fTgtDir = dir;      }
   //! Get Target track direction
   TVector3         GetTgtDirection()             { return fTgtDir;     }
   
   //! Set Target track position error
   void             SetTgtPosError(TVector3 err)  { fTgtPosError = err; }
   //! Get Target track position error
   TVector3         GetTgtPosError()              { return fTgtPosError;}
   
   //! Set Target track position
   void             SetTgtPosition(TVector3 pos)  { fTgtPos = pos;      }
   //! Get Target track position
   TVector3         GetTgtPosition()              { return fTgtPos;     }
   
   //! Set Target track momentum
   void             SetTgtMomentum(TVector3 pos)  { fTgtMom = pos;      }
   //! Get Target track momentum
   TVector3         GetTgtMomentum()              { return fTgtMom;     }
   
   //! Set Target track momentum error
   void             SetTgtMomError(TVector3 err)  { fTgtMomError = err;  }
   //! Get Target track momentum error
   TVector3         GetTgtMomError()              { return fTgtMomError; }
   
   //! Set TW track position
   void             SetTwPosition(TVector3 pos)   { fTwPos = pos;       }
   //! Get TW track position
   TVector3         GetTwPosition()               { return fTwPos;      }
   
   //! Set TW track position error
   void             SetTwPosError(TVector3 err)   { fTwPosError = err;  }
   //! Get TW track position error
   TVector3         GetTwPosError()               { return fTwPosError; }
   
   //! Set TW track momentum
   void             SetTwMomentum(TVector3 pos)   { fTwMom = pos;       }
   //! Get TW track momentum
   TVector3         GetTwMomentum()               { return fTwMom;      }
   
   //! Set TW track momentum error
   void             SetTwMomError(TVector3 err)   { fTwMomError = err;  }
   //! Get TW track momentum error
   TVector3         GetTwMomError()               { return fTwMomError; }

   // Distance to a track near target
   Double_t         Distance(TAGtrack* track, Float_t z) const;
   
   //! Get list of points
   TClonesArray*    GetListOfPoints()       const { return fListOfPoints;                       }
   
   //! Get number of points
   Int_t            GetPointsN()            const { return fListOfPoints->GetEntries();         }
   
   //! Get  point
   const TAGpoint*  GetPoint(Int_t index)   const { return (TAGpoint*)fListOfPoints->At(index); }

   //! Get  point
   TAGpoint*        GetPoint(Int_t index)         { return (TAGpoint*)fListOfPoints->At(index); }
   
   // Get MC info
   TArrayI          GetMcTrackIdx();
   
   // Get theta angle at target
   Double_t         GetTgtTheta()           const;
   
   // Get phi angle at target
   Double_t         GetTgtPhi()             const;
   
   // Intersection near target
   TVector3         Intersection(Double_t posZ) const;
   
   // Get Total Energy Loss (MSD+TW+CAL)
   Double_t         GetTotEnergyLoss()      const;
   
   // Get Total Energy Loss (MSD+TW+CAL)
   Double_t         GetMsdEnergyLoss()      const;
   
   // Get Total Energy Loss (MSD+TW+CAL)
   Double_t         GetTwEnergyLoss()       const;
   

   // Add measured point
   // with copy cstr
   TAGpoint*        AddPoint(TAGpoint* point);
   // from position, momentun and fitted position, momentun with associated errors
   TAGpoint*        AddPoint(TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr);
   // from position and errors
   TAGpoint*        AddPoint(TString name, TVector3 measPos, TVector3 measPosErr);
   // from position, errors and fitted position, errors
   TAGpoint*        AddPoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr);
   // from position, momentun and fitted position, momentun with associated errors
   TAGpoint*        AddPoint(TString name, TVector3 measPos, TVector3 measPosErr, TVector3 fitPos, TVector3 fitPosErr, TVector3 mom, TVector3 momErr);

   // Clear
   void             Clear(Option_t* opt="");
   
   // Set up clones
   void             SetupClones();
   
   //! Set paramters of polynom fit
   void             SetParameters( polynomial_fit_parameters parameters ){ fParameters = std::move( parameters ); }
   //! Get paramters of polynom fit
   polynomial_fit_parameters const&    GetParameters( ) const{ return fParameters; }
   // Get position in z from polynom fit
   TVector3         GetPosition( double z );
   
private:
   Long64_t         fEvtNumber;      ///< event number
   Int_t            fPdgId;          ///< PDG Id used in the fit
   Double32_t       fLength;         ///< track length from target to TW
   Double32_t       fChi2;           ///< Chi2 of the fitted track
   Int_t            fNdof;           ///< number of freedom of the fitted track
   Double32_t       fPval;           ///< p-Value of the fitted track
   Double32_t       fQuality;        ///< quality factor of the fitted track

   Double32_t       fMass;           ///< Initial mass
   Double32_t       fMomModule;      ///< Momentum module
   Int_t            fTwChargeZ;      ///< TW atomic charge Z
   Double32_t       fTwTof;          ///< TW time of flight
   Double32_t       fCalEnergy;      ///< CAL energy (loss)
   Int_t            fTrkId;          ///< track absolute Id
   
   Double32_t       fFitMass;         ///< fitted mass
   Int_t            fFitChargeZ;      ///< fitted charge Z
   Double32_t       fFitTof;          ///< fitted time of flight
   Double32_t       fFitEnergyLoss;   ///< fitted energy loss
   Double32_t       fFitEnergy;       ///< fitted energy

   //Particle momentum and positions computed on target middle
   TVector3         fTgtDir;          ///< Direction of particle at target
   TVector3         fTgtPos;          ///< Position of particle at target
   TVector3         fTgtPosError;     ///< Position error of particle at target
   TVector3         fTgtMom;          ///< Momentum of particle at target
   TVector3         fTgtMomError;     ///< Momentum error of particle at target

   //Particle momentum and positions computed on TW Wall
   TVector3         fTwPos;           ///< Position of particle at TW
   TVector3         fTwPosError;      ///< Position error of particle at TW
   TVector3         fTwMom;           ///< Momentum of particle at TW
   TVector3         fTwMomError;      ///< Momentum error of particle at TW

   TClonesArray*    fListOfPoints;    ///< Attached measured points
   
   TArrayI           fMcTrackIdx;     //! Idx of the track created in the simulation
   map<int, int>     fMcTrackMap;     //! Map of MC track Id


   polynomial_fit_parameters fParameters; ///< polynomial parameter
    
   ClassDef(TAGtrack,3)
   
};

//##############################################################################

class TAGntuGlbTrack : public TAGdata {
   
private:
   TClonesArray*    fListOfTracks;		///< tracks
    
private:
   static TString   fgkBranchName;    ///< Branch name in TTree
   
public:
   TAGntuGlbTrack();
   virtual         ~TAGntuGlbTrack();
   
   // Get global track
   TAGtrack*        GetTrack(Int_t i);
   // Get global track (comst)
   const TAGtrack*  GetTrack(Int_t i) const;
   // Get number of global tracks
   Int_t            GetTracksN()      const;
   
   //! Get list of global tracks
   TClonesArray*    GetListOfTracks() { return fListOfTracks; }
   
   // New track
   TAGtrack*        NewTrack();
   // New track with mass, momentum, charge and tof
   TAGtrack*        NewTrack(Double_t mass, Double_t mom, Double_t charge, Double_t tof);
   // Add a new track to the repo  --- Genfit
   TAGtrack* 		NewTrack(string name, long evNum, int pdgID, float pdgMass, int measCh, float mass, float length, float tof, float chi2, int ndof, float pVal, TVector3* TgtPos, TVector3* TgtMom, TMatrixD* TgtPos_cov, TMatrixD* TgtMom_cov, TVector3* TwPos, TVector3* TwMom, TMatrixD* TwPos_cov, TMatrixD* TwMom_cov, vector<TAGpoint*>* shoeTrackPointRepo);   
   // New track with copy cstr
   TAGtrack*        NewTrack(TAGtrack& track);

   // Set up clones
   virtual void     SetupClones();
   // Clear
   virtual void     Clear(Option_t* opt="");
   // To stream
   virtual void     ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   //! Get branch name
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   ClassDef(TAGntuGlbTrack,2)
};


#endif
