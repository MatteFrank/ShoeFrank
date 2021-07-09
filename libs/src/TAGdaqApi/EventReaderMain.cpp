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
#include "WDEvent.hh"
#include "fADCEvent.hh"
#include "DEMSDEvent.hh"




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
  int counter = 0;
  while ( daqFileReader.endOfFileReached()==false ){

    // read in memory an event and fill the BaseFragment map
    daqFileReader.getNextEvent();

    // Global event information
    // Trigger data
    TrgEvent*  evTrg  = daqFileReader.getTriggerEvent();

    // TDC # 0 and # 1
    const TDCEvent* evTDC0 =                                   // tdc # 0
    static_cast<const TDCEvent*>(daqFileReader.getFragmentID(dataV1190 | 0x30));
    
    const TDCEvent* evTDC1 =                                   // tdc # 1
      static_cast<const TDCEvent*>(daqFileReader.getFragmentID(dataV1190 | 0x31));

    //Wavedream
    const WDEvent* evWD =
       static_cast<const WDEvent*>(daqFileReader.getFragmentID(dataWD | 0x30));

    //MSD 1st station
    const DEMSDEvent* evMSD0 =
      static_cast<const DEMSDEvent*>(daqFileReader.getFragmentID(dataMSD | 0x30));

    //MSD 2nd station
    const DEMSDEvent* evMSD1 =
      static_cast<const DEMSDEvent*>(daqFileReader.getFragmentID(dataMSD | 0x31));

    //MSD 3rd station
    const DEMSDEvent* evMSD2 =
      static_cast<const DEMSDEvent*>(daqFileReader.getFragmentID(dataMSD | 0x32));

    if( evTDC0!=NULL ) evTDC0->printData();  // example how to use
    if( evTDC1!=NULL ) evTDC1->printData();

    if( evWD!=NULL ) evWD->printData();
    if( evMSD0!=NULL) evMSD0->printData();
    if( evMSD1!=NULL) evMSD1->printData();
    if( evMSD2!=NULL) evMSD2->printData();

    //HOW TO READ MSD DATA
    //    for(int i = 0; i<640; ++i ){
    //      std::cout << "Value of strip " << i << "of X layer is " << evMSD0->Xplane[i];
    //      std::cout << "Value of strip " << i << "of Y layer is " << evMSD0->Yplane[i];
    //    }
  
  
    if (evTrg!=NULL) evTrg->printData();


    //print all the data
    //daqFileReader.printData();
    
    bool checkOK = daqFileReader.check();
    if( !checkOK ){
      printf("--- @@@ Error on data!!! \n\n");
      nErrors++;
    }

    printf ( "\n-----------------END OF EVENT-------------------\n\n" );
    nEventsRead++;
  }
  std::cout << "End of File reached" << std::endl;
  std::cout << "EventsRead: "<< nEventsRead 
	    << " Errors: "<<nErrors << std::endl;
  std::cout << "COUNTER " << std::hex << counter << std::endl;
  daqFileReader.closeFile();
  
}
