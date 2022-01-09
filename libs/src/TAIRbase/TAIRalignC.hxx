#ifndef _TAIRalignC_HXX
#define _TAIRalignC_HXX
/*!
 \file TAIRalignC.cxx
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
   static TAIRalignC* fgInstance; ///< static instance of class
   
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
   //! Loop over events
   void   LoopEvent(Int_t nEvts = 1);
   
private:
   //! Create histogram
   void    CreateHistogram();
   //! Align
   Bool_t  Align(Bool_t rough);
   //! Fill histogram
   Bool_t  FillHistograms();
   //! Define weight
   Bool_t  DefineWeights();
   //! Fill cluster psoiton rough
   Bool_t  FillClusPosRough(Int_t i, TAGcluster* cluster);
   //! Fill cluster position precise
   Bool_t  FillClusPosPrecise(Int_t i, TAGcluster* cluster);

   //! Fill cluster array
   void    FillClusterArray();
   //! Fill parameter map
   void    InitParameters();
   //! Fill status map
   void    FillStatus();
   //! Fill positiom
   void    FillPosition();
   //! Fill status map
   void    FillStatus(TAVTbaseParConf* parConf, Int_t offset = 0);
   //! Fill position map
   void    FillPosition(TAVTbaseParGeo* parGeo, Int_t offset = 0);

   //! Update alignment parameters
   void    UpdateAlignmentParams();
   //! Update transformation
   void    UpdateTransfo(Int_t idx);
   //! Update geo maps
   void    UpdateGeoMaps();
   //! Update geom map file
   void    UpdateGeoMapsUVW(fstream &fileIn, fstream &fileOut, Int_t idx);
   //! Reset
   void    Reset();
      
private:
   TAGroot*             fAGRoot;        ///< pointer to TAGroot
   TAGcampaignManager*  fCampManager;   ///< Campaign manager
   const TString        fFileName;      ///< input file
   Int_t                fRunNumber;     ///< Run number
   TAGgeoTrafo*         fGeoTrafo;      ///< FOOT global transformation
   Bool_t               fFlagVtx;       ///< VTX presence flag
   Bool_t               fFlagIt;        ///< ITR presence flag
   Bool_t               fFlagMsd;       ///< MSD presence flag
   TAGdataDsc*          fpNtuClusVtx;	 ///< Cluster VTX
   TAGparaDsc*          fpConfigVtx;	 ///< configuration dsc
   TAGparaDsc*          fpGeoMapVtx;    ///< geometry para dsc
   TAGdataDsc*          fpNtuClusMsd;	 ///< Cluster MSD
   TAGparaDsc*          fpConfigMsd;	 ///< configuration dsc
   TAGparaDsc*          fpGeoMapMsd;    ///< geometry para dsc
   TAGparaDsc*          fpGeoMapG;      ///< geometry para dsc
   TADItrackEmProperties* fpDiff;		 ///< diffusion parameters
   TAGactTreeReader*    fInfile;        ///< action for reading cluster
   TAIRntuAlignC*       fAlign;         ///< pointer to align para
   
   TObjArray*           fClusterArray;  ///< contains all clusters
   
   Int_t                fWeighted;      ///< activation of the weight
   Int_t                fPreciseIt;     ///< number of iteration for precise alignment
   Int_t                fCut1;          ///< First cut
   Int_t                fCut2;          ///< Second cut
   Int_t                fCutFactor;     ///< Cut factor
   Int_t                fOffsetMsd;     ///< MSD offset
   
   TGraphErrors*        fResidualX;     ///< Residuals X graph
   TGraphErrors*        fResidualY;     ///< Residuals Y graph
   TArrayI              fSecArray;      ///< contains the sensor number to be aligned
   
   Int_t                fHitPlanes;     ///< hit per plane
   Int_t                fEvents1;       ///< Event proceeded
   
   Float_t              fEbeamInit;     ///< Init beam energy
   Float_t              fpcInit;        ///< PC initial value
   Float_t              fBetaInit;      ///< Beta initial value
   Float_t              fAbeam;         ///< A of beam
   Int_t                fZbeam;         ///< Z of beam
   Float_t              fEbeam;         ///< Beam energy
   Float_t              fpc;            ///< PC value
   Float_t              fBeta;          ///< Beta value
   
   Double_t             fSlopeU;        ///< Slope in U
   Double_t             fSlopeV;        ///< Slope in V
   Double_t             fNewSlopeU;     ///< New slope in U
   Double_t             fNewSlopeV;     ///< New slope in V
   
   Double_t*            fWeightQ;       ///< Weighted Q array
   Double_t*            fZposition;     ///< Z position array
   Double_t*            fThickDect;     ///< Detector thickness array
   Double_t*            fSigmaAlfaDist; ///< Sigma alpha paramater array
   Double_t*            fSigmaMeasQfinal; ///< Measured final Q array
   
   Double_t*            fPosUClusters;  ///< Cluster position array in U
   Double_t*            fPosVClusters;  ///< Cluster position array in V
   Double_t*            fErrUClusters;  ///< Cluster position error array in U
   Double_t*            fErrVClusters;  ///< Cluster position error array in V
   
   Float_t*             fTiltW;         ///< tilted W alignment value array
   Float_t*             fAlignmentU;    ///< alignmentU value array
   Float_t*             fAlignmentV;    ///< alignmentV value array
   
   Bool_t*              fStatus;        ///< Status array
   Int_t*               fDevStatus;     ///< Device status array
   
   Double_t             fSumWeightQ;    ///< Sum of weigthed Q value
   
   Bool_t               fFixPlaneRef1;  ///< Fix plane 1
   Bool_t               fFixPlaneRef2;  ///< Fix plane 2
   Int_t                fPlaneRef1;     ///< Plane reference 1
   Int_t                fPlaneRef2;     ///< Plane reference 2
   
   TH1F*                fpResXC[50];    ///< Residual in X histograms
   TH1F*                fpResYC[50];    ///< Residual in Y histograms
   TH1F*                fpResTotXC;     ///< Total residuals in x histogram
   TH1F*                fpResTotYC;     ///< Total residuals in y histogram
 
private:
   static Int_t fgkPreciseIt;           ///< max number of iterations
   
   ClassDef(TAIRalignC,0)
};

#endif
