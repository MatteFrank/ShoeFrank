
#ifndef _TACEwaveDisplay_HXX_
#define _TACEwaveDisplay_HXX_


#include <TGFrame.h>
#include <TArrayI.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TLine.h>


// --------------------------------------------------------------------------------------
/** TATLntuHit simple class containers for telescope raw hit
 
 \author Ch. Finck
 */

#include "TAGaction.hxx"
#include "TAGactionFile.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"

#include "TAGbaseWCparMap.hxx"
#include "TAPLntuRaw.hxx"
#include "TAPLntuHit.hxx"

#include "TACEntuRaw.hxx"
#include "TACEntuHit.hxx"

#include "TAPWntuRaw.hxx"
#include "TAPWntuHit.hxx"

#include "TAGactWCreader.hxx"
#include "TAPLactNtuHit.hxx"
#include "TACEactNtuHit.hxx"
#include "TAPWactNtuHit.hxx"

#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TGNumberEntry.h>
#include <TGTextView.h>
#include <TGTextEntry.h>
#include <TVirtualPad.h>

class TAVTtrack;

class TObject;
class TString;
class TACEwaveDisplay : public TGFrame
{
   
public:
   virtual ~TACEwaveDisplay();
   void   CreateActions(const TString name);
   void   Reset();
   void   Update();
   void   NextEvent();
   void   LoopEvent(Int_t nEvents = 0);
   void   DrawMap();
   
   void   ClearLogMessage();
      
   Int_t  GetHistoNumber(const Char_t* name);

public:
   //! Instance of class
   static TACEwaveDisplay* Instance(const TString name = "./data/Run__Binary.bin", const TString expName = "CLINM");
  
   //! Get Reader
   TAGactionFile* GetEvtReader() {return fActEvtReader; }
  
   // Seve view
   void           SaveView();


private:
   static TACEwaveDisplay* fgInstance; // static instance of class

private:
   TACEwaveDisplay(const TString name, const TString expName);
   
private:
   TString              fExpName;      // experiment name
   TAGroot*             fAGRoot;       // pointer to TAGroot
   TAGgeoTrafo*         fpGeoTrafo;    // Global transformation
   TAGdataDsc*          fpDatRawSt;      // input data dsc
   TAGdataDsc*          fpDatRawTw;      // input data dsc
   TAGdataDsc*          fpDatRawPw;      // input data dsc
   TAGdataDsc*          fpNtuRawSt;      // input data dsc
   TAGdataDsc*          fpNtuRawTw;      // input data dsc
   TAGdataDsc*          fpNtuRawPw;      // input data dsc
   TAGparaDsc*          fpMapWc;		// configuration dsc
   
   TAGactWCreader*      fActEvtReader;  // action for raw data
   TAPLactNtuHit*       fActNtuSt;
   TAPWactNtuHit*       fActNtuPw;
   TACEactNtuHit*       fActNtuTw;

   const TGWindow*      fkMainWindow;   // main window
   TGMainFrame*         fMain;          // main frame
   TRootEmbeddedCanvas* fEcanvas;       // canvas for spectra
   TVirtualPad*         fPad;           // pad for spectra
   TGTextEntry*         fSaveViewEntry; // text entry for saving viewer file name
   TGNumberEntry*       fPlaneEntry;    // plane botton
   TGNumberEntry*       fNextPlaneEntry; // next plane botton
   TGNumberEntry*       fxEntry;         // deltaX  button
   TGNumberEntry*       fyEntry;         // deltaY button
   TGNumberEntry*       fLoopEntry;      // loop event botton
   TGTextEntry*         fEventEntry;     // event entry view
   TGTextView*          fLogMessage;     // log message
   
   TLine*               fLElineSt;
   TLine*               fLElineTw;
   TLine*               fLElinePw;
   TLine*               fFALLlineSt;
   TLine*               fFALLlineTw;
   TLine*               fFALLlinePw;
   TLine*               fFASTlinePw;
   UInt_t               fEventsN;        // number of events
   TH1F*                fhOscSt;         // Oscillogram
   TH1F*                fhOscTw;         // Oscillogram
   TH1F*                fhOscPw;         // Oscillogram
   Int_t                fSTflag;
   Int_t                fTWflag;
   Int_t                fPWflag;
   
   ClassDef(TACEwaveDisplay,0) //GUI for drawing detection element segmentation
};
#endif

