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
#include "DEMSDEvent.hh"
#include "WDEvent.hh"

#include "TAGdaqEvent.hxx"

//##############################################################################

class TAGactDaqReader : public TAGactionFile {

public:
   explicit        TAGactDaqReader(const char* name=0, TAGdataDsc* p_datdaq=0);
   virtual         ~TAGactDaqReader();
      
   virtual Int_t   Open(const TString& name, Option_t* option="chain", const TString treeName="", Bool_t dscBranch = true);
   virtual void    Close();
   
   virtual Bool_t  IsOpen() const;
   
   virtual Bool_t  Process();

   virtual void    SkipEvents(Int_t nEvents);

   DAQFileHeader*  GetFileHeader()  const { return fDaqFileHeader;  }

private:
   EventReader*    fDaqFileReader;
   DAQFileHeader*  fDaqFileHeader;
   TAGdataDsc*     fDaqEvent;		    // input data dsc
   TString         fCurFileName;
   Int_t           fDaqFileIndex;
   Bool_t          fDaqFileChain;    // chaining files in reading

   ClassDef(TAGactDaqReader,0)
};


#endif
