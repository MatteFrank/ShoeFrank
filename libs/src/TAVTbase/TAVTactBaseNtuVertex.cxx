/*!
 \file
 \brief   Implementation of TAVTactBaseNtuVertex.
 */
#include "TClonesArray.h"
#include "TMath.h"
#include "TH2F.h"

//TAG
#include "TAGroot.hxx"
#include "TAGparGeo.hxx"
#include "TAGgeoTrafo.hxx"

//BM
#include "TABMntuTrack.hxx"


// VTX
#include "TAVTparGeo.hxx"
#include "TAVTparConf.hxx"
#include "TAVTtrack.hxx"
#include "TAVTntuCluster.hxx"
#include "TAVTntuVertex.hxx"
#include "TAVTactBaseNtuVertex.hxx"

/*!
 \class TAVTactBaseNtuVertex 
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactBaseNtuVertex);
Bool_t  TAVTactBaseNtuVertex::fgCheckBmMatching = false;
Bool_t  TAVTactBaseNtuVertex::fgCheckPileUp     = false;

//------------------------------------------+-----------------------------------
//! Default constructor.
TAVTactBaseNtuVertex::TAVTactBaseNtuVertex(const char* name, 
                                           TAGdataDsc* pNtuTrack,TAGdataDsc* pNtuVertex,
                                           TAGparaDsc* pConfig,  TAGparaDsc* pGeoMap, TAGparaDsc* pGeoMapG, TAGdataDsc* pBmTrack)
: TAGaction(name, "TAVTactBaseNtuVertex"),
  fpNtuTrack(pNtuTrack),
  fpNtuVertex(pNtuVertex),
  fpConfig(pConfig),
  fpGeoMap(pGeoMap),
  fpGeoMapG(pGeoMapG),
  fpBMntuTrack(pBmTrack)
{
    AddDataIn(pNtuTrack,   "TAVTntuTrack");
    AddDataOut(pNtuVertex, "TAVTntuVertex");
    AddPara(pGeoMap, "TAVTparGeo");
    AddPara(pConfig, "TAVTparConf");
   AddPara(pGeoMapG, "TAGparGeo");

   TAVTparConf* config = (TAVTparConf*) fpConfig->Object();
   fSearchClusDistance = config->GetAnalysisPar().SearchHitDistance;
   fVtxPos.SetXYZ(0,0,0);
   
   TAGparGeo* geoMapG = (TAGparGeo*) fpGeoMapG->Object();
   
   fMinZ = -geoMapG->GetTargetPar().Size[2]*2;
   fMaxZ =  geoMapG->GetTargetPar().Size[2]*2;

   TVector3 posMin(0,0, fMinZ);
   TVector3 posMax(0,0, fMaxZ);
   
   TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   posMin = geoTrafo->FromGlobalToVTLocal(posMin);
   posMax = geoTrafo->FromGlobalToVTLocal(posMax);
   
   fMinZ = posMin[2];
   fMaxZ = posMax[2];
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuVertex::~TAVTactBaseNtuVertex()
{   
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseNtuVertex::CreateHistogram()
{
   DeleteHistogram();   
   TAVTparGeo* pGeoMap   = (TAVTparGeo*) fpGeoMap->Object();
   TAGparGeo* pGeoMapG   = (TAGparGeo*) fpGeoMapG->Object();
   TAGgeoTrafo* pFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   
   Float_t liml = -pGeoMapG->GetTargetPar().Size[2] - pFootGeo->GetVTCenter()[2];
   Float_t limu =  pGeoMapG->GetTargetPar().Size[2] - pFootGeo->GetVTCenter()[2];
   
   fpHisPosZ = new TH1F("vtVtxPosZ", "Vertex position at Z", 100, liml, limu);
   AddHistogram(fpHisPosZ);
   
   TVector3 size(pGeoMap->GetPitchX()*pGeoMap->GetNPixelX(), pGeoMap->GetPitchY()*pGeoMap->GetNPixelY(), 0);
   
   if (fgCheckBmMatching) {
	  fpHisBmMatchX = new TH1F("vtBmMatchX", "Residual vertexing - BM in X", 500, -size[0]/2., size[0]/2.);
	  AddHistogram(fpHisBmMatchX);
   
	  fpHisBmMatchY = new TH1F("vtBmMatchY", "Residual vertexing - BM in Y", 500, -size[1]/2., size[2]/2.);
	  AddHistogram(fpHisBmMatchY);
   }
   
   if (TAVTparConf::IsMapHistOn()) {
	  fpHisPosXY = new TH2F( "vtVtxPosXY", "Vertex position at XY",
                           100, -size[0]/2., size[0]/2.,
                           100, -size[1]/2., size[1]/2.);
	  fpHisPosXY->SetMarkerColor(3);
	  fpHisPosXY->SetStats(kFALSE);
	  AddHistogram(fpHisPosXY);
   }
   
   SetValidHistogram(kTRUE);
}

//_______________________________________________________________________________
//!Action
Bool_t TAVTactBaseNtuVertex::Action()
{
   Bool_t ok = true;
  
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   
   Bool_t check = false;
   
   if (fgCheckPileUp)
     check = CheckPileUp();

   Int_t nTrack = pNtuTrack->GetTracksN();

   if( nTrack == 0) {
        fpNtuVertex->SetBit(kValid);
        return true;
   }

   if (nTrack == 1) {
		 SetNotValidVertex(0);
   } else if (nTrack >= 2 && check) { 
	  for(Int_t q =0; q< nTrack; ++q) 
		 SetNotValidVertex(q);
   } else
	  ok = ComputeVertex();

   if(ok)
	  fpNtuVertex->SetBit(kValid);
   
   if (fgCheckBmMatching)
	  CheckBmMatching();

    return ok;
}


//-------------------------------------------------------------------------------------
//!Check Pile Up
Bool_t TAVTactBaseNtuVertex::CheckPileUp()
{
	   ComputePileUp();
	   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
	   return pNtuTrack->IsPileUp();
}

//-------------------------------------------------------------------------------------
//!Check BM matching
Bool_t TAVTactBaseNtuVertex::CheckBmMatching()
{
   if (!fpBMntuTrack || !fpNtuVertex) return false;
   
   TABMntuTrack* pBMtrack    = (TABMntuTrack*) fpBMntuTrack->Object();
   TAVTntuVertex* pNtuVertex = (TAVTntuVertex*)fpNtuVertex->Object();

   Float_t min = 999999;

   TABMntuTrackTr* bmTrack = pBMtrack->Track(0);
   if (!bmTrack) return false;
   
   TAGgeoTrafo* pFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

   if (!pFootGeo) return false;
	   
   TVector3 bestRes;
   Int_t    bestIdx = -1;
   Bool_t isGood = false;
   for (Int_t i = 0; i < pNtuVertex->GetVertexN(); ++i) {
	  TAVTvertex* vtx      = pNtuVertex->GetVertex(i);
      
      if (!fgCheckBmMatching) {
         vtx->SetBmMatched();
         continue;
      }
      
	  TVector3 vtxPosition = vtx->GetVertexPosition();
	  vtxPosition  = pFootGeo->FromVTLocalToGlobal(vtxPosition);
	  
	  TVector3 bmPosition = pFootGeo->FromGlobalToBMLocal(vtxPosition);
	  vtxPosition *= TAGgeoTrafo::CmToMu();

	  bmPosition  = bmTrack->PointAtLocalZ(bmPosition.Z());
	  bmPosition  = pFootGeo->FromBMLocalToGlobal(bmPosition);
	  bmPosition *= TAGgeoTrafo::CmToMu();
	  
	  TVector3 res = vtxPosition - bmPosition;
	  if (res.Perp() < min) {
		 min = res.Perp();
		 bestRes = res;
		 isGood = true;
		 bestIdx = i;
	  }
   }
   
   if (!isGood) return false;

   if (fgCheckBmMatching) {
	  if (bestIdx != -1) {
		 TAVTvertex* vtx  = pNtuVertex->GetVertex(bestIdx);
		 vtx->SetBmMatched();

        if (ValidHistogram()) {
           fpHisBmMatchX->Fill(bestRes.X());
           fpHisBmMatchY->Fill(bestRes.Y());
        }
     }
   }
   
   return true;
}


//--------------------------------------------------------------
//!Compute the point interaction of diffusion
void TAVTactBaseNtuVertex::ComputeInteractionVertex(TABMntuTrackTr* lbm, TAVTtrack lvtx)
{
   // retrieve trafo
   TAGgeoTrafo* pFirstGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   
   //prendiamo il punto A della retta del bm
   Double_t z = 0;
   Double_t DZ = 1;
   TVector3 Apoint (lbm->PointAtLocalZ(z).X(), lbm->PointAtLocalZ(z).Y(),z); //coordinate del punto A appartenente alla retta del bm in Z = 0
   Apoint  = pFirstGeo->FromBMLocalToGlobal(Apoint);
   Apoint *= TAGgeoTrafo::CmToMu();
   TVector3 Bpoint (lvtx.GetPoint(z).X(),lvtx.GetPoint(z).Y(),z); //coordinate del punto B appartenente alla retta del vtx in Z = 0
   Bpoint  = pFirstGeo->FromVTLocalToGlobal(Bpoint*TAGgeoTrafo::MuToCm());
   Bpoint *= TAGgeoTrafo::CmToMu();
   
   TVector3 AmB = Apoint-Bpoint;
   
   TVector3 pSlopebm(lbm->GetPvers()[0]/lbm->GetPvers()[2], lbm->GetPvers()[1]/lbm->GetPvers()[2], 1);
   TVector3 pDirbm  = pFirstGeo->VecFromBMLocalToGlobal(pSlopebm*DZ); //director parameter of bm line
   TVector3 pDirvtx = pFirstGeo->VecFromVTLocalToGlobal(lvtx.GetSlopeZ()*DZ); //director parameter of vtx line
   
   Double_t etaBm = pDirbm*pDirbm;
   Double_t eta = pDirvtx*pDirvtx;
   Double_t mix = pDirbm*pDirvtx;
   Double_t Apar = AmB*pDirbm;
   Double_t Bpar = AmB*pDirvtx;
   
   
   Double_t q = (Apar*mix-etaBm*Bpar)/(mix*mix - eta*etaBm);
   Double_t p = (-Apar+q*mix)/etaBm;
   
   TVector3 P = Apoint + pDirbm*p;
   TVector3 Q = Bpoint + pDirvtx*q;
   
   fVtxPos = (P+Q)*0.5;
   // Again in local frame of VTX
   fVtxPos = pFirstGeo->FromGlobalToVTLocal(fVtxPos*TAGgeoTrafo::MuToCm());
   fVtxPos *= TAGgeoTrafo::CmToMu();
   
   if(FootDebugLevel(1))
	  fVtxPos.Print();
}

//------------------------------------------------------------------------------------
//!Set vertex not valid
Bool_t TAVTactBaseNtuVertex::SetNotValidVertex(Int_t idTk)
{
   TAVTntuVertex* pNtuVertex = (TAVTntuVertex*)fpNtuVertex->Object();
   //Crete a vertex
   TVector3 vtxPos(0,0,0);
   TAVTvertex* vtx = new TAVTvertex();
   vtx->SetVertexValidity(0);
   
   //Attacchiamo la traccia idTk al vertice 
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   TAVTtrack*    track0    = pNtuTrack->GetTrack(idTk);
   
   vtxPos = track0->Intersection(0);//Target center
   vtx->SetVertexPosition(vtxPos);
   track0->SetValidity(-1);
   track0->SetVertexZ(vtxPos.Z());
   vtx->AddTrack(track0);
   pNtuVertex->NewVertex(*vtx);
   
   delete vtx;
   
   return true;
}

//-----------------------------------------------------------------
//!SetValid Vertex

void TAVTactBaseNtuVertex::SetValidVertex()
{
   TAVTntuVertex* ntuVertex = (TAVTntuVertex*)fpNtuVertex->Object();
   
   TAVTvertex* vtx = new TAVTvertex();
   vtx->SetVertexValidity(1);
   vtx->SetVertexPosition(fVtxPos);
   
   
   TAVTntuTrack* ntuTrack = (TAVTntuTrack*)fpNtuTrack->Object();
   Int_t nTracks = ntuTrack->GetTracksN();
   for(Int_t q = 0; q<nTracks; ++q ){
	  TAVTtrack* track0 = ntuTrack->GetTrack(q);
	  track0->SetValidity(1);
	  track0->SetVertexZ(fVtxPos.Z());
	  vtx->AddTrack(track0);
   }
   
   ntuVertex->NewVertex(*vtx);
   
   delete vtx;
}

//--------------------------------------------
//!compute pileup
void TAVTactBaseNtuVertex::ComputePileUp()
{
   TAVTparGeo*  pGeoMap  = (TAVTparGeo*) fpGeoMap->Object();
   Float_t width         = pGeoMap->GetEpiSize()[0];
   
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   Int_t nTracks = pNtuTrack->GetTracksN();
   Bool_t pileup = false;
   pNtuTrack->SetPileUp(false);
   for (Int_t i = 0; i < nTracks; ++i) {
	  for (Int_t j = i+1; j < nTracks; ++j) {
		 TAVTtrack* track0 = pNtuTrack->GetTrack(i);
		 TAVTtrack* track1 = pNtuTrack->GetTrack(j);
		 TVector2 pos = track0->DistanceMin(track1);
		 if (TMath::Abs(pos.X()) > width/2. || pos.Y() > fSearchClusDistance) {
			track0->SetPileUp();
			track1->SetPileUp();
			pileup = true;
		 }
	  }
   }
   
   if (pileup) 
	  pNtuTrack->SetPileUp();
}
