/*!
 \file UpdatePDG.cxx
 \brief  Updated PDG database used for GenFit Global Reconstruction
 \author M. Franchini, R. Ridolfi and R. Zarrella
*/

#include "UpdatePDG.hxx"

/*!
 \class UpdatePDG
 \brief Utility class used in GenFit global Reconstruction to handle all the possible particles detectable in the FOOT experiment

 The class defines an updated version of the PDG database used by GenFit
 */

// Global static pointer used to ensure a single instance of the class.
UpdatePDG* UpdatePDG::m_pInstance = NULL;


//----------------------------------------------------------------------------------------------------

//! \brief Instance of the UpdatePDG object
UpdatePDG* UpdatePDG::Instance()  {
  
  if (!m_pInstance) {  // Only allow one instance of class to be generated.
    m_pInstance = new UpdatePDG();
  }
  else {
    cout << "ERROR::UpdatePDG::Instance  -->  already instanced, cannot instance twice!!!" << endl, exit(42);
  }
  
  return m_pInstance;
}


//----------------------------------------------------------------------------------------------------

//! \brief Get the UpdatePDG object from outside class
UpdatePDG* UpdatePDG::GetPDG()  {
  
  if (!m_pInstance) 
    cout << "ERROR::UpdatePDG::GetPDG()  -->  called a get before UpdatePDG object istance." << endl, exit(42);
  
  return m_pInstance;
}


//----------------------------------------------------------------------------------------------------

//! \brief Default constructor
UpdatePDG::UpdatePDG() {
  
  MakePdgDatabase();
  
}

//----------------------------------------------------------------------------------------------------

//! \brief Default destructor
//!
//! CURRENTLY NOT USED -> CHECK
UpdatePDG::~UpdatePDG() {
}


//----------------------------------------------------------------------------------------------------

//! \brief Create the UpdatePDG database of all particles
void UpdatePDG::MakePdgDatabase() {
  
  
  // clean the particle datatbase. Important!
  TDatabasePDG::Instance()->~TDatabasePDG();
  
  int nNewParticles = 41;
  int pdgCode = 66666600;
  // particle name
  vector<string> nameVector 		 = {
			"H1", "H2", "H3",
			"He3", "He4", "He6", "He8",
			"Li6", "Li7", "Li8", "Li9",
			"Be7", "Be9", "Be10", "Be11", "Be12", "Be14",
			"B8", "B10", "B11", "B12", "B13", "B14", "B15",
			"C9", "C10", "C11", "C12", "C13", "C14", "C15", "C16",
			"N12", "N13", "N14", "N15", "N16",
			"O13", "O14", "O15", "O16" };
  if ( (int)nameVector.size() != nNewParticles ) 	{
    cout << "ERROR::UpdatePDG::MakePdgDatabase  -->  particle collection name size not match "<< nameVector.size() <<endl;
    exit(42);
  }
  
  // particle mass
  double massV [] = {
			1, 2.014, 3.016,
			3.016, 4, 5.604, 7.480,
			5.612, 6.548, 7.47, 8.405,
			6.563, 8.357, 9.293, 10.262, 11.198, 13.075,
			7.47, 9.32444, 10.2525, 11.187, 12.121, 13.059, 13.995,
    	8.409, 9.328, 10.254, 11.1749, 12.1095, 13.07, 13.976, 14.911,
			11.19, 12.11, 13.1, 13.97, 14.903,
			12.13, 13.04, 14, 14.88 };
  
  // particle cherge x3
  double chargeV [] = {
			3, 3, 3,
			6, 6, 6, 6,
			9, 9, 9, 9,
			12, 12, 12, 12, 12, 12,
			15, 15, 15, 15, 15, 15, 15,
    		18, 18, 18, 18, 18, 18, 18, 18,
			21, 21, 21, 21, 21,
			24, 24, 24, 24  };
  
  // check that every particle defined in the parameter file is defined in nameVector
  for ( unsigned int i=0; i<TAGrecoManager::GetPar()->MCParticles().size(); i++) {
    if ( find( nameVector.begin(), nameVector.end(), TAGrecoManager::GetPar()->MCParticles()[i] ) == nameVector.end() ) {
      cout << "ERROR::UpdatePDG::MakePdgDatabase()  -->  required " << TAGrecoManager::GetPar()->MCParticles()[i] << " particle from input parameter not defined" << endl;
      exit(42);
    }
  }
  
  // add the new particles to the standard TDatabasePDG
  for ( int i=0; i<nNewParticles; i++) {
    TDatabasePDG::Instance()->AddParticle(nameVector[i].c_str(), nameVector[i].c_str(), massV[i], true, 0., chargeV[i], "ion", ++pdgCode);	
    m_pdgCodeMap[ nameVector[i] ] = pdgCode;	
  }
  
}

//----------------------------------------------------------------------------------------------------

//! \brief Check if a particle is defined in the UpdatePDG database
//!
//! \param partName Name of the particle to check (e.g. "He4", "Li9"...)
//! \return True if the particle exists in the database, False otherwise
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

//! \brief Get the particle identifier in the UpdatePDG database
//!
//! \param partName Name of the particle to check (e.g. "He4", "Li9"...)
//! \return PDG identifier of the particle in the database
int UpdatePDG::GetPdgCode( string partName ) {
  
  if ( !IsParticleDefined( partName ) ) return -1;
  
  return m_pdgCodeMap[ partName ];
}



//----------------------------------------------------------------------------------------------------

//! \brief Get the particle mass from the UpdatePDG database
//!
//! \param partName Name of the particle to check (e.g. "He4", "Li9"...)
//! \return Mass in GeV of the particle
float UpdatePDG::GetPdgMass( const string &partName ) {
  
  if ( !IsParticleDefined( partName ) ) return -1;
  
  return TDatabasePDG::Instance()->GetParticle( GetPdgCode( partName ) )->Mass();
}


//----------------------------------------------------------------------------------------------------

//! \brief Get the particle mass from the UpdatePDG database
//!
//! \param PdgCode Particle identifier in the UpdatePDG database
//! \return Mass in GeV of the particle
float UpdatePDG::GetPdgMass( int PdgCode ) {
  
  if ( PdgCode == -999 )	return -1;

  string name = GetPdgName(PdgCode);
  return GetPdgMass( name );
}


//----------------------------------------------------------------------------------------------------

//! \brief Get the PDG name of a particle
//!
//! \param[in] pdgCode PDG identifier of the particle
//! \return Name of the particle in the UpdatePDG database
string UpdatePDG::GetPdgName( int pdgCode ) {
  return TDatabasePDG::Instance()->GetParticle( pdgCode )->GetName();
}



//----------------------------------------------------------------------------------------------------

//! \brief Get the PDG identifier of the most common isotope with a given charge
//!
//! \param[in] partCharge Charge of the particle
//! \return PDG identifier of the most common isotope with the wanted charge
int UpdatePDG::GetPdgCodeMainIsotope(int partCharge)  {

  string partName;
  switch(partCharge)  {
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

  if(partName == "None")  {
    // Error("GetPdgCodeMainIsotope()", "Particle charge is out of range! Charge::%d", partCharge);
    cout << "GetPgdCodeMainIsotope() --> Particle charge is out of range! Charge::" << partCharge << endl; exit(42);
    return -999;
  }
  else  {
    return GetPdgCode(partName);
  }
}


