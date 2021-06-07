//
//
//
#include <iostream>
#include <string.h>
#include "EventReaderAndChecker.hh"
#include "DAQFileHeader.hh"
#include <stdlib.h>
#include <fstream>

int main( int argc, char *argv[] ){

  bool verbose = false;
  if( !(argc == 2 || (argc==3 && strncmp(argv[1],"-v",2)==0 )) ){
    std::cout<<argv[0]<<" [-v] DAQFilename"<<std::endl
	     <<"   It reads in a DAQ file and performs all the possible checks"
	     <<std::endl;
    exit(0);
  }
  if( argc==3 ){
    verbose = true;
  }

  std::string filename ( argv[argc-1] );
  EventReaderAndChecker daqFileChecker( (verbose? 2 : 0) );

  daqFileChecker.openFile( filename );
    if ( daqFileChecker.getIsOpened() ) {
    std::cout << "The file is now open" << std::endl;
  }
  else {
    std::cout << "Error in opening file" << std::endl;
    exit(0);
  }


  if( !daqFileChecker.endOfFileReached() ) {
    if( verbose ) std::cout << " reading header " << std::endl;
    daqFileChecker.readFileHeader();
    DAQFileHeader* head_p= daqFileChecker.getFileHeader();
    if( verbose ) head_p->printData();
  }

  int nEventsRead=0;
  int nErrors=0;
  
  while ( daqFileChecker.endOfFileReached()==false ){
   
    daqFileChecker.getNextEvent();
    
    if( !daqFileChecker.endOfFileReached() ){
      
      if( verbose ) daqFileChecker.printData();
      
      bool checkOK = daqFileChecker.check();
      
      if( !checkOK ){
	if( verbose ) std::cout<<"--- @@@ Error on data!!! "<<std::endl;
	nErrors++;
      }
      
      if( verbose ) std::cout<<std::endl;
      nEventsRead++;
    }
  }
  std::cout << "End of File reached" << std::endl;
  std::cout << "Events Read from file: "<< nEventsRead 
	    << " Errors: "<<nErrors << std::endl <<std::endl;
  
  daqFileChecker.printStatistics();
  daqFileChecker.closeFile();
  
}
