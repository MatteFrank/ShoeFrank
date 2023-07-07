#ifndef _TAMPntuHit_HXX
#define _TAMPntuHit_HXX
/*!
 \file TAMPntuHit.hxx
  \brief   Declaration of TAMPntuHit.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include "TAMPhit.hxx"
#include "TAGdata.hxx"

class TAMPparGeo;
class TAMPntuHit : public TAGdata {
   
protected:
   TObjArray*        fListOfPixels;      ///< List of pixels
   //! geometry parameter
   TAMPparGeo*       fpGeoMap;           //! do not store
   //! pixel map
    std::map<pair<int, int>, int > fMap; //! do not store
   
public:
   TAMPntuHit();
   virtual          ~TAMPntuHit();
   
   // Get hit for a given sensor
   TAMPhit*          GetPixel(Int_t iSensor, Int_t iPixel);
   // Get hit for a given sensor (const)
   const TAMPhit*    GetPixel(Int_t iSensor, Int_t iPixel) const;
   
   // Get list of pixels for a given sensor
   TClonesArray*     GetListOfPixels(Int_t iSensor);
   // Get list of pixels for a given sensor (const)
   TClonesArray*     GetListOfPixels(Int_t iSensor) const;
   
   // Get number of pixels for given sensor
   Int_t             GetPixelsN(Int_t iSensor) const;
   
   // New pixel
   TAMPhit*          NewPixel(Int_t sensor, Double_t value, Int_t aLine, Int_t aColumn);
   
   // Set up clone
   virtual void      SetupClones();
   // Clear
   virtual void      Clear(Option_t* opt="");
   // To stream
   virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
   ClassDef(TAMPntuHit,1)
};

#endif

























