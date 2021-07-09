//
//
//


#include <stdlib.h>
#include <stdint.h>
#include "EventReader.hh"
#include "DAQFileHeader.hh"
#include "DAQMarkers.hh"
#include "InfoEvent.hh"
#include "TrgEvent.hh"
#include "TDCEvent.hh"




int main( int argc, char *argv[] ){

  EventReader daqFileReader;

  if ( argc != 2 || (argc==2 && argv[1][0]=='-' ) )  {
    exit(0);
  }
  std::string filename ( argv[1] );
  daqFileReader.openFile( filename );
  if ( daqFileReader.getIsOpened() ) {
    std::cout << "The file is now open" << std::endl;
  }
  else {
    std::cout << "Error in opening file" << std::endl;
    exit(0);
  }


  if( !daqFileReader.endOfFileReached() ) {
    daqFileReader.readFileHeader();
    DAQFileHeader* head_p= daqFileReader.getFileHeader();
    head_p->printData();
  }

  int nEventsRead=0;
  int nErrors=0;
  int evNum=0;
  int lastBCO=0;
  printf("prog, HWnum, TRGnum,   BCO,    BX,  DeltaBCO \n");
  while ( daqFileReader.endOfFileReached()==false ){

    // read in memory an event and fill the BaseFragment map
    daqFileReader.getNextEvent();

    // Global event information
    //InfoEvent* evInfo = daqFileReader.getInfoEvent();
    // Trigger data
    TrgEvent*  evTrg  = daqFileReader.getTriggerEvent();

    printf("%5d  %5d  %5d   %9d   %9d   %9d\n",evNum, evTrg->eventNumber,
	   evTrg->triggerCounter,  evTrg->BCOofTrigger, evTrg->clockCounter,
	    evTrg->BCOofTrigger-lastBCO);
    evNum++;
    lastBCO =  evTrg->BCOofTrigger;
    // TDC # 0 and # 1
    //const TDCEvent* evTDC0 =                                   // tdc # 0
    //static_cast<const TDCEvent*>(daqFileReader.getFragmentID(dataV1190 | 0x30));
    //const TDCEvent* evTDC1 =                                   // tdc # 1
	//static_cast<const TDCEvent*>(daqFileReader.getFragmentID(dataV1190 | 0x31));
    
    //if( evTDC0!=NULL ) evTDC0->printData();  // example how to use
    //if( evTDC1!=NULL ) evTDC1->printData();


    // print all the data
    //    daqFileReader.printData();

    bool checkOK = daqFileReader.check();
    if( !checkOK ){
      //      printf("--- @@@ Error on data!!! \n\n");
      nErrors++;
    }

    //    printf ( "\n-----------------END OF EVENT-------------------\n\n" );
    nEventsRead++;
  }
  std::cout << "End of File reached" << std::endl;
  std::cout << "EventsRead: "<< nEventsRead 
	    << " Errors: "<<nErrors << std::endl;
  daqFileReader.closeFile();
  
}
