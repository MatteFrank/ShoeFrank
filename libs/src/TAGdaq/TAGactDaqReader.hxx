#ifndef _TAGactDaqReader_HXX
#define _TAGactDaqReader_HXX

/*!
 \file TAGactDaqReader.hxx
 \brief  Interface for DAQ file reader
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
#include "ArduinoEvent.hh"

#include "TAGdaqEvent.hxx"

//##############################################################################

class TAGactDaqReader : public TAGactionFile {

public:
   explicit        TAGactDaqReader(const char* name=0, TAGdataDsc* p_datdaq=0);
   virtual         ~TAGactDaqReader();
   
   // Open DAQ file
   virtual Int_t   Open(const TString& name, Option_t* option="chain", const TString treeName="", Bool_t dscBranch = true);
   // Close file
   virtual void    Close();
   // Check open file
   virtual Bool_t  IsOpen() const;
   // Process action
   virtual Bool_t  Process();
   // Skip event
   virtual void    SkipEvents(Int_t nEvents);
   //! return DAQ header file
   DAQFileHeader*  GetFileHeader()  const { return fDaqFileHeader;  }

private:
   EventReader*    fDaqFileReader;  ///< Event header
   DAQFileHeader*  fDaqFileHeader;  ///< DAQ file stream
   TAGdataDsc*     fDaqEvent;		   ///< input data dsc
   TString         fCurFileName;    ///< Current file name
   Int_t           fDaqFileIndex;   ///< current DAQ file index
   Bool_t          fDaqFileChain;   ///< chaining files in reading

   ClassDef(TAGactDaqReader,0)
};


#endif
