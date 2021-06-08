

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
  
  int nNewParticles = 20;
  int pdgCode = 66666600;
  // particle name
  vector<string> nameVector 		 = { 	"C11", "C12", "C13", "C14", 
						"Li6", "Li7",
						"Be7", "Be9", "Be10",
						"B10", "B11",
						"N14", "N15",
						"He4", "He3", "H1", "H2", "H3",
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
			4, 3.016, 1, 2.014, 3.016,
			14, 14.88 };
  
  // particle cherge x3
  double chargeV [] = { 	18, 18, 18, 18,
				9, 9,
				12, 12, 12,
				15, 15, 
				21, 21,
				6, 6, 3, 3, 3,
				24, 24  };
  
  // check that every particle defined in the parameter file is defined in nameVector
  for ( unsigned int i=0; i<TAGrecoManager::GetPar()->MCParticles().size(); i++) {
    if ( find( nameVector.begin(), nameVector.end(), TAGrecoManager::GetPar()->MCParticles()[i] ) == nameVector.end() ) {
      cout << "ERROR::UpdatePDG::MakePdgDatabase()  -->  required " << TAGrecoManager::GetPar()->MCParticles()[i] << " particle from input parameter not defined" << endl;
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
  
  if ( !IsParticleDefined( partName ) ) return -1;
  
  return m_pdgCodeMap[ partName ];
}



//----------------------------------------------------------------------------------------------------
int UpdatePDG::GetPdgMass( string partName ) {
  
  if ( !IsParticleDefined( partName ) ) return -1;
  
  return TDatabasePDG::Instance()->GetParticle( GetPdgCode( partName ) )->Mass();
}


//----------------------------------------------------------------------------------------------------
int UpdatePDG::GetPdgMass( int PdgCode ) {
  
  string name = GetPdgName(PdgCode);
  return GetPdgMass( name );
}


//----------------------------------------------------------------------------------------------------
string UpdatePDG::GetPdgName( int pdgCode ) {
  return TDatabasePDG::Instance()->GetParticle( pdgCode )->GetName();
}



//----------------------------------------------------------------------------------------------------
int UpdatePDG::GetPdgCodeMainIsotope(int partCharge)
{
  string partName;
  switch(partCharge)
  {
    case 1: partName = "H1"; break;
    case 2: partName = "He4"; break;
    case 3: partName = "Li7"; break;
    case 4: partName = "Be9"; break;
    case 5: partName = "B11"; break;
    case 6: partName = "C12"; break;
    case 7: partName = "N14"; break;
    case 8: partName = "O16"; break;
    default: partName = "None"; break;
  }

  if(partName == "None")
  {
    // Error("GetPdgCodeMainIsotope()", "Particle charge is out of range! Charge::%d", partCharge);
    cout << "GetPgdCodeMainIsotope() --> Particle charge is out of range! Charge::" << partCharge << endl; exit(0);
    return -999;
  }
  else
  {
    return GetPdgCode(partName);
  }
}

