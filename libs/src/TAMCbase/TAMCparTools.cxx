
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

map<TString, Int_t> TAMCparTools::fgkUpdatePdgMap =
{
  {"C11",   66666601}, {"C12",  66666602}, {"C13", 66666603}, {"C14", 66666604},
  {"Li6",   66666605}, {"Li7",  66666606},
  {"B7",    66666607}, {"B9",   66666608}, {"B10", 66666609},
  {"Be10",  66666610}, {"Be11", 66666611},
  {"N14",   66666612}, {"N15",  66666613},
  {"Alpha", 66666614}, {"He3",  66666615},
  {"H",     66666616}, {"H2",   66666617}, {"H3", 66666618},
  {"O15",   66666619}, {"O16",  66666620}
};

// mass = u*0.9315 in GeV/c^2
map<TString, double> TAMCparTools::fgkUpdatePdgMass =
{
  {"H",     1.0},     {"H2",   2.014},   {"H3", 3.016},
  {"Alpha", 4.0},     {"He3",  3.016},
  {"Li6",   5.612},   {"Li7",  6.548},
  {"Be7",   6.563},   {"Be9",  8.357},   {"Be10", 9.293},
  {"B10",   9.32444}, {"B11", 10.2525},
  {"C11",   10.254},  {"C12",  11.1749}, {"C13", 12.1095}, {"C14", 13.07},
  {"N14",   13.1},    {"N15",  13.97},
  {"O15",   14.0},    {"O16",  14.88}
};

// charge = Z*3 (counting in 1/3q)
map<TString, double> TAMCparTools::fgkUpdatePdgCharge =
{
  {"H",     1},  {"H2",   1},  {"H3", 1},
  {"Alpha", 2},  {"He3",  2},
  {"Li6",   3},  {"Li7",  3},
  {"Be7",   4},  {"Be9",  4},  {"Be10", 4},
  {"B10",   5},  {"B11",  5},
  {"C11",   6},  {"C12",  6},  {"C13", 6}, {"C14", 6},
  {"N14",   7},  {"N15",  7},
  {"O15",   8},  {"O16",  8}
};

//______________________________________________________________________________
TAMCparTools::TAMCparTools()
: TAGpara()
{
  // Standard constructor
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
  
  // check that every particle defined in the parameter file is defined in nameVector
  for ( unsigned int i=0; i<GlobalPar::GetPar()->MCParticles().size(); i++) {
    if ( fgkUpdatePdgMap.find( GlobalPar::GetPar()->MCParticles()[i] ) == fgkUpdatePdgMap.end() ) {
      cout << "ERROR :: TAMCparTools::UpdatePDG  -->   Required %s particle from input parameter not defined" << GlobalPar::GetPar()->MCParticles()[i] << endl;
      exit(0);
    }
  }
  
  // add the new particles to the standard TDatabasePDG
  for (map<TString, Int_t>::iterator it=fgkUpdatePdgMap.begin(); it!=fgkUpdatePdgMap.end(); ++it) {
    TDatabasePDG::Instance()->AddParticle(it->first.Data(), it->first.Data(), fgkUpdatePdgMass[it->first], true, 0., fgkUpdatePdgCharge[it->first]*3.0, "ion", it->second);
   // cout << it->first.Data() << " " << it->second << " " << fgkUpdatePdgMass[it->first] << " " << fgkUpdatePdgCharge[it->first] << endl;
  }
}

//----------------------------------------------------------------------------------------------------
bool TAMCparTools::IsParticleDefined( string partName ){
  
  // check if the category is defined in fPdgCodeMap
  if ( fgkUpdatePdgMap.find( partName ) == fgkUpdatePdgMap.end() ) {
    cout << "ERROR :: TAMCparTools::IsParticleDefined  -->   in fPdgCodeMap not found the category " << partName << endl;
    return false;
  }
  else {
    return true;
  }
}


//----------------------------------------------------------------------------------------------------
int TAMCparTools::GetPdgCode( string partName ) {
  
  IsParticleDefined( partName );
  
  return fgkUpdatePdgMap[ partName ];
}
