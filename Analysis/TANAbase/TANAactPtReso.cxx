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
TANAactPtReso::TANAactPtReso(const char* name, TAGdataDsc* pNtuTrack, TAGdataDsc* pNtuMcTrk, TAGdataDsc* pNtuMcReg, TAGparaDsc* pgGeoMap, TAGparaDsc* pgTwGeo)
: TANAactBaseNtu(name, pNtuTrack, pgGeoMap),
fpNtuMcTrk(pNtuMcTrk),
fpNtuMcReg(pNtuMcReg),
fpTwGeo(pgTwGeo)
{
	AddDataIn(pNtuTrack, "TAGntuGlbTrack");
	AddDataIn(pNtuMcTrk, "TAMCntuPart");
	AddDataIn(pNtuMcReg, "TAMCntuRegion");

	fNtuGlbTrack = (TAGntuGlbTrack*)fpNtuTrack->Object();

	fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
	if (!fpFootGeo)
		Error("TANAactPtReso()", "No GeoTrafo action available yet\n");
	
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

//------------------------------------------+-----------------------------------

//! \brief Destructor.
TANAactPtReso::~TANAactPtReso()
{
	ClearData();
}


//! \brief Create the histograms for momentum resolution
void TANAactPtReso::CreateHistogram()
{

}


//! \brief Action for momentum resolution
Bool_t TANAactPtReso::Action()
{
	//Skip event if empty
	if( fNtuGlbTrack->GetTracksN() < 1 )
		return true;

	ClearData();
	GetMcMomentaAtTgt();

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

}


//! \brief Perform analysis after the event loop
void TANAactPtReso::Finalize()
{

}


//! \brief Get the MC momentum of all particles crossing the TG
void TANAactPtReso::GetMcMomentaAtTgt()
{

}


//! \brief Clear all internal data structures for new loop
void TANAactPtReso::ClearData()
{
	fMcMomMap.clear();
}