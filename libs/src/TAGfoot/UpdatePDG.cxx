

#include "UpdatePDG.hxx"




// Global static pointer used to ensure a single instance of the class.
UpdatePDG* UpdatePDG::m_pInstance = NULL;





//----------------------------------------------------------------------------------------------------
UpdatePDG* UpdatePDG::Instance()  {
  
  if (!m_pInstance) {  // Only allow one instance of class to be generated.
    m_pInstance = new UpdatePDG();
  }
  else {
    cout << "ERROR::UpdatePDG::Instance  -->  already instanced, cannot instance twice!!!" << endl, exit(0);
  }
  
  return m_pInstance;
}





//----------------------------------------------------------------------------------------------------
UpdatePDG* UpdatePDG::GetPDG()  {
  
  if (!m_pInstance) 
    cout << "ERROR::UpdatePDG::GetPDG()  -->  called a get before UpdatePDG object istance." << endl, exit(0);
  
  return m_pInstance;
}





//----------------------------------------------------------------------------------------------------
UpdatePDG::UpdatePDG() {
  
  MakePdgDatabase();
  
}






//----------------------------------------------------------------------------------------------------
void UpdatePDG::MakePdgDatabase() {
  
  
  // clean the particle datatbase. Important!
  TDatabasePDG::Instance()->~TDatabasePDG();
  
  int nNewParticles = 17;
  int pdgCode = 66666600;
  // particle name
  vector<string> nameVector 		 = { 	"C11", "C12", "C13", "C14", 
						"Li6", "Li7",
						"B7", "B9", "B10",
						"Be10", "Be11",
						"N14", "N15",
						"Alpha", "H",
						"O15", "O16" };
  if ( (int)nameVector.size() != nNewParticles ) 	{
    cout << "ERROR::UpdatePDG::MakePdgDatabase  -->  particle collection name size not match "<< nameVector.size() <<endl;
    exit(0);
  }
  
  // particle mass
  double massV [] = { 	10.254, 11.1749, 12.1095, 13.07,
			5.612, 6.548,
			6.563, 8.357, 9.293,
			9.32444, 10.2525,
			13.1, 13.97,
			4, 1,
			14, 14.88 };
  
  // particle cherge x3
  double chargeV [] = { 	18, 18, 18, 18,
				9, 9,
				12, 12, 12,
				15, 15, 
				21, 21,
				6, 3,
				24, 24  };
  
  
  // int nNewParticles = 18;
  // int pdgCode = 66666600;
  // // particle name
  // vector<string> nameVector 		 = { 	"C10", "C11", "C12", 
  // 										"Li6", "Li7",
  // 										"B7", "B8", "B9",
  // 										"Be9", "Be10", "Be11",
  // 										"N12", "N13", "N14",
  // 										"Alpha", "H",
  // 										"O15", "O16" };
  // if ( (int)nameVector.size() != nNewParticles ) 	{
  // 	cout << "ERROR::UpdatePDG::MakePdgDatabase  -->  particle collection name size not match "<< nameVector.size() <<endl;
  // 	exit(0);
  // }
  
  // // particle mass
  // double massV [] = { 	10.254, 11.1749, 12.1095, 
  // 									6.53383, 7,
  // 									7, 8, 9.3255,
  // 									9.32444, 10.2525, 11,
  // 									12.1112, 13, 14,
  // 									4, 1,
  // 									15, 16 };
  
  // // particle cherge x3
  // double chargeV [] = { 	18, 18, 18, 
  // 									9, 9,
  // 									12, 12, 12,
  // 									15, 15, 15,
  // 									21, 21, 21,
  // 									6, 3,
  // 									24, 24  };
  
  // check that every particle defined in the parameter file is defined in nameVector
  for ( unsigned int i=0; i<GlobalPar::GetPar()->MCParticles().size(); i++) {
    if ( find( nameVector.begin(), nameVector.end(), GlobalPar::GetPar()->MCParticles()[i] ) == nameVector.end() ) {
      cout << "ERROR::UpdatePDG::MakePdgDatabase()  -->  required " << GlobalPar::GetPar()->MCParticles()[i] << " particle from input parameter not defined" << endl;
      exit(0);
    }
  }
  
  // add the new particles to the standard TDatabasePDG
  for ( int i=0; i<nNewParticles; i++) {
    TDatabasePDG::Instance()->AddParticle(nameVector[i].c_str(), nameVector[i].c_str(), massV[i], true, 0., chargeV[i], "ion", ++pdgCode);	
    m_pdgCodeMap[ nameVector[i] ] = pdgCode;	
  }
  
}

//----------------------------------------------------------------------------------------------------
bool UpdatePDG::IsParticleDefined( string partName ){
  
  // check if the category is defined in m_pdgCodeMap
  if ( m_pdgCodeMap.find( partName ) == m_pdgCodeMap.end() ) {
    cout << "ERROR :: UpdatePDG::IsParticleDefined  -->	 in m_pdgCodeMap not found the category " << partName << endl;
    return false;
  }
  else {
    return true;
  }
}


//----------------------------------------------------------------------------------------------------
int UpdatePDG::GetPdgCode( string partName ) {
  
  IsParticleDefined( partName );
  
  return m_pdgCodeMap[ partName ];
}

