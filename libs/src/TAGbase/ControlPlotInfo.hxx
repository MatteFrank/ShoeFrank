#ifndef ControlPlotInfo_H
#define ControlPlotInfo_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <sys/stat.h>

#include "TCanvas.h"
#include "TH1F.h"
#include "TVector3.h"
#include "GlobalPar.hxx"
#include "MagicSkills.hxx"
#include "TTree.h"



using namespace std;

class ControlPlotInfo {
  
public:
  
  ControlPlotInfo() {};

  ControlPlotInfo( string name, double x ) {
    m_name = name;
    m_variablesDoub_x.clear();
    m_variablesDoub_x.push_back( x );
    isDoub = true;
    isInt = false;
    isHisto2D = false;  
    Initialise();
  };

  ControlPlotInfo( string name, int x ) {
    m_name = name;
    m_variablesInt_x.clear();
    m_variablesInt_x.push_back( x );
    isInt = true;
    isDoub = false;
    isHisto2D = false;  
    Initialise();
  };

  ControlPlotInfo( string name, double x, double y) {
    m_name = name;
    m_variablesDoub_x.clear();
    m_variablesDoub_y.clear();
    m_variablesDoub_x.push_back( x );
    m_variablesDoub_y.push_back( y );
    isDoub = true;
    isInt = false;
    isHisto2D = true;  
    Initialise();
  };

  ControlPlotInfo( string name, int x, int y) {
    m_name = name;
    m_variablesInt_x.clear();
    m_variablesInt_y.clear();
    m_variablesInt_x.push_back( x );
    m_variablesInt_y.push_back( y );
    isInt = true;
    isDoub = false;
    isHisto2D = true;  
    Initialise();
  };
  
  
  ~ControlPlotInfo() {};
  
  
  void Initialise() {
    m_title = m_name;
    
    m_minBin_x = m_maxBin_x = -1;
    m_minBin_y = m_maxBin_y = -1;
    m_nBin = -1;
    
    m_titleX = "";
    m_titleY = "";
    m_dir = getenv("FOOTRES"); 
  };
  
  
  void addInfo( double x ) {
    if ( !isDoub )	{
      cout << "ERROR  >>  ControlPlotInfo::addInfo  ::  wrong type of the variable "<< m_name <<endl;
      exit(0);
    }
    m_variablesDoub_x.push_back( x );
  };
  
  void addInfo( int x ) {
    if ( !isInt )	{
      cout << "ERROR  >>  ControlPlotInfo::addInfo  ::  wrong type of the variable "<< m_name <<endl;
      exit(0);
    }
    m_variablesInt_x.push_back( x );
  };
  
  void addInfo( double x, double y ) {
    if ( !isDoub )	{
      cout << "ERROR  >>  ControlPlotInfo::addInfo  ::  wrong type of the variable "<< m_name <<endl;
      exit(0);
    }
    m_variablesDoub_x.push_back( x );
    m_variablesDoub_y.push_back( y );
  };
  
  void addInfo( int x, int y ) {
    if ( !isInt )	{
      cout << "ERROR  >>  ControlPlotInfo::addInfo  ::  wrong type of the variable "<< m_name <<endl;
      exit(0);
    }
    m_variablesInt_x.push_back( x );
    m_variablesInt_y.push_back( y );
  };
  
  
  
  
  void SetOutputDir( string outDir ) { 
    vector<string> token = Tokenize( m_name, "__" );
    m_dir = outDir +"/"+ token[0]; 
  };
  
  void PrintHisto( TCanvas* mirror);
  
  
  /***	variables	***/
  
  string m_name;
  string m_title;
  string m_titleX, m_titleY, m_dir;
  
  // vector<double> m_variablesDoub;
  // vector<double> m_variablesInt;
  
  vector<double> m_variablesDoub_x;
  vector<double> m_variablesInt_x;
  vector<double> m_variablesDoub_y;
  vector<double> m_variablesInt_y;
  
  bool isDoub;
  bool isInt;
  bool isHisto2D;
  
  
  int m_nBin, m_nBinX, m_nBinY;
  double m_minBin_x;
  double m_maxBin_x;
  
  double m_minBin_y;
  double m_maxBin_y;
  
  bool m_printoutfile;
  
private:
  
  TH1F* histo1D;
  TH2F* histo2D;
};

#endif
