#ifndef _TAMCntuPart_HXX
#define _TAMCntuPart_HXX
/*!
  \file TAMCntuPart.hxx
  \brief   Declaration of TAMCpart and TAMCntuPart.
*/
/*------------------------------------------+---------------------------------*/

#include "TVector3.h"
#include "TClonesArray.h"

#include "TAGobject.hxx"
#include "TAGdata.hxx"


class TAMCpart : public TAGobject
{
public:
   TAMCpart();

   TAMCpart(Int_t i_id, Int_t i_chg, Int_t i_type,
               Int_t i_reg, Int_t i_bar, Int_t i_dead,
               Double_t i_mass, Int_t i_moth,
               Double_t i_time,
               Double_t i_tof, Double_t i_trlen,
               TVector3 i_ipos, TVector3 i_fpos,
               TVector3 i_ip,TVector3 i_fp);

	virtual         ~TAMCpart();

   //! Get initial position
   TVector3         GetInitPos()      const { return fInitPos;        }
   //! Get initial momentum
   TVector3         GetInitP()        const { return fInitMom;        }
   //! Get final position
   TVector3         GetFinalPos()     const { return fFinalPos;       }
   //! Get final momentum
   TVector3         GetFinalP()       const { return fFinalMom;       }
   //! Get mass
   Double_t         GetMass()         const { return fMass;           }
   //! Get type
   Int_t            GetType()         const { return fType;           }
   //! Get region
   Int_t            GetRegion()       const { return fRegion;         }
   //! Get baryon number
   Int_t            GetBaryon()       const { return fBaryon;         }
   //! Get Fluka Id
   Int_t            GetFlukaID()      const { return fFlukaId;        }
   //! Get mother Id
   Int_t            GetMotherID()     const { return fMotherId;       }
   //! Get atomic charge
   Int_t            GetCharge()       const { return fCharge;         }
   //! Get dead status
   Int_t            GetDead()         const { return fDead;           }
   //! Get particle time
   Double32_t       GetTime()         const { return fTime;           }
   //! Get track length
   Double32_t       GetTrkLength()    const { return fTrkLength;      }
   //! Get time of flight
   Double32_t       GetTof()          const { return fTof;            }

   //! Set initial position
   void  SetInitPos(TVector3 pos)           { fInitPos = pos;         }
   //! Set initial momentum
   void  SetInitP(TVector3 mom)             { fInitMom = mom;         }
   //! Set final position
   void  SetFinalPos(TVector3 pos)          { fFinalPos = pos;        }
   //! Set final momentum
   void  SetFinalP(TVector3 mom)            { fFinalMom = mom;        }
   //! Set mass
   void  SetMass(double amass)              { fMass = amass;          }
   //! Set Fluka Id
   void  SetFlukaID(int aid)                { fFlukaId = aid;         }
   //! Set dead status
   void  SetDead(int adead)                 { fDead = adead;          }
   //! Set region
   void  SetRegion(int areg)                { fRegion = areg;         }
   //! Set baryon number
   void  SetBaryon(int abar)                { fBaryon = abar;         }
   //! Set type
   void  SetType(int atyp)                  { fType = atyp;           }
   //! Set atomic charge
   void  SetCharge(int achg)                { fCharge = achg;         }
   //! Set mother Id
   void  SetMotherID(int aid)               { fMotherId = aid;        }
   //! Set particle time
   void  SetTime(double atime)              { fTime = atime;          }
   //! Set time of flight
   void  SetTof(double atof)                { fTof = atof;            }
   //! Set track length
   void  SetTrkLength(double atrlen)        { fTrkLength = atrlen;    }

 private:
	Int_t         fFlukaId;                 ///< fluka identity
	Int_t         fCharge;                  ///< charge
	Int_t         fType;                    ///< type
	Int_t         fRegion;                  ///< region
	Int_t         fBaryon;                  ///< baryonic number
	Int_t         fDead;                    ///< region in which it dies
	Double32_t    fMass;                    ///< mass
   Int_t         fMotherId;                ///< mother identity
	Double32_t    fTime;                    ///< time of production
	Double_t      fTof;                     ///< time of flight
	Double_t      fTrkLength;               ///< track length
	TVector3      fInitPos;		             ///< initial position
	TVector3      fFinalPos;		          ///< final position
	TVector3      fInitMom;	          	    ///< initial momentum
	TVector3      fFinalMom;	          	 ///< final momentum

	ClassDef(TAMCpart,3)
};

//##############################################################################

class TAMCntuPart : public TAGdata {
  public:
                    TAMCntuPart();
	virtual         ~TAMCntuPart();

   //! Get particle
	TAMCpart*       GetTrack(Int_t i);
   //! Get particle (const)
	const TAMCpart* GetTrack(Int_t i) const;
   
   //! Get nuimber of tracks
   Int_t           GetTracksN() const;
   
   //! New track
   TAMCpart*       NewTrack(Int_t i_id, Int_t i_fCharge, Int_t i_type,
                                Int_t i_reg, Int_t i_bar, Int_t i_dead,
                                Double_t i_mass, Int_t i_moth,
                                Double_t i_time,
                                Double_t i_tof, Double_t i_trlen,
                                TVector3 i_ipos, TVector3 i_fpos,
                                TVector3 i_ip,TVector3 i_fp);
   //! Setup clones
	virtual void        SetupClones();
   //! Clear
	virtual void        Clear(Option_t* opt="");
   //! To stream
	virtual void        ToStream(ostream& os=cout, Option_t* option="") const;

public:
   //! Get branch name
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();  }
   //! Get default parameter name
   static const Char_t* GetDefDataName()  { return fgkDefDataName.Data(); }

private:
   TClonesArray*   fListOfTracks;       ///< ttracks
   
private:
   static TString fgkBranchName;        ///< Branch name in TTree
   static const TString fgkDefDataName; ///< Default parameter name

   ClassDef(TAMCntuPart,2)
};


#endif
