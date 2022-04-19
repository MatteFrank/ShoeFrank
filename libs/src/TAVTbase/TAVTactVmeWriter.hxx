#ifndef _TAVTactVmeWriter_HXX
#define _TAVTactVmeWriter_HXX
/*!
  \file TAVTactVmeWriter.hxx
  \brief   Declaration of TAVTactVmeWriter.
*/
/*------------------------------------------+---------------------------------*/
#include <fstream>
#include <map>

#include "TString.h"

#include "TAVTactVmeReader.hxx"

//##############################################################################
class TAVTactVmeWriter : public TAVTactVmeReader  {
public:
   explicit        TAVTactVmeWriter(const char* name=0, TAGdataDsc* pDatRaw = 0, TAGparaDsc* p_geomap=0, TAGparaDsc* p_config=0, TAGparaDsc* pParMap=0);
   virtual         ~TAVTactVmeWriter();
      
   // Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = "");
   
   // close files
   virtual void    Close();
   
   // Process
   virtual Bool_t  Process();
   
private:
   ofstream        fDaqFile;   ///< out stream
   vector<UInt_t>  fDaqEvent;  ///< array of event
   Int_t           fDaqSize;   ///< event size
   
private:
   // Fill DAQ event
   void             FillDaqEvent();
   
   // Write DAQ event
   void             WriteDaqEvent();
   
   ClassDef(TAVTactVmeWriter,0)
};

#endif
