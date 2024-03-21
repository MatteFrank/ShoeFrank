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
#include "TObjString.h"

//TAGroot
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGrecoManager.hxx"
#include "TAGnameManager.hxx"
#include "TAVTntuVertex.hxx"

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
TANAactNtuSelectionCuts::TANAactNtuSelectionCuts(const char *name, TAGdataDsc *pNtuTrack,TAGdataDsc *pNtuVtx, TAGdataDsc *pNtuRecTw, TTree *p_tree, TAGdataDsc *pNtuMcTrk, TAGdataDsc *pNtuMcReg, TAGparaDsc *pgGeoMap, TAGparaDsc *pgTwGeo)
	: TANAactBaseNtu(name, pNtuTrack, pgGeoMap, p_tree),
	  fpNtuTrack(pNtuTrack),
	  fpNtuVtx(pNtuVtx),
	  fpNtuRecTw(pNtuRecTw),
	  fpTree(p_tree),
	  fpNtuMcTrk(pNtuMcTrk),
	  fpNtuMcReg(pNtuMcReg),
	  fpGeoMapG(pgGeoMap),
	  fpTwGeo(pgTwGeo)
{

	AddDataIn(pNtuTrack, "TAGntuGlbTrack");
	AddDataIn(pNtuMcTrk, "TAMCntuPart");
	AddDataIn(pNtuMcReg, "TAMCntuRegion");
	AddDataIn(pNtuVtx, "TAVTntuVertex");
	AddDataIn(pNtuRecTw, "TATWntuPoint");


	fNtuGlbTrack = (TAGntuGlbTrack *)fpNtuTrack->Object();
	fNtuVtx = (TAVTntuVertex *)fpNtuVtx->Object();
	fNtuRecTw = (TATWntuPoint *)fpNtuRecTw->Object();

	fpFootGeo = (TAGgeoTrafo *)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
	if (!fpFootGeo)
		Error("TANAactNtuSelectionCuts()", "No GeoTrafo action available yet\n");
	
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

	// track cuts
  for (int it = 0; it < nt; it++)
  { 
	fGlbTrack = fNtuGlbTrack->GetTrack(it);
	TrackQualityCut(it,fGlbTrack); // add "TrackQuality" cut in map
	VtxPositionCut(it,fGlbTrack); // add "vtPos" cut in map
	//TwPointCut(it,fGlbTrack);// add "twCut" cut in map
  }
PrintCutsMap(fTrackCutsMap);
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
	tgSize = pGeoMapG->GetTargetPar().Size;
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
  Int_t nt_TW = 0; // number of reconstructed tracks with TW point for every event

  for (int it = 0; it < nt; it++)
  {
	fGlbTrack = fNtuGlbTrack->GetTrack(it);
	  if (!fGlbTrack->HasTwPoint())
	  {
		  vectTwId.push_back(-1);
		  nt_TW++;
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

	if (nt_TW == fNtuRecTw->GetPointsN()){
		fTrackCutsMap[i]["TWnum"]=1;
	} 
	else
	{
	  fTrackCutsMap[i]["TWnum"]=0;
	}

	i++;
  }

}

//! \brief Cuts about quality chi2 and residual of a track
void TANAactNtuSelectionCuts::TrackQualityCut(Int_t track_id, TAGtrack* fGlbTrack)
{
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
	  if (residualCut && chi2Cut ) fTrackCutsMap[track_id]["TrackQuality"]=1;
	  else fTrackCutsMap[track_id]["TrackQuality"]=0;
}


//! \brief Cuts about vtx position with the target dimension
void TANAactNtuSelectionCuts::VtxPositionCut(Int_t track_id,TAGtrack* fGlbTrack)
{
//TString name = fGlbTrack->GetName();
//TObjArray *tx = name.Tokenize("_");

//from the name of a track, i take the vtID information 
Int_t vt_ID = stoi(((TObjString *) ((TObjArray *) ((TString(fGlbTrack->GetName()).Tokenize("_"))->At(2))))->String().Data()); 
Int_t iVtx = vt_ID/1000; // vertex id
Int_t iTracklet = vt_ID%1000; // tracklet id of a specific vtx
TAVTvertex* vt = fNtuVtx->GetVertex(iVtx);
 if( vt->IsBmMatched() ) {
	TVector3 vtPos = fpFootGeo->FromGlobalToTGLocal(fpFootGeo->FromVTLocalToGlobal(vt->GetPosition()));          // vertex position in TG frame
    TVector3 vtPosErr = fpFootGeo->VecFromGlobalToTGLocal(fpFootGeo->VecFromVTLocalToGlobal(vt->GetPosError())); // vertex position error in TG frame
    if (TMath::Abs(vtPos.X()) < tgSize.X() / 2 + vtPosErr.X() && TMath::Abs(vtPos.Y()) < tgSize.Y() / 2 + vtPosErr.Y() && TMath::Abs(vtPos.Z()) < tgSize.Z() / 2 + vtPosErr.Z())
    {
      fTrackCutsMap[track_id]["VTXposCut"]=1;
    }
	else
 	fTrackCutsMap[track_id]["VTXposCut"]=0;
 
 } else
 fTrackCutsMap[track_id]["VTXposCut"]=0;
 



}

