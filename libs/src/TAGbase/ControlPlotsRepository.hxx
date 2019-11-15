#ifndef ControlPlotsRepository_H
#define ControlPlotsRepository_H


#include <Track.h>
#include <TMath.h>

#include "ControlPlotInfo.hxx"


#define build_string(expr)						\
    (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str())


using namespace genfit;


class ControlPlotsRepository {
  
public:
  
  static ControlPlotsRepository* Instance();
  static ControlPlotsRepository* GetControlObject( string outputDir );
  ~ControlPlotsRepository() {};
  
  void SetOutputDir( string outputDir ) { m_outputDir = outputDir; };
  void FillMap( string mapName, double x );
  void FillMap( string mapName, double x,  double y);
  void PrintMap();
  void PrintOutputFile();
  void PrintOutputNtuple();
  
  
  //change dir
  void SetControlPos_4eachState( string hitSampleName, int i, TVector3 *kal, TVector3 *trueMC, TVector3 *detector ) {
    
    FillMap( hitSampleName + "__State" +build_string(i)+ "__position__kalVStrue__mag", fabs( (*kal - *trueMC).Mag() ) );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__position__kalVSdet__mag",  fabs( (*kal - *detector).Mag() ) );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__position__trueVSdet__mag", fabs( (*trueMC - *detector).Mag() ) );
    
    FillMap( hitSampleName + "__State" +build_string(i)+ "__position__kalVStrue__x", fabs( (*kal - *trueMC).x() ) );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__position__kalVStrue__y", fabs( (*kal - *trueMC).y() ) );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__position__kalVStrue__z", fabs( (*kal - *trueMC).z() ) );
    
    FillMap( hitSampleName + "__State" +build_string(i)+ "__position__kalVSdet__x", fabs( (*kal - *detector).x() ) );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__position__kalVSdet__y", fabs( (*kal - *detector).y() ) );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__position__kalVSdet__z", fabs( (*kal - *detector).z() ) );
    
  }
  
  //change dir
  void SetControlMom_4eachState( string hitSampleName, int i, TVector3 *kal, TVector3 *trueMC, TVector3 *gen ) {
    
    FillMap( hitSampleName + "__State" +build_string(i)+ "__momentum__kalVStrue__mag", (kal->Mag() - trueMC->Mag()) / trueMC->Mag() );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__momentum__kalVSgen__mag",  (kal->Mag() - gen->Mag()) / gen->Mag() );
    
    FillMap( hitSampleName + "__State" +build_string(i)+ "__momentum__kalVStrue__angle",  (kal->Angle(*trueMC) * 180 / TMath::Pi() ) );
    
    FillMap( hitSampleName + "__State" +build_string(i)+ "__momentum__kalVStrue__x", ( kal->x() - trueMC->x() ) / trueMC->x() );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__momentum__kalVStrue__y", ( kal->x() - trueMC->x() ) / trueMC->y() );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__momentum__kalVStrue__z", ( kal->x() - trueMC->x() ) / trueMC->z() );
    
    FillMap( hitSampleName + "__State" +build_string(i)+ "__momentum__kalVSgen__x", ( kal->x() - gen->x() ) / gen->x() );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__momentum__kalVSgen__y", ( kal->x() - gen->x() ) / gen->y() );
    FillMap( hitSampleName + "__State" +build_string(i)+ "__momentum__kalVSgen__z", ( kal->x() - gen->x() ) / gen->z() );
    
  }
  
  
  void SetMom_Gen( string hitSampleName, TVector3 *p ) {
    
    FillMap( hitSampleName + "__momentum__gen__mag", p->Mag() );
    FillMap( hitSampleName + "__momentum__gen__x", p->x() );
    FillMap( hitSampleName + "__momentum__gen__y", p->y() );
    FillMap( hitSampleName + "__momentum__gen__z", p->z() );
    
  };
  
  
  void SetMom_TrueMC( string hitSampleName, TVector3 *p, double mass ) {
    
    FillMap( hitSampleName + "__momentum__trueMC__mag", p->Mag() );
    FillMap( hitSampleName + "__momentum__trueMC__x", p->x() );
    FillMap( hitSampleName + "__momentum__trueMC__y", p->y() );
    FillMap( hitSampleName + "__momentum__trueMC__z", p->z() );
    
    FillMap( hitSampleName + "__mass__trueMC", mass );
    
  };

  void SetMom_Kal( string hitSampleName, TVector3 *p, TVector3 *err ) {
    
    FillMap( hitSampleName + "__momentum__kalman__mag", p->Mag() );
    FillMap( hitSampleName + "__momentum__kalman__polarAngle", p->Theta() *180 / M_PI );
    // Normalised
    FillMap( hitSampleName + "__momentum__kalman__x", p->x() / p->Mag() );
    FillMap( hitSampleName + "__momentum__kalman__y", p->y() / p->Mag() );
    FillMap( hitSampleName + "__momentum__kalman__z", p->z() / p->Mag() );
    
    // err/mom 
    FillMap( hitSampleName + "__momentum__kalman__sigmax", sqrt(err->x()) / p->x() );
    FillMap( hitSampleName + "__momentum__kalman__sigmay", sqrt(err->y()) / p->y() );
    FillMap( hitSampleName + "__momentum__kalman__sigmaz", sqrt(err->z()) / p->z() );
    
  };


  // void SetPos_Gen( string hitSampleName, TVector3* p, TVector3* err ) { };
  // void SetPos_TrueMC( string hitSampleName, TVector3* p, TVector3* err ) { };
  void SetPos_Kal( string hitSampleName, TVector3* p, TVector3* err ) {

    double mag_err = (TVector3( sqrt(err->x()), sqrt(err->y()), sqrt(err->z()) )).Mag();
    FillMap( hitSampleName + "__position__kalman__mag", sqrt( mag_err )/ p->Mag() );
    FillMap( hitSampleName + "__position__kalman__x", sqrt( err->x() )/p->x() );
    FillMap( hitSampleName + "__position__kalman__y", sqrt( err->y() )/p->y() );
    FillMap( hitSampleName + "__position__kalman__z", sqrt( err->z() )/p->z() );
  };


  void SetTrackInfo( string hitSampleName, Track* track ) {
    FillMap( hitSampleName + "__chi2__kalman", track->getFitStatus(track->getCardinalRep())->getChi2() );
    // if ( m_debug > 0 )		cout << "Chi2 = "<< track->getFitStatus(track->getCardinalRep())->getChi2() << endl;
    
    // 		!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                ±  
    FillMap( hitSampleName + "__mass__kalman", track->getFittedState(0).getMass() );
    
    FillMap( hitSampleName + "__charge__kalman", track->getFitStatus(track->getCardinalRep())->getCharge() );
    // FillMap( hitSampleName + "__position__kalman__mag", track->getFittedState(i).getMass() );
    FillMap( hitSampleName + "__isFitConvergedFully__kalman", track->getFitStatus(track->getCardinalRep())->isFitConvergedFully() );
    FillMap( hitSampleName + "__isFitConvergedPartially__kalman", track->getFitStatus(track->getCardinalRep())->isFitConvergedPartially() );
    FillMap( hitSampleName + "__NFailedPoints__kalman", track->getFitStatus(track->getCardinalRep())->getNFailedPoints() );
    FillMap( hitSampleName + "__isTrackPruned__kalman", track->getFitStatus(track->getCardinalRep())->isTrackPruned() );
    FillMap( hitSampleName + "__number__ofDegreeOfFreedom__kalman", track->getFitStatus(track->getCardinalRep())->getNdf() );

    FillMap( hitSampleName + "__trackLenght__kalman", track->getTrackLen( track->getCardinalRep() ) );
    // FillMap( hitSampleName + "__position__kalman__mag", track->getTOF( track->getCardinalRep() ) );		// doesn't work!!!!
    // FillMap( hitSampleName + "__position__kalman__mag", (KalmanPos - tmpPos).Mag() );	
    
  };
  
  
  void SetCovarianceMomentum_2D( string hitSampleName, TMatrixD* KalmanPos_cov ) {
    
    // init
    // if ( h_covariance.find( hitSampleName ) == h_covariance.end() )					
    // 	h_covariance[hitSampleName] = new TH2F( "covariance", "covariance", 3, 0, 3, 3, 0, 3 );
    
    
    // fill
    // prduce the covariance matrix on the measured state
    // for ( int j=0; j<3; j++ ) {
    // 	for ( int k=0; k<3; k++ ) {
    // 		// USA KalmanMom_cov !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // 		// h_covariance[ hitSampleName ]->Fill( j, k, (track->getFittedState(i).get6DCov())[j+3][k+3] );
    // 	}
    // }

		
    //	print
    // 2D
    // for ( map< string, TH2F* >::iterator it=h_covariance.begin(); it != h_covariance.end(); it++ ) {

    // 	// (*it).second->GetXaxis()->SetTitle("p [GeV]");
    // 	(*it).second->Scale( 1./( (float)((*it).second->GetEntries())/9. ) );
    // 	gStyle->SetPalette(1);
    // 	mirror->SetLogz(1);
    // 	(*it).second->Draw("colz text");
    // 	mirror->SaveAs( ((string)getenv("FOOTRES")+"/"+(*it).first+"/"+"covariance" + ".png").c_str() );
    // 	mirror->SaveAs( ((string)getenv("FOOTRES")+"/"+(*it).first+"/"+"covariance" + ".root").c_str() );
    // 	mirror->SetLogz(0);
    // }

  }

  void SetMultiTrackPlots( string hitSampleName,  TVector3* Truepos, TVector3* Recopos) { 
   
    FillMap( hitSampleName + "__trueMC__x", Truepos->x() );
    FillMap( hitSampleName + "__trueMC__y", Truepos->y() );
    FillMap( hitSampleName + "__trueMC__z", Truepos->z() );

    FillMap( hitSampleName + "__recoMC__x", Recopos->x() );
    FillMap( hitSampleName + "__recoMC__y", Recopos->y() );
    FillMap( hitSampleName + "__recoMC__z", Recopos->z() );

    FillMap( hitSampleName + "__recoVStrue__x", Recopos->x(), Truepos->x());
    FillMap( hitSampleName + "__recoVStrue__y", Recopos->y(), Truepos->y());
    FillMap( hitSampleName + "__recoVStrue__z", Recopos->z(), Truepos->z());

    FillMap( hitSampleName + "__recoMC__zx", Recopos->z(), Recopos->x());
    FillMap( hitSampleName + "__recoMC__zy", Recopos->z(), Recopos->y());
    FillMap( hitSampleName + "__trueMC__zx", Truepos->z(), Truepos->x());
    FillMap( hitSampleName + "__trueMC__zy", Truepos->z(), Truepos->y());

    FillMap( hitSampleName + "__recoMC__xy", Recopos->x(), Recopos->y());
    FillMap( hitSampleName + "__trueMC__xy", Truepos->x(), Truepos->y());

    FillMap( hitSampleName + "__recotrue__diff__x", Recopos->x() - Truepos->x());
    FillMap( hitSampleName + "__recotrue__diff__y", Recopos->y() - Truepos->y());
    FillMap( hitSampleName + "__recotrue__diff__z", Recopos->z() - Truepos->z());
    

    //only for reco 
    float r=pow(Recopos->x()*Recopos->x() + Recopos->y()*Recopos->y() + Recopos->z()*Recopos->z(),0.5);	     
    double th=acos(Recopos->z()/r);
    double eta=-log(tan(0.5*th));
    double phi = atan2(Recopos->y(), Recopos->x());
    
    FillMap( hitSampleName + "__recoMC__rphi", r, phi);
    FillMap( hitSampleName + "__recoMC__etaphi", eta, phi);
    FillMap( hitSampleName + "__recoMC__thetaphi", th, phi);
    
    
  }
  
  void SetMultiTrackChi2( string hitSampleName,  double Chi2_zx, double Chi2_zy, bool Truetrack) { 
    FillMap( hitSampleName + "__Chi2__zx__all",  Chi2_zx);
    FillMap( hitSampleName + "__Chi2__zy__all",  Chi2_zy);
    FillMap( hitSampleName + "__Chi2zx__VS__Chi2zy__all",   Chi2_zx, Chi2_zy);
    if (!Truetrack) {
      FillMap( hitSampleName + "__Chi2__zx__nottrue",  Chi2_zx);
      FillMap( hitSampleName + "__Chi2__zy__nottrue",  Chi2_zy);
      FillMap( hitSampleName + "__Chi2zx__VS__Chi2zy__nottrue",   Chi2_zx, Chi2_zy);
    } else{
      FillMap( hitSampleName + "__Chi2__zx__true",  Chi2_zx);
      FillMap( hitSampleName + "__Chi2__zy__true",  Chi2_zy);
      FillMap( hitSampleName + "__Chi2zx__VS__Chi2zy__true",   Chi2_zx, Chi2_zy);	
    }
    
    for ( int j=1; j<11; j++ ) {
      if ((Chi2_zx < j) &&  (Chi2_zy < j)){
	if (!Truetrack)
	  { FillMap( hitSampleName + "__Chi2zxANDzy__<__" +build_string(j)+ " __nottrue",  Chi2_zx, Chi2_zy);
	  } else{ FillMap( hitSampleName + "__Chi2zxANDzy__<__" +build_string(j)+ " __true",  Chi2_zx, Chi2_zy);}
      }
      
      if ((Chi2_zx < j) ||  (Chi2_zy < j)){
	if (!Truetrack)
	  { FillMap( hitSampleName + "__Chi2zxORzy__<__" +build_string(j)+ " __nottrue",  Chi2_zx, Chi2_zy);
	  } else{ FillMap( hitSampleName + "__Chi2zxORzy__<__" +build_string(j)+ " __true",  Chi2_zx, Chi2_zy);}
      }
      
    }
  }
  
  void SetMultiTrackChi2PerFrag ( string hitSampleName,  double Chi2_zx, double Chi2_zy, string fragtype) {
    
    FillMap( hitSampleName + "__Chi2__zx__ChargeMass__"+fragtype,  Chi2_zx);
    FillMap( hitSampleName + "__Chi2__zy__ChargeMass__"+fragtype,  Chi2_zy);
    FillMap( hitSampleName + "__Chi2zx__VS__Chi2zy__"+fragtype,   Chi2_zx, Chi2_zy);
    
  }
  
  void SetMultiTrackHitInfo( string hitSampleName, int nVT,  int nIT, int nMSD){
    
    FillMap( hitSampleName + "__VT__hit",  nVT);
    FillMap( hitSampleName + "__IT__hit",  nIT);
    FillMap( hitSampleName + "__MSD__hit", nMSD);
    
    int TrackSystem__hit = nMSD+nIT+nVT;
    FillMap( hitSampleName + "__MC__hit", TrackSystem__hit);
    
    
  }

  void SetMultiTrackInfo( string hitSampleName, int trackonVT,  int trackonIT, int trackonMSD, int ntrackMC){
     
    FillMap( hitSampleName + "__VT__track" , trackonVT);
    FillMap( hitSampleName + "__IT__track" , trackonIT);
    FillMap( hitSampleName + "__MSD__track", trackonMSD);
    
    FillMap( hitSampleName + "__MC__Track", ntrackMC);
    
    
  }

  void SetVertexCoord(string hitSampleName, double zx_Vertex, double zy_Vertex ){

    if (fabs(zx_Vertex) < 0.1 && fabs(zy_Vertex) < 0.1){
      
      FillMap( hitSampleName + "__zxvert__z" , zx_Vertex);
      FillMap( hitSampleName + "__zyvert__z" , zy_Vertex);
      FillMap( hitSampleName + "__zxdiffzy__z", zx_Vertex-zy_Vertex );
      
    }
    
    else{
      if (fabs(zx_Vertex) >= 0.1) FillMap( hitSampleName + "__Nosensezx__z" , zx_Vertex);
      else if (fabs(zy_Vertex) >= 0.1) FillMap( hitSampleName + "__Nosensezy__z" , zy_Vertex);
    }
  }

  void SetClusterView( string hitSampleName, double x, double y  ) {
    FillMap( hitSampleName + "__clusterView",  x, y);
  }

  void SetTW_HitView( string hitSampleName, bool isCol, int bar  ) {
    if ( isCol )		FillMap( hitSampleName + "__column",  bar, (int)0 );
    else 				FillMap( hitSampleName + "__row",  (int)0, bar );
  }
  
  void SetTW_HitPoint( string hitSampleName, int x, int y  ) {
    FillMap( hitSampleName + "",  x, y);
  }
  
  
  struct Ntuple_out {
    vector< double >  Reco_track_px;
    vector< double >  Reco_track_py;
    vector< double >  Reco_track_pz;
    vector< double >  Reco_track_x;
    vector< double >  Reco_track_y;
    vector< double >  Reco_track_z;
    vector< double >  Truth_track_px;
    vector< double >  Truth_track_py;
    vector< double >  Truth_track_pz;
  };
  
  Ntuple_out  ntuple_out;
  
  void Set_Outputntuple( TVector3 *Reco_mom, TVector3 *Reco_pos, TVector3 *Truth_mom) {
    
    ntuple_out.Reco_track_px.push_back(Reco_mom->x());
    ntuple_out.Reco_track_py.push_back(Reco_mom->y());
    ntuple_out.Reco_track_pz.push_back(Reco_mom->z());
    
    ntuple_out.Reco_track_x.push_back(Reco_pos->x());
    ntuple_out.Reco_track_y.push_back(Reco_pos->y());
    ntuple_out.Reco_track_z.push_back(Reco_pos->z());
    
    ntuple_out.Truth_track_px.push_back(Truth_mom->x());
    ntuple_out.Truth_track_py.push_back(Truth_mom->y());
    ntuple_out.Truth_track_pz.push_back(Truth_mom->z());

  }
  
  
  
  bool m_alreadyPrintedOut;
  
  string m_lastPrintedDir;
  string m_outputDir;
  string m_outputfilename;
  string m_outputntuplename;
	
  map< string, ControlPlotInfo > m_controlPlotMapList;
  
  
private:
  
  ControlPlotsRepository();
  static ControlPlotsRepository* m_pInstance;
  
  
};

#endif
