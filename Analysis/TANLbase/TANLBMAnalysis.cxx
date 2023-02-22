#include "TANLBMAnalysis.hxx"
#include "TH1D.h"
#include "TH2D.h"

TANLBMAnalysis::TANLBMAnalysis(){
}


TANLBMAnalysis::~TANLBMAnalysis(){
}


  //
void TANLBMAnalysis::Setup(){
gTAGroot->FindDataDsc(GetName())
}


void TANLBMAnalysis::BeforeEventLoop(){
  Setup();
  Booking();
}


void TANLBMAnalysis::Booking(){

  gDirectory->mkdir("BM");
  gDirectory->cd("BM");

  h1v[ntracks] = new TH1D("BM_NTracks",
			  "Number of BM track in each event; # of BM tracks; Events",
			  21,-0.5,20.5);
  h1v[chix] = new TH1D("BM_chix",
		       "Chisquare of the X projection; chisquare (X); entries",
		       101,-0.5,100.5);
  h1v[chiy] = new TH1D("BM_chiy",
		       "Chisquare of the Y projection; chisquare (Y); entries",
		       101,-0.5,100.5);
  h1v[nx] = new TH1D("BM_nx",
		     "Number of BM measurements in the X projection; Number of X hits in BM; entries",
		       13,-0.5,12.5);
  h1v[ny] = new TH1D("BM_ny",
		     "Number of BM measurements in the Y projection; Number of Y hits in BM; entries",
		       13,-0.5,12.5);
  h1v[probx] = new TH1D("BM_probx",
		     "BM: Chisq probability of the X projection; X chisq probability in BM; entries",
		       51,-0.01,1.01);
  h1v[proby] = new TH1D("BM_proby",
		     "BM: Chisq probability of the Y projection; Y chisq probability in BM; entries",
		       51,-0.01,1.01);
  h1v[prob3] = new TH1D("BM_prob3",
		     "BM: Chisq probability for 3 hits; chisq probability in BM for 3 hits (in x or y); entries",
		       51,-0.01,1.01);
  h1v[prob4] = new TH1D("BM_prob4",
		     "BM: Chisq probability for 4 hits; chisq probability in BM for 4 hits (in x or y); entries",
		       51,-0.01,1.01);
  h1v[prob5] = new TH1D("BM_prob5",
		     "BM: Chisq probability for 5 hits; chisq probability in BM for 5 hits (in x or y); entries",
		       51,-0.01,1.01);
  h1v[prob6] = new TH1D("BM_prob6",
		     "BM: Chisq probability for 6 hits; chisq probability in BM for 6 hits (in x or y); entries",
		       51,-0.01,1.01);

  //////////////////////////


  gDirectory->mkdir("all-events");
  gDirectory->cd("all-events");
  h2v[onTargetLocal = new TH2D("bm_target_bmsys","BM tracks on target  projections in BM sys ;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
  h2v[onTargetGlb] = new TH2D("bm_target_glbsys","BM tracks on target projections in GLB sys;X[cm];Y[cm]",600,-3.,3., 600, -3., 3);
  h1v[XposGlb] = new TH1D("bm_target_Xpos_glbsys","BM tracks X pos on target projections in GLB sys (other method);X[cm]",600,-3.,3.);
  h1v[XposGlb] = new TH1D("bm_target_Ypos_glbsys","BM tracks Y pos on target projections in GLB sys (other method);Y[cm]",600,-3.,3.);
  h1v[ThetaLoc] = new TH1D("bm_target_bmsys_theta","theta of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
  h1v[ThetaGlb] = new TH1D("bm_target_glbsys_theta","theta of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
  h1v[PhiLoc] = new TH1D("bm_target_bmsys_phi","phi of tracks on target  projections in local system; deg; counts",100, 0 ,50.);
  h1v[PhiGlb] = new TH1D("bm_target_glbsys_phi","phi of tracks on target  projections in global system; deg; counts",100, 0 ,50.);
  gDirectory->cd("..");
}


void TANLBMAnalysis::ProcessEvent(){
  //BM track position study
  TH1D* h1;
  TH2D* h2;

  h1v[ntracks]->Fill(myBMNtuTrk->GetTracksN());

  for( Int_t iTrack = 0; iTrack < myBMNtuTrk->GetTracksN(); ++iTrack ) {
    TABMtrack* track = myBMNtuTrk->GetTrack(iTrack);
    int nhx = track->GetHitsNx();
    int nhy = track->GetHitsNy();
    Double_t chi2x = track->GetChiSquareX()*(nhx-2);
    Double_t chi2y = track->GetChiSquareY()*(nhy-2);
    h1v[chix]->Fill(chi2x );
    h1v[chiy]->Fill(chi2y );
    h1v[nx]->Fill( nhx );
    h1v[ny]->Fill( nhy );
    Double_t probxval = TMath::Prob( chi2x, nhx-2);
    Double_t probyval = TMath::Prob( chi2y, nhy-2);
    h1v[probx]->Fill( probxval );
    h1v[proby]->Fill( probyval );
    if( nhx==3 )       h1v[prob3]->Fill(probxval);
    else if( nhx==4 )  h1v[prob4]->Fill(probxval);
    else if( nhx==5 )  h1v[prob5]->Fill(probxval);
    else if( nhx==6 )  h1v[prob6]->Fill(probxval);
    if( nhy==3 )       h1v[prob3]->Fill(probyval);
    else if( nhy==4 )  h1v[prob4]->Fill(probyval);
    else if( nhy==5 )  h1v[prob5]->Fill(probyval);
    else if( nhy==6 )  h1v[prob6]->Fill(probyval);
  }


  if (myBMNtuTrk->GetTracksN() == 1){  // select only events with 1 bm track
    for( Int_t iTrack = 0; iTrack < myBMNtuTrk->GetTracksN(); ++iTrack ) {
      TABMtrack* track = myBMNtuTrk->GetTrack(iTrack);
      //project to the target in the BM ref., then move to the global ref.
      TVector3 bmlocalproj=ProjectToZ(track->GetSlope(), track->GetOrigin(),
				      fpFootGeo->FromGlobalToBMLocal(fpFootGeo->GetTGCenter()).Z());
      TVector3 bmgloproj=ProjectToZ(fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope()), 
				    fpFootGeo->FromBMLocalToGlobal(track->GetOrigin()),
				    fpFootGeo->GetTGCenter().Z());

      h2v[onTargetLocal]->Fill(bmlocalproj.X(),bmlocalproj.Y());
      h2v[onTargetGlb]->Fill(bmgloproj.X(),bmgloproj.Y());
      h1v[XposGlb]->Fill(bmgloproj.X());
      h1v[YposGlb]->Fill(bmgloproj.Y());
      h1v[ThetaLoc]-> Fill( track->GetSlope().Theta()*TMath::RadToDeg() );
      h1v[PhiLoc]-> Fill( track->GetSlope().Phi()*TMath::RadToDeg() );

      auto globalSlope = (fpFootGeo->VecFromBMLocalToGlobal(track->GetSlope()));
      h1v[ThetaGlb]-> Fill( globalSlope.Theta()*TMath::RadToDeg() );
      h1v[PhiGlb]-> Fill( globalSlope.Phi()*TMath::RadToDeg() );
      
    }

    // only for single track events. 
    // check interesting hits...
    
  }

}

void TANLBMAnalysis::AfterEventLoop(){
    }
