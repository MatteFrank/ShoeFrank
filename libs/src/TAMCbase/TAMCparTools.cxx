
#include "TDatabasePDG.h"

#include "TAMCparTools.hxx"
#include "TAGroot.hxx"
#include "GlobalPar.hxx"


//##############################################################################

/*!
 \class TAMCparTools TAMCparTools.hxx "TAMCparTools.hxx"
 \brief Helper for different conversion. **
 */

ClassImp(TAMCparTools);

map<TString, Int_t> TAMCparTools::fgkG4PartNameToFlukaId =
  {
   {"alpha",-6},      {"He3",-5},          {"triton",-4},     {"deuteron", -3},
   {"GenericIon",-2}, {"opticalphoton",-1},
   {"Ray", 0},        {"proton",1},        {"anti_proton",2}, {"e-",3},
   {"e+",4},          {"gamma",7},         {"neutron",8},     {"anti_neutron",9},
   {"mu+",10},        {"mu-",11},          {"Li6",-2},        {"Li7",-2},
   {"B7",-2},         {"B8",-2},           {"B9",-2},         {"B10",-2},
   {"Be9",-2},        {"Be10",-2},         {"Be11",-2},       {"C10",-2},
   {"C11",-2},        {"C12",-2},          {"C13",-2},        {"C14",-2},
   {"O14",-2},        {"O15",-2},          {"O16",-2},        {"O17",-2},
   {"N13",-2},        {"N14",-2},          {"N15",-2}
  };

map<Int_t, TString> TAMCparTools::fgFlukaIdToPartName =
  {
   {-1, "OPTIPHOT"},  {-2, "HEAVYION"},  {-3, "DEUTERON"},  {-4, "TRITON  "},
   {-5, "3-HELIUM"},  {-6, "4-HELIUM"},
   {0,  "RAY     "},  {1,  "PROTON  "},  {2,  "APROTON "},  {3,  "ELECTRON"},
   {4,  "POSITRON"},  {5,  "NEUTRIE "},  {6,  "ANEUTRIE"},  {7,  "PHOTON  "},
   {8,  "NEUTRON "},  {9,  "ANEUTRON"},  {10, "MUON+   "},  {11, "MUON-   "},
   {12, "KAONLONG"},  {13, "PION+   "},  {14, "PION-   "},  {15, "KAON+   "},
   {16, "KAON-   "},  {17, "LAMBDA  "},  {18, "ALAMBDA "},  {19, "KAONSHRT"},
   {20, "SIGMA-  "},  {21, "SIGMA+  "},  {22, "SIGMAZER"},  {23, "PIZERO  "},
   {24, "KAONZERO"},  {25, "AKAONZER"},  {26, "Reserved"},  {27, "NEUTRIM "},
   {28, "ANEUTRIM"},  {29, "Blank   "},  {30, "Reserved"},  {31, "ASIGMA- "},
   {32, "ASIGMAZE"},  {33, "ASIGMA+ "},  {34, "XSIZERO "},  {35, "AXSIZERO"},
   {36, "XSI-    "},  {37, "AXSI+   "},  {38, "OMEGA-  "},  {39, "AOMEGA+ "},
   {40, "Reserved"},  {41, "TAU+    "},  {42, "TAU-    "},  {43, "NEUTRIT "},
   {44, "ANEUTRIT"},  {45, "D+      "},  {46, "D-      "},  {47, "D0      "},
   {48, "D0BAR   "},  {49, "DS+     "},  {50, "DS-     "},  {51, "LAMBDAC+"},
   {52, "XSIC+   "},  {53, "XSIC0   "},  {54, "XSIPC+  "},  {55, "XSIPC0  "},
   {56, "OMEGAC0 "},  {57, "ALAMBDC-"},  {58, "AXSIC-  "},  {59, "AXSIC0  "},
   {60, "AXSIPC- "},  {61, "AXSIPC0 "},  {62, "AOMEGAC0"}
  };


//______________________________________________________________________________
TAMCparTools::TAMCparTools()
: TAGpara()
{
  // Standard constructor
  UpdatePDG();
}

//______________________________________________________________________________
TAMCparTools::~TAMCparTools()
{
  // Destructor
}

//______________________________________________________________________________
const Char_t* TAMCparTools::GetFlukaPartName(Int_t id)
{
   if( id < -40 || id >   63 ) { return Form("Invalid Particle ID %d", id); }
   if( id <  -6 && id >= -40 ) { return "NUC-EVAP"; }

   return fgFlukaIdToPartName[id].Data();   
}


//----------------------------------------------------------------------------------------------------
void TAMCparTools::UpdatePDG()
{
  // clean the particle datatbase. Important!
  TDatabasePDG::Instance()->~TDatabasePDG();
  
  int nNewParticles = 20;
  int pdgCode = 66666600;
  // particle name
  vector<string> nameVector      = {   "C11", "C12", "C13", "C14",
    "Li6", "Li7",
    "B7", "B9", "B10",
    "Be10", "Be11",
    "N14", "N15",
    "Alpha", "He3", "H", "H2", "H3",
    "O15", "O16" };
  if ( (int)nameVector.size() != nNewParticles )   {
    Error("UpdatePDG()", "Particle collection name size not match %ld", nameVector.size());
    exit(0);
  }
  
  // particle mass
  double massV [] = {   10.254, 11.1749, 12.1095, 13.07,
    5.612, 6.548,
    6.563, 8.357, 9.293,
    9.32444, 10.2525,
    13.1, 13.97,
    4, 3.016, 1, 2.014, 3.016,
    14, 14.88 };
  
  // particle cherge x3
  double chargeV [] = {   18, 18, 18, 18,
    9, 9,
    12, 12, 12,
    15, 15,
    21, 21,
    6, 6, 3, 3, 3,
    24, 24  };
  
  // check that every particle defined in the parameter file is defined in nameVector
  for ( unsigned int i=0; i<GlobalPar::GetPar()->MCParticles().size(); i++) {
    if ( find( nameVector.begin(), nameVector.end(), GlobalPar::GetPar()->MCParticles()[i] ) == nameVector.end() ) {
      Error("UpdatePDG()", "Required %s particle from input parameter not defined", GlobalPar::GetPar()->MCParticles()[i].data());
      exit(0);
    }
  }
  
  // add the new particles to the standard TDatabasePDG
  for ( int i=0; i<nNewParticles; i++) {
    TDatabasePDG::Instance()->AddParticle(nameVector[i].c_str(), nameVector[i].c_str(), massV[i], true, 0., chargeV[i], "ion", ++pdgCode);
    fPdgCodeMap[ nameVector[i] ] = pdgCode;
  }
  
}

//----------------------------------------------------------------------------------------------------
bool TAMCparTools::IsParticleDefined( string partName ){
  
  // check if the category is defined in fPdgCodeMap
  if ( fPdgCodeMap.find( partName ) == fPdgCodeMap.end() ) {
    cout << "ERROR :: UpdatePDG::IsParticleDefined  -->   in fPdgCodeMap not found the category " << partName << endl;
    return false;
  }
  else {
    return true;
  }
}


//----------------------------------------------------------------------------------------------------
int TAMCparTools::GetPdgCode( string partName ) {
  
  IsParticleDefined( partName );
  
  return fPdgCodeMap[ partName ];
}
