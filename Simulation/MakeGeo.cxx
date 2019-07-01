
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

    TString parFileName = Form("./geomaps/%sFOOT_geo.map", exp.Data());
    geoTrafo.FromFile(parFileName.Data());

    // GlobalFootGeo footGeo;

    TADIparGeo* dipGeo = new TADIparGeo();
    TASTparGeo* stcGeo = new TASTparGeo();
    TABMparGeo* bmGeo = new TABMparGeo();
    TAVTparGeo* vtxGeo = new TAVTparGeo();
    TAITparGeo* itrGeo = new TAITparGeo();
    TAMSDparGeo* msdGeo = new TAMSDparGeo();
    TATWparGeo* twGeo = new TATWparGeo();
    TACAparGeo* caGeo = new TACAparGeo();
    TAGparGeo* generalGeo = new TAGparGeo();

    // read geomap files
    dipGeo->FromFile();
    stcGeo->FromFile();
    parFileName = "./geomaps/TABMdetector.map";
    bmGeo->FromFile(parFileName.Data());

    parFileName = Form("./geomaps/%sTAVTdetector.map", exp.Data());
    vtxGeo->SetMcFlag();
    vtxGeo->FromFile(parFileName.Data());

    itrGeo->FromFile();
    msdGeo->FromFile();
    twGeo->FromFile();
    caGeo->FromFile();

    parFileName = Form("./geomaps/%sTAGdetector.map", exp.Data());
    generalGeo->FromFile(parFileName.Data());

    if(GlobalPar::GetPar()->IncludeDI())
      genfit::FieldManager::getInstance()->init(new FootField( GlobalPar::GetPar()->MagFieldInputMapName().data(),dipGeo) ); // variable field

    ifstream file;
    string fileName = "foot.inp";
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
    string geofileName = "foot.geo";
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
    // geofile << "* ***Magnets\n";
    // geofile << "RCC MagCvOu0   " << MAG_X << " " << MAG_Y << " "
    // 	    << MAG_Z - MAG_CV_LENGTH/2. - MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    // << MAG_Z - MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    << MAG_CV_LENGTH << " " << MAG_CV0_OUTRAD << endl;
    // geofile << "RCC MagCvOu1   " << MAG_X << " " << MAG_Y << " "
    // 	    << MAG_Z - MAG_CV_LENGTH/2. + MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    // << MAG_Z + MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    << MAG_CV_LENGTH << " " << MAG_CV1_OUTRAD << endl;
    // geofile << "RCC MagPMOu0   " << MAG_X << " " << MAG_Y << " "
    // 	    << MAG_Z - MAG_PM_LENGTH/2. - MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    // << MAG_Z - MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    << MAG_PM_LENGTH << " " << MAG_PM0_OUTRAD << endl;
    // geofile << "RCC MagPMOu1   " << MAG_X << " " << MAG_Y << " "
    // 	    << MAG_Z - MAG_PM_LENGTH/2. + MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    // << MAG_Z + MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    << MAG_PM_LENGTH << " " << MAG_PM1_OUTRAD << endl;
    // geofile << "RCC MagPMIn0   " << MAG_X << " " << MAG_Y << " "
    // 	    << MAG_Z - MAG_PM_LENGTH/2. - MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    // << MAG_Z - MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    << MAG_PM_LENGTH << " " << MAG_PM0_INRAD << endl;
    // geofile << "RCC MagPMIn1   " << MAG_X << " " << MAG_Y << " "
    // 	    << MAG_Z - MAG_PM_LENGTH/2. + MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    // << MAG_Z + MAG_DIST/2. << " 0.000000 0.000000 "
    // 	    << MAG_PM_LENGTH << " " << MAG_PM1_INRAD << endl;
    // geofile << "* ***Gap for magnets\n";
    // geofile << "ZCC Gap0       0.000000 0.000000 " << MAG_CV0_INRAD << endl;
    // geofile << "ZCC Gap1       0.000000 0.000000 " << MAG_CV1_INRAD << endl;
    // geofile << "* ***Magnetic field air region\n";
    // //mappa sanelli si estende per: -5<x<5 , -5<y<5 , -30<z<30
    // geofile << "RPP MagAir     " << MAG_X - 5. << " " << MAG_X + 5. << " "
    // 	    << MAG_Y - 5. << " " << MAG_Y + 5. << " "
    // 	    << MAG_Z - 30. << " " << MAG_Z + 30. << endl;
    geofile << msdGeo->PrintBodies(  );
    // geofile << "* ***Air Box for Scintillator and Calorimeter\n";
    // geofile << "RPP box     " << SCN_X - SCN_BAR_HEIGHT/2. - 1. << " "
    // 	    << SCN_X + SCN_BAR_HEIGHT/2. + 1. << " "
    // 	    << SCN_Y - SCN_BAR_HEIGHT/2. - 1. << " "
    // 	    << SCN_Y + SCN_BAR_HEIGHT/2. + 1. << " "
    // 	    << SCN_Z - SCN_BAR_THICK/2. - 1. << " "
    // 	    << CAL_Z + CAL_CRY_THICK/2. +1. << endl;
    geofile << twGeo->PrintBodies(  );
    // geofile << caGeo->PrintBodies(  );

    // end print bodies
    geofile << "END        " <<endl;

    // region print begins here
    geofile <<"* ******************************************************************************"<<endl;
    geofile <<"*                         REGIONS                                              *"<<endl;
    geofile <<"* ******************************************************************************"<<endl;
    //print regions
    geofile << generalGeo->PrintStandardRegions();
    geofile << stcGeo->PrintSubtractBodiesFromAir();
    geofile << bmGeo->PrintSubtractBodiesFromAir();
    geofile << generalGeo->PrintSubtractTargBodyFromAir();
    geofile << vtxGeo->PrintSubtractBodiesFromAir();
    geofile << itrGeo->PrintSubtractBodiesFromAir();
    geofile << msdGeo->PrintSubtractBodiesFromAir();
    geofile << twGeo->PrintSubtractBodiesFromAir();
    geofile << stcGeo->PrintRegions();
    geofile << bmGeo->PrintRegions();
    geofile << generalGeo->PrintTargRegion();
    geofile << vtxGeo->PrintRegions();
    geofile << itrGeo->PrintRegions();
    // geofile <<"* ***Magnets\n";
    // geofile <<"MAG_PM0      5 MagPMOu0 -MagPMIn0\n";
    // geofile <<"MAG_CV0      5 MagCvOu0 -(MagPMOu0 -MagPMIn0) -Gap0\n";
    // geofile <<"MAG_PM1      5 MagPMOu1 -MagPMIn1\n";
    // geofile <<"MAG_CV1      5 MagCvOu1 -(MagPMOu1 -MagPMIn1) -Gap1\n";
    // geofile <<"* ***Magnetic field air region\n";
    // geofile <<"MAG_AIR      5 MagAir -tgt -(BmnShiIn -BmnMyl0 +BmnMyl3) -(MagCvOu0 -Gap0) -(MagCvOu1 -Gap1) ";
    // geofile << vtxGeo->PrintSubtractBodiesFromAir();
    // geofile << itrGeo->PrintSubtractBodiesFromAir();
    // geofile << msdGeo->PrintSubtractBodiesFromAir();
    geofile << msdGeo->PrintRegions(  );
    // geofile <<"* ***Air Box for Scintillator and Calorimeter\n";
    // geofile <<"BOX          5 box ";
    // geofile << caGeo->PrintSubtractBodiesFromAir();
    // geofile <<"\n";
    geofile << twGeo->PrintRegions(  );
    // geofile << caGeo->PrintRegions(  );

    // end print regions
    geofile << "END        " <<endl;

    //print lattice in the geofile
    //geofile << vtxGeo->PrintLattice();

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
    // outfile << "ASSIGNMA        SmCo   MAG_PM0\n";
    // outfile << "ASSIGNMA    ALUMINUM   MAG_CV0\n";
    // outfile << "ASSIGNMA        SmCo   MAG_PM1\n";
    // outfile << "ASSIGNMA    ALUMINUM   MAG_CV1\n";
    // outfile << "ASSIGNMA         AIR   MAG_AIR                             1\n";
    outfile << msdGeo->PrintAssignMaterial();
    // outfile << "ASSIGNMA         AIR       BOX\n";
    outfile << twGeo->PrintAssignMaterial();
    // outfile << caGeo->PrintAssignMaterial();

    // print rotations
    outfile << vtxGeo->PrintRotations();
    outfile << itrGeo->PrintRotations();
    outfile << msdGeo->PrintRotations();
    outfile << twGeo->PrintRotations();

    outfile << end.str();

    outfile.close();

    //parameter file needed by the user routines
    ofstream paramfile;
    paramfile.open("../../Simulation/ROUTINES/parameters.inc");
    if ( !paramfile.is_open() )
      cout<< "ERROR  --> I do not find the parameters.inc file"<<fileName.c_str()<< endl;

    paramfile << bmGeo->PrintParameters();
    paramfile << vtxGeo->PrintParameters();
    paramfile << itrGeo->PrintParameters();
    paramfile << msdGeo->PrintParameters();
    paramfile << twGeo->PrintParameters();
    // paramfile << caGeo->PrintParameters();

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




// string PrintCard(TString fTitle, TString fWHAT1, TString fWHAT2, TString fWHAT3,
// 		 TString fWHAT4, TString fWHAT5, TString fWHAT6, TString fSDUM) {

//   stringstream fLine;

//   if (fTitle.Sizeof() != 10) fTitle.Resize(10);
//   if (fSDUM.Sizeof() != 10) fSDUM.Resize(10);
//   if (fWHAT1.Sizeof() > 10) fWHAT1.Resize(10);
//   if (fWHAT2.Sizeof() > 10) fWHAT2.Resize(10);
//   if (fWHAT3.Sizeof() > 10) fWHAT3.Resize(10);
//   if (fWHAT4.Sizeof() > 10) fWHAT4.Resize(10);
//   if (fWHAT5.Sizeof() > 10) fWHAT5.Resize(10);
//   if (fWHAT6.Sizeof() > 10) fWHAT6.Resize(10);

//   fLine << setw(10) << fTitle << setw(10) << fWHAT1 << setw(10) << fWHAT2
// 	<< setw(10) << fWHAT3 << setw(10) << fWHAT4 << setw(10) << fWHAT5
// 	<< setw(10) << fWHAT6 << setw(10) << fSDUM;

//   return fLine.str();

// }
