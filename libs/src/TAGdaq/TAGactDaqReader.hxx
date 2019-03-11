#ifndef _TAGactDaqReader_HXX
#define _TAGactDaqReader_HXX

/*------------------------------------------+---------------------------------*/
/* Interface for DAQ file reader
*/

#include "TList.h"
#include "TString.h"

#include "TAGactionFile.hxx"
#include "EventReader.hh"
#include "DAQFileHeader.hh"
#include "DAQMarkers.hh"
#include "InfoEvent.hh"
#include "TrgEvent.hh"
#include "TDCEvent.hh"
#include "DECardEvent.hh"
#include "WDEvent.hh"

#include "TAGdaqEvent.hxx"

//##############################################################################

class TAGactDaqReader : public TAGactionFile {

public:
   explicit        TAGactDaqReader(const char* name=0);
   virtual         ~TAGactDaqReader();
      
   virtual Int_t   Open(const TString& name, Option_t* option=0);
   virtual void    Close();
   
   virtual Bool_t  IsOpen() const;
   
   virtual Bool_t  Process();

   DAQFileHeader*  GetFileHeader()  const { return fDaqFileHeader;  }

private:
   EventReader*    fDaqFileReader;
   DAQFileHeader*  fDaqFileHeader;
   TAGdaqEvent*    fDaqEvent;
   
   ClassDef(TAGactDaqReader,0)
};


#endif
