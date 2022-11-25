#ifndef _TAGactWCreader_HXX
#define _TAGactWCreader_HXX
/*!
 \file
 \version $Id: TAGactWCreader.hxx,v 1.3 2003/06/15 18:27:04 mueller Exp $
 \brief   Declaration of TAGactWCreader.
 */
/*------------------------------------------+---------------------------------*/

#include <fstream>
#include <iostream>

#include "TAGactionFile.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include "TAPLntuRaw.hxx"
#include "TACEntuRaw.hxx"
#include "TAGbaseWCpar.hxx"

class TAGactWCreader : public TAGactionFile {
   
public:
   explicit        TAGactWCreader(const char* name = "",
                                  TAGparaDsc* p_WCmap = 0x0,
                                  TAGdataDsc* p_stwd = 0x0,
                                  TAGdataDsc* p_twwd = 0x0);
   
   virtual        ~TAGactWCreader();
   void            CreateHistogram();
   Bool_t          Process();
   Int_t           Open(const TString& inputDataFile, Option_t* option="", const TString treeName="tree", Bool_t dscBranch = true);
   void            Clear(Option_t* option ="");

private:
   ifstream        fInputFile;
   TAGdataDsc*     fpdatRawSt;		    // output data dsc
   TAGdataDsc*     fpdatRawTw;          // output data dsc
   TAGparaDsc*     fpParMapWC;		    // parameter dsc
   Int_t           fEventsN;
   Int_t           fDebugLevel;
   TString         fSoftwareVersion;
   Int_t           fSamplesN;
   Int_t           fChannelsN;
   Float_t         fPeriod;
   ULong_t         fTimeStamp;
   Bool_t          fHasMeasurement;
   ULong_t         fFileSize;
   Int_t           fFrequency;

   
   TAGwaveCatcher  fWaves;
   EventData       fEventData;

private:
   Bool_t DecodeWaveForms();
   void   DecodeHeader();
   
private:
   static const Float_t fgkAdc2mv;
   static const Float_t fgkSize2Evts;

   ClassDef(TAGactWCreader,0)
};

#endif
