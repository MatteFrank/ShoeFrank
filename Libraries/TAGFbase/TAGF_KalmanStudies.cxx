
#include "TAGF_KalmanStudies.hxx"


ClassImp(TAGF_KalmanStudies);


//Default constructor
TAGF_KalmanStudies::TAGF_KalmanStudies() : TAGobject()  {

	m_resoP_step = 0.2;
    
}

//Default destructor
TAGF_KalmanStudies::~TAGF_KalmanStudies()	{}




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//----------------------------------------------------------------------------------------------------
//  evaluate uncertainty from the diagonal of the covariance matrix ONLY. No correlation is considered!!!
double TAGF_KalmanStudies::EvalError( TVector3 mom, TVector3 err ) {

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
double TAGF_KalmanStudies::EvalError( TVector3 mom, TMatrixD cov ) {

  // if ( cov.GetNcols() != 3 || cov.GetNrows() != 3 )

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
void TAGF_KalmanStudies::FillMomentumInfo( TVector3 meas, TVector3 expected, TMatrixD cov, string hitSampleName, map<string, TH1F*> *h_deltaP, map<string, TH1F*> *h_sigmaP ) {

	double dP = meas.Mag() - expected.Mag();
	double err = EvalError( meas, cov );

	if ( (*h_deltaP).find( hitSampleName ) == (*h_deltaP).end() ) {
		string name = "h_deltaP_" + hitSampleName;
		TH1F* h = new TH1F( name.c_str(), name.c_str(), 150 , -1.5, 1.5 );
		h->GetXaxis()->SetRange(0, h->GetNbinsX() + 1);
		(*h_deltaP)[ hitSampleName ] = h;
	}

	if ( (*h_sigmaP).find( hitSampleName ) == (*h_sigmaP).end() ) {
		string name = "h_sigmaP_" + hitSampleName;
		TH1F* h = new TH1F( name.c_str(), name.c_str(), 150 , 0, 1.5 );
		h->GetXaxis()->SetRange(0, h->GetNbinsX() + 1);
		(*h_sigmaP)[ hitSampleName ] = h;
	}

	// if ( (*h_momentumRes).find( hitSampleName ) == (*h_momentumRes).end() ) {
	// 	string name = "h_momentumRes_" + hitSampleName;
	// 	TH1F* h = new TH1F( name.c_str(), name.c_str(), 100 , -1, 1 );
	// 	h->GetXaxis()->SetRange(0, h->GetNbinsX() + 1);
	// 	(*h_momentumRes)[ hitSampleName ] = h;
	// }

	(*h_deltaP)[ hitSampleName ]->Fill( dP );
	(*h_sigmaP)[ hitSampleName ]->Fill(err);
	// (*h_momentumRes)[ hitSampleName ]->Fill( dP /err);



	// h_dP_over_Ptrue[ hitSampleName ]->Fill( dP / expected.Mag() );
	// h_dP_over_Pkf[ hitSampleName ]->Fill( dP / meas.Mag() );
	// h_sigmaP_over_Ptrue[ hitSampleName ]->Fill( err / expected.Mag() );
	// h_sigmaP_over_Pkf[ hitSampleName ]->Fill( err / meas.Mag() );

}





//----------------------------------------------------------------------------------------------------
// Called from outside!
void TAGF_KalmanStudies::Fill_MomentumResidual( TVector3 meas, TVector3 expected, TMatrixD cov, string hitSampleName, map<string, map<float, TH1F*> >* h_dPOverP_x_bin ) {

	if ( FootDebugLevel(1) )		
		cout << "TAGF_KalmanStudies::PrintMomentumResidual -- Start!!!!  " << endl;

	double dP = meas.Mag() - expected.Mag();
	double err = EvalError( meas, cov );

	if ( FootDebugLevel(1) )
	{
		cout << "dp= " <<meas.Mag() << "-"<<expected.Mag() << "   err= " << err<< endl;
		cout << " residuo= "<< dP / err <<endl;
	}

	// histos for momentum reso
	if ( meas.Mag() == 0 || expected.Mag() == 0 )
		cout<< "ERROR::TAGF_KalmanStudies::PrintMomentumResidual  -->  track momentum - 0. "<< endl, exit(0);

	// find the center of the momentum bin
	int roundUp = 11;//ceil( (double)expected.Mag() );
	int roundDown = 0;//floor( (double)expected.Mag() );
	float binCenter = -666;
	int nstep = ((float)(roundUp - roundDown)) / m_resoP_step;

	for ( int i=0; i<nstep; i++ ) {
		if ( expected.Mag() > roundDown+(i*m_resoP_step) && expected.Mag() <= roundDown+( (i+1)*m_resoP_step) ) {
			binCenter = roundDown + m_resoP_step*i + 0.5*m_resoP_step;
			break;
		}
	}


	// fill the h_dPOverP_x_bin
	if ( (*h_dPOverP_x_bin).find( hitSampleName ) == (*h_dPOverP_x_bin).end() ) {
		map<float, TH1F*> tmp_dPOverP_x_bin;
		string name = "dPOverP_dist_" + hitSampleName + "_" + build_string( binCenter );
		TH1F* h = new TH1F( name.c_str(), name.c_str(), 400 , -1., 1. );
		tmp_dPOverP_x_bin[ binCenter ] = h;
		(*h_dPOverP_x_bin)[ hitSampleName ] = tmp_dPOverP_x_bin;
	}

	// create plot the h_dPOverP_x_bin if not already defined
	else if ( (*h_dPOverP_x_bin)[ hitSampleName ].find( binCenter ) == (*h_dPOverP_x_bin)[ hitSampleName ].end() ) {
		string name = "dPOverP_dist_" + hitSampleName + "_" + build_string( binCenter );
		TH1F* h = new TH1F( name.c_str(), name.c_str(), 400 , -1., 1. );
		(*h_dPOverP_x_bin)[ hitSampleName ][ binCenter ] = h;
	}
	if ( FootDebugLevel(1) ) cout << "Filling h_dPOverP_x_bin " << hitSampleName << " of bincenter " << binCenter << " with " << dP/expected.Mag() << endl;

	// fill the h_dPOverP_x_bin
	// h_dPOverP_x_bin->at(binCenter)->Fill( dP/expected.Mag() );
	// h_dPOverP_x_bin[ hitSampleName ][ binCenter ]->Fill( dP/expected.Mag() );
	( ((*h_dPOverP_x_bin).at( hitSampleName )).at(binCenter) )->Fill( dP/expected.Mag() );

	if ( FootDebugLevel(2) )		
		cout << "TAGF_KalmanStudies::PrintMomentumResidual -- End!!!!  " << ( ((*h_dPOverP_x_bin).at( hitSampleName )).at(binCenter) )->GetTitle() << endl;
}






//----------------------------------------------------------------------------------------------------
// Called from outside!
void TAGF_KalmanStudies::EvaluateAndFill_MomentumResolution( map<string, map<float, TH1F*> > *h_dPOverP_x_bin, map<string, TH1F*> *h_resoP_over_Pkf, map<string, TH1F*> *h_biasP_over_Pkf ) {


	//problem on iterator it not resolved
	for ( map<string, map<float, TH1F*> >::iterator collIt=h_dPOverP_x_bin->begin(); collIt != h_dPOverP_x_bin->end(); ++collIt ) {
		//  initialize output resolution histos
		float resoP_min = (*(*collIt).second.begin()).first - m_resoP_step*0.5;
		float resoP_max = (*(*collIt).second.rbegin()).first + m_resoP_step*0.5;
		float nfbin = (resoP_max-resoP_min)/m_resoP_step;

		// if ( FootDebugLevel(1) ) 
			cout << "in TAGF_KalmanStudies::EvaluateMomentumResolution-> " << (*collIt).first << " resoP_min/max " << resoP_min << " " << resoP_max << " nfbin " << nfbin << endl;
		//if ( modf( (resoP_max-resoP_min)/m_resoP_step, &nfbin ) == 0.0 )

		if ( fabs( nfbin-round(nfbin) ) > 0.001 )
			cout<<"ERROR :: TAGF_KalmanStudies::EvaluateMomentumResolution  --> "<<(*collIt).first<<" resolution binning not round! min=" <<resoP_min<<" max="<<resoP_max<<" step="<<m_resoP_step<<" = "<<nfbin<< endl, exit(0);		// check correct binning
		// cout<<"ERROR :: TAGF_KalmanStudies::EvaluateMomentumResolution  --> "<<(*collIt).first<<" resolution binning not round! min=" <<resoP_min<<" max="<<resoP_max<<" step="<<m_resoP_step<<" = "<<nfbin<< endl;		// check correct binning
		int nbin = round(nfbin);

		string histoName = (string)"h_resoP_over_Pkf"+"__"+(*collIt).first;
		(*h_resoP_over_Pkf)[ (*collIt).first ] = new TH1F( histoName.c_str(), histoName.c_str(), nbin, resoP_min, resoP_max );

		histoName = (string)"h_biasP_over_Pkf"+"__"+(*collIt).first;
		(*h_biasP_over_Pkf)[ (*collIt).first ] = new TH1F( histoName.c_str(), histoName.c_str(), nbin, resoP_min, resoP_max );

		int k=0;
		for ( map<float, TH1F*>::iterator it=(*collIt).second.begin(); it != (*collIt).second.end(); ++it ) {
			k++; //jump the underflow
			if ( k > (*h_resoP_over_Pkf)[ (*collIt).first ]->GetNbinsX() )	{
				cout<<"ERROR :: TAGF_KalmanStudies::EvaluateMomentumResolution  --> "<<(*collIt).first<< "  binning problem! do not fill all the reso plot. " << endl, exit(0);
			}

			if ( (*it).second->GetEntries() < 100 ) continue;

			// check if the binning produce even bounds for the bins, not irrational numbers for examples
			float a = ((*h_resoP_over_Pkf)[ (*collIt).first ]->GetXaxis()->GetBinLowEdge(k) + (*h_resoP_over_Pkf)[ (*collIt).first ]->GetXaxis()->GetBinUpEdge(k)) /2;
			float b = (*it).first;
			if ( fabs(a - b) > 0.00001 ) {
				cout << "WARNING::TAGF_KalmanStudies::EvaluateMomentumResolution \t --> \t chosen binning do not make bin's round bounds. Histo: "
				<<(*it).second->GetName() << "  "
				<<(*collIt).first<<": " << (*h_resoP_over_Pkf)[ (*collIt).first ]->GetXaxis()->GetBinLowEdge(k) << " + "
				<< (*h_resoP_over_Pkf)[ (*collIt).first ]->GetXaxis()->GetBinUpEdge(k) <<" = " << a << " instead of " << b
				<< ".\n\t\t Worry not too much, can be caused by bins not filled." << endl;
				--it;
				continue;
			}

			TF1 *f1 = new TF1("f1","gaus",-50 ,50);
			//TF1* f1 = new TF1("gauss", "[0] / sqrt(2.0 * TMath::Pi()) / [2] * exp(-(x-[1])*(x-[1])/2./[2]/[2])", 0, 100);
			f1->SetParNames("Constant","Mean","Sigma");
			//      cout << " getmean is " << ((*it).second)->GetParameter(1) << " and getrms is " << ((*it).second)->GetParameter(2) << std::endl;
			f1->SetParameters( 0., ((*it).second)->GetMean(), ((*it).second)->GetRMS() );
			f1->SetParLimits(0.,  0.1, ((*it).second)->GetMaximum());
			f1->SetParLimits(1., -0.18,  0.18);
			f1->SetParLimits(2.,  0.001,  0.16);
			f1->SetLineWidth(2);
			f1->SetLineColor(2);
			(*it).second->Fit("f1","R");
			// (*it).second->Fit("f1", "LQ");	// log likelihood fit, quiet mode

			if ( (f1->GetParError(f1->GetParNumber("Sigma")) / f1->GetParameter(f1->GetParNumber("Sigma")) > 0.1) )
			continue;

			// GetNumberFreeParameters()
			// (*h_resoP_over_Pkf)[ (*collIt).first ]->SetBinContent( k, f1->GetParameter(1) );
			(*h_resoP_over_Pkf)[ (*collIt).first ]->SetBinContent( k, f1->GetParameter(f1->GetParNumber("Sigma")) );
			(*h_resoP_over_Pkf)[ (*collIt).first ]->SetBinError( k, f1->GetParError (f1->GetParNumber("Sigma")) );
			// (*h_resoP_over_Pkf)[ (*collIt).first ]->SetBinContent( k, (*it).second->GetStdDev(1) );
			// (*h_resoP_over_Pkf)[ (*collIt).first ]->SetBinError( k, (*it).second->GetStdDev(1) );


			(*h_biasP_over_Pkf)[ (*collIt).first ]->SetBinContent( k, f1->GetParameter(f1->GetParNumber("Mean")) );
			(*h_biasP_over_Pkf)[ (*collIt).first ]->SetBinError( k, f1->GetParameter(f1->GetParNumber("Mean")) );
	    }
	}

	// Save();

}










//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// void TAGtrackRepoKalman::PrintMomentumResidual( double meas, double expected, double err, string hitSampleName ) {

// 	if ( m_debug > -1 )		cout << "TAGtrackRepoKalman::PrintMomentumResidual -- Start!!!!  " << endl;

// 	double dP = meas - expected;

// 	if ( m_debug > -1 )		cout << "dp = " << meas << "-"<< expected << "   err= " << err<< endl;
// 	if ( m_debug > -1 )		cout << " residuo= "<< dP / err <<endl;

// 	// h_deltaP[ hitSampleName ]->Fill( dP );
// 	// h_sigmaP[ hitSampleName ]->Fill(err);
// 	// h_momentumRes[ hitSampleName ]->Fill( dP /err);

// 	// h_dP_over_Ptrue[ hitSampleName ]->Fill( dP / expected.Mag() );
// 	// h_dP_over_Pkf[ hitSampleName ]->Fill( dP / meas.Mag() );
// 	// h_sigmaP_over_Ptrue[ hitSampleName ]->Fill( err / expected.Mag() );
// 	// h_sigmaP_over_Pkf[ hitSampleName ]->Fill( err / meas.Mag() );

// 	//can ask for TH1::Print

// 	// histos for momentum reso
// 	if ( meas == 0 || expected == 0 )
// 		cout<< "ERROR::TAGtrackRepoKalman::PrintMomentumResidual  -->  track momentum - 0. "<< endl, exit(0);

// 	// find the center of the momentum bin
// 	int roundUp = ceil( (double)expected );
// 	int roundDown = floor( (double)expected );
// 	float binCenter = -666;
// 	int nstep = ((float)(roundUp - roundDown)) / m_resoP_step;

// 	for ( int i=0; i<nstep; i++ ) {
// 		if ( expected > roundDown+(i*m_resoP_step) && expected <= roundDown+( (i+1)*m_resoP_step) ) {
// 			binCenter = roundDown + m_resoP_step*i + 0.5*m_resoP_step;
// 			break;
// 		}
// 	}

// 	// fill the h_dP_x_bin
// 	if ( h_dP_x_bin.find( hitSampleName ) == h_dP_x_bin.end() ) {
// 		map<float, TH1F*> tmp_dP_x_bin;
// 		string name = "dP_dist_" + hitSampleName + "_" + build_string( binCenter );
// 		TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -2, 2 );
// 		tmp_dP_x_bin[ binCenter ] = h;

// 		h_dP_x_bin[ hitSampleName ] = tmp_dP_x_bin;
// 	}

// 	else if ( h_dP_x_bin[ hitSampleName ].find( binCenter ) == h_dP_x_bin[ hitSampleName ].end() ) {
// 		string name = "dP_dist_" + hitSampleName + "_" + build_string( binCenter );
// 		TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -2, 2 );
// 		h_dP_x_bin[ hitSampleName ][ binCenter ] = h;
// 	}


// 	if ( FootDebugLevel(1) ) cout << "Filling h_dP_x_bin " << hitSampleName << " of bincenter " << binCenter << " with " << dP << endl;
// 	h_dP_x_bin[ hitSampleName ][ binCenter ]->Fill( dP );

// 	// fill the h_dPOverP_x_bin
// 	if ( h_dPOverP_x_bin.find( hitSampleName ) == h_dPOverP_x_bin.end() ) {
// 		map<float, TH1F*> tmp_dPOverP_x_bin;
// 		string name = "dPOverP_dist_" + hitSampleName + "_" + build_string( binCenter );
// 		TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -0.2, 0.2 );
// 		tmp_dPOverP_x_bin[ binCenter ] = h;
// 		h_dPOverP_x_bin[ hitSampleName ] = tmp_dPOverP_x_bin;
// 	}

// 	else if ( h_dPOverP_x_bin[ hitSampleName ].find( binCenter ) == h_dPOverP_x_bin[ hitSampleName ].end() ) {
// 		string name = "dPOverP_dist_" + hitSampleName + "_" + build_string( binCenter );
// 		TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -0.2, 0.2 );
// 		h_dPOverP_x_bin[ hitSampleName ][ binCenter ] = h;
// 	}

// 	if ( FootDebugLevel(1) ) cout << "Filling h_dPOverP_x_bin " << hitSampleName << " of bincenter " << binCenter << " with " << dP/expected << endl;
// 	// h_dPOverP_x_bin[ hitSampleName ][ binCenter ]->Fill( dP/expected.Mag() );
// 	( (h_dPOverP_x_bin.at( hitSampleName )).at(binCenter) )->Fill( dP/expected );

// 	// fill the h_dPOverSigmaP_x_bin
// 	if ( h_dPOverSigmaP_x_bin.find( hitSampleName ) == h_dPOverSigmaP_x_bin.end() ) {
// 		map<float, TH1F*> tmp_dPOverSigmaP_x_bin;
// 		string name = "dPOverSigmaP_dist_" + hitSampleName + "_" + build_string( binCenter );
// 		TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -2, 2 );
// 		tmp_dPOverSigmaP_x_bin[ binCenter ] = h;

// 		h_dPOverSigmaP_x_bin[ hitSampleName ] = tmp_dPOverSigmaP_x_bin;
// 	}

// 	else if ( h_dPOverSigmaP_x_bin[ hitSampleName ].find( binCenter ) == h_dPOverSigmaP_x_bin[ hitSampleName ].end() ) {
// 		string name = "dPOverSigmaP_dist_" + hitSampleName + "_" + build_string( binCenter );
// 		TH1F* h = new TH1F( name.c_str(), name.c_str(), 80 , -4, 4 );
// 		h_dPOverSigmaP_x_bin[ hitSampleName ][ binCenter ] = h;
// 	}

// 	h_dPOverSigmaP_x_bin[ hitSampleName ][ binCenter ]->Fill( dP/err );

// 	if ( m_debug > -1 )		cout << "TAGtrackRepoKalman::PrintMomentumResidual -- End!!!!  " << endl;

// }
// //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


