#ifndef _TAVTntuHit_HXX
#define _TAVTntuHit_HXX
/*!
 \file
 \version $Id: TAVTntuHit.hxx
 \brief   Declaration of TAVTntuHit.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include "TAVThit.hxx"
#include "TAGdata.hxx"

class TAVTparGeo;
class TAVTntuHit : public TAGdata {
   
protected:
   //using TObjArray here
   TObjArray*        fListOfPixels;    /// list of pixels
   TAVTparGeo*       fpGeoMap;         //!< do not store
   
    std::map<pair<int, int>, int > fMap; //!< pixel map
    
private:
   static TString    fgkBranchName;    /// Branch name in TTree
   
public:
   TAVTntuHit();
   virtual          ~TAVTntuHit();
   
   //! Get hit for a given sensor
   TAVThit*          GetPixel(Int_t iSensor, Int_t iPixel);
   //! Get hit  for a given sensor const
   const TAVThit*    GetPixel(Int_t iSensor, Int_t iPixel) const;
   
   //! Get list of pixels for a given sensor
   TClonesArray*     GetListOfPixels(Int_t iSensor);
   //! Get list of pixels for a given sensor (const)
   TClonesArray*     GetListOfPixels(Int_t iSensor) const;
   
   //! Get number of pixels for given sensor
   Int_t             GetPixelsN(Int_t iSensor) const;
   
   //! New pixel
   TAVThit*          NewPixel(Int_t sensor, Double_t value, Int_t aLine, Int_t aColumn);
   
   //! Set up clone
   virtual void      SetupClones();
   //! Clear
   virtual void      Clear(Option_t* opt="");
   //! To stream
   virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   //! Get branch
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   ClassDef(TAVTntuHit,1)
};

#endif

























