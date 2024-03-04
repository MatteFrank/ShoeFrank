/*!
 \file TANAactNtuSelectionCuts.cxx
 \brief   Implementation of TANAactNtuSelectionCuts.
 \author G. Ubaldi
 */
#include "TClonesArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"

//TAGroot
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"
#include "TAGnameManager.hxx"

// GLB
#include "TAGntuPoint.hxx"
#include "TAGntuGlbTrack.hxx"

#include "TANAactNtuSelectionCuts.hxx"

/*!
 \class TANAactNtuMass
 \brief Base class for global analysis
 */

//------------------------------------------+-----------------------------------
//! Default constructor.
//!
//! \param[in] name action name
//! \param[out] pNtuTrack global track container descriptor
//! \param[in] pgGeoMap target geometry parameter descriptor
TANAactNtuSelectionCuts::TANAactNtuSelectionCuts(const char *name, TAGdataDsc *pNtuTrack, TTree *p_tree, TAGdataDsc *pNtuMcTrk, TAGdataDsc *pNtuMcReg, TAGparaDsc *pgGeoMap, TAGparaDsc *pgTwGeo)
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

	fNtuGlbTrack = (TAGntuGlbTrack *)fpNtuTrack->Object();

	fpFootGeo = (TAGgeoTrafo *)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
	if (!fpFootGeo)
		Error("TANAactPtReso()", "No GeoTrafo action available yet\n");
	
}

//------------------------------------------+-----------------------------------
//! Destructor.
TANAactNtuSelectionCuts::~TANAactNtuSelectionCuts()
{
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TANAactNtuSelectionCuts::CreateHistogram()
{
}

//_____________________________________________________________________________
//! Action
Bool_t TANAactNtuSelectionCuts::Action()
{
	fTrackCutsMap.clear();
	long evNum = (long)gTAGroot->CurrentEventId().EventNumber();
	Int_t nt = fNtuGlbTrack->GetTracksN(); // number of reconstructed tracks for every event
	
	// event cuts
	// studies concerning tw points
    TwClonesCut(); // add "TWclone" cut in map
	TrackQualityCut(); // add "TrackQuality" cut in map
	
	for (int i = 0; i < nt; i++){
		cout << fTrackCutsMap[i]["TWclone"] << endl;
		cout << fTrackCutsMap[i]["TrackQuality"] << endl;
	}

return true;
}


//! \brief Print all the elements, keys and values of the cuts map
void TANAactNtuSelectionCuts::PrintCutsMap(std::map<Int_t, std::map<string, bool>> aTrackCutsMap)
{
	for (const auto &[key, value] : aTrackCutsMap)
	{
		std::cout << "Element " << key << endl;
		for (const auto &[key2, value2] : value)
			std::cout << "[" << key2 << "] = " << value2 << "; " << endl;
	}
	cout << endl;
}


//! \brief Perform preliminary operations before the event loop
void TANAactNtuSelectionCuts::BeginEventLoop()
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
void TANAactNtuSelectionCuts::EndEventLoop()
{

}

//! \brief Check of all the tracks with the same tw point
void TANAactNtuSelectionCuts::TwClonesCut()
{
  vector<Int_t> vectTwId;         // vector of all the twpoint reco ID
  vector<Int_t> vecSameTWid;      // vector of the twpoint reco ID wich are the same in more than a track
  Int_t nt = fNtuGlbTrack->GetTracksN(); // number of reconstructed tracks for every event

  for (int it = 0; it < nt; it++)
  {
	fGlbTrack = fNtuGlbTrack->GetTrack(it);
	  if (!fGlbTrack->HasTwPoint())
	  {
		  vectTwId.push_back(-1);
		  continue;
    }

    TAGpoint *twpoint = fGlbTrack->GetPoint(fGlbTrack->GetPointsN() - 1);

    if ((std::find(vectTwId.begin(), vectTwId.end(), twpoint->GetClusterIdx()) != vectTwId.end())) // if twpoint id is already in the vector
      vecSameTWid.push_back(twpoint->GetClusterIdx());
    vectTwId.push_back(twpoint->GetClusterIdx());
  }
   
  int i=0; 
  for (auto allTWid : vectTwId)
  {
    if ((std::find(vecSameTWid.begin(), vecSameTWid.end(), allTWid) != vecSameTWid.end())){
	  fTrackCutsMap[i]["TWclone"]=1;
	}
    else
	{
	  fTrackCutsMap[i]["TWclone"]=0;
	}
	i++;
  }
}

//! \brief Cuts about quality chi2 and residual of a track
void TANAactNtuSelectionCuts::TrackQualityCut()
{
  Int_t nt = fNtuGlbTrack->GetTracksN(); // number of reconstructed tracks for every event
  for (int it = 0; it < nt; it++)
  {
	fGlbTrack = fNtuGlbTrack->GetTrack(it);
	// compute chi2 and residuals
      Float_t residual = 0;
      Float_t res_temp = 0;
      for (int i = 0; i < fGlbTrack->GetPointsN(); i++)
      {
        auto point = fGlbTrack->GetPoint(i);
        if ((string)point->GetDevName() == "MSD")
        {
          if (point->GetSensorIdx() % 2 == 0) // if it is the sensor x of the msd
            res_temp = point->GetMeasPosition().X() - point->GetFitPosition().X();
          else
            res_temp = point->GetMeasPosition().Y() - point->GetFitPosition().Y();
        }

        if ((string)point->GetDevName() == "VT")
        {
          res_temp = (point->GetMeasPosition() - point->GetFitPosition()).Mag();
        }
        if (abs(res_temp) > abs(residual) )
          residual = res_temp;
      }
	 // cout << abs(residual) << endl;
      bool residualCut = true;
      // bool residualCut = abs(residual) < 0.01;
      // bool chi2Cut = fGlbTrack->GetChi2() < 2;
      bool chi2Cut = fGlbTrack->GetPval() > 0.01;
	  if (residualCut && chi2Cut ) fTrackCutsMap[it]["TrackQuality"]=1;
	  else fTrackCutsMap[it]["TrackQuality"]=0;

}
}