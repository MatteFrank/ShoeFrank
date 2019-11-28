#ifndef _TAGbaseDigitizer_HXX
#define _TAGbaseDigitizer_HXX
/*!
 \file
 \version $Id: TAGbaseDigitizer.hxx,v $
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
   
   virtual Bool_t Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin = 0, Double_t zout = 0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99) = 0;

   ClassDef(TAGbaseDigitizer,0)
};

#endif
