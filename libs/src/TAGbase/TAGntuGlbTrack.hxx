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

#include "TAGobject.hxx"
#include "TAGdata.hxx"
#include "TAGntuPoint.hxx"


#define build_string(expr) \
    (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())

using namespace std;

//
class TAGpoint;
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

   virtual         ~TAGtrack();
   
   void             SetMass(Double_t amass)       { fMass = amass;     }
   Double_t         GetMass()               const { return fMass;      }  //(also Genfit)
   
   void             SetMomentum(Double_t amom)    { fMom = amom;       }
   Double_t         GetMomentum()           const { return fMom;       }  //(also Genfit)
   
   void             SetCharge(Int_t acharge)      { fCharge = acharge; }	
   Int_t            GetCharge()             const { return fCharge;    }  //(also Genfit)
   
   void             SetTof(Double_t atoff)        { fTof = atoff;      }
   Double_t         GetTof()                const { return fTof;       }
   
   void             SetTrackId(Int_t trid)        { fTrkId = trid;     }
   Int_t            GetTrackId()            const { return fTrkId;     }
   
   void             SetEnergy(Double_t e)         { fEnergy = e;       }
   Double_t         GetEnergy()             const { return fEnergy;    }
   
   void             SetTgtDirection(TVector3 dir) { fTgtDir = dir;     }
   TVector3         GetTgtDirection()             { return fTgtDir;    }
   
   void             SetTgtPosition(TVector3 pos)  { fTgtPos = pos;     }
   TVector3         GetTgtPosition()              { return fTgtPos;    }  //(also Genfit)
   
   void             SetTofPosition(TVector3 pos)  { fTofPos = pos;     }
   TVector3         GetTofPosition()              { return fTofPos;    }  //(also Genfit?)
   
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

   
   //! Get list of measured points  //(also Genfit)
   TClonesArray*    GetListOfMeasPoints()   const { return fListOfMeasPoints;                       }
   
   //! Get list of corrected points
   TClonesArray*    GetListOfCorrPoints()   const { return fListOfCorrPoints;                       }
   
   //! Get number of measured points  //(also Genfit)
   Int_t            GetMeasPointsN()        const { return fListOfMeasPoints->GetEntries();         }
   
   //! Get number of corrected points
   Int_t            GetCorrPointsN()        const { return fListOfCorrPoints->GetEntries();         }
   
   //! Get measured point  //(also Genfit)
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
   polynomial_fit_parameters const&    GetParameters( ) const{ return fParameters; }
   TVector3         GetPosition( double z );
   
protected:

	string 			m_name;					// particleName_mass_id
	long 			m_evNum;
	int m_pdgID;							// PDG ID used in the fit
	// int m_pdgCh;							// charge used in the fit by the cardinal rep
	int m_measCh;							// charge measured at state 0 (VTX 1st ly)
	double m_length;						// track length from first to last measure

	double 		m_chi2;
	int 		m_ndof;
	double 		m_pVal;

	Double32_t       fMass;
	Double32_t       fMom;
	Int_t            fCharge;
	Double32_t       fTof;
	Double32_t       fEnergy;
	Int_t            fTrkId;

	//Particle directions and positions computed on target middle
	TVector3         fTgtDir;
	TVector3         fTgtPos;  		//(also Genfit now at VTX 1st layer, to be fixed )
	TVector3 		 m_target_mom; 	//(also Genfit  )

	//Particle directions and positions computed on ToF Wall
	TVector3         fTofPos;  //(also Genfit) todo
	TVector3         fTofDir;
	TVector3 		 m_TW_mom;   //(also Genfit) todo

	TVector3         m_calo_pos;  //(also Genfit)	todo
	TVector3 		 m_calo_mom;  //(also Genfit) 	todo

	vector<TAGpoint> m_shoeTrackPointRepo;

	TClonesArray*    fListOfMeasPoints;          // Attached measured points   //(also Genfit)
	TClonesArray*    fListOfCorrPoints;          // Attached corrected points

	/// ONLY MC QUANTITIES  //////////

	int m_MCparticle_id;					//	ID in the shoe particle container ( TAMCpart )
	float m_trackQuality;					//	ratio of (N of most frequent particle measure)/(N all measurements)

	/////////////////////////////////

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

















