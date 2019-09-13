
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

#include "GlobalPar.hxx"
#include "FootField.hxx"
#include <FieldManager.h>


using namespace std;


int main (int argc, char *argv[]) {

    TString exp("");

    for (int i = 0; i < argc; i++) {
       if(strcmp(argv[i],"-exp") == 0)   { exp = TString(argv[++i]); }   // extention for config/geomap files
    }

    if (!exp.IsNull())
       exp += "/";

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
    // GlobalPar* GlobalPar;

    GlobalPar::Instance("FootGlobal.par");
    GlobalPar::GetPar()->Print();

    TAGroot* fTAGroot = new TAGroot();
    TAGmaterials* fTAGmat = new TAGmaterials();

    TAGgeoTrafo geoTrafo;

    TString parFileName;
    geoTrafo.FromFile("../Reconstruction/level0/geomaps/FOOT_geo.map");

    // GlobalFootGeo footGeo;

    TADIparGeo* diGeo = new TADIparGeo();
    TASTparGeo* stcGeo = new TASTparGeo();
    TABMparGeo* bmGeo = new TABMparGeo();
    TAVTparGeo* vtxGeo = new TAVTparGeo();
    TAITparGeo* itrGeo = new TAITparGeo();
    TAMSDparGeo* msdGeo = new TAMSDparGeo();
    TATWparGeo* twGeo = new TATWparGeo();
    TACAparGeo* caGeo = new TACAparGeo();
    TAGparGeo* generalGeo = new TAGparGeo();

    // read geomap files
    stcGeo->FromFile();
    bmGeo->FromFile();
    vtxGeo->SetMcFlag();
    vtxGeo->FromFile();
    itrGeo->FromFile();
    msdGeo->FromFile();
    diGeo->FromFile();
    twGeo->FromFile();
    caGeo->FromFile();
    generalGeo->FromFile();

    if(GlobalPar::GetPar()->IncludeDI())
      genfit::FieldManager::getInstance()->init(new FootField( GlobalPar::GetPar()->MagFieldInputMapName().data(),diGeo) ); // variable field

    ifstream file;
    string fileName = Form("foot.inp");
    file.open( fileName.c_str(), ios::in );
    if ( !file.is_open() )        cout<< "ERROR  -->  wrong input in GlobalPar::ReadParemFile file:: "<<fileName.c_str()<< endl, exit(0);

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
    geofile << stcGeo->PrintBodies(  );
    geofile << bmGeo->PrintBodies(  );
    geofile << generalGeo->PrintTargBody(  );
    geofile << vtxGeo->PrintBodies(  );
    geofile << itrGeo->PrintBodies(  );
    geofile << msdGeo->PrintBodies(  );
    geofile << diGeo->PrintBodies(  );
    geofile << twGeo->PrintBodies(  );
    geofile << caGeo->PrintBodies(  );

    // end print bodies
    geofile << "END        " <<endl;

    // region print begins here
    geofile <<"* ******************************************************************************"<<endl;
    geofile <<"*                         REGIONS                                              *"<<endl;
    geofile <<"* ******************************************************************************"<<endl;
    //print regions
    geofile << generalGeo->PrintStandardRegions1();
    geofile << stcGeo->PrintSubtractBodiesFromAir();
    geofile << bmGeo->PrintSubtractBodiesFromAir();
    geofile << generalGeo->PrintSubtractTargBodyFromAir();
    geofile << vtxGeo->PrintSubtractBodiesFromAir();
    geofile << itrGeo->PrintSubtractBodiesFromAir();
    geofile << msdGeo->PrintSubtractBodiesFromAir();
    geofile << diGeo->PrintSubtractBodiesFromAir();
    geofile << generalGeo->PrintStandardRegions2();
    geofile << twGeo->PrintSubtractBodiesFromAir();
    geofile << caGeo->PrintSubtractBodiesFromAir();    
    geofile << stcGeo->PrintRegions();
    geofile << bmGeo->PrintRegions();
    geofile << generalGeo->PrintTargRegion();
    geofile << vtxGeo->PrintRegions();
    geofile << itrGeo->PrintRegions();
    geofile << msdGeo->PrintRegions(  );
    geofile << diGeo->PrintRegions(  );
    geofile << twGeo->PrintRegions(  );
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
    outfile << fTAGmat->SaveFileFluka();

    //print assig nmaterials
    outfile << generalGeo->PrintStandardAssignMaterial();
    outfile << stcGeo->PrintAssignMaterial();
    outfile << bmGeo->PrintAssignMaterial();
    outfile << generalGeo->PrintTargAssignMaterial();
    outfile << vtxGeo->PrintAssignMaterial();
    outfile << itrGeo->PrintAssignMaterial();
    outfile << msdGeo->PrintAssignMaterial();
    outfile << diGeo->PrintAssignMaterial();
    outfile << twGeo->PrintAssignMaterial();
    outfile << caGeo->PrintAssignMaterial();

    // print rotations
    outfile << stcGeo->PrintRotations();
    outfile << bmGeo->PrintRotations();
    outfile << generalGeo->PrintTargRotations();
    outfile << vtxGeo->PrintRotations();
    outfile << itrGeo->PrintRotations();
    outfile << msdGeo->PrintRotations();
    outfile << diGeo->PrintRotations();
    outfile << twGeo->PrintRotations();
    outfile << caGeo->PrintRotations();

    outfile << end.str();

    outfile.close();

    //parameter file needed by the user routines
    parFileName = Form("./ROUTINES/parameters.inc");
    ofstream paramfile;
    paramfile.open(parFileName);
    if ( !paramfile.is_open() )
      cout<< "ERROR  --> I do not find the parameters.inc file"<<fileName.c_str()<< endl;

    paramfile << bmGeo->PrintParameters();
    paramfile << vtxGeo->PrintParameters();
    paramfile << itrGeo->PrintParameters();
    paramfile << msdGeo->PrintParameters();
    paramfile << diGeo->PrintParameters();
    paramfile << twGeo->PrintParameters();
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

