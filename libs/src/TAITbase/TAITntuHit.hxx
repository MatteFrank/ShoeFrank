#ifndef _TAITntuHit_HXX
#define _TAITntuHit_HXX
/*!
 \file
 \version $Id: TAITntuHit.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAITntuHit.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include "TAIThit.hxx"
#include "TAGdata.hxx"

class TAITparGeo;
class TAITntuHit : public TAGdata {
   
protected:
   //using TObjArray here
   TObjArray*        fListOfPixels;
   TAITparGeo*       fpGeoMap;         //! do not store
   
    std::map<pair<int, int>, int > fMap; //!
    
private:
   static TString    fgkBranchName;    // Branch name in TTree
   
public:
   TAITntuHit();
   virtual          ~TAITntuHit();
   
   TAIThit*          GetPixel(Int_t iSensor, Int_t iPixel);
   const TAIThit*    GetPixel(Int_t iSensor, Int_t iPixel) const;
   
   TClonesArray*     GetListOfPixels(Int_t iSensor);
   TClonesArray*     GetListOfPixels(Int_t iSensor) const;
   
   Int_t             GetPixelsN(Int_t iSensor) const;
   
   TAIThit*          NewPixel(Int_t sensor, Double_t value, Int_t aLine, Int_t aColumn);
   
   virtual void      SetupClones();
   
   virtual void      Clear(Option_t* opt="");
   
   virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   ClassDef(TAITntuHit,1)
};

#endif

























