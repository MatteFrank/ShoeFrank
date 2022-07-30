

#include <TROOT.h>
#include <TMath.h>

#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include "TADIparGeo.hxx"
#include "TASTparGeo.hxx"
#include "TABMparGeo.hxx"
#include "TAVTparGeo.hxx"
#include "TAITparGeo.hxx"
#include "TAMSDparGeo.hxx"
#include "TATWparGeo.hxx"
#include "TACAparGeo.hxx"
#include "TAGparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGmaterials.hxx"
#include "TAGroot.hxx"

#include "TAGrecoManager.hxx"
#include <FieldManager.h>

#include "TAGcampaignManager.hxx"

using namespace std;


int main (int argc, char *argv[]) {

    TString exp("");
    Int_t runNumber = -1;
   
    for (int i = 0; i < argc; i++) {
       if(strcmp(argv[i],"-exp") == 0)
          exp = TString(argv[++i]);  // experiment name
       
        if(strcmp(argv[i],"-run") == 0)
           runNumber = TString(argv[++i]).Atoi();
    }

    clock_t start_tot, end_tot;

    // start time
    start_tot = clock();
    time_t now = time(0);
    tm *ltm = localtime(&now);
    cout << "Date: "<<  ltm->tm_mday <<" / "<< 1 + ltm->tm_mon
        <<" / "<< 1900 + ltm->tm_year << "  Time: "<< 1 + ltm->tm_hour << ":";
    cout << 1 + ltm->tm_min << ":";
    cout << 1 + ltm->tm_sec << endl;


    cout << "Hello Footer!" << endl;

    // real coding starts here!

    cout<<" GlobalPar "<<Form("%s/FootGlobal.par",exp.Data())<<endl;
    TAGrecoManager::Instance(exp);
    TAGrecoManager::GetPar()->FromFile();
    TAGrecoManager::GetPar()->Print();

    TAGroot* fTAGroot = new TAGroot();
    TAGmaterials* fTAGmat = new TAGmaterials();

    TAGgeoTrafo geoTrafo;

    TADIparGeo* diGeo = 0x0;
    TASTparGeo* stcGeo = 0x0;
    TABMparGeo* bmGeo = 0x0;
    TAVTparGeo* vtxGeo = 0x0;
    TAITparGeo* itrGeo = 0x0;
    TAMSDparGeo* msdGeo = 0x0;
    TATWparGeo* twGeo = 0x0;
    TACAparGeo* caGeo = 0x0;
      
   diGeo = new TADIparGeo();
   if (TAGrecoManager::GetPar()->IncludeST())
      stcGeo = new TASTparGeo();
   if (TAGrecoManager::GetPar()->IncludeBM())
      bmGeo = new TABMparGeo();
   if (TAGrecoManager::GetPar()->IncludeVT())
      vtxGeo = new TAVTparGeo();
   if (TAGrecoManager::GetPar()->IncludeIT())
      itrGeo = new TAITparGeo();
   if (TAGrecoManager::GetPar()->IncludeMSD())
      msdGeo = new TAMSDparGeo();
   if (TAGrecoManager::GetPar()->IncludeTW())
      twGeo = new TATWparGeo();
   if (TAGrecoManager::GetPar()->IncludeCA())
      caGeo = new TACAparGeo();
   
   // always there
   TAGparGeo* generalGeo = new TAGparGeo();

    // load campaign file
    TAGcampaignManager* campManager = new TAGcampaignManager(exp);
    campManager->FromFile();
   
    // read geomap files
    TString parFileName;
    parFileName = campManager->GetCurGeoFile(TAGgeoTrafo::GetBaseName(), runNumber);

    cout<<" From TCAmpaign manager! "<<parFileName.Data()<<endl;
    geoTrafo.FromFile(parFileName.Data());

    parFileName = campManager->GetCurGeoFile(TAGparGeo::GetBaseName(), runNumber);
    generalGeo->FromFile(parFileName.Data());
   
   if (TAGrecoManager::GetPar()->IncludeST()) {
    parFileName = campManager->GetCurGeoFile(TASTparGeo::GetBaseName(), runNumber);
    stcGeo->FromFile(parFileName.Data());
   }
   if (TAGrecoManager::GetPar()->IncludeBM()) {
      parFileName = campManager->GetCurGeoFile(TABMparGeo::GetBaseName(), runNumber);
      bmGeo->FromFile(parFileName.Data());
   }
   if (TAGrecoManager::GetPar()->IncludeVT()) {
      vtxGeo->SetMcFlag();
      parFileName = campManager->GetCurGeoFile(TAVTparGeo::GetBaseName(), runNumber);
      vtxGeo->FromFile(parFileName.Data());
   }
   if (TAGrecoManager::GetPar()->IncludeIT()) {
      parFileName = campManager->GetCurGeoFile(TAITparGeo::GetBaseName(), runNumber);
      itrGeo->FromFile(parFileName.Data());
   }
   if (TAGrecoManager::GetPar()->IncludeMSD()) {
      parFileName = campManager->GetCurGeoFile(TAMSDparGeo::GetBaseName(), runNumber);
      msdGeo->FromFile(parFileName.Data());
   }
   if (TAGrecoManager::GetPar()->IncludeDI()) {
    parFileName = campManager->GetCurGeoFile(TADIparGeo::GetBaseName(), runNumber);
    diGeo->FromFile(parFileName.Data());
   }
   if (TAGrecoManager::GetPar()->IncludeTW()) {
      parFileName = campManager->GetCurGeoFile(TATWparGeo::GetBaseName(), runNumber);
      twGeo->FromFile(parFileName.Data());
   }
   if (TAGrecoManager::GetPar()->IncludeCA()) {
    parFileName = campManager->GetCurGeoFile(TACAparGeo::GetBaseName(), runNumber);
    caGeo->FromFile(parFileName.Data());
   }
  
    ifstream file;
    string fileName = Form("foot.inp");
    file.open( fileName.c_str(), ios::in );
    if ( !file.is_open() )        cout<< "ERROR  -->  wrong input in TAGrecoManager::ReadParemFile file:: "<<fileName.c_str()<< endl, exit(0);

    string line = "";
    stringstream init, geomat, end;
    bool readInit = true;
    bool readGeoMat = false;
    bool readEnd = false;
    // read the old file
    while( getline( file, line ) ) {

      if ( readInit )           init    << line << endl;

      if ( line == "* @@@START GENERATED, DO NOT MODIFY:GENERAL@@@ *********************************" )
	readInit = false;

      if ( line == "* @@@END GENERATED:GENERAL@@@ **************************************************" )
	readGeoMat = true;

      if ( readGeoMat )         geomat  << line << endl;

      if ( line == "* @@@START GENERATED, DO NOT MODIFY:MATERIAL&MAGFIELD@@@ ***********************" )
	readGeoMat = false;

      if ( line == "* @@@END GENERATED:MATERIAL&MAGFIELD@@@ ****************************************" )
	readEnd = true;

      if ( readEnd )            end     << line << endl;

    }


    file.close();


    // PRINT OUT foot.geo -> FLUKA geometry file
    string geofileName = Form("foot.geo");
    ofstream geofile;
    geofile.open( geofileName.c_str(), std::ofstream::out | std::ofstream::trunc );

    geofile << "    0    0          FOOT experiment geometry" << endl;
    geofile <<"* ******************************************************************************"<<endl;
    geofile <<"*                         BODIES                                               *"<<endl;
    geofile <<"* ******************************************************************************"<<endl;

    //print bodies
    geofile << generalGeo->PrintStandardBodies(  );
   if (TAGrecoManager::GetPar()->IncludeST())
      geofile << stcGeo->PrintBodies(  );
   if (TAGrecoManager::GetPar()->IncludeBM())
      geofile << bmGeo->PrintBodies(  );
   if (TAGrecoManager::GetPar()->IncludeTG())
    geofile << generalGeo->PrintTargBody(  );
   if (TAGrecoManager::GetPar()->IncludeVT())
      geofile << vtxGeo->PrintBodies(  );
   if (TAGrecoManager::GetPar()->IncludeIT())
      geofile << itrGeo->PrintBodies(  );
   if (TAGrecoManager::GetPar()->IncludeMSD())
      geofile << msdGeo->PrintBodies(  );
   if (TAGrecoManager::GetPar()->IncludeDI())
      geofile << diGeo->PrintBodies(  );
   if (TAGrecoManager::GetPar()->IncludeTW())
      geofile << twGeo->PrintBodies(  );
   if (TAGrecoManager::GetPar()->IncludeCA())
      geofile << caGeo->PrintBodies(  );

    // end print bodies
    geofile << "END        " <<endl;

    // region print begins here
    geofile <<"* ******************************************************************************"<<endl;
    geofile <<"*                         REGIONS                                              *"<<endl;
    geofile <<"* ******************************************************************************"<<endl;
    //print regions
    geofile << generalGeo->PrintStandardRegions1();
   if (TAGrecoManager::GetPar()->IncludeST())
      geofile << stcGeo->PrintSubtractBodiesFromAir();
   if (TAGrecoManager::GetPar()->IncludeBM())
      geofile << bmGeo->PrintSubtractBodiesFromAir();
   if (TAGrecoManager::GetPar()->IncludeTG())
      geofile << generalGeo->PrintSubtractTargBodyFromAir();
   if (TAGrecoManager::GetPar()->IncludeVT())
      geofile << vtxGeo->PrintSubtractBodiesFromAir();
   if (TAGrecoManager::GetPar()->IncludeIT())
      geofile << itrGeo->PrintSubtractBodiesFromAir();
   if (TAGrecoManager::GetPar()->IncludeMSD())
      geofile << msdGeo->PrintSubtractBodiesFromAir();
   if (TAGrecoManager::GetPar()->IncludeDI())
      geofile << diGeo->PrintSubtractBodiesFromAir();
   if (TAGrecoManager::GetPar()->IncludeTG())
      geofile << generalGeo->PrintStandardRegions2();
   if (TAGrecoManager::GetPar()->IncludeTW())
      geofile << twGeo->PrintSubtractBodiesFromAir();
   if (TAGrecoManager::GetPar()->IncludeCA())
      geofile << caGeo->PrintSubtractBodiesFromAir();
   
   if (TAGrecoManager::GetPar()->IncludeST())
      geofile << stcGeo->PrintRegions();
   if (TAGrecoManager::GetPar()->IncludeBM())
      geofile << bmGeo->PrintRegions();
   if (TAGrecoManager::GetPar()->IncludeTG())
      geofile << generalGeo->PrintTargRegion();
   if (TAGrecoManager::GetPar()->IncludeVT())
      geofile << vtxGeo->PrintRegions();
   if (TAGrecoManager::GetPar()->IncludeIT())
      geofile << itrGeo->PrintRegions();
   if (TAGrecoManager::GetPar()->IncludeMSD())
      geofile << msdGeo->PrintRegions(  );
   if (TAGrecoManager::GetPar()->IncludeDI())
      geofile << diGeo->PrintRegions(  );
   if (TAGrecoManager::GetPar()->IncludeTW())
      geofile << twGeo->PrintRegions(  );
   if (TAGrecoManager::GetPar()->IncludeCA())
      geofile << caGeo->PrintRegions(  );

    // end print regions
    geofile << "END        " <<endl;

    // close geometry file
    geofile.close();

    // rewrite the input file in the correct way
    ofstream outfile;
    outfile.open( fileName.c_str(), fstream::trunc );

    outfile << init.str();

    outfile << generalGeo->PrintBeam();
    outfile << generalGeo->PrintPhysics();

    outfile << geomat.str();

    //print materials and compounds
    outfile << fTAGmat->PrintMaterialFluka();

    //print assig nmaterials
    outfile << generalGeo->PrintStandardAssignMaterial();
   if (TAGrecoManager::GetPar()->IncludeST())
      outfile << stcGeo->PrintAssignMaterial(fTAGmat);
   if (TAGrecoManager::GetPar()->IncludeBM())
      outfile << bmGeo->PrintAssignMaterial(fTAGmat);
   if (TAGrecoManager::GetPar()->IncludeTG())
    outfile << generalGeo->PrintTargAssignMaterial(fTAGmat);
   if (TAGrecoManager::GetPar()->IncludeVT())
    outfile << vtxGeo->PrintAssignMaterial(fTAGmat);
   if (TAGrecoManager::GetPar()->IncludeIT())
    outfile << itrGeo->PrintAssignMaterial(fTAGmat);
   if (TAGrecoManager::GetPar()->IncludeMSD())
      outfile << msdGeo->PrintAssignMaterial(fTAGmat);
   if (TAGrecoManager::GetPar()->IncludeDI())
      outfile << diGeo->PrintAssignMaterial(fTAGmat);
   if (TAGrecoManager::GetPar()->IncludeTW())
    outfile << twGeo->PrintAssignMaterial(fTAGmat);
   if (TAGrecoManager::GetPar()->IncludeCA())
      outfile << caGeo->PrintAssignMaterial(fTAGmat);

    // print rotations
   if (TAGrecoManager::GetPar()->IncludeST())
      outfile << stcGeo->PrintRotations();
   if (TAGrecoManager::GetPar()->IncludeBM())
      outfile << bmGeo->PrintRotations();
   if (TAGrecoManager::GetPar()->IncludeTG())
      outfile << generalGeo->PrintTargRotations();
   if (TAGrecoManager::GetPar()->IncludeVT())
      outfile << vtxGeo->PrintRotations();
   if (TAGrecoManager::GetPar()->IncludeIT())
    outfile << itrGeo->PrintRotations();
   if (TAGrecoManager::GetPar()->IncludeMSD())
    outfile << msdGeo->PrintRotations();
   if (TAGrecoManager::GetPar()->IncludeDI())
    outfile << diGeo->PrintRotations();
   if (TAGrecoManager::GetPar()->IncludeTW())
    outfile << twGeo->PrintRotations();
   if (TAGrecoManager::GetPar()->IncludeCA())
    outfile << caGeo->PrintRotations();

    outfile << end.str();

    outfile.close();

    //parameter file needed by the user routines
    parFileName = Form("./ROUTINES/parameters.inc");
    ofstream paramfile;
    paramfile.open(parFileName);
    if ( !paramfile.is_open() )
      cout<< "ERROR  --> I do not find the parameters.inc file"<<fileName.c_str()<< endl;
 
   if (TAGrecoManager::GetPar()->IncludeBM())
      paramfile << bmGeo->PrintParameters();
   if (TAGrecoManager::GetPar()->IncludeVT())
      paramfile << vtxGeo->PrintParameters();
   if (TAGrecoManager::GetPar()->IncludeIT())
      paramfile << itrGeo->PrintParameters();
   if (TAGrecoManager::GetPar()->IncludeMSD())
      paramfile << msdGeo->PrintParameters();
   if (TAGrecoManager::GetPar()->IncludeDI())
      paramfile << diGeo->PrintParameters();
   if (TAGrecoManager::GetPar()->IncludeTW())
      paramfile << twGeo->PrintParameters();
   if (TAGrecoManager::GetPar()->IncludeCA())
      paramfile << caGeo->PrintParameters();

    paramfile.close();

    // stop time
    end_tot = clock();
    double tempo=((double)(end_tot-start_tot ))/CLOCKS_PER_SEC;
    int t_h = trunc( tempo/3600 );
    int t_m = trunc( fmod(tempo, 3600)/60 );
    int t_s = trunc( fmod(fmod(tempo, 3600), 60) );
    cout<< "\n\nExecution Time: "<< tempo << " seconds" << endl;
    cout<< "Execution Time in human units: "<< t_h <<" : "<< t_m <<" : "<< t_s << endl;


    cout<<" Job done! "<<endl;
    return 0;
}

