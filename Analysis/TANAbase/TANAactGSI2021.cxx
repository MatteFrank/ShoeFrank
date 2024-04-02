/*!
 \file TANAactGSI2021.cxx
 \brief   Implementation of TANAactGSI2021
 \author R. Zarrella
 */

#include "TANAactGSI2021.hxx"

/*!
 \class TANAactGSI2021
 \brief Base class for global reconstruction momentum performance analysis

 The class creates a set of histograms to compare reconstructed and MC momentum, then performs a fit of the distributions to evaluate their mean (offset, "delta") and standard deviations (resolution, "sigma")
 */

//------------------------------------------+-----------------------------------
//! \brief Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuTrack global track container descriptor
//! \param[in,out] p_tree output tree (currently not used)
//! \param[in] pNtuMcTrk MC track container descriptor
//! \param[in] pNtuMcReg MC crossings container descriptor
//! \param[in] pgGeoMap target geometry parameter descriptor
//! \param[in] pgTwGeo TW geometry parameter descriptor
TANAactGSI2021::TANAactGSI2021(const char* name, TAGdataDsc* pNtuTrack, TTree* p_tree, TAGdataDsc* pNtuMcTrk, TAGdataDsc* pNtuMcReg, TAGparaDsc* pgGeoMap, TAGparaDsc* pgTwGeo, std::map<Int_t, std::map<string, Int_t>> *pTrackCutsMap,std::map<string, Int_t> *pEventCutsMap  )
: TANAactBaseNtu(name, pNtuTrack, pgGeoMap, p_tree),
fpNtuTrack(pNtuTrack),
fpTree(p_tree),
fpNtuMcTrk(pNtuMcTrk),
fpNtuMcReg(pNtuMcReg),
fpGeoMapG(pgGeoMap),
fpTwGeo(pgTwGeo),
fTrackCutsMap(*pTrackCutsMap),
fEventCutsMap(*pEventCutsMap)
{
	AddDataIn(pNtuTrack, "TAGntuGlbTrack");
	AddDataIn(pNtuMcTrk, "TAMCntuPart");
	AddDataIn(pNtuMcReg, "TAMCntuRegion");

	fNtuGlbTrack = (TAGntuGlbTrack*)fpNtuTrack->Object();

	fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
	if (!fpFootGeo)
		Error("TANAactGSI2021()", "No GeoTrafo action available yet\n");
}


//! \brief Destructor.
TANAactGSI2021::~TANAactGSI2021()
{

}

//------------------------------------------+-----------------------------------

//! \brief Perform preliminary operations before the event loop
void TANAactGSI2021::BeginEventLoop()
{
	TAGparGeo* pGeoMapG = (TAGparGeo*) fpGeoMapG->Object();
	TATWparGeo* twGeo = (TATWparGeo*) fpTwGeo->Object();

	// beam loss in target
	fBeamEnergyTarget  = pGeoMapG->GetBeamPar().Energy;
	fBeamA             = pGeoMapG->GetBeamPar().AtomicMass;
	fBeamZ             = pGeoMapG->GetBeamPar().AtomicNumber;

	
	//Get some relevant regions
	fRegTgt = pGeoMapG->GetRegTarget();
	fRegAir1 = pGeoMapG->GetRegAirPreTW();
	fRegAir2 = pGeoMapG->GetRegAirTW();
	fRegFirstTWbar = twGeo->GetRegStrip(0, 0);
	fRegLastTWbar = twGeo->GetRegStrip(1, twGeo->GetNBars() - 1);
}


//! \brief Action for momentum resolution
Bool_t TANAactGSI2021::Action()
{

if(!fEventCutsMap.empty())
PrintCutsMap(fEventCutsMap);  
if(!fTrackCutsMap.empty())
PrintCutsMap(fTrackCutsMap);

Int_t nt = fNtuGlbTrack->GetTracksN(); // number of reconstructed tracks for every event
if (ValidHistogram())
	{
		FillCountHist();		
	}

return true;
}

//! \brief Perform analysis after the event loop
void TANAactGSI2021::EndEventLoop()
{

}

//! Setup all histograms.
void TANAactGSI2021::CreateHistogram()
{
	DeleteHistogram();

	fpCount_hist = new TH1D("countEvents", "", 5, 0., 5.);
	fpCount_hist->GetXaxis()->SetBinLabel(1, "Total events");
	fpCount_hist->GetXaxis()->SetBinLabel(2, "Total Reco Events");
	fpCount_hist->GetXaxis()->SetBinLabel(3, "Total Reco Ana Cuts");
	fpCount_hist->GetXaxis()->SetBinLabel(4, "Total True Events");
	fpCount_hist->GetXaxis()->SetBinLabel(5, "others");
	AddHistogram(fpCount_hist);

	SetValidHistogram(kTRUE);
}


//------------------------------------------------------------------------------
//------------------------------------ ACTION ----------------------------------
//------------------------------------------------------------------------------

//! \brief Print all the elements, keys and values of the event cuts map
void TANAactGSI2021::PrintCutsMap(std::map<string, Int_t> aEventCutsMap)
{
	std::cout << "GSI2021 Action: Event cuts " << endl;
	for (const auto &[key, value] : aEventCutsMap)
	{
	std::cout << "[" << key << "] = " << value << "; " << endl;
	}
	cout << endl;

}

//! \brief Print all the elements, keys and values of the track cuts map
void TANAactGSI2021::PrintCutsMap(std::map<Int_t, std::map<string, Int_t>> aTrackCutsMap)
{	
	std::cout << "GSI2021 Action: Track cuts "  << endl;
	for (const auto &[key, value] : aTrackCutsMap)
	{
		std::cout << "Element " << key << endl;
		for (const auto &[key2, value2] : value)
			std::cout << "[" << key2 << "] = " << value2 << "; " << endl;
	}
	cout << endl;
}

//! \brief Fill a histogram with all the counts according to several cuts
void TANAactGSI2021::FillCountHist(){
	Int_t nt = fNtuGlbTrack->GetTracksN(); // number of reconstructed tracks for every event
	fpCount_hist->Fill(0);
	for (int it = 0; it < nt; it++)
	{
		if (fEventCutsMap["SCcut"] != 0 && fEventCutsMap["NTracksCut"] == 1 && fEventCutsMap["BMcut"] && fTrackCutsMap[it]["VTXposCut"] == 1)
		{
			fpCount_hist->Fill(1);

			// =================== Chi2 cuts + multitrack + NO TW in multitracks + n_tracks == n_twpoints
			if (fTrackCutsMap[it]["TrackQuality"] == 1 && fEventCutsMap["TWnum"] != 0 and fTrackCutsMap[it]["TWclone"] != 0)
			{
				fpCount_hist->Fill(2);
			}
		}
	}
}

//------------------------------------------------------------------------------
//---------------------------------- AFTER LOOP --------------------------------
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
//----------------------------------- UTILITIES --------------------------------
//------------------------------------------------------------------------------


