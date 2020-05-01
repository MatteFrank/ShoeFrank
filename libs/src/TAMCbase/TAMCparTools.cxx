

#include "TAMCparTools.hxx"
#include "TAGroot.hxx"


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
const Char_t* TAMCparTools::GetFlukaPartName(Int_t id)
{
   if( id < -40 || id >   63 ) { return Form("Invalid Particle ID %d", id); }
   if( id <  -6 && id >= -40 ) { return "NUC-EVAP"; }

   return fgFlukaIdToPartName[id].Data();   
}

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
