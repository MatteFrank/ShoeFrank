#ifndef MAGICSKILLS_h
#define MAGICSKILLS_h


#include <TROOT.h>
#include "TFile.h"
#include <TSystem.h> 

// #include <TTree.h>
// #include "TH1.h"
// #include "TH1D.h"
#include "TH1F.h"
// #include <TF1.h>
// #include "TH2.h"
#include "TH2F.h"
// #include "TF2.h"
// #include <TGraph.h>
// #include <TGraphErrors.h>
#include "TGraphAsymmErrors.h"
// #include <TGraphPainter.h>
// #include <TAxis.h>
// #include "TProfile.h"
// #include "THStack.h"
// #include "TBox.h"

#include <TStyle.h>
#include <TLegend.h>
// #include <TCanvas.h>
// #include <TPaveStats.h>
// #include "TPaveText.h"
// #include "TArrayI.h"

// #include "TObject.h"
// #include "TKey.h"
// #include <TString.h>
// #include <TClass.h>
// #include "TVector3.h"
// #include "TRotation.h" 
// #include "TString.h"
// #include "TFormula.h"
// #include "TLorentzVector.h"
// #include "TRandom3.h"
// #include "TRandom.h"
// #include "TChain.h" 
// #include "TGenPhaseSpace.h"

#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <memory>

using namespace std;

#define LegX1 0.6
#define LegY1 0.6
#define LegX2 0.75
#define LegY2 0.75


vector<string> Tokenize(string inString, string divisor );

int ThinkPositive(int x) ;


// return num of signif  
// BG vers
int ForinosRule(double d) ;

void RemoveSpace( string* s );
double Round( double in, int precision );

//traspose a matrix
void Traspose(TH2F* h2);

//Replace part of "original" if it founds "erase" with "add". Otherwise return input string.
string StrReplace(string original, string erase, string add);

bool isInt(double x) ;

void StdLegend( TLegend* fLegend );

void ColorList () ;
/********************************************************/
double mcm (vector<double> x) ;
  
void SmartCopy( const TH1F * h_in, TH1F *&h_out );
  
void SmartCopy( const TH2F * h_in, TH2F *&h_out );
 

void DivideByBinWidth( TH1F * h );
  
  
 string MakeLatexArray( float up, float down );
void GetPlusAndMinus( float & u, float & d );
void MirrorRebin( TH2F * h_in );
void MirrorRebin( TH1F * h_in );


#endif

































