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
#include "TADItrackEmProperties.hxx"
#include "TAVTactBaseNtuVertex.hxx"

/*!
 \class TAVTactBaseNtuVertex
 \brief NTuplizer for vertex raw hits. **
 */

ClassImp(TAVTactBaseNtuVertex);
Bool_t  TAVTactBaseNtuVertex::fgCheckBmMatching = true;

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
  fpBMntuTrack(pBmTrack),
  fEmProp(new TADItrackEmProperties())
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

   fMinZ = -geoMapG->GetTargetPar().Size[2]/2.;
   fMaxZ =  geoMapG->GetTargetPar().Size[2]/2.;
   fEps = 1e-4; // 1 microns precision

   TVector3 posMin(0,0, fMinZ);
   TVector3 posMax(0,0, fMaxZ);

   fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   posMin = fpFootGeo->FromGlobalToVTLocal(posMin);
   posMax = fpFootGeo->FromGlobalToVTLocal(posMax);

   fMinZ = posMin[2];
   fMaxZ = posMax[2];

   // scattering angle in rad (at 2 sigma)
   fScatterAng = ComputeScatterAngle()*2.;
}

//------------------------------------------+-----------------------------------
//! Destructor.
TAVTactBaseNtuVertex::~TAVTactBaseNtuVertex()
{
   delete fEmProp;
}

//------------------------------------------+-----------------------------------
//! Setup all histograms.
void TAVTactBaseNtuVertex::CreateHistogram()
{
   DeleteHistogram();
   TAVTparGeo* pGeoMap   = (TAVTparGeo*) fpGeoMap->Object();
   TAGparGeo* pGeoMapG   = (TAGparGeo*) fpGeoMapG->Object();


   Float_t liml = -pGeoMapG->GetTargetPar().Size[2] - fpFootGeo->GetVTCenter()[2];
   Float_t limu =  pGeoMapG->GetTargetPar().Size[2] - fpFootGeo->GetVTCenter()[2];

   fpHisPosZ = new TH1F("vtVtxPosZ", "Vertex position at Z", 100, liml, limu);
   AddHistogram(fpHisPosZ);

   TVector3 size(pGeoMap->GetPitchX()*pGeoMap->GetPixelsNx(), pGeoMap->GetPitchY()*pGeoMap->GetPixelsNy(), 0);

   if (fgCheckBmMatching) {
	  fpHisBmMatchX = new TH1F("vtBmMatchX", "Residual vertexing - BM in X", 500, -size[0]/2., size[0]/2.);
	  AddHistogram(fpHisBmMatchX);

	  fpHisBmMatchY = new TH1F("vtBmMatchY", "Residual vertexing - BM in Y", 500, -size[1]/2., size[1]/2.);
	  AddHistogram(fpHisBmMatchY);
   }

   fpHisPosXY = new TH2F( "vtVtxPosXY", "Vertex position at XY",
                           100, -size[0]/2., size[0]/2.,
                           100, -size[1]/2., size[1]/2.);
   fpHisPosXY->SetMarkerColor(3);
   fpHisPosXY->SetStats(kFALSE);
   AddHistogram(fpHisPosXY);


   SetValidHistogram(kTRUE);
}

//_______________________________________________________________________________
//!Action
Bool_t TAVTactBaseNtuVertex::Action()
{
   Bool_t ok = true;

   TAVTntuTrack*  pNtuTrack  = (TAVTntuTrack*) fpNtuTrack->Object();
   TAVTntuVertex* pNtuVertex = (TAVTntuVertex*)fpNtuVertex->Object();

   Int_t nTrack = pNtuTrack->GetTracksN();

   if( nTrack == 0) {
      fpNtuVertex->SetBit(kValid);
      return true;
   }

   if (nTrack == 1) {
		 SetNotValidVertex(0);

   } else
	  ok = ComputeVertex();

   if(ok)
	  fpNtuVertex->SetBit(kValid);

   if (fgCheckBmMatching)
	  CheckBmMatching();
   
   else {
      for (Int_t i = 0; i < pNtuVertex->GetVertexN(); ++i) {
         TAVTvertex* vtx  = pNtuVertex->GetVertex(i);
         vtx->SetBmMatched();
      }
   }

    return ok;
}

//-------------------------------------------------------------------------------------
//!Check BM matching
Bool_t TAVTactBaseNtuVertex::CheckBmMatching()
{
   if (!fpBMntuTrack || !fpNtuVertex) return false;

   TABMntuTrack* pBMtrack    = (TABMntuTrack*) fpBMntuTrack->Object();
   if(pBMtrack->GetTracksN() != 1) return false;

   TAVTntuVertex* pNtuVertex = (TAVTntuVertex*)fpNtuVertex->Object();
   TAVTbaseParConf* config   = (TAVTbaseParConf*) fpConfig->Object();

   TVector3 minRes(0.,1.,1.);
   Int_t bestVtxIndex        = -1;

   TABMtrack* bmTrack = pBMtrack->GetTrack(0);
   if (!bmTrack) return false;

   if (!fpFootGeo) return false;

   for (Int_t i = 0; i < pNtuVertex->GetVertexN(); ++i) {
	  TAVTvertex* vtx = pNtuVertex->GetVertex(i);

	  TVector3 vtxPosition = vtx->GetPosition();
	  vtxPosition          = fpFootGeo->FromVTLocalToGlobal(vtxPosition);
	  TVector3 bmPosition  = fpFootGeo->FromGlobalToBMLocal(vtxPosition);

	  bmPosition   = bmTrack->PointAtLocalZ(bmPosition.Z());
	  bmPosition   = fpFootGeo->FromBMLocalToGlobal(bmPosition);
	  TVector3 res = vtxPosition - bmPosition;

	  if (res.Perp() < minRes.Perp()) {
      bestVtxIndex=i;
      minRes=res;
	  }
   }

   if(bestVtxIndex>=0){
      TAVTvertex* vtx = pNtuVertex->GetVertex(bestVtxIndex);
      vtx->SetBmMatched();
      if (ValidHistogram()) {
         fpHisBmMatchX->Fill(minRes.X());
         fpHisBmMatchY->Fill(minRes.Y());
      }
   }

   return true;
}


//--------------------------------------------------------------
//!Compute the point interaction of diffusion (not used anymore)
void TAVTactBaseNtuVertex::ComputeInteractionVertex(TABMtrack* lbm, TAVTtrack lvtx)
{
   //taking point A of the straight line of bm
   Double_t z = 0;
   Double_t DZ = 1;
   TVector3 Apoint (lbm->PointAtLocalZ(z).X(), lbm->PointAtLocalZ(z).Y(),z); //coordinates of point A belonging to the straight line of bm in Z = 0
   Apoint  = fpFootGeo->FromBMLocalToGlobal(Apoint);

   TVector3 Bpoint (lvtx.GetPoint(z).X(),lvtx.GetPoint(z).Y(),z); //coordinate of point B belonging to the straight line of vtx in Z = 0
   Bpoint  = fpFootGeo->FromVTLocalToGlobal(Bpoint);

   TVector3 AmB = Apoint-Bpoint;

   TVector3 pSlopebm(lbm->GetSlope()[0]/lbm->GetSlope()[2], lbm->GetSlope()[1]/lbm->GetSlope()[2], 1);
   TVector3 pDirbm  = fpFootGeo->VecFromBMLocalToGlobal(pSlopebm*DZ); //director parameter of bm line
   TVector3 pDirvtx = fpFootGeo->VecFromVTLocalToGlobal(lvtx.GetSlopeZ()*DZ); //director parameter of vtx line

   Double_t etaBm = pDirbm*pDirbm;
   Double_t eta   = pDirvtx*pDirvtx;
   Double_t mix   = pDirbm*pDirvtx;
   Double_t Apar  = AmB*pDirbm;
   Double_t Bpar  = AmB*pDirvtx;

   Double_t q = (Apar*mix-etaBm*Bpar)/(mix*mix - eta*etaBm);
   Double_t p = (-Apar+q*mix)/etaBm;

   TVector3 P = Apoint + pDirbm*p;
   TVector3 Q = Bpoint + pDirvtx*q;

   fVtxPos = (P+Q)*0.5;
   // Again in local frame of VTX
   fVtxPos = fpFootGeo->FromGlobalToVTLocal(fVtxPos);

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
   TVector3 tgtPos(0,0,0);
   TAVTvertex* vtx = new TAVTvertex();

   //Attached track idTk to vertex
   TAVTntuTrack* pNtuTrack = (TAVTntuTrack*) fpNtuTrack->Object();
   TAVTtrack*    track0    = pNtuTrack->GetTrack(idTk);

   tgtPos = fpFootGeo->FromGlobalToVTLocal(tgtPos);

   vtxPos = track0->Intersection(tgtPos.Z());//Target center
   vtx->SetVertexPosition(vtxPos);

   Int_t nucReacFlag = SearchNucReac(track0);
   track0->SetValidity(nucReacFlag);
   vtx->SetVertexValidity(nucReacFlag);

   track0->SetPosVertex(vtxPos);
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
	  track0->SetPosVertex(fVtxPos);
	  vtx->AddTrack(track0);
   }

   ntuVertex->NewVertex(*vtx);

   delete vtx;
}

//--------------------------------------------
//!compute scattering angle after target
Double_t TAVTactBaseNtuVertex::ComputeScatterAngle()
{
   TAGparGeo* geoMapG = (TAGparGeo*) fpGeoMapG->Object();

   TString matTarget  = geoMapG->GetTargetPar().Material;
   Float_t depth      = geoMapG->GetTargetPar().Size[2];

   TString matBeam   = geoMapG->GetBeamPar().Material;
   Float_t enBeam    = geoMapG->GetBeamPar().Energy * geoMapG->GetBeamPar().AtomicMass;

   return fEmProp->GetSigmaTheta(matTarget, depth, enBeam, matBeam);
}

//--------------------------------------------
//! Check scattering of track
Int_t TAVTactBaseNtuVertex::SearchNucReac(TAVTtrack* track0)
{
   // returns -1 no BM, 2 nuclear reaction, 0, diffusion
   TABMntuTrack* pBMntuTrack = 0x0;
   TVector3 lineBM(0,0,0);
   Int_t nTrackBM = 0;

   if (fpBMntuTrack) {
      pBMntuTrack = (TABMntuTrack*) fpBMntuTrack->Object();
      nTrackBM    = pBMntuTrack->GetTracksN();

      if(nTrackBM > 0)
         lineBM = pBMntuTrack->GetTrack(0)->GetSlope();

   } else {
      return -1;
   }

   Int_t result = 0;

   // retrieve trafo
   TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
   TVector3 direction = lineBM.Unit();
   direction = fpFootGeo->VecFromBMLocalToGlobal(direction);
   Double_t angleBm   = direction.Theta();

   //Track from vtx
   direction = track0->GetSlopeZ().Unit();
   direction = fpFootGeo->VecFromVTLocalToGlobal(direction);
   Double_t angleTk = direction.Theta();


   if(TMath::Abs(angleBm-angleTk) > fScatterAng)
      result = 2; //No diffusion
   else
      result = 0;

   return result;
}
