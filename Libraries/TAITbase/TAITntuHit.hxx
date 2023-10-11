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
    UInt_t            fTriggerNumber;   ///< trigger number
    ULong64_t         fTimeStamp;       ///< time stamp
    TObjArray*        fListOfPixels;    ///< list of pixels
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
   TAIThit*           NewPixel(Int_t sensor,  Double_t value, Int_t aLine, Int_t aColumn, Int_t frame = 0);

   
   //! Get trigger number
   Int_t             GetTriggerNumber()    const { return fTriggerNumber; }
   //! Set trigger number
   void              SetTriggerNumber(Int_t nb)  { fTriggerNumber = nb;   }
   
   //! Get time stamp
   ULong64_t         GetTimeStamp()        const { return fTimeStamp;     }
   //! Set time stamp
   void              SetTimeStamp(ULong64_t nb)  { fTimeStamp = nb;       }
   
   // Set up clone
   virtual void      SetupClones();
   // Clear
   virtual void      Clear(Option_t* opt="");
   // To stream
   virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
   ClassDef(TAITntuHit,2)
};

#endif

























