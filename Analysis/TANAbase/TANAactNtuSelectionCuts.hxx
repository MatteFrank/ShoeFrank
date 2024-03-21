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
									 TAGdataDsc *pNtuVtx= 0,
									 TAGdataDsc *pNtuRecTw= 0,
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
	void TwClonesCut();     // check if more than one glb track has the same tw point; check that num of tw points is the same of reco tracks
	void TrackQualityCut(Int_t track_id,TAGtrack* fGlbTrack);     // cuts about chi2 and residuals
	void VtxPositionCut(Int_t track_id, TAGtrack* fGlbTrack);     // cuts about vertex point position
	void TwPointCut(Int_t track_id, TAGtrack* fGlbTrack);     // cuts about tw point features
	void PrintCutsMap(std::map<Int_t, std::map<string, bool>> aTrackCutsMap);
	std::vector<string> customSplit(string str, char separator); // split a string in elements by separator (ideally the .split() function of python)


	TAGdataDsc*		fpNtuTrack;				///< input global tracks
	TAGdataDsc*		fpNtuVtx;				///< input VTX points
	TAGdataDsc*		fpNtuRecTw;				///< input TW points
	TAGdataDsc*		fpNtuMcTrk;				///< input MC tracks
	TAGdataDsc*		fpNtuMcReg;				///< input MC region
	TAGgeoTrafo*	fpFootGeo;				///< First geometry transformer
	TAGparaDsc*		fpTwGeo;				///< First geometry transformer
	TAGparaDsc*		fpGeoMapG;				///< geometry para dsc
	TTree*			fpTree;					///< tree

	TAGntuGlbTrack*	fNtuGlbTrack;			///< input ntu global tracks
	TAGtrack *fGlbTrack;					///< variable to handle global track
	TATWntuPoint *fNtuRecTw;				///< input ntu TW points
	TAVTntuVertex *fNtuVtx;

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
	TVector3		tgSize;					///< dimension of target

	std::map<Int_t, std::map<string, bool>> fTrackCutsMap; ///< Map of track cuts for every track in the event. key = track ID

	ClassDef(TANAactNtuSelectionCuts, 0)
};

#endif
