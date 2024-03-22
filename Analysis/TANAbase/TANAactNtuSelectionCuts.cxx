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

//Detectors
#include "TASTntuHit.hxx"
#include "TABMntuTrack.hxx"
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
TANAactNtuSelectionCuts::TANAactNtuSelectionCuts(map<Int_t, map<string, Int_t>> &pTrackCutsMap, map<string, Int_t> &pEventCutsMap,const char *name, Bool_t fFlagMC, TAGdataDsc *pNtuTrack,TAGdataDsc *pNtuHitSt, TAGdataDsc *pNtuTrackBm,TAGdataDsc *pNtuVtx, TAGdataDsc *pNtuRecTw, TTree *p_tree, TAGdataDsc *pNtuMcTrk, TAGdataDsc *pNtuMcReg, TAGparaDsc *pgGeoMap, TAGparaDsc *pgTwGeo)
	: TANAactBaseNtu(name, pNtuTrack, pgGeoMap, p_tree),
	  isMC(fFlagMC),	
	  fpNtuTrack(pNtuTrack),
	  fpNtuHitSt(pNtuHitSt),
	  fpNtuTrackBm(pNtuTrackBm),
	  fpNtuVtx(pNtuVtx),
	  fpNtuRecTw(pNtuRecTw),
	  fpTree(p_tree),
	  fpNtuMcTrk(pNtuMcTrk),
	  fpNtuMcReg(pNtuMcReg),
	  fpGeoMapG(pgGeoMap),
	  fpTwGeo(pgTwGeo),
    fTrackCutsMap(pTrackCutsMap),
    fEventCutsMap(pEventCutsMap)
{

	AddDataIn(pNtuTrack, "TAGntuGlbTrack");
	AddDataIn(pNtuMcTrk, "TAMCntuPart");
	AddDataIn(pNtuMcReg, "TAMCntuRegion");
	AddDataIn(pNtuHitSt, "TASTntuHit");
	AddDataIn(pNtuTrackBm, "TABMntuTrack");
	AddDataIn(pNtuVtx, "TAVTntuVertex");
	AddDataIn(pNtuRecTw, "TATWntuPoint");

	fNtuGlbTrack = (TAGntuGlbTrack *)fpNtuTrack->Object();
	fNtuHitSt = (TASTntuHit *)fpNtuHitSt->Object();
	fNtuTrackBm = (TABMntuTrack *)fpNtuTrackBm->Object();
	fNtuVtx = (TAVTntuVertex *)fpNtuVtx->Object();
	fNtuRecTw = (TATWntuPoint *)fpNtuRecTw->Object();
	fNtuMcTrk = (TAMCntuPart *)fpNtuMcTrk->Object();

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
	fEventCutsMap.clear();
	fTrackCutsMap.clear();
	long evNum = (long)gTAGroot->CurrentEventId().EventNumber(); // number of event
	Int_t nt = fNtuGlbTrack->GetTracksN(); // number of reconstructed tracks for every event
	
	// event cuts
	SCpileUpCut(); // add "SCcut" in event map
	BMCut();  // add "BMcut" in event map
    TwClonesCut(); // add "TWclone" in track map and "TWnum" cut in event map
	NTracksCut(); // add "NTracksCut" in event map

	// track cuts
  for (int it = 0; it < nt; it++)
  { 
	fGlbTrack = fNtuGlbTrack->GetTrack(it);
	VtxPositionCut(it,fGlbTrack); // add "vtPos" cut in track map
	TrackQualityCut(it,fGlbTrack); // add "TrackQuality" cut in track map

	if (isMC){	// MC cuts
	MC_VTMatch(it,fGlbTrack);	// add "MC_VTMatch" cut in track map
	MC_MSDMatch(it,fGlbTrack);	// add "MC_MSDMatch" cut in track map
	MC_TwParticleOrigin(it,fGlbTrack); // add "MC_TwParticleOrigin" cut in track map
	}
  }

PrintCutsMap(fEventCutsMap);  
PrintCutsMap(fTrackCutsMap);
return true;
}

//! \brief Print all the elements, keys and values of the event cuts map
void TANAactNtuSelectionCuts::PrintCutsMap(std::map<string, Int_t> aEventCutsMap)
{
	std::cout << "from SC action Event cuts " << endl;
	for (const auto &[key, value] : aEventCutsMap)
	{
	std::cout << "[" << key << "] = " << value << "; " << endl;
	}
	cout << endl;

}

//! \brief Print all the elements, keys and values of the track cuts map
void TANAactNtuSelectionCuts::PrintCutsMap(std::map<Int_t, std::map<string, Int_t>> aTrackCutsMap)
{	
	std::cout << "from SC action Track cuts "  << endl;
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
		  continue;		  
    }
	nt_TW++;
    TAGpoint *twpoint = fGlbTrack->GetPoint(fGlbTrack->GetPointsN() - 1);
    if ((std::find(vectTwId.begin(), vectTwId.end(), twpoint->GetClusterIdx()) != vectTwId.end())) // if twpoint id is already in the vector
      vecSameTWid.push_back(twpoint->GetClusterIdx());
	vectTwId.push_back(twpoint->GetClusterIdx());
  }

	//TW num cut
	if (fNtuRecTw){
	if (nt_TW == fNtuRecTw->GetPointsN()){
		fEventCutsMap["TWnum"]=1;
	} 
	else
	{
	  fEventCutsMap["TWnum"]=0;
	}
	} else fEventCutsMap["TWnum"]=-99;	


  //TW clone cut
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

	if (allTWid == -1)
	fTrackCutsMap[i]["TWclone"]=-99;

	i++;
  }

}


//! \brief Check if there is pile up in the SC triggering an event
void TANAactNtuSelectionCuts::SCpileUpCut()
{
	if (TAGrecoManager::GetPar()->IsSaveHits()){
	bool ok_status = false;	
    if(fNtuHitSt && fNtuHitSt->GetHitsN()>0){
	  ok_status = true;
      ok_status &= !(fNtuHitSt->GetHit(0)->GetPileUp());       // if there is NOT pileup in the SC
      ok_status &= (fNtuHitSt->GetHit(0)->GetDe() > 0.005); // the energy release should be higher than .005 GeV (energy release of Primary)
  }
	fEventCutsMap["SCcut"]=ok_status;
  } else 
  fEventCutsMap["SCcut"]=-99;
}

//! \brief Check if there is only one track in BM
void TANAactNtuSelectionCuts::BMCut()
{
	if (fNtuTrackBm)
fEventCutsMap["BMcut"] = (fNtuTrackBm->GetTracksN() == 1);
else fEventCutsMap["BMcut"] = -99;
}

//! \brief Check if there is pile up in the SC triggering an event
void TANAactNtuSelectionCuts::NTracksCut()
{
	if ( fNtuGlbTrack->GetTracksN() >1) // number of reconstructed tracks for every event
	fEventCutsMap["NTracksCut"] = 1;
	else fEventCutsMap["NTracksCut"] = 0;
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
//from the name of a track, i take the vtID information 
Int_t vt_ID = stoi(((TObjString *) ((TObjArray *) ((TString(fGlbTrack->GetName()).Tokenize("_"))->At(2))))->String().Data()); 
Int_t iVtx = vt_ID/1000; // vertex id
Int_t iTracklet = vt_ID%1000; // tracklet id of a specific vtx
TAVTvertex* vt = fNtuVtx->GetVertex(iVtx);
if (fNtuVtx){
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
} else fTrackCutsMap[track_id]["VTXposCut"]=-99;
}

//! \brief compute MC VT match
void TANAactNtuSelectionCuts::MC_VTMatch(Int_t track_id, TAGtrack* fGlbTrack)
{	

	vector<vector<Int_t>> vecVtZMC;  // vector of all MC charge of the 4 vt hits
	 for (int i = 0; i < fGlbTrack->GetPointsN(); i++) // for all the points of a track...
  {
    TAGpoint *point = fGlbTrack->GetPoint(i);

    if ((string)point->GetDevName() == "VT")
    {                        //... i take the vt cluster
      vector<Int_t> vecVT_z; // vector of all Z of a cluster of the vtx
      vecVT_z.clear();
      for (int j = 0; j < point->GetMcTracksN(); j++)
      {

        TAMCpart *particleMC = fNtuMcTrk->GetTrack(point->GetMcTrackIdx(j));
        vecVT_z.push_back(particleMC->GetCharge()); // i take all the charges of a cluster
      }
      vecVtZMC.push_back(vecVT_z); // and i put all in a vector
    }
  }

        bool VTMatch = true;
        bool VTZ8Match = true;

        if( vecVtZMC.size() > 0 )
        {
          for (int i = 0; i < vecVtZMC.size(); i++)
          {
            if (std::find(vecVtZMC.at(i).begin(), vecVtZMC.at(i).end(), 8) != vecVtZMC.at(i).end()) // if ALL the clusters of the VTX contains Z=8
              VTZ8Match = VTZ8Match && true;
            else
              VTZ8Match = VTZ8Match && false;
          }


          if (VTZ8Match == true)
          {                   // if a primary entered the first layer of the vtx and did not fragmented up to its last plane...
            if (fGlbTrack->GetTwChargeZ() > 7)   // ... and the charge reconstructed in TW is higher than 7
             fTrackCutsMap[track_id]["MC_VTMatch"]=1; // --> it means there is no fragmentation between VTX and up to TW
            else
             fTrackCutsMap[track_id]["MC_VTMatch"]=0;
          }

          if ( (std::find(vecVtZMC.at(0).begin(), vecVtZMC.at(0).end(), 8) != vecVtZMC.at(0).end()) && (VTZ8Match == false))
          // if the first cluster was a Z=8 but then some fragmentation happened...
          {
            fTrackCutsMap[track_id]["MC_VTMatch"]=0;
          } else if (VTZ8Match == false)
		  { 
			// maybe other fragmentation happened
            fTrackCutsMap[track_id]["MC_VTMatch"]=2;
          }

        }

}


//! \brief compute MC VT match
void TANAactNtuSelectionCuts::MC_MSDMatch(Int_t track_id, TAGtrack* fGlbTrack)
{	
 // compute MC MSD match
        bool MSDMatch = true;
		vector<vector<Int_t>> vecMsdZMC; // vector of all MC charge of the 6 msd clus
		for (int i = 0; i < fGlbTrack->GetPointsN(); i++) // for all the points of a track...
  {
    TAGpoint *point = fGlbTrack->GetPoint(i);
	if ((string)point->GetDevName() == "MSD")
    {
      // cout << " it is a MSD" << endl;
      vector<Int_t> vecMSD_z; // vector of all Z of a cluster of the vtx
      vecMSD_z.clear();
      for (int j = 0; j < point->GetMcTracksN(); j++)
      {

        TAMCpart *particleMC = fNtuMcTrk->GetTrack(point->GetMcTrackIdx(j));
        vecMSD_z.push_back(particleMC->GetCharge());
      }
      vecMsdZMC.push_back(vecMSD_z);
    }


  }

        if( vecMsdZMC.size() > 0 )
        {
          for (int i = 0; i < vecMsdZMC.size(); i++)
          {
            if (std::find(vecMsdZMC.at(i).begin(), vecMsdZMC.at(i).end(), fGlbTrack->GetTwChargeZ()) != vecMsdZMC.at(i).end()) // if all the cluster of the MSD contains Z
              MSDMatch = MSDMatch && true;                                                                  // reconstructed by the TW
            else
              MSDMatch = MSDMatch && false;
          }
        }

		fTrackCutsMap[track_id]["MC_MSDMatch"]=MSDMatch;

}

//! \brief compute MC TW point origin match
void TANAactNtuSelectionCuts::MC_TwParticleOrigin(Int_t track_id, TAGtrack* fGlbTrack)
{
      if (fGlbTrack->HasTwPoint())
    {
      auto twpoint = fGlbTrack->GetPoint(fGlbTrack->GetPointsN() - 1);
      auto initTWPosition = fNtuMcTrk->GetTrack(twpoint->GetMcTrackIdx(0))->GetInitPos().Z();

      if (initTWPosition > -(tgSize.Z() /2 ) && initTWPosition < (tgSize.Z() /2))
    fTrackCutsMap[track_id]["MC_TWOrigin"]=1; // origin in TG
  	else if (initTWPosition < -100 )
	fTrackCutsMap[track_id]["MC_TWOrigin"]=2;   // origin of the beam
    }
	else  
      fTrackCutsMap[track_id]["MC_TWOrigin"]=-99;
    
}