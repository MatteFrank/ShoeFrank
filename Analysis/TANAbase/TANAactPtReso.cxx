/*!
 \file TANAactPtReso.cxx
 \brief   Implementation of TANAactPtReso
 \author R. Zarrella
 */

#include "TANAactPtReso.hxx"

/*!
 \class TANAactPtReso
 \brief Base class for global reconstruction momentum performance analysis
 */

//------------------------------------------+-----------------------------------
//! \brief Default constructor.
//!
//! \param[in] name action name
//! \param[in] pNtuTrack global track container descriptor
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

//------------------------------------------+-----------------------------------

//! \brief Destructor.
TANAactPtReso::~TANAactPtReso()
{
	ClearData();
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


//! \brief Fill the momentum residual for a track
void TANAactPtReso::FillMomResidual(TAGtrack* track)
{
	if( !track->HasTwPoint() )
		return;

	Int_t mcPart = FindMcParticleAtTgt(track);
	if( mcPart == -1 )
		return;

	TVector3 recoMom = track->GetTgtMomentum();
	TVector3 mcMom = fMcMomMap[mcPart];

	Float_t momBin = GetMomentumBinCenter(recoMom.Mag());
	TString hisName = GetParticleNameFromCharge(track->GetTwChargeZ());
	hisName = hisName + Form("_%.2f",momBin);
	CheckMomentumHistogram(hisName.Data());

	fpHisMomRes[hisName.Data()]->Fill((recoMom.Mag() - mcMom.Mag())/mcMom.Mag());
}


//! \brief Perform preliminary operations before the event loop
void TANAactPtReso::BeginEventLoop()
{
	TAGparGeo* pGeoMapG = (TAGparGeo*) fpGeoMapG->Object();
	TATWparGeo* twGeo = (TATWparGeo*) fpTwGeo->Object();

	// beam loss in target
	fBeamEnergyTarget  = pGeoMapG->GetBeamPar().Energy;
	fBeamA             = pGeoMapG->GetBeamPar().AtomicMass;
	fBeamZ             = pGeoMapG->GetBeamPar().AtomicNumber;

	TString matTgt     = pGeoMapG->GetTargetPar().Material;
	Float_t thickTgt   = pGeoMapG->GetTargetPar().Size[2]/2.; // in average

	//Get some relevant regions
	fRegTgt = pGeoMapG->GetRegTarget();
	fRegAir1 = pGeoMapG->GetRegAirPreTW();
	fRegAir2 = pGeoMapG->GetRegAirTW();
	fRegFirstTWbar = twGeo->GetRegStrip(0, 0);
	fRegLastTWbar = twGeo->GetRegStrip(1, twGeo->GetNBars() - 1);
}


//! \brief Perform analysis after the event loop
void TANAactPtReso::EndEventLoop()
{

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

	for ( auto itMc : fMcMomMap )
	{
		if( std::find(particleID_vec.begin(), particleID_vec.end(), itMc.first) != particleID_vec.end() )
			return itMc.first;
	}

	return -1;
}


//! \brief Check if a momentum histogram exists, create it otherwise
//!
//! \param[in] name Name of the histogram containing particle name and momentum bin center
void TANAactPtReso::CheckMomentumHistogram(std::string name)
{
	if( fpHisMomRes.find(name) != fpHisMomRes.end() )
		return;

	fpHisMomRes[name];
	fpHisMomRes[name] = new TH1D(name.c_str(), Form("%s;(p_{reco} - p_{MC})/p_{MC} at Target;Entries", name.c_str()), 1000, -1, 1);
	AddHistogram(fpHisMomRes[name]);
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
