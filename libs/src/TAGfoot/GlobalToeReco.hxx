
#ifndef _GlobalToeReco_HXX_
#define _GlobalToeReco_HXX_

#include "TString.h"
#include "TNamed.h"

#include "TAGparaDsc.hxx"
#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGactNtuGlbTrack.hxx"


class GlobalToeReco : public TNamed
{
public:
   //! default constructor
   GlobalToeReco(TString expName, TString fileNameIn = "", TString fileNameout = "");
   
   virtual ~GlobalToeReco();
   
   //! Read parameters
   void ReadParFiles();
   
   //! Create raw action
   virtual void CreateRecAction();
   
   //! Add required items
   virtual void AddRecRequiredItem();
   
   //! Set rec histogram directory
   virtual void SetRecHistogramDir();
   
   //! Loop events
   void LoopEvent(Int_t nTotEv);

   //! Begin loop
   void BeforeEventLoop();

   //! End loop
   void AfterEventLoop();

   //! Open File In
   virtual void OpenFileIn();
   
   //! Close File in
   virtual void CloseFileIn();
   
   //! Open File Out
   virtual void OpenFileOut();
   
   //! Close File Out
   virtual void CloseFileOut();
   
   //! Set experiment name
   virtual void SetExpName(const Char_t* name) { fExpName = name; }

private:
   TString           fExpName;       // exp name
   TAGroot*          fTAGroot;       // pointer to TAGroot
   TAGgeoTrafo*      fpFootGeo;      // trafo prointer
   TAGactTreeWriter* fActEvtWriter;  // write histo and tree
   
   //! Par geo
   TAGparaDsc*       fpParGeoDi;
   TAGparaDsc*       fpParGeoVtx;
   TAGparaDsc*       fpParGeoIt;
   TAGparaDsc*       fpParGeoMsd;
   TAGparaDsc*       fpParGeoTw;
   
   //! Containers
   TAGdataDsc*        fpNtuVtx;
   TAGdataDsc*        fpNtuClusIt;
   TAGdataDsc*        fpNtuClusMsd;
   TAGdataDsc*        fpNtuRecTw;
   TAGdataDsc*        fpNtuGlbTrack;     // input data dsc

   TAGactNtuGlbTrack* fActGlbTrack;    // Global tracking action

   ClassDef(GlobalToeReco, 0); 
};


#endif
