/*!
 \file TANAactPtReso.cxx
 \brief   Implementation of TANAactPtReso
 \author R. Zarrella
 */

#include "TANAactPtReso.hxx"

/*!
 \class TANAactPtReso
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
TANAactPtReso::TANAactPtReso(const char* name, TAGdataDsc* pNtuTrack, TTree* p_tree, TAGdataDsc* pNtuMcTrk, TAGdataDsc* pNtuMcReg, TAGparaDsc* pgGeoMap, TAGparaDsc* pgTwGeo)
: TANAactBaseNtu(name, pNtuTrack, pgGeoMap, p_tree),
fpNtuTrack(pNtuTrack),
fpTree(p_tree),
fpNtuMcTrk(pNtuMcTrk),
fpNtuMcReg(pNtuMcReg),
fpGeoMapG(pgGeoMap),
fpTwGeo(pgTwGeo)
{
	AddDataIn(pNtuTrack, "TAGntuGlbTrack");
	AddDataIn(pNtuMcTrk, "TAMCntuPart");
	AddDataIn(pNtuMcReg, "TAMCntuRegion");

	fNtuGlbTrack = (TAGntuGlbTrack*)fpNtuTrack->Object();

	fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
	if (!fpFootGeo)
		Error("TANAactPtReso()", "No GeoTrafo action available yet\n");
}


//! \brief Destructor.
TANAactPtReso::~TANAactPtReso()
{
	ClearData();
}

//------------------------------------------+-----------------------------------

//! \brief Perform preliminary operations before the event loop
void TANAactPtReso::BeginEventLoop()
{
	TAGparGeo* pGeoMapG = (TAGparGeo*) fpGeoMapG->Object();
	TATWparGeo* twGeo = (TATWparGeo*) fpTwGeo->Object();

	// beam loss in target
	fBeamEnergyTarget  = pGeoMapG->GetBeamPar().Energy;
	fBeamA             = pGeoMapG->GetBeamPar().AtomicMass;
	fBeamZ             = pGeoMapG->GetBeamPar().AtomicNumber;

	for(int iCh=1; iCh<=fBeamZ; ++iCh)
	{
		fpDelGraphs[iCh];
		fpDelGraphs[iCh] = new TGraphErrors();
		fpResGraphs[iCh];
		fpResGraphs[iCh] = new TGraphErrors();
	}
	fpAllDelGraph = new TMultiGraph("P_delta", "P delta");
	fpAllResGraph = new TMultiGraph("P_resolution", "P resolution");

	fGauss = new TF1("fGauss", "[0]+[1]*TMath::Gaus(x,[2],[3])", -.5, .5);

	//Get some relevant regions
	fRegTgt = pGeoMapG->GetRegTarget();
	fRegAir1 = pGeoMapG->GetRegAirPreTW();
	fRegAir2 = pGeoMapG->GetRegAirTW();
	fRegFirstTWbar = twGeo->GetRegStrip(0, 0);
	fRegLastTWbar = twGeo->GetRegStrip(1, twGeo->GetNBars() - 1);
}


//! \brief Action for momentum resolution
Bool_t TANAactPtReso::Action()
{
	ClearData();

	//Skip event if empty
	if( fNtuGlbTrack->GetTracksN() < 1 )
		return true;

	GetMcMomentaAtTgt();
	if( fMcMomMap.size() < 1 )
		return true;
	

	for(int iTrack = 0; iTrack < fNtuGlbTrack->GetTracksN(); ++iTrack )
	{
		TAGtrack* track = fNtuGlbTrack->GetTrack(iTrack);
		FillMomResidual(track);
	}

	return true;
}


//! \brief Perform analysis after the event loop
void TANAactPtReso::EndEventLoop()
{
	for( auto itMap : fpHisMomRes )
		AddHistogram(itMap.second);
	SetHistogramDir(fDirectory);
	SetValidHistogram(kTRUE);

	for(Int_t iCh = 1; iCh <= fBeamZ; ++iCh )
		ExtractMomentumResolution(iCh);

	if( fpAllDelGraph->GetListOfGraphs() && fpAllDelGraph->GetListOfGraphs()->GetSize() > 0 )
		SaveAllGraphs();
}


//------------------------------------------------------------------------------
//------------------------------------ ACTION ----------------------------------
//------------------------------------------------------------------------------


//! \brief Fill the momentum residual for a track
//!
//! \param[in] track Global track under study
void TANAactPtReso::FillMomResidual(TAGtrack* track)
{
	if( !track->HasTwPoint() )
		return;

	Int_t mcPart = FindMcParticleAtTgt(track);
	if( mcPart == -1 )
		return;

	TVector3 recoMom = track->GetTgtMomentum();
	TVector3 mcMom = fMcMomMap[mcPart];

	Float_t momBin = GetMomentumBinCenter(mcMom.Mag());
	TString hisName = GetParticleNameFromCharge(track->GetTwChargeZ());
	hisName = hisName + Form("_%.2f",momBin);
	CheckMomentumHistogram(hisName);

	fpHisMomRes[hisName]->Fill((recoMom.Mag() - mcMom.Mag())/mcMom.Mag());
}



//! \brief Find the MC particle needed for momentum comparison
//!
//! \param[in] track Track to check
//! \return Id of the corresponding MC particle, -1 if not found
Int_t TANAactPtReso::FindMcParticleAtTgt(TAGtrack* track)
{
	TATWpoint *twpoint = (TATWpoint *)track->GetPoint(track->GetPointsN() - 1);
	Int_t trackId_TW = twpoint->GetMcTrackIdx(0);
	Bool_t isGammaDecay = false;
	std::vector<Int_t> particleID_vec;

	isGammaDecay = CheckRadiativeDecayChain(trackId_TW, &particleID_vec);

	if( !isGammaDecay )
		CheckFragIn1stTWlayer(trackId_TW, &particleID_vec);

	if(std::find(particleID_vec.begin(), particleID_vec.end(), trackId_TW) == particleID_vec.end())
		particleID_vec.insert(particleID_vec.begin(), trackId_TW);

	if(std::find(particleID_vec.begin(), particleID_vec.end(), track->GetMcMainTrackId()) == particleID_vec.end())
		return -1;

	for ( auto itMc : fMcMomMap )
	{
		if( std::find(particleID_vec.begin(), particleID_vec.end(), itMc.first) != particleID_vec.end() )
			return itMc.first;
	}

	return -1;
}


//! \brief Check if a momentum histogram exists, create it otherwise
//!
//! \param[in] name Name of the histogram containing particle name and momentum bin center (e.g. "He_2.5" for Helium at 2.5 GeV/c)
void TANAactPtReso::CheckMomentumHistogram(TString name)
{
	if( fpHisMomRes.find(name) != fpHisMomRes.end() )
		return;

	fpHisMomRes[name];
	fpHisMomRes[name] = new TH1D(name.Data(), Form("%s;(p_{reco} - p_{MC})/p_{MC} at Target;Entries", name.Data()), 1000, -1, 1);
}


//! \brief Get the MC momentum of all particles crossing the TG
void TANAactPtReso::GetMcMomentaAtTgt()
{
	TAMCntuPart* mcNtuPart = (TAMCntuPart*)fpNtuMcTrk->Object();
	if( !mcNtuPart )
		return;

	for(int i=0; i < mcNtuPart->GetTracksN(); ++i)
	{
		TAMCpart* part = mcNtuPart->GetTrack(i);
		if( !part )
			continue;
		// cout << Form("Id::%d\tZ::%d\tA::%d\tReg::%d\tZfin::%f\tMothId::%d\n",i,part->GetCharge(),part->GetBaryon(),part->GetRegion(),part->GetFinalPos().Z(),part->GetMotherID());
		
		if( part->GetMotherID() == 0 && part->GetRegion() == fRegTgt )
			fMcMomMap[i] = part->GetInitP();
	}
	if( fMcMomMap.size() > 0 ) // fragments born in target found, exit
		return;
	
	//Check if there is a primary crossing target without interaction
	TAMCntuRegion* mcNtuReg = (TAMCntuRegion*)fpNtuMcReg->Object();
	for(int i = 0; mcNtuReg && i < mcNtuReg->GetRegionsN(); ++i)
	{
		TAMCregion* mcReg = (TAMCregion*)mcNtuReg->GetRegion(i);
		if( mcReg->GetOldCrossN() == fRegTgt && mcReg->GetCrossN() == fRegAir1 && mcReg->GetTrackIdx() == 1 )
		{
			fMcMomMap[ mcReg->GetTrackIdx() - 1 ] = mcReg->GetMomentum();
			break;
		}
	}
}


//------------------------------------------------------------------------------
//---------------------------------- AFTER LOOP --------------------------------
//------------------------------------------------------------------------------


//! \brief Calculate the momentum resolution for a given Z
//!
//! \param[in] iCh Charge (atomic number) of the fragments studied
void TANAactPtReso::ExtractMomentumResolution(Int_t iCh)
{
	TString partName(GetParticleNameFromCharge(iCh));
	cout << "Extracting momentum resolution for " << partName << endl;

	for( auto itHisMap : fpHisMomRes )
	{
		if( (itHisMap.second)->GetEntries() < 100 || !(itHisMap.first).Contains(TString(partName+"_")) )
			continue;
		
		double mean = (itHisMap.second)->GetBinCenter((itHisMap.second)->GetMaximumBin());
		double dev = (itHisMap.second)->GetStdDev();
		fGauss->SetParameter(0,0.);
		fGauss->SetParameter(1,(itHisMap.second)->GetMaximum());
		fGauss->SetParameter(2,mean);
		fGauss->SetParLimits(2,mean-3*dev,mean+3*dev);
		fGauss->SetParameter(3,dev);
		fGauss->SetParLimits(3,0,dev*2);

		TF1* func = (TF1*)fGauss->Clone();
		(itHisMap.second)->Fit(func, "Q0", "", -0.3, 0.3);

		TString sub = (itHisMap.first);
		sub.ReplaceAll(TString(partName+"_"), "");
		double pMC = sub.Atof();

		fpDelGraphs[iCh]->Set(fpDelGraphs[iCh]->GetN() + 1);
		fpDelGraphs[iCh]->SetPoint(fpDelGraphs[iCh]->GetN()-1, pMC, func->GetParameter(2)*100);
		fpDelGraphs[iCh]->SetPointError(fpDelGraphs[iCh]->GetN()-1, fMomStep/sqrt(12), func->GetParError(2)*100);

		fpResGraphs[iCh]->Set(fpResGraphs[iCh]->GetN() + 1);
		fpResGraphs[iCh]->SetPoint(fpResGraphs[iCh]->GetN()-1, pMC, func->GetParameter(3)*100);
		fpResGraphs[iCh]->SetPointError(fpResGraphs[iCh]->GetN()-1, fMomStep/sqrt(12), func->GetParError(3)*100);
	}

	if( fpResGraphs[iCh]->GetN() > 0 )
	{
		fpDelGraphs[iCh]->SetName(partName);
		fpDelGraphs[iCh]->SetTitle(Form("%s;p_{MC} [GeV/c];#Delta (p) [%%]",partName.Data()));
		fpDelGraphs[iCh]->SetMarkerColor(fColors[iCh-1]);
		fpDelGraphs[iCh]->SetMarkerStyle(20);
		fpDelGraphs[iCh]->SetMarkerSize(1.2);
		fpDelGraphs[iCh]->SetLineColor(fColors[iCh-1]);
		fpDelGraphs[iCh]->SetLineWidth(2);
		fpAllDelGraph->Add(fpDelGraphs[iCh], "p");

		fpResGraphs[iCh]->SetName(partName);
		fpResGraphs[iCh]->SetTitle(Form("%s;p_{MC} [GeV/c];#sigma (p) [%%]",partName.Data()));
		fpResGraphs[iCh]->SetMarkerColor(fColors[iCh-1]);
		fpResGraphs[iCh]->SetMarkerStyle(20);
		fpResGraphs[iCh]->SetMarkerSize(1.2);
		fpResGraphs[iCh]->SetLineColor(fColors[iCh-1]);
		fpResGraphs[iCh]->SetLineWidth(2);
		fpAllResGraph->Add(fpResGraphs[iCh], "p");
	}
}


//! \brief Save all the momentum resolution graphs to the output file
void TANAactPtReso::SaveAllGraphs()
{
	TDirectory* cwd = gDirectory;
	gDirectory = fDirectory;
	gROOT->SetBatch(kTRUE);
	for( auto itGraph : fpDelGraphs )
	{
		if( (itGraph.second)->GetN() > 0 )
		{
			TCanvas* c1 = new TCanvas(Form("pDelta_Z%d",itGraph.first), Form("pDelta_Z%d",itGraph.first), 1200, 1000);
			(itGraph.second)->Draw("ALP");
			c1->SetGrid();
			c1->Write();
		}
	}

	for( auto itGraph : fpResGraphs )
	{
		if( (itGraph.second)->GetN() > 0 )
		{
			TCanvas* c1 = new TCanvas(Form("pSigma_Z%d",itGraph.first), Form("pSigma_Z%d",itGraph.first), 1200, 1000);
			(itGraph.second)->Draw("ALP");
			c1->SetGrid();
			c1->Write();
		}
	}

	TCanvas* cDelta = new TCanvas("pDeltaAll", "pDeltaAll", 1200, 1000);
	fpAllDelGraph->GetXaxis()->SetTitle("p_{MC} [GeV/c]");
	fpAllDelGraph->GetYaxis()->SetTitle("#delta (p) [%%]");
	fpAllDelGraph->Draw("ap");
	TLegend* leg = cDelta->BuildLegend();
	leg->SetNColumns(2);
	leg->Draw("same");
	cDelta->SetGrid();
	cDelta->Write();

	TCanvas* cReso = new TCanvas("pResoAll", "pResoAll", 1200, 1000);
	fpAllResGraph->GetXaxis()->SetTitle("p_{MC} [GeV/c]");
	fpAllResGraph->GetYaxis()->SetTitle("#sigma (p) [%%]");
	fpAllResGraph->Draw("ap");
	TLegend* leg2 = cReso->BuildLegend();
	leg2->SetNColumns(2);
	leg2->Draw("same");
	cReso->SetGrid();
	cReso->Write();
	gDirectory = cwd;
}


//------------------------------------------------------------------------------
//----------------------------------- UTILITIES --------------------------------
//------------------------------------------------------------------------------


//! \brief Get the momentum bin center from the momentum magnitude
//! 
//! \param[in] mom Momentum magnitude
//! \return Center of the corresponding momentum bin for resolution histograms
Float_t TANAactPtReso::GetMomentumBinCenter(Double_t mom)
{
	if( mom < fMomMin || mom > fMomMax )
		return -1;

	Int_t nSteps = floor( ( mom - fMomMin )/fMomStep );
	return fMomMin + fMomStep*(nSteps + 0.5);
}


//! \brief Create the histograms for momentum resolution
void TANAactPtReso::CreateHistogram()
{
}


//! \brief Clear all internal data structures for new loop
void TANAactPtReso::ClearData()
{
	fMcMomMap.clear();
}


/**
 * @brief Check for gamma decay in the current MC particle
 * 
 * This function checks if a particle was born in the target and has then undergone one or more gamma decays. This is needed because the particle ID changes whenever a gamma decay occurs, even if the nucleus is the same.
 * @param[in] partID MC id of the particle under study
 * @param[out] partIDvec Vector containing all the MC ids of the nucleus between consecutive gammay decays
 * @return True if the particle has undergone one or more gamma decays and if the first nucleus was born in the target
 */
Bool_t TANAactPtReso::CheckRadiativeDecayChain(Int_t partID, std::vector<Int_t>* partIDvec)
{
	bool isGammaDecay = false;
	TAMCpart* part;
	TAMCpart* partMoth;
	
	TAMCntuPart* mcNtuPart = (TAMCntuPart*)fpNtuMcTrk->Object();

	part = mcNtuPart->GetTrack(partID);
	if( !part )
		goto nodecaychain;

	partMoth = mcNtuPart->GetTrack( part->GetMotherID() );
	if ( !partMoth )
		goto nodecaychain;
	

	if( partMoth->GetCharge() == part->GetCharge() && //if Z and A are the same
		partMoth->GetBaryon() == part->GetBaryon() )
	{ //check if there is a gamma from same particle
		for(int iGamma =0; iGamma < mcNtuPart->GetTracksN(); ++iGamma)
		{
		TAMCpart* maybeGamma = mcNtuPart->GetTrack(iGamma);
		if( maybeGamma->GetMotherID() == part->GetMotherID() && maybeGamma->GetFlukaID() == 7 )
		{ //there is a gamma with same mother id!
			isGammaDecay = true; //Particle comes from radiative decay! -> ok!
			break;
		}
		}
	}
	else
		goto nodecaychain;
	
	if( isGammaDecay )
	{
		partIDvec->push_back(partID);
		if( partMoth->GetMotherID() == 0 && partMoth->GetRegion() == fRegTgt ) // mother particle comes from primary and is born in target
		{
		partIDvec->push_back(part->GetMotherID());
		return true;
		}
		else
			return CheckRadiativeDecayChain(part->GetMotherID(), partIDvec); //Check next step of chain
	}
	else
		goto nodecaychain;


	nodecaychain: //directive for function exit in case of no gamma decay
		partIDvec->clear();
		return false;
}


/**
 * @brief Check if the particle reaching the TW undergoes fragmentation in the 1st layer
 * 
 * This function is needed since the TWpoint logic does not save all MC ids of the TWhits. This functions checks if the MCid associated to the TW point is the one that made the crossing air-TW, which is used to compute the Z/beta true used in MC cross section calculation (N_ref yield)
 * @param[in] partID MC id of the particle in the TW point
 * @param[out] partIDvec Vector that will contain the MC ids of all particles involved in the creation of the TWpoint
 * @return True if fragmentation in the 1st TW layer happened
 */
Bool_t TANAactPtReso::CheckFragIn1stTWlayer(Int_t partID, std::vector<Int_t>* partIDvec)
{
	TAMCntuPart* mcNtuPart = (TAMCntuPart*)fpNtuMcTrk->Object();
	TAMCpart* part = mcNtuPart->GetTrack(partID);
	if( !part )
	{
		partIDvec->clear();
		return false;
	}

	TAMCpart* partMoth = mcNtuPart->GetTrack( part->GetMotherID() );
	if ( !partMoth )
	{
		partIDvec->clear();
		return false;
	}

	TATWparGeo* twGeo = (TATWparGeo*) fpTwGeo->Object();

	TVector3 mothFinPos = fpFootGeo->FromGlobalToTWLocal( partMoth->GetFinalPos() );
	if( (partMoth->GetRegion() < fRegFirstTWbar || partMoth->GetRegion() > fRegLastTWbar) && //particle born outside TW
		TMath::Abs(mothFinPos.Z()) < twGeo->GetBarThick() )                          //but dies inside of TW
	{
		partIDvec->push_back(partID);
		partIDvec->push_back(part->GetMotherID());
		std::vector<int> temp;
		if( CheckRadiativeDecayChain(part->GetMotherID(),&temp) )
			partIDvec->insert(partIDvec->end(),temp.begin(),temp.end()); //merge two vectors

		return true;
	}

	partIDvec->clear();
	return false;
}
