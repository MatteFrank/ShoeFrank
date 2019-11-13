
#include "GlobalTrackRepostory.hxx"





//----------------------------------------------------------------------------------------------------
//  evaluate uncertainty from the diagonal of the covariance matrix ONLY. No correlation is considered!!!
double GlobalTrackRepostory::EvalError( TVector3 mom, TVector3 err ) {

  double dx2 = 2 * sqrt(err.x()) * mom.x();
  double dy2 = 2 * sqrt(err.y()) * mom.y();
  double dz2 = 2 * sqrt(err.z()) * mom.z();

  double p = mom.Mag();
  double dp2 = dx2 + dy2 + dz2;
  double dp = dp2 / ( 2 * p );

  return dp;
}



//----------------------------------------------------------------------------------------------------
//  measure the Kalman uncertainty INCLUDING the cross terms in the covariance matrix. CORRELATION considered!!!
double GlobalTrackRepostory::EvalError( TVector3 mom, TMatrixD cov ) {

  if ( cov.GetNcols() != 3 || cov.GetNrows() != 3 ) 
    cout << "ERROR :: GlobalTrackRepostory::EvalError  >>  covariance dimension (should be 6) is wrong " << cov.GetNcols() << " x " << cov.GetNrows() << endl, exit(0);

  array<double,3> partialDer = { mom.x()/sqrt(mom.Mag()), mom.y()/sqrt(mom.Mag()), mom.z()/sqrt(mom.Mag()) };


  double err = 0;
  for ( int j=0; j<cov.GetNrows(); j++ ) {
    for ( int k=0; k<cov.GetNcols(); k++ ) {
			
      err += partialDer[j] * partialDer[k] * cov(j,k); // * diagFactor;
			
    }
  }

  double dp = sqrt(err);

  return dp;
}





//----------------------------------------------------------------------------------------------------
void GlobalTrackRepostory::PrintPositionResidual( TVector3 pos, TVector3 expectedPos, string hitSampleName ) {
  double dR = pos.DeltaR( expectedPos );
  // h_posRes[ hitSampleName ]->Fill(dR);
}



//----------------------------------------------------------------------------------------------------
void GlobalTrackRepostory::PrintMomentumResidual( TVector3 meas, TVector3 expected, TVector3 cov, string hitSampleName ) {

  if ( m_debug > 2 )		cout << "GlobalTrackRepostory::PrintMomentumResidual -- Start!!!!  " << endl;
  double err = EvalError( meas, cov );

  PrintMomentumResidual( meas, expected, err, hitSampleName );
	
}


//----------------------------------------------------------------------------------------------------
void GlobalTrackRepostory::PrintMomentumResidual( TVector3 meas, TVector3 expected, TMatrixD cov, string hitSampleName ) {

  if ( m_debug > 2 )		cout << "GlobalTrackRepostory::PrintMomentumResidual -- Start!!!!  " << endl;
  double err = EvalError( meas, cov );

  PrintMomentumResidual( meas, expected, err, hitSampleName );
}


//----------------------------------------------------------------------------------------------------
void GlobalTrackRepostory::PrintMomentumResidual( TVector3 meas, TVector3 expected, double err, string hitSampleName ) {

  if ( m_debug > 2 )		cout << "GlobalTrackRepostory::PrintMomentumResidual -- Start!!!!  " << endl;
  double dP = meas.Mag() - expected.Mag();

  if ( m_debug > 1 )		cout << "dp= " <<meas.Mag() << "-"<<expected.Mag() << "   err= " << err<< endl;
  if ( m_debug > 1 )		cout << " residuo= "<< dP / err <<endl;

  // h_deltaP[ hitSampleName ]->Fill( dP );
  // h_sigmaP[ hitSampleName ]->Fill(err);
  // h_momentumRes[ hitSampleName ]->Fill( dP /err);

  // h_dP_over_Ptrue[ hitSampleName ]->Fill( dP / expected.Mag() );
  // h_dP_over_Pkf[ hitSampleName ]->Fill( dP / meas.Mag() );
  // h_sigmaP_over_Ptrue[ hitSampleName ]->Fill( err / expected.Mag() );
  // h_sigmaP_over_Pkf[ hitSampleName ]->Fill( err / meas.Mag() );

	
  // histos for momentum reso
  if ( meas.Mag() == 0 || expected.Mag() == 0 ) 
    cout<< "ERROR::GlobalTrackRepostory::PrintMomentumResidual  -->  track momentum - 0. "<< endl, exit(0);
  // find the center of the momentum bin
  int roundUp = ceil( (double)expected.Mag() );
  int roundDown = floor( (double)expected.Mag() );
  float binCenter = -666;
  int nstep = ((float)(roundUp - roundDown)) / m_resoP_step;
  for ( int i=0; i<nstep; i++ ) {
    if ( expected.Mag() > roundDown+(i*m_resoP_step) &&  expected.Mag() <= roundDown+((i+1)*m_resoP_step) ) {
      binCenter = roundDown + m_resoP_step*i + 0.5*m_resoP_step;
      break;
    }
  }

  // fill the h_dP_x_bin
  if ( h_dP_x_bin.find( hitSampleName ) == h_dP_x_bin.end() ) {
    map<float, TH1F*> tmp_dP_x_bin;
		
    string name = "dP_dist_"+hitSampleName+"_"+build_string(binCenter);
    TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -2, 2 );
    tmp_dP_x_bin[ binCenter ] = h;
				
    h_dP_x_bin[ hitSampleName ] = tmp_dP_x_bin;
  }
  else if ( h_dP_x_bin[ hitSampleName ].find( binCenter ) == h_dP_x_bin[ hitSampleName ].end() ) {
    string name = "dP_dist_"+hitSampleName+"_"+build_string(binCenter);
    TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -2, 2 );
    h_dP_x_bin[ hitSampleName ][ binCenter ] = h;
  }
  h_dP_x_bin[ hitSampleName ][ binCenter ]->Fill( dP );

  // fill the h_dPOverP_x_bin
  if ( h_dPOverP_x_bin.find( hitSampleName ) == h_dPOverP_x_bin.end() ) {
    map<float, TH1F*> tmp_dPOverP_x_bin;
		
    string name = "dPOverP_dist_"+hitSampleName+"_"+build_string(binCenter);
    TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -0.2, 0.2 );
    tmp_dPOverP_x_bin[ binCenter ] = h;
				
    h_dPOverP_x_bin[ hitSampleName ] = tmp_dPOverP_x_bin;
  }
  else if ( h_dPOverP_x_bin[ hitSampleName ].find( binCenter ) == h_dPOverP_x_bin[ hitSampleName ].end() ) {
    string name = "dPOverP_dist_"+hitSampleName+"_"+build_string(binCenter);
    TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -0.2, 0.2 );
    h_dPOverP_x_bin[ hitSampleName ][ binCenter ] = h;
  }
  h_dPOverP_x_bin[ hitSampleName ][ binCenter ]->Fill( dP/expected.Mag() );

  // fill the h_dPOverSigmaP_x_bin
  if ( h_dPOverSigmaP_x_bin.find( hitSampleName ) == h_dPOverSigmaP_x_bin.end() ) {
    map<float, TH1F*> tmp_dPOverSigmaP_x_bin;
		
    string name = "dPOverSigmaP_dist_"+hitSampleName+"_"+build_string(binCenter);
    TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -2, 2 );
    tmp_dPOverSigmaP_x_bin[ binCenter ] = h;
				
    h_dPOverSigmaP_x_bin[ hitSampleName ] = tmp_dPOverSigmaP_x_bin;
  }
  else if ( h_dPOverSigmaP_x_bin[ hitSampleName ].find( binCenter ) == h_dPOverSigmaP_x_bin[ hitSampleName ].end() ) {
    string name = "dPOverSigmaP_dist_"+hitSampleName+"_"+build_string(binCenter);
    TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -4, 4 );
    h_dPOverSigmaP_x_bin[ hitSampleName ][ binCenter ] = h;
  }
  h_dPOverSigmaP_x_bin[ hitSampleName ][ binCenter ]->Fill( dP/err );



  if ( m_debug > 1 )		cout << "GlobalTrackRepostory::PrintMomentumResidual -- End!!!!  " << endl;
}





//----------------------------------------------------------------------------------------------------
// Called from outside!
void GlobalTrackRepostory::EvaluateMomentumResolution() {


  for ( map<string, map<float, TH1F*> >::iterator collIt=h_dPOverP_x_bin.begin(); collIt != h_dPOverP_x_bin.end(); collIt++ ) {
		
    //  initialize output resolution histos
    float resoP_min = (*(*collIt).second.begin()).first - m_resoP_step*0.5;
    float resoP_max = (*(*collIt).second.rbegin()).first + m_resoP_step*0.5;
    float nfbin = (resoP_max-resoP_min)/m_resoP_step;
    // if ( modf( (resoP_max-resoP_min)/m_resoP_step, &nfbin ) == 0.0 ) 
    if ( fabs(nfbin-round(nfbin)) > 0.001 )
      cout<<"ERROR :: GlobalTrackRepostory::EvaluateMomentumResolution  --> "<<(*collIt).first<<" resolution binning not round! min=" <<resoP_min<<" max="<<resoP_max<<" step="<<m_resoP_step<<" = "<<nfbin<< endl, exit(0);		// check correct binning
    int nbin = round(nfbin);

    string histoName = (string)"h_resoP_over_Pkf"+"__"+(*collIt).first;
    h_resoP_over_Pkf[ (*collIt).first ] = new TH1F( histoName.c_str(), histoName.c_str(), nbin, resoP_min, resoP_max );
		
    histoName = (string)"h_biasP_over_Pkf"+"__"+(*collIt).first;
    h_biasP_over_Pkf[ (*collIt).first ] = new TH1F( histoName.c_str(), histoName.c_str(), nbin, resoP_min, resoP_max );
	


    int k=0;
    for ( map<float, TH1F*>::iterator it=(*collIt).second.begin(); it != (*collIt).second.end(); it++ ) {
			
      k++;
      if ( k > h_resoP_over_Pkf[ (*collIt).first ]->GetNbinsX() )			
	cout<<"ERROR :: GlobalTrackRepostory::EvaluateMomentumResolution  --> "<<(*collIt).first<< "  binning problem! do not fill all the reso plot. " << endl, exit(0);	

      if ( m_debug <= 0 && (*it).second->GetEntries() < 100 )		continue;

      // check if the binning produce even bounds for the bins, not irrational numbers for examples
      float a = (h_resoP_over_Pkf[ (*collIt).first ]->GetXaxis()->GetBinLowEdge(k) + h_resoP_over_Pkf[ (*collIt).first ]->GetXaxis()->GetBinUpEdge(k)) /2;
      float b = (*it).first;
      if ( fabs(a - b) > 0.00001 ) {
	cout << "WARNING::GlobalTrackRepostory::EvaluateMomentumResolution \t >> \t chosen binning do not make bin's round bounds. Histo: "
	     <<(*it).second->GetName() << "  " 
	     <<(*collIt).first<<": " << h_resoP_over_Pkf[ (*collIt).first ]->GetXaxis()->GetBinLowEdge(k) << " + "
	     << h_resoP_over_Pkf[ (*collIt).first ]->GetXaxis()->GetBinUpEdge(k) <<" = " << a << " instead of " << b 
	     << ".\n\t\t Worry not too much, can be caused by bins not filled." << endl; 
	it--;
	continue;
      }

      // TF1 *f1 = new TF1("f1","gaus",-3,3);
      TF1 * f1 = new TF1("gauss", "[0] / sqrt(2.0 * TMath::Pi()) / [2] * exp(-(x-[1])*(x-[1])/2./[2]/[2])", 0, 100);
      f1->SetParNames("Constant","Mean","Sigma");
      cout << " getmean is " << (*it).second->GetMean() << " and getrms is "  <<  (*it).second->GetRMS() << std::endl;
      f1->SetParameters( 0, (*it).second->GetMean(), (*it).second->GetRMS() );
      // f1->SetParameters( 0, h_resoP_over_Pkf[ (*collIt).first ]->GetMean(), h_resoP_over_Pkf[ (*collIt).first ]->GetStdDev() );
      f1->SetParLimits(0,  0.1, (*it).second->GetMaximum());
      f1->SetParLimits(1, -0.18,  0.18);
      f1->SetParLimits(2,  0.001,  0.16);
      f1->SetLineWidth(2);
      f1->SetLineColor(2);
      // h_resoP_over_Pkf[ (*collIt).first ]->Fit("f1","R");
      (*it).second->Fit("gauss", "LQ");	// log likelihood fit, quiet mode

      if ( m_debug <= 0 && (f1->GetParError(f1->GetParNumber("Sigma")) / f1->GetParameter(f1->GetParNumber("Sigma")) > 0.1) )	
	continue;

      // GetNumberFreeParameters()
      // h_resoP_over_Pkf[ (*collIt).first ]->SetBinContent( k, f1->GetParameter(1) );
      h_resoP_over_Pkf[ (*collIt).first ]->SetBinContent( k, f1->GetParameter(f1->GetParNumber("Sigma")) );
      h_resoP_over_Pkf[ (*collIt).first ]->SetBinError( k, f1->GetParError (f1->GetParNumber("Sigma")) );
      // h_resoP_over_Pkf[ (*collIt).first ]->SetBinContent( k, (*it).second->GetStdDev(1) );
      // h_resoP_over_Pkf[ (*collIt).first ]->SetBinError( k, (*it).second->GetStdDev(1) );
			

      h_biasP_over_Pkf[ (*collIt).first ]->SetBinContent( k, f1->GetParameter(f1->GetParNumber("Mean")) );
      h_biasP_over_Pkf[ (*collIt).first ]->SetBinError( k, f1->GetParameter(f1->GetParNumber("Mean")) );
    }
  }

  Save();

}




//----------------------------------------------------------------------------------------------------
void GlobalTrackRepostory::Save( ) {

  struct stat info;

  TCanvas* mirror = new TCanvas("TrackChi2Plot", "TrackChi2Plot", 700, 700);

  for ( map<string, map<float, TH1F*> >::iterator collIt=h_dP_x_bin.begin(); collIt != h_dP_x_bin.end(); collIt++ ) {
    string pathName = m_kalmanOutputDir+"/"+(*collIt).first+"/dP_x_bin";
    if( stat( pathName.c_str(), &info ) != 0 )		//cannot access
      system(("mkdir "+pathName).c_str());
    for ( map<float, TH1F*>::iterator it=(*collIt).second.begin(); it != (*collIt).second.end(); it++ ) {
      (*it).second->GetXaxis()->SetTitle("dP(GeV)");
      (*it).second->Draw();
      mirror->SaveAs(( pathName+"/"+"dP_x_bin__"+(*collIt).first+"__"+build_string( (*it).first )+".png").c_str());
    }
  }
  gStyle->SetOptFit(111111);
  for ( map<string, map<float, TH1F*> >::iterator collIt=h_dPOverP_x_bin.begin(); collIt != h_dPOverP_x_bin.end(); collIt++ ) {
    string pathName = m_kalmanOutputDir+"/"+(*collIt).first+"/dPOverP_x_bin";
    if( stat( pathName.c_str(), &info ) != 0 )		//cannot access
      system(("mkdir "+pathName).c_str());
    for ( map<float, TH1F*>::iterator it=(*collIt).second.begin(); it != (*collIt).second.end(); it++ ) {
      (*it).second->GetXaxis()->SetTitle("dP/P");
      (*it).second->Draw();
      mirror->SaveAs(( pathName+"/"+"dPOverP_x_bin__"+(*collIt).first+"__"+build_string( (*it).first )+".png").c_str());
    }
  }
  gStyle->SetOptFit(111);
  for ( map<string, map<float, TH1F*> >::iterator collIt=h_dPOverSigmaP_x_bin.begin(); collIt != h_dPOverSigmaP_x_bin.end(); collIt++ ) {
    string pathName = m_kalmanOutputDir+"/"+(*collIt).first+"/dPOverSigmaP_x_bin";
    if( stat( pathName.c_str(), &info ) != 0 )		//cannot access
      system(("mkdir "+pathName).c_str());
    for ( map<float, TH1F*>::iterator it=(*collIt).second.begin(); it != (*collIt).second.end(); it++ ) {
      (*it).second->GetXaxis()->SetTitle("dP/sigmaP");
      (*it).second->Draw();
      mirror->SaveAs(( pathName+"/"+"dPOverSigmaP_x_bin__"+(*collIt).first+"__"+build_string( (*it).first )+".png").c_str());
    }
  }

  for ( map< string, TH1F* >::iterator it=h_resoP_over_Pkf.begin(); it != h_resoP_over_Pkf.end(); it++ ) {
    (*it).second->GetXaxis()->SetTitle("p(GeV)");
    (*it).second->GetYaxis()->SetTitle("dp/p");
    (*it).second->Draw();
		
    mirror->SaveAs( (m_kalmanOutputDir+"/"+(*it).first+"/"+"differentialMomentumReso" + ".png").c_str() );
    mirror->SaveAs( (m_kalmanOutputDir+"/"+(*it).first+"/"+"differentialMomentumReso" + ".root").c_str() );
    // string aaa = mirror->GetName();
    // mirror->SetName( (*it).second->GetName() );
    // mirror->SaveAs( (m_kalmanOutputDir+"/"+(*it).first+"/"+"differentialMomentumReso" + ".root").c_str() );
    // mirror->SetName( aaa.c_str() );
  }

  for ( map< string, TH1F* >::iterator it=h_biasP_over_Pkf.begin(); it != h_biasP_over_Pkf.end(); it++ ) {
    (*it).second->GetXaxis()->SetTitle("p(GeV)");
    (*it).second->GetYaxis()->SetTitle("Bias(dp/p)");
    (*it).second->Draw();
    mirror->SaveAs( (m_kalmanOutputDir+"/"+(*it).first+"/"+"differentialResoBias" + ".png").c_str() );
    mirror->SaveAs( (m_kalmanOutputDir+"/"+(*it).first+"/"+"differentialResoBias" + ".root").c_str() );
  }

}
















