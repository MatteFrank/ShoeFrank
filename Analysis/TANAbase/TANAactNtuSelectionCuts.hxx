#ifndef _TANAactNtuSelectionCuts_HXX
#define _TANAactNtuSelectionCuts_HXX
/*!
 \file TANAactNtuSelectionCuts.hxx
 \brief   Declaration of TANAactNtuSelectionCuts.
 \author G. Ubaldi
 */
/*------------------------------------------+---------------------------------*/
#include "TTree.h"

#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"
#include "TATWntuPoint.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TANAactBaseNtu.hxx"

class TANAactNtuSelectionCuts : public TANAactBaseNtu {
   
public:
	explicit TANAactNtuSelectionCuts(const char *name = 0,
									 TAGdataDsc *pNtuTrack = 0,
									 TTree *p_tree = 0,
									 TAGdataDsc *pNtuMcTrk = 0,
									 TAGdataDsc *pNtuMcReg = 0,
									 TAGparaDsc *pgGeoMap = 0,
									 TAGparaDsc *pgTwGeo = 0);

	virtual ~TANAactNtuSelectionCuts();

	// Create histograms
	void CreateHistogram();

	// Action
	Bool_t Action();
	void BeginEventLoop();
	void  EndEventLoop();

protected:
	Bool_t CheckRadiativeDecayChain(Int_t partID, std::vector<Int_t>* partIDvec);
	Bool_t CheckFragIn1stTWlayer(Int_t partID, std::vector<Int_t>* partIDvec);
	void TwClonesCut();     // check if more than one glb track has the same tw point
	void TrackQualityCut();     // cuts about chi2 and residuals
	void PrintCutsMap(std::map<Int_t, std::map<string, bool>> aTrackCutsMap);


	TAGdataDsc*		fpNtuTrack;				///< input global tracks
	TAGdataDsc*		fpNtuMcTrk;				///< input MC tracks
	TAGdataDsc*		fpNtuMcReg;				///< input MC region
	TAGgeoTrafo*	fpFootGeo;				///< First geometry transformer
	TAGparaDsc*		fpTwGeo;				///< First geometry transformer
	TAGparaDsc*		fpGeoMapG;				///< geometry para dsc
	TTree*			fpTree;					///< tree

	TAGntuGlbTrack*	fNtuGlbTrack;			///< input ntu global tracks
	TAGtrack *fGlbTrack;					///< variable to handle global track
	
	long			evNum;					///< current event number 

	Float_t			fBeamEnergyTarget;		///< Beam energy at target
	Float_t			fBeamEnergy;			///< Beam energy
	Float_t			fBeamA;					///< Beam atomic mass
	Float_t			fBeamZ;					///< Beam atomic number

	Int_t			fRegTgt;				///< MC region for target
	Int_t			fRegAir1;				///< MC region for air after target
	Int_t			fRegAir2;				///< MC region for air around TW
	Int_t			fRegFirstTWbar;			///< MC region for first TW bar
	Int_t			fRegLastTWbar;			///< MC region for last TW bar

	std::map<Int_t, std::map<string, bool>> fTrackCutsMap; ///< Map of track cuts for every track in the event. key = track ID

	ClassDef(TANAactNtuSelectionCuts, 0)
};

#endif