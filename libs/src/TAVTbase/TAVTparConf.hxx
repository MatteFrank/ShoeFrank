#ifndef _TAVTparConf_HXX
#define _TAVTparConf_HXX
/*!
  \file
  \version $Id: TAVTparConf.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
  \brief   Declaration of TAVTparConf.
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "TAGparTools.hxx"

//##############################################################################

class TAVTparConf : public TAGparTools {
      
private:
   Int_t      fSensorsN;         // Number of sensors	 
   
   static Bool_t  fgIsMapHist;   // enable  map histo

   struct SensorParameter_t : public  TObject {
	  Int_t     SensorIdx;              // sensor index
	  Int_t     Status;                 // Status flag: Primary Reference = 1., Secondary Reference = 2. DeviceUunderTest = 3. Out = -1.
	  Int_t     MinNofPixelsInCluster;  // minimum pixels in a cluster
	  Int_t     MaxNofPixelsInCluster;  // maximum pixels in a cluster

   };
   
   struct AnalysisParameter_t : public  TObject {
	  Int_t      TracksMaximum;          // maximum number of tracks to be allowed
	  Int_t      PlanesForTrackMinimum;  // min # planes to build a track in an event
	  Float_t    SearchHitDistance;      // max distance hit-track to add hit to track
	  Int_t      HitsInPlaneMaximum;     // maximum number of hits per plane to be allowed
	  Float_t    TrackChi2Limit;         // chi2 limit on the track
	  Int_t      TracksForAlignmentFit;  // minimum number of tracks to fit alignement parameters
	  Float_t    MaxSlopeForAlignment;   // maximum slope value in track for alignment
	  Float_t    BmTrackChi2Limit;       // chi2 limit on the BM track
   };
   
   
private: 
   static       TString fgkDefaultConfName; // default detector configuration file

public:

   TAVTparConf();
    virtual ~TAVTparConf();

   
   Bool_t          FromFile(const TString& name = "");
    
   virtual void    Clear(Option_t* opt="");
   
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
   
public:
      
   SensorParameter_t fSensorParameter[32];
   SensorParameter_t& GetSensorPar(Int_t idx){return fSensorParameter[idx];}
   
   AnalysisParameter_t fAnalysisParameter;
   AnalysisParameter_t& GetAnalysisPar(){return fAnalysisParameter;}
   
   Int_t  GetSensorsN()  const { return fSensorsN; }
   
public:
   
   static Bool_t IsMapHistOn()                   { return fgIsMapHist; }
   static void   SetHistoMap(Bool_t flag = true) { fgIsMapHist = flag; }
   
   static const Char_t* GetDefaultConfName()     { return fgkDefaultConfName.Data(); }

   ClassDef(TAVTparConf,1)
};

#endif
