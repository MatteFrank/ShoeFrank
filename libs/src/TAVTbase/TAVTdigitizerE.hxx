#ifndef _TAVTdigitizerE_HXX
#define _TAVTdigitizerE_HXX

/*!
 \file TAVTdigitizerE.hxx
 \brief   Declaration of TAVTdigitizerE
 */
/*------------------------------------------+---------------------------------*/

#include <map>

#include "Riostream.h"
#include "TF1.h"
#include "TH2F.h"
#include "TMath.h"
#include "TObject.h"
#include "TRandom3.h"
#include "TString.h"

#include "TAVTbaseDigitizer.hxx"

// --------------------------------------------------------------------------------------
class TAVTparGeo;
class TAVTdigitizerE : public TAVTbaseDigitizer {
   
   // Class to digitize the energy into pixel based on given patterns
public:
   TAVTdigitizerE(TAVTbaseParGeo* parGeo);
   virtual ~TAVTdigitizerE();
   
   // Define radii of shell
   void           DefineRadii();
   // Make cluster
   Bool_t         MakeCluster(Double_t x0, Double_t y0, Double_t zin, Double_t zout);
   
private:
   std::map<Int_t, Float_t>  fMapRadius55;  ///< map radius for half-half pixel impact
   std::map<Int_t, Float_t>  fMapRadius50;  ///< map radius for half-edge pixel impact
   std::map<Int_t, Float_t>  fMapRadius00;  ///< map radius for edge-edge pixel impact
   std::map<Int_t, Float_t>* fMapRadius;    ///< current map radius

private:
   // Set region of impact
   Bool_t          SetRegion(Double_t x0, Double_t y0);
   // define radius for edge-edge pixel impact
   void            DefineRadius00();
   // define radius for half-edge pixel impact
   void            DefineRadius50();
   // define radius for edge-edge pixel impact
   void            DefineRadius55();
   
private:
   static Int_t    fgkMaxTurn;   ///< maximum turn
   static Int_t    fgkShel00[];  ///< shell array 00
   static Int_t    fgkShel55[];  ///< shell array 55
   static Int_t    fgkShel50[];  ///< shell array 50
   static Int_t    fgkMeshWidth; ///< Mesh width
   
};
        

#endif

