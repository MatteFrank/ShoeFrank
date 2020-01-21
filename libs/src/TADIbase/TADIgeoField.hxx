#ifndef TADIgeoField_H
#define TADIgeoField_H


#include <map>
#include <tuple>

#include <TVector3.h>
#include <TH3F.h>
#include "TVirtualMagField.h"

#include "TAGgeoTrafo.hxx"
#include "TADIparGeo.hxx"

using namespace std;

class TADIgeoField : public TVirtualMagField {

public:

	TADIgeoField(TADIparGeo* diGeo);

   ~TADIgeoField();

   TVector3 GetField(const TVector3& position)    const;
   TVector3 Interpolate(const TVector3& position) const;
   
   void     Field(const Double_t* pos, Double_t* fieldB);
   
   void     FromFile(TString& name);

private:
   TADIparGeo*  fpDiGeoMap;
   TAGgeoTrafo* fpFootGeo;

   TH3F*        fMagHistoX;
   TH3F*        fMagHistoY;
   TH3F*        fMagHistoZ;
   
   Int_t        fMaxBinX;
   Int_t        fMaxBinY;
   Int_t        fMaxBinZ;
   
   ClassDef(TADIgeoField,1)
};


#endif
