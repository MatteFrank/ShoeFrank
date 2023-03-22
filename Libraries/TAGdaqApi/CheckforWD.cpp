#include <stdlib.h>
#include <stdint.h>
#include "EventReader.hh"
#include "DAQFileHeader.hh"
#include "DAQMarkers.hh"
#include "InfoEvent.hh"
#include "TrgEvent.hh"
#include "WDEvent.hh"



int main( int argc, char *argv[] ){

  EventReader checkWD;

  if ( argc != 2 || (argc==2 && argv[1][0]=='-' ) )  {
    exit(0);
  }
  std::string filename ( argv[1] );
  checkWD.openFile( filename );
  if ( checkWD.getIsOpened() ) {
    std::cout << "The file is now open" << std::endl;
  }
  else {
    std::cout << "Error in opening file" << std::endl;
    exit(0);
  }


  if( !checkWD.endOfFileReached() ) {
    checkWD.readFileHeader();
    DAQFileHeader* head_p= checkWD.getFileHeader();
    head_p->printData();
  }

  int nEventsRead=0;

  int lastBCO=0;
  int lastBCOWD=0;

  std::cout << "EventNumber         TriggerCounter        BCOTrigger        WDHWnumber        BCOWD      BCOTrgDiff     BCOWDDiff" << std::endl;

  while ( checkWD.endOfFileReached()==false ){

    // read in memory an event and fill the BaseFragment map

    checkWD.getNextEvent();

    // Global event information
    //InfoEvent* evInfo = daqFileReader.getInfoEvent();
    // Trigger data
    TrgEvent*  evTrg  = checkWD.getTriggerEvent();

    // TDC # 0 and # 1
    //const TDCEvent* evTDC0 =                                   // tdc # 0
    //static_cast<const TDCEvent*>(daqFileReader.getFragmentID(dataV1190 | 0x30));
    //const TDCEvent* evTDC1 =                                   // tdc # 1
	//static_cast<const TDCEvent*>(daqFileReader.getFragmentID(dataV1190 | 0x31));
    
    //if( evTDC0!=NULL ) evTDC0->printData();  // example how to use
    //if( evTDC1!=NULL ) evTDC1->printData();
 
    const WDEvent* evWD = static_cast<const WDEvent*>(checkWD.getFragmentID(dataWD | 0x30));
    
    if (evWD==NULL){
      nEventsRead++;
      std::cout << "ERROR ON WD" << std::endl;
      continue;
    }
    printf("%5d               %5d                  %5x            %5d            %5x         %5d           %5d\n",
	   evTrg->eventNumber, evTrg->triggerCounter,  evTrg->BCOofTrigger,
	   evWD->hardwareEventNumber, evWD->BCOofTrigger,
	   evTrg->BCOofTrigger-lastBCO, evWD->BCOofTrigger-lastBCOWD );

    // print all the data
    //daqFileReader.printData();
    lastBCO =  evTrg->BCOofTrigger;
    lastBCOWD = evWD->BCOofTrigger;

    // bool checkOK = daqFileReader.check();
    // if( !checkOK ){
    //   printf("--- @@@ Error on data!!! \n\n");
    //   nErrors++;
    // }

    //printf ( "\n-----------------END OF EVENT-------------------\n\n" );
    nEventsRead++;
  }
  std::cout << "End of File reached" << std::endl;
  std::cout << "EventsRead: "<< nEventsRead << std::endl;;
  checkWD.closeFile();
  
}

