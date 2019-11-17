#ifndef _TAIRalignM_HXX
#define _TAIRalignM_HXX
/*!
 \file
 \brief   Declaration of TAIRalignM.
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"
#include "TH1F.h"
#include "TObject.h"
#include "TString.h"
#include "TArrayI.h"
#include "TGraphErrors.h"

#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAGactTreeReader.hxx"
#include "TAIRmillepede.hxx"

class TAVTbaseParGeo;
class TAVTbaseParConf;
class TObjArray;
class TAIRtrack;
class TAIRcluster;

class TAIRalignM : public TObject {
   
private:
   static TAIRalignM* fgInstance; // static instance of class
   
public:
   //! Instance of class
   static TAIRalignM* Instance(const TString name = "16O_C2H4_200_1.root");
   virtual ~TAIRalignM();
   
   void ProcessTrack(TAIRtrack *track, Double_t* param = 0x0);
   
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
   TAIRalignM(const TString name);
   void ResetLocalEquation();
   void LocalEquationX(TAIRcluster* cluster, Double_t* param = 0x0);
   void LocalEquationY(TAIRcluster* cluster, Double_t* param = 0x0);

public:
   void   LoopEvent(Int_t nEvts = 1);
   
private:
   void    CreateHistogram();   
   void    InitParameters();
   void    FillStatus();
   void    FillStatus(TAVTbaseParConf* parConf, Int_t offset = 0);

   void    UpdateAlignmentParams(Double_t* parameters);
   void    UpdateGeoMaps();
   void    UpdateGeoMapsUVW(fstream &fileIn, fstream &fileOut, Int_t idx);
   void    Reset();
      
private:
   TAGroot*             fAGRoot;        // pointer to TAGroot
   TAGparaDsc*          fpConfigVtx;	 // configuration dsc
   TAGparaDsc*          fpGeoMapVtx;    // geometry para dsc
   TAGparaDsc*          fpConfigItr;	 // configuration dsc
   TAGparaDsc*          fpGeoMapItr;    // geometry para dsc
   TAGdataDsc*          fpNtuTrackIr;   // Track IR
   TAGactTreeReader*    fInfile;        // action for reading track
   TAIRmillepede*       fMillepede;     // pointer to align para
   
   const TString        fFileName;      // input file
      
   Float_t*             fTiltW;
   Float_t*             fAlignmentU;
   Float_t*             fAlignmentV;
   Bool_t*              fStatus;
   Int_t*               fDevStatus;
   Int_t                fOffsetItr;
   TArrayI              fSecArray;   // contains the sensor number to be aligned
   
   // Millepede members
   Bool_t               fDoF[3];         // Flags degrees of freedom to align (x,y,phi)
   Double_t             fAllowVar[3];    // "Encouraged" variation for degrees of freedom
   Double_t             fStartFac;       // Initial value for chi2 cut
                             // if > 1 Iterations in AliMillepede are turned on
   Double_t             fResCutInitial;  // Cut on residual for first iteration
   Double_t             fResCut;         // Cut on residual for other iterations
   
   Int_t                fNGlobal;        // Number of global parameters
   Int_t                fNLocal;         // Number of local parameters
   Int_t                fNStdDev;        // Number of standard deviations for chi2 cut
   Int_t                fNParPlane;      // Number of degrees of freedom per chamber
   Int_t                fNPlanes;        // Total number of detection elements
   
   Double_t             fMeas[2];        // Current measurement (depend on B field On/Off)
   Double_t             fSigma[2];       // Estimated resolution on measurement
   
   Double_t             fGlobalDerivatives[30]; // Array of global derivatives
   Double_t             fLocalDerivatives[4];   // Array of local derivatives
   
   Double_t             fPhi;            // Azimuthal tilt of detection element
   Double_t             fCosPhi;         // Cosine of fPhi
   Double_t             fSinPhi;         // Sine of fPhi
   Int_t                fDebugLevel;     // debug level

   TH1F*                fpResXC[50];        // Residual in X
   TH1F*                fpResYC[50];        // Residual in Y
   TH1F*                fpResTotXC;        // Total residuals in x
   TH1F*                fpResTotYC;        // Total residuals in y
   
   ClassDef(TAIRalignM,0)
};

#endif
