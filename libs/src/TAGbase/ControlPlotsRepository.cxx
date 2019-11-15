

#include "ControlPlotsRepository.hxx"


// Global static pointer used to ensure a single instance of the class.
ControlPlotsRepository* ControlPlotsRepository::m_pInstance = NULL;

ControlPlotsRepository::ControlPlotsRepository() {
  m_alreadyPrintedOut = false;
  m_lastPrintedDir = "none";
};

ControlPlotsRepository* ControlPlotsRepository::Instance() {
  if (!m_pInstance) {  // Only allow one instance of class to be generated.
    m_pInstance = new ControlPlotsRepository();
  }
  else {
    cout << "ERROR::ControlPlotsRepository::Instance  -->  already instanced, cannot instance twice!!!" << endl, exit(0);
  }

  return m_pInstance;
};

ControlPlotsRepository* ControlPlotsRepository::GetControlObject( string outputDir ) {
  if (!m_pInstance) 
    cout << "ERROR::ControlPlotsRepository::GetControlObject()  -->  called a get before GlobalPar object istance." << endl, exit(0);

  m_pInstance->SetOutputDir( outputDir );

  return m_pInstance;
};

void ControlPlotsRepository::FillMap( string mapName, double x ) {

  if ( m_controlPlotMapList.find( mapName ) == m_controlPlotMapList.end() ) {
    m_controlPlotMapList[ mapName ] = ControlPlotInfo( mapName, x );	// create a new map element
  }
  else {
    m_controlPlotMapList[ mapName ].addInfo( x );
  }
  m_controlPlotMapList[ mapName ].SetOutputDir( m_outputDir );

};

void ControlPlotsRepository::FillMap( string mapName, double x,  double y) {

  if ( m_controlPlotMapList.find( mapName ) == m_controlPlotMapList.end() ) {
    m_controlPlotMapList[ mapName ] = ControlPlotInfo( mapName, x, y );	// create a new map element
  }
  else {
    m_controlPlotMapList[ mapName ].addInfo( x, y );
  }
  m_controlPlotMapList[ mapName ].SetOutputDir( m_outputDir );
};

void ControlPlotsRepository::PrintMap() {

  if( m_alreadyPrintedOut ) {
    cout << "\n\n\nWARNING >> ControlPlotsRepository::PrintMap()  ::  Control plot already printed out. Plots will be re-written. Please, leave ONLY ONE PrintMap call.\n";
    cout << "Last call from the file that write out in the dir:  " << m_lastPrintedDir << endl<<endl<<endl;
  }

  m_alreadyPrintedOut = true;
  m_lastPrintedDir = m_outputDir;

  // fare struttura a cartelle come nel root file

  TCanvas* mirror = new TCanvas( "mirror", "mirror", 700, 700);
  for ( map< string, ControlPlotInfo >::iterator it = m_controlPlotMapList.begin(); it != m_controlPlotMapList.end(); it++ ) {

    (*it).second.PrintHisto( mirror);

	 
  }

};

void ControlPlotsRepository::PrintOutputFile() {

  m_outputfilename = GlobalPar::GetPar()->OutputFile();
  TFile* f_out = new TFile(m_outputfilename.c_str(),"RECREATE");  
	
  TCanvas* mirror = new TCanvas( "mirror", "mirror", 700, 700);

  for ( map< string, ControlPlotInfo >::iterator it = m_controlPlotMapList.begin(); it != m_controlPlotMapList.end(); it++ ) {
	  
    vector<string> token = Tokenize((*it).first, "__" );	
    string first_token =  token[0];	
    TDirectory *dir = f_out->GetDirectory(first_token.c_str());
	 
    if(!dir)	  
      {
	f_out->mkdir(first_token.c_str());
	f_out->cd(first_token.c_str()); 
      }
	  
    (*it).second.PrintHisto( mirror);	
	
  }
  f_out->Write();
  f_out->Close();
};
 
void ControlPlotsRepository::PrintOutputNtuple() {
  
  m_outputntuplename = GlobalPar::GetPar()->OutputNtuple();
  TFile* n_out = new TFile(m_outputntuplename.c_str(),"RECREATE");  

  //cout <<" outputntuplename  " << m_outputntuplename.c_str() << endl;

  TTree* tree_out = new TTree("EventTree","Reco Event Tree");

  for (int i = 0; i< ntuple_out.Reco_track_px.size(); i++){
    //cout <<" Reco_px "  << ntuple_out.Reco_px.at(i) << endl;
    tree_out->Branch("Reco_track_px",            &ntuple_out.Reco_track_px.at(i)   );
    tree_out->Branch("Reco_track_py",            &ntuple_out.Reco_track_py.at(i)   );
    tree_out->Branch("Reco_track_pz",            &ntuple_out.Reco_track_pz.at(i)   );
    tree_out->Branch("Reco_track_x",             &ntuple_out.Reco_track_x.at(i)    );
    tree_out->Branch("Reco_track_y",             &ntuple_out.Reco_track_y.at(i)    );
    tree_out->Branch("Reco_track_z",             &ntuple_out.Reco_track_z.at(i)    );
    tree_out->Branch("Truth_track_px",           &ntuple_out.Truth_track_px.at(i)  );
    tree_out->Branch("Truth_track_py",           &ntuple_out.Truth_track_py.at(i)  );
    tree_out->Branch("Truth_track_pz",           &ntuple_out.Truth_track_pz.at(i)  );

    tree_out->Fill();

  }

  n_out->Write();
  n_out->Close();
}
