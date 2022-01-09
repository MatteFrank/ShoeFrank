#ifndef _TAGbaseDigitizer_HXX
#define _TAGbaseDigitizer_HXX
/*!
 \file TAGbaseDigitizer.hxx
 \brief   Declaration of TAGbaseDigitizer.
 */
/*------------------------------------------+---------------------------------*/


#include "TAGobject.hxx"


class TF1;
class TAVTbaseParGeo;
class TAGbaseDigitizer : public TAGobject {
   
public:
   TAGbaseDigitizer();
   virtual ~TAGbaseDigitizer();
   
   //! Pure virtual process method
   virtual Bool_t Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin = 0, Double_t zout = 0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0) = 0;

   ClassDef(TAGbaseDigitizer,0)
};

#endif
