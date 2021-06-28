#ifndef _TAIRalignC_HXX
#define _TAIRalignC_HXX
/*!
 \file
 \brief   Declaration of TAIRalignC.
 
 \author C.A. Reidel & Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"
#include "TH1F.h"
#include "TObject.h"
#include "TString.h"
#include "TArrayI.h"
#include "TGraphErrors.h"

#include "TAGroot.hxx"
#include "TAGgeoTrafo.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAGactTreeReader.hxx"
#include "TADItrackEmProperties.hxx"
#include "TAIRntuAlignC.hxx"

class TAGcampaignManager;
class TAGcluster;
class TAVTbaseParGeo;
class TAVTbaseParConf;
class TObjArray;
class TAIRalignC : public TObject {
   
private:
   static TAIRalignC* fgInstance; // static instance of class
   
public:
   //! Instance of class
   static TAIRalignC* Instance(const TString name = "16O_C2H4_200_1.root", const TString exp = "", Int_t runNUmber = 1,
                               Bool_t flagVtx = false,
                               Bool_t flagIt  = false,
                               Bool_t flagMsd = false,
                               Int_t weight = -1); // The weight is barely influencing the results
   virtual ~TAIRalignC();
   
private:
   //! ctr
   TAIRalignC(const TString name, const TString exp, Int_t runNUmber, Bool_t flagVtx, Bool_t flagIt, Bool_t flagMsd, Int_t weight);
public:
   void   LoopEvent(Int_t nEvts = 1);
   
private:
   void    CreateHistogram();
   Bool_t  Align(Bool_t rough);
   Bool_t  FillHistograms();
   Bool_t  DefineWeights();
   Bool_t  FillClusPosRough(Int_t i, TAGcluster* cluster);
   Bool_t  FillClusPosPrecise(Int_t i, TAGcluster* cluster);

   void    FillClusterArray();
   void    InitParameters();
   void    FillStatus();
   void    FillPosition();
   void    FillStatus(TAVTbaseParConf* parConf, Int_t offset = 0);
   void    FillPosition(TAVTbaseParGeo* parGeo, Int_t offset = 0);


   void    UpdateAlignmentParams();
   void    UpdateTransfo(Int_t idx);
   void    UpdateGeoMaps();
   void    UpdateGeoMapsUVW(fstream &fileIn, fstream &fileOut, Int_t idx);
   void    Reset();
      
private:
   TAGroot*             fAGRoot;        // pointer to TAGroot
   TAGcampaignManager*  fCampManager;
   const TString        fFileName;      // input file
   Int_t                fRunNumber;
   TAGgeoTrafo*         fGeoTrafo;
   Bool_t               fFlagVtx;
   Bool_t               fFlagIt;
   Bool_t               fFlagMsd;
   TAGdataDsc*          fpNtuClusVtx;	 // Cluster VTX
   TAGparaDsc*          fpConfigVtx;	 // configuration dsc
   TAGparaDsc*          fpGeoMapVtx;    // geometry para dsc
   TAGdataDsc*          fpNtuClusMsd;	 // Cluster MSD
   TAGparaDsc*          fpConfigMsd;	 // configuration dsc
   TAGparaDsc*          fpGeoMapMsd;    // geometry para dsc
   TAGparaDsc*          fpGeoMapG;      // geometry para dsc
   TADItrackEmProperties*  fpDiff;		    // diffusion parameters
   TAGactTreeReader*    fInfile;        // action for reading cluster
   TAIRntuAlignC*       fAlign;         // pointer to align para
   
   
   TObjArray*           fClusterArray;  // contains all clusters
   
   Int_t     fWeighted;   // activation of the weight
   Int_t     fPreciseIt;  // number of iteration for precise alignment
   Int_t     fCut1;
   Int_t     fCut2;
   Int_t     fCutFactor;
   Int_t     fOffsetMsd;
   
   TGraphErrors*        fResidualX;
   TGraphErrors*        fResidualY;
   TArrayI              fSecArray;   // contains the sensor number to be aligned
   
   Int_t    fHitPlanes;
   Int_t    fEvents1;
   
   Float_t fEbeamInit;
   Float_t fpcInit;
   Float_t fBetaInit;
   Float_t fAbeam;
   Int_t   fZbeam;
   Float_t fEbeam;
   Float_t fpc;
   Float_t fBeta;
   
   Double_t fSlopeU;
   Double_t fSlopeV;
   Double_t fNewSlopeU;
   Double_t fNewSlopeV;
   
   Double_t*  fWeightQ;
   Double_t*  fZposition;
   Double_t*  fThickDect;
   Double_t*  fSigmaAlfaDist;
   Double_t*  fSigmaMeasQfinal;
   
   Double_t* fPosUClusters;
   Double_t* fPosVClusters;
   Double_t* fErrUClusters;
   Double_t* fErrVClusters;
   
   Float_t*  fTiltW;
   Float_t*  fAlignmentU;
   Float_t*  fAlignmentV;
   
   Bool_t*   fStatus;
   Int_t*    fDevStatus;
   
   Double_t  fSumWeightQ;
   
   Bool_t fFixPlaneRef1;
   Bool_t fFixPlaneRef2;
   Int_t  fPlaneRef1;
   Int_t  fPlaneRef2;
   
   TH1F*   fpResXC[50];        // Residual in X
   TH1F*   fpResYC[50];        // Residual in Y
   TH1F*   fpResTotXC;        // Total residuals in x
   TH1F*   fpResTotYC;        // Total residuals in y
 
private:
   static Int_t fgkPreciseIt; // max number of iterations
   
   ClassDef(TAIRalignC,0)
};

#endif
