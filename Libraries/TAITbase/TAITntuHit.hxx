#ifndef _TAITntuHit_HXX
#define _TAITntuHit_HXX
/*!
 \file TAITntuHit.hxx
  \brief   Declaration of TAITntuHit.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include "TAIThit.hxx"
#include "TAGdata.hxx"

class TAITparGeo;
class TAITntuHit : public TAGdata {
   
protected:
    Int_t             fSensorsN;        ///< number of sensors
    TObjArray*        fListOfPixels;      ///< List of pixels
    std::map<pair<int, int>, int > fMap; //! do not store
   
public:
   TAITntuHit(Int_t sensorsN = 0);
   virtual          ~TAITntuHit();
   
   // Get hit for a given sensor
   TAIThit*          GetPixel(Int_t iSensor, Int_t iPixel);
   // Get hit for a given sensor (const)
   const TAIThit*    GetPixel(Int_t iSensor, Int_t iPixel) const;
   
   // Get list of pixels for a given sensor
   TClonesArray*     GetListOfPixels(Int_t iSensor);
   // Get list of pixels for a given sensor (const)
   TClonesArray*     GetListOfPixels(Int_t iSensor) const;
   
   // Get number of pixels for given sensor
   Int_t             GetPixelsN(Int_t iSensor) const;
   
   // New pixel
   TAIThit*          NewPixel(Int_t sensor, Double_t value, Int_t aLine, Int_t aColumn);
   
   // Set up clone
   virtual void      SetupClones();
   // Clear
   virtual void      Clear(Option_t* opt="");
   // To stream
   virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
   ClassDef(TAITntuHit,1)
};

#endif

























