
#include "ControlPlotInfo.hxx"

void  ControlPlotInfo::PrintHisto( TCanvas* mirror) {
  // check if parameter from file is enabled		-> 		otherwise guess them

  // nBin
  if ( GlobalPar::GetPar()->GetLowBinHisto( m_name ) != -666 )
    m_nBin = GlobalPar::GetPar()->GetLowBinHisto( m_name ); 
  else 
    m_nBin = 100;

  m_nBinX = 100;
  m_nBinY = 100;

  // range
  if ( GlobalPar::GetPar()->GetNBinHisto( m_name ) != -666 ) {
    m_minBin_x = GlobalPar::GetPar()->GetNBinHisto( m_name ); 
    m_maxBin_x = GlobalPar::GetPar()->GetUpBinHisto( m_name ); 

    m_minBin_y = GlobalPar::GetPar()->GetNBinHisto( m_name ); 
    m_maxBin_y = GlobalPar::GetPar()->GetUpBinHisto( m_name ); 


  }

  else {

    if(!isHisto2D){
      // automatic calculation of the range if not defined in the parameters
      if ( isDoub ) {
	double min_x =  *min_element( m_variablesDoub_x.begin(), m_variablesDoub_x.end() );
	double max_x =  *max_element( m_variablesDoub_x.begin(), m_variablesDoub_x.end() );
	      
	double length_x = max_x - min_x;
	m_minBin_x = min_x - length_x*0.1;
	m_maxBin_x = max_x + length_x*0.1;
      }
      
      if ( isInt ) {
	m_minBin_x = *min_element( m_variablesInt_x.begin(), m_variablesInt_x.end() ) - 1;
	m_maxBin_x = *max_element( m_variablesInt_x.begin(), m_variablesInt_x.end() ) + 1;
	if ( GlobalPar::GetPar()->GetLowBinHisto( m_name ) == -666 )
	  m_nBin =  m_maxBin_x - m_maxBin_x + 2;
      }
    }

    else if(isHisto2D){
	    
      // automatic calculation of the range if not defined in the parameters
      if ( isDoub ) {
	double min_x =  *min_element( m_variablesDoub_x.begin(), m_variablesDoub_x.end() );
	double max_x =  *max_element( m_variablesDoub_x.begin(), m_variablesDoub_x.end() );

	double min_y =  *min_element( m_variablesDoub_y.begin(), m_variablesDoub_y.end() );
	double max_y =  *max_element( m_variablesDoub_y.begin(), m_variablesDoub_y.end() );

	double length_x = max_x - min_x;
	m_minBin_x = min_x - length_x*0.1;
	m_maxBin_x = max_x + length_x*0.1;

	double length_y = max_y - min_y;
	m_minBin_y = min_y - length_y*0.1;
	m_maxBin_y = max_y + length_y*0.1;
	      
	      
      }

      if ( isInt ) {
	m_minBin_x = *min_element( m_variablesInt_x.begin(), m_variablesInt_x.end() ) - 1;
	m_maxBin_x = *max_element( m_variablesInt_x.begin(), m_variablesInt_x.end() ) + 1;
	if ( GlobalPar::GetPar()->GetLowBinHisto( m_name ) == -666 )
	  m_nBin =  m_maxBin_x - m_maxBin_x + 2;

	m_minBin_y = *min_element( m_variablesInt_y.begin(), m_variablesInt_y.end() ) - 1;
	m_maxBin_y = *max_element( m_variablesInt_y.begin(), m_variablesInt_y.end() ) + 1;
	// if ( GlobalPar::GetPar()->GetLowBinHisto( m_name ) == -666 )
	// 	m_nBin =  m_maxBin_x - m_maxBin_x + 2;

      }
	    
      // if ( GlobalPar::GetPar()->GetLowBinHisto( m_name ) == -666 )

      if ( m_name.find("Hit__column") != string::npos ) {
	m_minBin_x = 0;
	m_maxBin_x = 21;
	m_minBin_y = 0;
	m_maxBin_y = 1;
	m_nBinX =  22;
	m_nBinY =  1;
      }
      if ( m_name.find("Hit__row") != string::npos ) {
	m_minBin_x = 0;
	m_maxBin_x = 1;
	m_minBin_y = 0;
	m_maxBin_y = 21;
	m_nBinX =  1;
	m_nBinY =  22;
      }
      if ( m_name.find("Point") != string::npos ) {
	m_minBin_x = 0;
	m_maxBin_x = 21;
	m_minBin_y = 0;
	m_maxBin_y = 21;
	m_nBinX =  22;
	m_nBinY =  22;
      }
	    

    }
	    
  }

  // saving directory
  if ( GlobalPar::GetPar()->GetSaveDirHisto( m_name ) != "default" ) 
    m_dir = GlobalPar::GetPar()->GetSaveDirHisto( m_name ); 
  // x axis title
  if ( GlobalPar::GetPar()->GetXTitlesHisto( m_name ) != "default" ) 
    m_titleX = GlobalPar::GetPar()->GetXTitlesHisto( m_name ); 
  // y axis title
  if ( GlobalPar::GetPar()->GetYTitlesHisto( m_name ) != "default" ) 
    m_titleY = GlobalPar::GetPar()->GetYTitlesHisto( m_name ); 


	
  if(!isHisto2D){
    // init histo 1D
    histo1D = new TH1F( m_name.c_str(), m_title.c_str(), m_nBin, m_minBin_x, m_maxBin_x );
	  
    // fill histo 1D
    if (isDoub) {
      for (unsigned int i=0; i<m_variablesDoub_x.size(); i++) 
	histo1D->Fill( m_variablesDoub_x.at(i) );
    }
    if (isInt) {
      for (unsigned int i=0; i<m_variablesInt_x.size(); i++) 
	histo1D->Fill( m_variablesInt_x.at(i) );
    }
	  
    if ( m_titleX != "" )		histo1D->GetXaxis()->SetTitle( m_titleX.c_str() );
    if ( m_titleY != "" )		histo1D->GetYaxis()->SetTitle( m_titleY.c_str() );
	  
  }
  else if(isHisto2D){
    // init histo 2D
    histo2D = new TH2F( m_name.c_str(), m_title.c_str(), m_nBinX, m_minBin_x, m_maxBin_x, m_nBinY, m_minBin_y, m_maxBin_y );
	  
    // fill histo 2D
    if (isDoub) {
      for (unsigned int i=0; i<m_variablesDoub_x.size(); i++) 
	histo2D->Fill( m_variablesDoub_x.at(i),  m_variablesDoub_y.at(i));
	    
    }
    if (isInt) {
      for (unsigned int i=0; i<m_variablesInt_x.size(); i++) 
	histo2D->Fill( m_variablesInt_x.at(i),m_variablesInt_y.at(i) );
    }
	  
    if ( m_titleX != "" )		histo2D->GetXaxis()->SetTitle( m_titleX.c_str() );
    if ( m_titleY != "" )		histo2D->GetYaxis()->SetTitle( m_titleY.c_str() );
	  

  }

  m_printoutfile = GlobalPar::GetPar()->IsPrintOutputFile();

  if (!m_printoutfile) { 

    struct stat info;
    if( stat( m_dir.c_str(), &info ) != 0 )		//cannot access
      system(("mkdir "+m_dir).c_str());

    // save histo
    //	histo->Draw();
    mirror->SaveAs(  (m_dir + "/" + m_name + ".png").c_str() ); // very slow operation !!!!!!!!!!!!!!!!
  }

  // clear histo ???

};

