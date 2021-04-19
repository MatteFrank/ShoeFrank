

#include "GlobalTrackingStudies.hxx"


GlobalTrackingStudies::GlobalTrackingStudies(const char* name)
: TAGaction(name, "GlobalTrackingStudies - Global GenFit Tracking Studies")
{

	m_kalmanOutputDir = (string)getenv("FOOTRES")+"/Kalman";
	// for (auto i=1; i<9; i++)	m_possibleCharges.push_back( i );
	gStyle->SetPalette(1);

	m_moreThanOnePartPerCluster = 0;
	m_allNoiseTrack = 0;
}

Bool_t GlobalTrackingStudies::Action() {

	TAMCntuPart*  eve = (TAMCntuPart*)   gTAGroot->FindDataDsc("eveMc", "TAMCntuPart")->Object();
	TAGgeoTrafo* geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());
	TAITparGeo* itGeo = (TAITparGeo*) ( (TAITparGeo*) gTAGroot->FindParaDsc("itGeo", "TAITparGeo")->Object() );


	TAVTntuTrack* vtTrk_container = (TAVTntuTrack*) gTAGroot->FindDataDsc("vtTrack","TAVTntuTrack")->Object();
	// vtTrk_container->GetTracksN();

	vector<int> genParticleID_vec;
	vector<int> nPixels_vec;
	TVector3 lastVTclust_pos(0,0,0); 
	float mass;
	float charge;

	//loop over tracks
	for (int track_i = 0; track_i< vtTrk_container->GetTracksN(); track_i++) {

		TAVTtrack* track = vtTrk_container->GetTrack( track_i );

		// N clusters per track
		int ncluster = track->GetClustersN();
		m_histo_nClusXTrk->Fill( ncluster );

		float meanPixPerCluster = 0;

		// loop over clusters in the track
		for (int i=0; i<ncluster; i++) {

			TAVTcluster* clus = (TAVTcluster*) track->GetCluster( i );
			if (!clus->IsValid()) continue;
			
			TAMCpart* genParticle;

			// if just one particle producing clusters
			if ( clus->GetMcTracksN() == 1 ) {
				if ( clus->GetMcTrackIdx(0) < 0 ) {
					// cout << "INFO << Noise cluster " << endl;
					continue;
				}
				genParticle = eve->GetTrack( clus->GetMcTrackIdx(0) );
			}
			else { // more than one partilcle associated to the cluster
				// cout << "more than one partilcle associated to the cluster" << endl;
				m_moreThanOnePartPerCluster ++;
				continue;
			}

			genParticleID_vec.push_back( clus->GetMcTrackIdx(0) );
			mass = genParticle->GetMass();
			charge = genParticle->GetCharge();
			if ( lastVTclust_pos.z() < geoTrafo->FromVTLocalToGlobal( clus->GetPositionG() ).z() )
				lastVTclust_pos = geoTrafo->FromVTLocalToGlobal( clus->GetPositionG() );

			nPixels_vec.push_back( clus->GetPixelsN() );
			meanPixPerCluster += clus->GetPixelsN();


		} // end of cluster loop

		// for each track check if made by the same particle
		if ( genParticleID_vec.size() <= 1 ) {
			// cout << "All noise track" << endl;
			m_allNoiseTrack++;
			genParticleID_vec.clear();
			nPixels_vec.clear();
			continue;
		}
		
		// Evaluating MC cluster status code flag 
		int even=0; int odd=0;
		for (auto j=0; j<genParticleID_vec.size()-1; j++ ) {
			if ( genParticleID_vec.at(j) != genParticleID_vec.at(j+1) ) 	odd++;
			else 															even++;
		}
		int mcStustFlag;
		if 		( odd == 0 && even != 0  )		mcStustFlag = 0;	// good
		else if ( odd == 1 )					mcStustFlag = 1;	// loose
		else if ( odd > 1 )						mcStustFlag = 2;	// bad
		else 									cout << "Error" << endl, exit(0);
		m_histo_mcGoodTrk->Fill( mcStustFlag );

		// evaluating mean cluster size 
		meanPixPerCluster = meanPixPerCluster * (1./ncluster);
		
		m_histo_MeanNPix_inClusPerTrk[ (int)charge ]->Fill( meanPixPerCluster );

		// find the maximum difference in number of clusters 
		int maxDeltaPix = 0;
		for (auto j=0; j<nPixels_vec.size(); j++ ) {
			for (auto k=0; k<nPixels_vec.size(); k++ ) {
				if ( maxDeltaPix < abs(nPixels_vec[j] - nPixels_vec[k]) )	maxDeltaPix = abs(nPixels_vec[j] - nPixels_vec[k]);
			}
		}
		
		m_histo_MaxDeltaNPix_inClusPerTrk[ (int)charge ]->Fill( maxDeltaPix );

		// propagating the track
		if (mcStustFlag) {

			TVector3 extrapPos_front( 
								track->GetSlopeZ().X() * geoTrafo->FromITLocalToGlobal( itGeo->GetSensorPosition(0) ).Z() + lastVTclust_pos.X(),
								track->GetSlopeZ().Y() * geoTrafo->FromITLocalToGlobal( itGeo->GetSensorPosition(0) ).Z() + lastVTclust_pos.Y(),
								geoTrafo->FromITLocalToGlobal( itGeo->GetSensorPosition(0) ).z()
								 );
			TVector3 extrapPos_rear( 
								track->GetSlopeZ().X() * geoTrafo->FromITLocalToGlobal( itGeo->GetSensorPosition(4) ).Z() + lastVTclust_pos.X(),
								track->GetSlopeZ().Y() * geoTrafo->FromITLocalToGlobal( itGeo->GetSensorPosition(4) ).Z() + lastVTclust_pos.Y(),
								geoTrafo->FromITLocalToGlobal( itGeo->GetSensorPosition(4) ).Z()
								 );
			// newX_retro = track->GetSlopeZ.X() * geoTrafo->FromITLocalToGlobal( itGeo->GetSensorPosition(0) ).Print(); + lastVT.X();
			// newX = track->GetSlopeZ.X() * geoTrafo->FromITLocalToGlobal( itGeo->GetSensorPosition(0) ).Print(); + lastVT.X();
			// newY = track->GetSlopeZ.Y() * 16.88 + lastVT.Y();

			TAITntuCluster* itclus = (TAITntuCluster*) gTAGroot->FindDataDsc("itClus","TAITntuCluster")->Object();
			TAMCntuHit* itMc =  (TAMCntuHit*) gTAGroot->FindDataDsc("itMc", "TAMCntuHit")->Object();
			
			TVector3 sameParticleHitPos (0,0,0);
			TVector3 deltaPos (0,0,0);
			Int_t nPlanes = itGeo->GetSensorsN();
			for(Int_t iPlane = 0; iPlane < nPlanes; iPlane++){
				Int_t nclus = itclus->GetClustersN(iPlane);
				for(Int_t iClus = 0; iClus < nclus; ++iClus){
					TAITcluster* clus = itclus->GetCluster(iPlane, iClus);
					// TVector3 itCluster = geoTrafo->FromITLocalToGlobal( clus->GetPositionG() );
					
					// TAMCpart* genParticle = eve->GetTrack( clus->GetMcTrackIdx(0) );
					if ( clus->GetMcTrackIdx(0) != genParticleID_vec[0] )	continue;

					int nHits = clus->GetPixelsN();
					for (Int_t jHit = 0; jHit < nHits; ++jHit) {
				      	TAIThit* hit = (TAIThit*)clus->GetPixel(jHit);
				      	for (Int_t k = 0; k < hit->GetMcTracksN(); ++k) {
							Int_t idx = hit->GetMcIndex(k);
							TAMChit* mcHit = itMc->GetHit(idx);
							sameParticleHitPos = ( mcHit->GetInPosition() + mcHit->GetOutPosition() ) * 0.5 ;
							if ( fabs(sameParticleHitPos.z() - extrapPos_front.z()) < fabs(sameParticleHitPos.z() - extrapPos_rear.z()) )	
								deltaPos = sameParticleHitPos - extrapPos_front;
							else 
								deltaPos = sameParticleHitPos - extrapPos_rear;
						}
					}

				}
			}


			// fill
			m_histo_DeltaX_VtIt_straight[ (int)charge ]->Fill( deltaPos.x() );
			m_histo_DeltaY_VtIt_straight[ (int)charge ]->Fill( deltaPos.y() );
			m_histo_DeltaXDeltaY_VtIt_straight[ (int)charge ]->Fill( deltaPos.x(), deltaPos.y() );

		} // if ( MC status is good )


		genParticleID_vec.clear();
		nPixels_vec.clear();

	}// end of tracks loop
   
   return true;

}





void GlobalTrackingStudies::CreateHistogram()
{
	m_histo_nClusXTrk = new TH1F( "nClusXTrk", "nClusXTrk", 7, -1.5, 5.5 );
   AddHistogram(m_histo_nClusXTrk);

	m_histo_mcGoodTrk = new TH1F( "mcGoodTrk", "mcGoodTrk", 4, -0.5, 3.5 );
   AddHistogram(m_histo_mcGoodTrk);
   
   for(Int_t charge = 0; charge < 8; ++charge) {
      string name = "MeanNPix_ch" + build_string( charge );
      m_histo_MeanNPix_inClusPerTrk[ charge ] = new TH1F( name.c_str(), name.c_str(), 40 , -0.5, 39.5 );
      AddHistogram(m_histo_MeanNPix_inClusPerTrk[charge]);
   }
   
   for(Int_t charge = 0; charge < 8; ++charge) {
      string name = "MaxDeltaNPix" + build_string( charge );
      m_histo_MaxDeltaNPix_inClusPerTrk[ charge ] = new TH1F( name.c_str(), name.c_str(), 40 , -0.5, 39.5 );
      AddHistogram(m_histo_MaxDeltaNPix_inClusPerTrk[charge]);
   }
   
   for(Int_t charge = 0; charge < 8; ++charge) {
      string name = "DeltaX_VtIt_straight+ch=" + build_string( charge );
      m_histo_DeltaX_VtIt_straight[ charge ] = new TH1F( name.c_str(), name.c_str(), 500 , -2.5, 2.5 );
      AddHistogram(m_histo_DeltaX_VtIt_straight[charge]);
   }
   
   for(Int_t charge = 0; charge < 8; ++charge) {
      string name = "DeltaY_VtIt_straight+ch=" + build_string( charge );
      m_histo_DeltaY_VtIt_straight[ charge ] = new TH1F( name.c_str(), name.c_str(), 500 , -2.5, 2.5 );
      AddHistogram(m_histo_DeltaY_VtIt_straight[charge]);
   }
   
   for(Int_t charge = 0; charge < 8; ++charge) {
      string name = "DeltaXDeltaY_VtIt_straight+ch=" + build_string( charge );
      m_histo_DeltaXDeltaY_VtIt_straight[ charge ] = new TH2F( name.c_str(), name.c_str(), 100 , -2.5, 2.5, 100 , -2.5, 2.5 );
      AddHistogram(m_histo_DeltaXDeltaY_VtIt_straight[charge]);
   }
   
   SetValidHistogram(kTRUE);
}


void GlobalTrackingStudies::Finalize()
{
    // no more
   string pathName = m_kalmanOutputDir+"/TrackingStudies";
   system(("mkdir "+pathName).c_str());

   TCanvas* mirror = new TCanvas("GTstudy", "GTstudy", 800, 800);

   m_histo_nClusXTrk->GetXaxis()->SetTitle("nCluster");
   m_histo_nClusXTrk->Draw();
   mirror->SaveAs(( pathName+"/"+"nClusXTrk.png").c_str());
   mirror->SaveAs(( pathName+"/"+"nClusXTrk.root").c_str());

   m_histo_mcGoodTrk->GetXaxis()->SetTitle("flagging");
   m_histo_mcGoodTrk->Draw("hist text");
   mirror->SaveAs(( pathName+"/"+"mcGoodTrk.png").c_str());
   mirror->SaveAs(( pathName+"/"+"mcGoodTrk.root").c_str());

   for (Int_t i = 0; i < 8; ++i) {
      m_histo_MeanNPix_inClusPerTrk[i]->GetXaxis()->SetTitle("mean pix n");
      m_histo_MeanNPix_inClusPerTrk[i]->GetYaxis()->SetTitle("Events");
      m_histo_MeanNPix_inClusPerTrk[i]->Draw();
      mirror->SaveAs( (pathName+"/MeanNPix_inClusPerTrk_ch"+build_string( i) + ".png").c_str() );
      mirror->SaveAs( (pathName+"/MeanNPix_inClusPerTrk_ch"+build_string( i) + ".root").c_str() );
   }
   
   for (Int_t i = 0; i < 8; ++i) {
      m_histo_MaxDeltaNPix_inClusPerTrk[i]->GetXaxis()->SetTitle("Max deltaPix");
      m_histo_MaxDeltaNPix_inClusPerTrk[i]->GetYaxis()->SetTitle("Events");
      m_histo_MaxDeltaNPix_inClusPerTrk[i]->Draw();
      mirror->SaveAs( (pathName+"/MaxDeltaNPix_inClusPerTrk_ch"+build_string( i) + ".png").c_str() );
      mirror->SaveAs( (pathName+"/MaxDeltaNPix_inClusPerTrk_ch"+build_string( i) + ".root").c_str() );
   }
   
   for (Int_t i = 0; i < 8; ++i) {
      m_histo_DeltaX_VtIt_straight[i]->GetXaxis()->SetTitle("DeltaX");
      m_histo_DeltaX_VtIt_straight[i]->GetYaxis()->SetTitle("Events");
      m_histo_DeltaX_VtIt_straight[i]->Draw();
      mirror->SaveAs( (pathName+"/DeltaX_VtIt_straight_ch"+build_string( i) + ".png").c_str() );
      mirror->SaveAs( (pathName+"/DeltaX_VtIt_straight_ch"+build_string( i) + ".root").c_str() );
   }
   
   for (Int_t i = 0; i < 8; ++i) {
      m_histo_DeltaY_VtIt_straight[i]->GetXaxis()->SetTitle("DeltaY");
      m_histo_DeltaY_VtIt_straight[i]->GetYaxis()->SetTitle("Events");
      m_histo_DeltaY_VtIt_straight[i]->Draw();
      mirror->SaveAs( (pathName+"/DeltaY_VtIt_straight_ch"+build_string( i) + ".png").c_str() );
      mirror->SaveAs( (pathName+"/DeltaY_VtIt_straight_ch"+build_string( i) + ".root").c_str() );
   }
   
   for (Int_t i = 0; i < 8; ++i) {
      m_histo_DeltaXDeltaY_VtIt_straight[i]->GetXaxis()->SetTitle("DeltaX");
      m_histo_DeltaXDeltaY_VtIt_straight[i]->GetYaxis()->SetTitle("DeltaY");
      m_histo_DeltaXDeltaY_VtIt_straight[i]->Draw("colz");
      mirror->SaveAs( (pathName+"/DeltaXDeltaY_VtIt_straight_ch"+build_string( i) + ".png").c_str() );
      mirror->SaveAs( (pathName+"/DeltaXDeltaY_VtIt_straight_ch"+build_string( i) + ".root").c_str() );
   }


   cout << endl << endl << "-------------------------------------------------------------" << endl;
   cout << "\t moreThanOnePartPerCluster = " << m_moreThanOnePartPerCluster << endl;
   cout << "\t m_allNoiseTrack = " << m_allNoiseTrack << endl;
   cout << "-------------------------------------------------------------" << endl << endl << endl;
}




























