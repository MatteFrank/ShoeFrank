#ifndef _TAIRalignM_HXX
#define _TAIRalignM_HXX
/*!
 \file TAIRalignM.hxx
 \brief   Declaration of TAIRalignM.
 
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"
#include "TH1F.h"
#include "TObject.h"
#include "TString.h"
#include "TArrayI.h"
#include "TVector3.h"
#include "TGraphErrors.h"

#include "TAGroot.hxx"
#include "TAGdataDsc.hxx"
#include "TAGparaDsc.hxx"
#include "TAGactTreeReader.hxx"
#include "TAIRmillepede.hxx"

class TAGcampaignManager;
class TAVTbaseParGeo;
class TAVTbaseParConf;
class TObjArray;
class TAIRtrack;
class TAIRcluster;

class TAIRalignM : public TObject {
   
private:
   static TAIRalignM* fgInstance; // static instance of class
   
public:
   // Instance of class
   static TAIRalignM* Instance(const TString name = "16O_C2H4_200_1.root", const TString exp = "", Int_t runNUmber = 1);
   virtual ~TAIRalignM();
   
   // Process track
   void ProcessTrack(TAIRtrack *track, Double_t* param = 0x0);
   
   // Allow parameter variations
   void AllowVariations(Bool_t *bSensorOnOff);
   // Non linear extrapolation (e.g.: magnetic field)
   void SetNonLinear(Bool_t *bSensorOnOff, Bool_t *bVarXYT);
   
   // Fix sensor
   void FixSensor(Int_t iSensor);
   // Fix parameters
   void FixParameter(Int_t param, Double_t value);
   // Set non linear parameters
   void SetNonLinear(Int_t param);
   // Add constraints
   void AddConstraint(Double_t *factor, Double_t value );
   // Init global parameters
   void InitGlobalParameters(Double_t *par);
   // Global initialisation
   void Init(Int_t nGlobal, Int_t nLocal, Int_t nStdDev, Int_t nSensors);
   // Set iteration
   void SetIterations(Int_t iter);
   
   //! Set array of local derivatives
   void SetLocalDerivative(Int_t index, Double_t value) {
      fLocalDerivatives[index] = value;
   }
   
   //! Set array of global derivatives
   void SetGlobalDerivative(Int_t index, Double_t value) {
      fGlobalDerivatives[index] = value;
   }
   // Local fit
   void LocalFit(Int_t iTrack, Double_t *lTrackParam, Int_t lSingleFit);
   // Global fit
   void GlobalFit(Double_t *parameters,Double_t *errors,Double_t *pulls);
   // Print parameters
   void PrintGlobalParameters();
   // Set parameter errors
   Double_t GetParError(Int_t iPar);
   //! Set number of sensors
   Int_t  GetNParSensor() const {return fNParSensor;}
   
private:
   // ctr
   TAIRalignM(const TString name, const TString exp = "", Int_t runNUmber = 1);
   // Reset local equations
   void ResetLocalEquation();
   // Local equation on X
   void LocalEquationX(TAIRcluster* cluster, Double_t* param, TAIRtrack* track);
   // Local equation on Y
   void LocalEquationY(TAIRcluster* cluster, Double_t* param, TAIRtrack* track);

public:
   // Loop over events
   void   LoopEvent(Int_t nEvts = 1);
   
private:
   // Create histograms
   void    CreateHistogram();
   // Init parameters
   void    InitParameters();
   // Fill status
   void    FillStatus();
   // Fill status from a given configuration file
   void    FillStatus(TAVTbaseParConf* parConf, Int_t offset = 0);
   // Update alignment parameters
   void    UpdateAlignmentParams(Double_t* parameters);
   // Update geo maps
   void    UpdateGeoMaps();
   // Update geo file
   void    UpdateGeoMapsUVW(fstream &fileIn, fstream &fileOut, Int_t idx);
   // Reset
   void    Reset();
      
private:
   TAGroot*             fAGRoot;        ///< pointer to TAGroot
   TAGcampaignManager*  fCampManager;   ///< campaigne manager
   const TString        fFileName;      ///< input file
   Int_t                fRunNumber;     ///< Run number
   TAGparaDsc*          fpConfigVtx;	 ///< configuration dsc
   TAGparaDsc*          fpGeoMapVtx;    ///< geometry para dsc
   TAGparaDsc*          fpConfigItr;	 ///< configuration dsc
   TAGparaDsc*          fpGeoMapItr;    ///< geometry para dsc
   TAGdataDsc*          fpNtuTrackIr;   ///< Track IR
   TAGactTreeReader*    fInfile;        ///< action for reading track
   TAIRmillepede*       fMillepede;     ///< pointer to align para
         
   Int_t*               fDevStatus;     ///< Status device array
   Int_t                fOffsetItr;     ///< ITR offset
   TArrayI              fSecArray;      ///< contains the sensor number to be aligned
   
   // Millepede members
   TVector3             fTrackPos0;     ///< First track position
   TVector3             fTrackPos;      ///< Track position
   Double_t             fCosTheta;      ///< Cos theta
   Double_t             fSinTheta;      ///< Sin theta
   Bool_t               fDoF[3];        ///< Flags degrees of freedom to align (x,y,phi)
   Double_t             fAllowVar[3];   ///< "Encouraged" variation for degrees of freedom
   Double_t             fStartFac;      ///< Initial value for chi2 cut, if > 1 Iterations in AliMillepede are turned on
   Double_t             fResCutInitial; ///< Cut on residual for first iteration
   Double_t             fResCut;        ///< Cut on residual for other iterations
   
   Int_t                fNGlobal;       ///< Number of global parameters
   Int_t                fNLocal;        ///< Number of local parameters
   Int_t                fNStdDev;       ///< Number of standard deviations for chi2 cut
   Int_t                fNParSensor;    ///< Number of degrees of freedom per sensor
   Int_t                fNSensors;      ///< Total number of detection elements
   
   Double_t             fMeas[2];       ///< Current measurement (depend on B field On/Off)
   Double_t             fSigma[2];      ///< Estimated resolution on measurement
   
   Double_t             fGlobalDerivatives[30]; ///< Array of global derivatives
   Double_t             fLocalDerivatives[4];   ///< Array of local derivatives
   
   Double_t             fPhi;           ///< Azimuthal tilt of detection element
   Double_t             fCosPhi;        ///< Cosine of fPhi
   Double_t             fSinPhi;        ///< Sine of fPhi

   TH1F*                fpResXC[50];    ///< Residual in X
   TH1F*                fpResYC[50];    ///< Residual in Y
   TH1F*                fpResTotXC;     ///< Total residuals in x
   TH1F*                fpResTotYC;     ///< Total residuals in y
   
   ClassDef(TAIRalignM,0)
};

#endif
