#ifndef _TAIRalignM_HXX
#define _TAIRalignM_HXX
/*!
 \file
 \brief   Declaration of TAIRalignM.
 
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
#include "TAIRmillepede.hxx"

class TAVTbaseCluster;
class TAVTbaseParGeo;
class TAVTbaseParConf;
class TObjArray;
class TAVTbaseTrack;
class TAVTbaseCluster;

class TAIRalignM : public TObject {
   
private:
   static TAIRalignM* fgInstance; // static instance of class
   
public:
   //! Instance of class
   static TAIRalignM* Instance(const TString name = "16O_C2H4_200_1.root",
                               Bool_t flagVtx = false,
                               Bool_t flagIt  = false,
                               Bool_t flagMsd = false,
                               Int_t weight = -1); // The weight is barely influencing the results
   virtual ~TAIRalignM();
   
   void ProcessTrack(TAVTbaseTrack *track, Double_t* param = 0x0);
   
   void AllowVariations(Bool_t *bPlaneOnOff);
   void SetNonLinear(Bool_t *bPlaneOnOff, Bool_t *bVarXYT);
   
   void FixPlane(Int_t iPlane);
   void FixParameter(Int_t param, Double_t value);
   void SetNonLinear(Int_t param);
   void AddConstraint(Double_t *factor, Double_t value );
   void InitGlobalParameters(Double_t *par);
   void Init(Int_t nGlobal, Int_t nLocal, Int_t nStdDev, Int_t nPlanes);
   void SetIterations(Int_t iter);
   
   //! Set array of local derivatives
   void SetLocalDerivative(Int_t index, Double_t value) {
      fLocalDerivatives[index] = value;
   }
   
   //! Set array of global derivatives
   void SetGlobalDerivative(Int_t index, Double_t value) {
      fGlobalDerivatives[index] = value;
   }
   
   void LocalFit(Int_t iTrack, Double_t *lTrackParam, Int_t lSingleFit);
   void GlobalFit(Double_t *parameters,Double_t *errors,Double_t *pulls);
   void PrintGlobalParameters();
   Double_t GetParError(Int_t iPar);
   
   Int_t  GetNParPlane() const {return fNParPlane;}
   
private:
   //! ctr
   TAIRalignM(const TString name, Bool_t flagVtx, Bool_t flagIt, Bool_t flagMsd, Int_t weight);
   void ResetLocalEquation();
   void LocalEquationX(Double_t* param = 0x0);
   void LocalEquationY(Double_t* param = 0x0);

public:
   void   LoopEvent(Int_t nEvts = 1);
   
private:
   void    CreateHistogram();   
   void    InitParameters();
   void    FillStatus();
   void    FillStatus(TAVTbaseParConf* parConf, Int_t offset = 0);


   void    UpdateTransfo(Int_t idx);
   void    UpdateAlignmentParams(Double_t* parameters);
   void    UpdateGeoMaps();
   void    UpdateGeoMapsUVW(fstream &fileIn, fstream &fileOut, Int_t idx);
   void    Reset();
      
private:
   TAGroot*             fAGRoot;        // pointer to TAGroot
   TAGgeoTrafo*         fGeoTrafo;
   Bool_t               fFlagVtx;
   Bool_t               fFlagIt;
   Bool_t               fFlagMsd;
   TAGdataDsc*          fpNtuTrackVtx;	 // Track VTX
   TAGparaDsc*          fpConfigVtx;	 // configuration dsc
   TAGparaDsc*          fpGeoMapVtx;    // geometry para dsc
   TAGdataDsc*          fpNtuTrackItr;	 // Track IT
   TAGparaDsc*          fpConfigItr;	 // configuration dsc
   TAGparaDsc*          fpGeoMapItr;    // geometry para dsc
   TAGparaDsc*          fpGeoMapG;      // geometry para dsc
   TAGactTreeReader*    fInfile;        // action for reading cluster
   TAIRmillepede*       fMillepede;     // pointer to align para
   
   const TString        fFileName;      // input file
      
   Float_t*  fTiltW;
   Float_t*  fAlignmentU;
   Float_t*  fAlignmentV;
   
   Bool_t*   fStatus;
   Int_t*    fDevStatus;
   
   
   Bool_t fFixPlaneRef1;
   Bool_t fFixPlaneRef2;
   Int_t  fPlaneRef1;
   Int_t  fPlaneRef2;
   
   Int_t  fOffsetItr;
   
   TGraphErrors*        fResidualX;
   TGraphErrors*        fResidualY;
   TArrayI              fSecArray;   // contains the sensor number to be aligned
   
   
   Bool_t   fDoF[3];         // Flags degrees of freedom to align (x,y,phi)
   Double_t fAllowVar[3];    // "Encouraged" variation for degrees of freedom
   Double_t fStartFac;       // Initial value for chi2 cut
                             // if > 1 Iterations in AliMillepede are turned on
   Double_t fResCutInitial;  // Cut on residual for first iteration
   Double_t fResCut;         // Cut on residual for other iterations
   
   TAVTbaseCluster* fCluster;
   
   Int_t    fNGlobal;        // Number of global parameters
   Int_t    fNLocal;         // Number of local parameters
   Int_t    fNStdDev;        // Number of standard deviations for chi2 cut
   Int_t    fNParPlane;      // Number of degrees of freedom per chamber
   Int_t    fNPlanes;        // Total number of detection elements
   
   Double_t fMeas[2];        // Current measurement (depend on B field On/Off)
   Double_t fSigma[2];       // Estimated resolution on measurement
   
   Double_t fGlobalDerivatives[30]; // Array of global derivatives
   Double_t fLocalDerivatives[4];   // Array of local derivatives
   
   
   Double_t fPhi;            // Azimuthal tilt of detection element
   Double_t fCosPhi;         // Cosine of fPhi
   Double_t fSinPhi;         // Sine of fPhi
   Int_t    fDebugLevel;     // debug level

 
   
   TH1F*   fpResXC[50];        // Residual in X
   TH1F*   fpResYC[50];        // Residual in Y
   TH1F*   fpResTotXC;        // Total residuals in x
   TH1F*   fpResTotYC;        // Total residuals in y
   
   
   
   ClassDef(TAIRalignM,0)
};

#endif
