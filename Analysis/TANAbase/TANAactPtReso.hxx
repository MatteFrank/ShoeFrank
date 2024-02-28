#ifndef _TANAactPtReso_HXX
#define _TANAactPtReso_HXX
/*!
 \file TANAactPtReso.hxx
 \brief   Declaration of TANAactPtReso
 \author R. Zarrella
 */
/*------------------------------------------+---------------------------------*/

#include "TVector3.h"
#include "TSystem.h"

#include "TATWparGeo.hxx"

#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"
#include "TATWntuPoint.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TANAactBaseNtu.hxx"

class TANAactPtReso : public TANAactBaseNtu {
private:
	Float_t fMomMin = 0.;  // GeV/c
	Float_t fMomMax = 21.; // GeV/c
	Float_t fMomStep = 0.2;  // GeV/c

public:
	explicit  TANAactPtReso(const char* name         = 0,
								TAGdataDsc* pNtuTrack = 0,
								TTree*		p_tree = 0,
								TAGdataDsc* pNtuMcTrk = 0,
								TAGdataDsc* pNtuMcReg = 0,
								TAGparaDsc* pgGeoMap = 0,
								TAGparaDsc* pgTwGeo = 0);

	virtual ~TANAactPtReso();

	// Create histograms
	virtual  void    CreateHistogram();

	// Action
	virtual  Bool_t  Action();
	void  BeginEventLoop();
	void  EndEventLoop();

protected:
	Int_t		FindMcParticleAtTgt(TAGtrack* track);
	void		CheckMomentumHistogram(std::string name);
	Float_t		GetMomentumBinCenter(Double_t momentum);
	void		FillMomResidual(TAGtrack* track);
	void		GetMcMomentaAtTgt();
	void		ClearData();

	Bool_t CheckRadiativeDecayChain(Int_t partID, std::vector<Int_t>* partIDvec);
	Bool_t CheckFragIn1stTWlayer(Int_t partID, std::vector<Int_t>* partIDvec);

	TAGdataDsc*		fpNtuTrack;				///< input global tracks
	TAGdataDsc*		fpNtuMcTrk;				///< input MC tracks
	TAGdataDsc*		fpNtuMcReg;				///< input MC region
	TAGgeoTrafo*	fpFootGeo;				///< First geometry transformer
	TAGparaDsc*		fpTwGeo;				///< First geometry transformer
	TAGparaDsc*		fpGeoMapG;				///< geometry para dsc
	TTree*			fpTree;					///< tree

	TAGntuGlbTrack*	fNtuGlbTrack;			///< input global tracks

	Float_t			fBeamEnergyTarget;		///< Beam energy at target
	Float_t			fBeamEnergy;			///< Beam energy
	Float_t			fBeamA;					///< Beam atomic mass
	Float_t			fBeamZ;					///< Beam atomic number

	Int_t			fRegTgt;				///< MC region for target
	Int_t			fRegAir1;				///< MC region for air after target
	Int_t			fRegAir2;				///< MC region for air around TW
	Int_t			fRegFirstTWbar;			///< MC region for first TW bar
	Int_t			fRegLastTWbar;			///< MC region for last TW bar

	std::map<Int_t, TVector3> fMcMomMap;	///< Map of MC momenta in the event. key = MCid

	std::map<std::string, TH1D*> fpHisMomRes;	///< Map for momentum resolution histograms; key = particle name + true momentum bin center (e.g. "H_1.7" for hydrogen tracks w/ momentum around 1.7 GeV/c)

	ClassDef(TANAactPtReso, 0)
};

#endif
