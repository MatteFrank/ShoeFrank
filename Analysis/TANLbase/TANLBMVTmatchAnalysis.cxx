#include "TANLBMVTmatchAnalysis.hxx"
#include "TH1D.h"
#include "TH2D.h"

TANLBMVTmatchAnalysis::TANLBMVTmatchAnalysis(){
}


TANLBMVTmatchAnalysis::~TANLBMVTmatchAnalysis(){
}


  //
void TANLBMVTmatchAnalysis::Setup(){
}


void TANLBMVTmatchAnalysis::BeforeEventLoop(){
 
  gDirectory->mkdir("VT");
  gDirectory->cd("VT");

  gDirectory->mkdir("vtxmatched");
  gDirectory->cd("vtxmatched");
  h1v[nvtx] = new TH1D("VT_NTracks",
			  "Number of VT tracks in each event where BMtracks=1; # of VT tracks; Events",
		       21,-0.5,20.5);
  h1v[allDistances] = new TH1D("VT_ALLMatchDist","All BM-VT tracks distance; distance [cm]; Entries",100, 0.,2.);
  h1v[matchDistance] = new TH1D("VT_MatchDist","Match BM-VT tracks distance; distance [cm]; Entries",100, 0.,1.);
  h1v[sensors] = new TH1D("VT_NSENS","Match BM-VT: selected VT track; # planes ; Entries",5, -.5, 4.5);
  h1v[xdiff] = new TH1D("VT_BM_X","Match BM-VT: Xbm-Xvt; BM-VT x difference [cm]; Entries",200, -1.,1.);
  h1v[ydiff] = new TH1D("VT_BM_Y","Match BM-VT: Ybm-Yvt; BM-VT y difference [cm]; Entries",200, -1.,1.);
  h1v[theta] = new TH1D("VT_BM_theta","Match BM-VT: theta angle; tracklet opening angle [rad]; Entries",500, 0,0.05);
  h2v[h2match] = new TH2D("VT_BM_XY","Match BM-VT; BM-VT x difference [cm]; BM-VT y difference",100, -1.,1., 100, -1.,1.);
  h1v[chi2xvt] = new TH1D("VT_Chi2_X","VT selected track; Chisq X; Entries",2000, 0.,100.);
  h1v[chi2yvt] = new TH1D("VT_Chi2_Y","VT selected track; Chisq Y; Entries",2000, 0.,100.);
  h2v[resvstx] = new TH2D("VT_BM_resvsTx",
		       "Match BM-VT Residuals vs tx; tx BM; BM-VT x difference",
		       100, -0.01,0.01, 100, -0.5,0.5);
  h2v[resvsty] = new TH2D("VT_BM_resvsTy",
		       "Match BM-VT Residuals vs ty; ty BM; BM-VT y difference",
		       100, -0.01,0.01, 100, -0.5,0.5);

  gDirectory->cd("..");
}


void TANLBMVTmatchAnalysis::ProcessEvent(){
    
  TAGgeoTrafo* fpFootGeo = (TAGgeoTrafo*)
    gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

  // strong requirements for matching
  if( myBMNtuTrk->GetTracksN()!=1 ) return;

  //skip empty or too crowded events
  TAVTntuVertex *vertexContainer = (TAVTntuVertex*)fpNtuVtx->GenerateObject();
  int vertexNumber = vertexContainer->GetVertexN();
  TAVTvertex* vtxPD   = 0x0; //NEW
  int nvtxtracks=0;
  for (Int_t iVtx = 0; iVtx < vertexNumber; ++iVtx) { // for every vertexEvent
    vtxPD = vertexContainer->GetVertex(iVtx);
    nvtxtracks += vtxPD->GetTracksN();
  }

  h1v[nvtx]->Fill(nvtxtracks);

  if( nvtxtracks<1 || nvtxtracks>3 ) return;

  TABMtrack* bmTrack = myBMNtuTrk->GetTrack(0);
  TVector3 bmPos   = fpFootGeo->FromBMLocalToGlobal( bmTrack->GetOrigin() );
  TVector3 bmSlope = fpFootGeo->VecFromBMLocalToGlobal( bmTrack->GetSlope() );
  bmSlope *= 1./bmSlope.Z();

  Double_t zTgt = fpFootGeo->GetTGCenter().Z();
  
  TVector3 bmTrkg = extrapolate( zTgt, bmPos, bmSlope);


  TAVTtrack* bestMatchTracklet = 0;
  Double_t bestDistancesq = 100000000.0;
  Double_t distancesq = 0.0;
  Double_t xBM = bmTrkg.X();
  Double_t yBM = bmTrkg.Y();
  Double_t zBM = bmTrkg.Z();
  for (Int_t iVtx = 0; iVtx < vertexNumber; ++iVtx) { // for every vertexEvent
    vtxPD = vertexContainer->GetVertex(iVtx);
    for (int iTrack = 0; iTrack < vtxPD->GetTracksN(); iTrack++) {  //for every tracklet
      TAVTtrack* tracklet = vtxPD->GetTrack( iTrack );
      
      TVector3 vtPos   = fpFootGeo->FromVTLocalToGlobal( tracklet->GetOrigin() );
      TVector3 vtSlope = fpFootGeo->VecFromVTLocalToGlobal( tracklet->GetSlopeZ() );

      TVector3 vtTrkg = extrapolate( zTgt, vtPos, vtSlope );
      Double_t xVTX = vtTrkg.X();
      Double_t yVTX = vtTrkg.Y();
      distancesq = pow(xBM-xVTX,2)+pow(yBM-yVTX,2);
      if( distancesq<bestDistancesq ){
	bestDistancesq = distancesq;
	bestMatchTracklet = tracklet;
      }
      h1v[allDistances]->Fill(TMath::Sqrt(distancesq));
    }
  }
  h1v[matchDistance]->Fill(TMath::Sqrt(bestDistancesq));



  // here with the best BM-VTX match
  TVector3 vtPos   = fpFootGeo->FromVTLocalToGlobal( bestMatchTracklet->GetOrigin() );
  TVector3 vtSlope = fpFootGeo->VecFromVTLocalToGlobal( bestMatchTracklet->GetSlopeZ() );
  
  TVector3 vtTrkg = extrapolate( zTgt, vtPos, vtSlope );

  Double_t xVTX = vtTrkg.X();
  Double_t yVTX = vtTrkg.Y();
  Double_t zVTX = vtTrkg.Z();
  Double_t angle= bmSlope.Angle(vtSlope);
  h1v[sensors]->Fill( bestMatchTracklet->GetClustersN() );
  h1v[xdiff]->Fill(xBM-xVTX);
  h1v[ydiff]->Fill(yBM-yVTX);
  h1v[theta]->Fill(angle);
  Double_t ndf = bestMatchTracklet->GetClustersN()-2.;
  h1v[chi2xvt]->Fill( bestMatchTracklet->GetChi2U()*ndf );
  h1v[chi2yvt]->Fill( bestMatchTracklet->GetChi2V()*ndf );
  h2v[resvstx]->Fill( bmTrack->GetSlope().X(), xBM-xVTX);
  h2v[resvsty]->Fill( bmTrack->GetSlope().Y(), yBM-yVTX);
  h2v[h2match]->Fill(xBM-xVTX, yBM-yVTX);

}

void TANLBMVTmatchAnalysis::AfterEventLoop(){
}

TVector3 TANLBMVTmatch::extrapolate(Double_t z, 
				   const TVector3 & pos, 
				   const TVector3 & dir) const {

  TVector3 result = pos + dir*(z-pos.Z())*(1./dir.Z());
  return result;
}
