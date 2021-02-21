#ifndef _TAMCntuEve_HXX
#define _TAMCntuEve_HXX
/*!
  \file
  \version $Id: TAMCntuEve.hxx,v 1.7 2003/06/22 11:48:38 mueller Exp $
  \brief   Declaration of TAMCntuEve.
*/
/*------------------------------------------+---------------------------------*/

#include "TVector3.h"
#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"


class TAMCeveTrack : public TAGobject
{
public:
   TAMCeveTrack();

   TAMCeveTrack(Int_t i_id, Int_t i_chg, Int_t i_type,
               Int_t i_reg, Int_t i_bar, Int_t i_dead,
               Double_t i_mass, Int_t i_moth,
               Double_t i_time,
               Double_t i_tof, Double_t i_trlen,
               TVector3 i_ipos, TVector3 i_fpos,
               TVector3 i_ip,TVector3 i_fp,
               TVector3 i_mothip,
	       TVector3 i_mothfp);

	virtual         ~TAMCeveTrack();

   TVector3         GetInitPos()      const { return fInitPos;        }
   TVector3         GetInitP()        const { return fInitMom;        }
   TVector3         GetFinalPos()     const { return fFinalPos;       }
   TVector3         GetFinalP()       const { return fFinalMom;       }
   Double_t         GetMass()         const { return fMass;           }
   Int_t            GetType()         const { return fType;           }
   Int_t            GetRegion()       const { return fRegion;         }
   Int_t            GetBaryon()       const { return fBaryon;         }
   Int_t            GetFlukaID()      const { return fFlukaId;        }
   Int_t            GetMotherID()     const { return fMotherId;       }
   Int_t            GetCharge()       const { return fCharge;         }
   Int_t            GetDead()         const { return fDead;           }
   Double32_t       GetTime()         const { return fTime;           }
   Double32_t       GetTrkLength()    const { return fTrkLength;      }
   Double32_t       GetTof()          const { return fTof;            }
   Double32_t       GetDeltaE()       const { return fDeltaE;         }
   TVector3         GetMotherInitP()  const { return fMotherInitMom;  }
   TVector3         GetMotherFinalP() const { return fMotherFinalMom; }

   void  SetInitPos(TVector3 pos)           { fInitPos = pos;         }
   void  SetInitP(TVector3 mom)             { fInitMom = mom;         }
   void  SetMotherInitP(TVector3 mom)       { fMotherInitMom = mom;   }
   void  SetFinalPos(TVector3 pos)          { fFinalPos = pos;        }
   void  SetFinalP(TVector3 mom)            { fFinalMom = mom;        }
   void  SetMotherFinalP(TVector3 mom)      { fMotherFinalMom = mom;  }
   void  SetMass(double amass)              { fMass = amass;          }
   void  SetFlukaID(int aid)                { fFlukaId = aid;         }
   void  SetDead(int adead)                 { fDead = adead;          }
   void  SetRegion(int areg)                { fRegion = areg;         }
   void  SetBaryon(int abar)                { fBaryon = abar;         }
   void  SetType(int atyp)                  { fType = atyp;           }
   void  SetCharge(int achg)                { fCharge = achg;         }
   void  SetMotherID(int aid)               { fMotherId = aid;        }
   void  SetTime(double atime)              { fTime = atime;          }
   void  SetDeltaE(double adE)              { fDeltaE = adE;          }
   void  SetTof(double atof)                { fTof = atof;            }
   void  SetTrkLength(double atrlen)        { fTrkLength = atrlen;    }

 private:
	Int_t         fFlukaId;                 // fluka identity
	Int_t         fCharge;                  // charge
	Int_t         fType;                    // type
	Int_t         fRegion;                  // region
	Int_t         fBaryon;                  // baryonic number
	Int_t         fDead;                    // region in which it dies
	Double32_t    fMass;                    // mass
   Int_t         fMotherId;               // mother identity
	Double32_t    fTime;                    // time of production
	Double_t      fTof;                     // time of flight
	Double_t      fTrkLength;               // track length
   Double32_t    fDeltaE;                 // deposited energy
	TVector3      fInitPos;		              // initial position
	TVector3      fFinalPos;		            // final position
	TVector3      fInitMom;	          	    // initial momentum
	TVector3      fFinalMom;	          	  // final momentum
	TVector3      fMotherInitMom;		        // mother init momentum
	TVector3      fMotherFinalMom;		      // mother final momentum

	ClassDef(TAMCeveTrack,2)
};

//##############################################################################

class TAMCntuEve : public TAGdata {
  public:
                       TAMCntuEve();
	virtual           ~TAMCntuEve();

	TAMCeveTrack*       GetTrack(Int_t i);
	const TAMCeveTrack* GetTrack(Int_t i) const;
   
   Int_t               GetTracksN() const;
   
   TAMCeveTrack*       NewTrack(Int_t i_id, Int_t i_fCharge, Int_t i_type,
                                Int_t i_reg, Int_t i_bar, Int_t i_dead,
                                Double_t i_mass, Int_t i_moth,
                                Double_t i_time,
                                Double_t i_tof, Double_t i_trlen,
                                TVector3 i_ipos, TVector3 i_fpos,
                                TVector3 i_ip,TVector3 i_fp,
                                TVector3 i_mothip,
                                TVector3 i_mothfp);
   
	virtual void        SetupClones();
	virtual void        Clear(Option_t* opt="");
	virtual void        ToStream(ostream& os=cout, Option_t* option="") const;

public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   static const Char_t* GetDefDataName()  { return fgkDefDataName.Data(); }

private:
   TClonesArray*   fListOfTracks; // ttracks
   
private:
   static TString fgkBranchName;    // Branch name in TTree
   static const TString fgkDefDataName;

   ClassDef(TAMCntuEve,1)
};


#endif
