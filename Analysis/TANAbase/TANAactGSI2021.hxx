/*!
 \file TANAactGSI2021.hxx
 \brief   Declaration of TANAactGSI2021
 \author R. Zarrella
 */

#ifndef _TANAactGSI2021_HXX
#define _TANAactGSI2021_HXX


#include "TSystem.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TF1.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"

#include "TATWparGeo.hxx"
#include "TAMCntuPart.hxx"
#include "TAMCntuRegion.hxx"
#include "TATWntuPoint.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TANAactBaseNtu.hxx"

class TANAactGSI2021 : public TANAactBaseNtu {
private:
	Float_t fMomMin = 0.;  // GeV/c
	Float_t fMomMax = 21.; // GeV/c
	Float_t fMomStep = 0.2;  // GeV/c

	std::vector<Int_t> fColors = {kBlack, kRed, kBlue, kGreen+2, kMagenta+1, kOrange, kGray+1, kViolet+2, kAzure+1};
	TF1* fGauss;

public:
	explicit  TANAactGSI2021(const char* name         = 0,
								TAGdataDsc* pNtuTrack = 0,
								TTree*		p_tree = 0,
								TAGdataDsc* pNtuMcTrk = 0,
								TAGdataDsc* pNtuMcReg = 0,
								TAGparaDsc* pgGeoMap = 0,
								TAGparaDsc* pgTwGeo = 0,
								std::map<Int_t, std::map<string, Int_t>> *pTrackCutsMap= 0,
									 std::map<string, Int_t> *pEventCutsMap= 0
									 );
	virtual ~TANAactGSI2021();

	// Create histograms
	void    CreateHistogram();

	// Action
	virtual  Bool_t  Action();
	void  BeginEventLoop();
	void  EndEventLoop();

protected:

	void PrintCutsMap(std::map<Int_t, std::map<string, Int_t>> aTrackCutsMap);
	void PrintCutsMap(std::map<string, Int_t> aEventCutsMap);

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
	ClassDef(TANAactGSI2021, 0)

	std::map<Int_t, std::map<string, Int_t>>& fTrackCutsMap; ///< Map of track cuts for every track in the event. key = track ID; string = name of the cut
	std::map<string, Int_t> &fEventCutsMap; ///< Map of event cuts. key = name of the cut
};

#endif
