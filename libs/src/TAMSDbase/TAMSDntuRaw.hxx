#ifndef _TAMSDntuRaw_HXX
#define _TAMSDntuRaw_HXX
/*!
 \file
 \version $Id: TAMSDntuRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
 \brief   Declaration of TAMSDntuRaw.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include "TAMSDntuHit.hxx"
#include "TAGdata.hxx"

class TAMSDparGeo;
class TAMSDntuRaw : public TAGdata {
   
protected:
   //using TObjArray here
   TObjArray*        fListOfStrips;
   TAMSDparGeo*      fpGeoMap;         //! do not store
   
   std::map<pair<int, int>, int > fMap; //!

private:
   static TString    fgkBranchName;    // Branch name in TTree
   
public:
   TAMSDntuRaw();
   virtual           ~TAMSDntuRaw();
   
   TAMSDntuHit*       GetStrip(Int_t iSensor, Int_t iStrip);
   const TAMSDntuHit* GetStrip(Int_t iSensor, Int_t iStrip) const;
   
   TClonesArray*      GetListOfStrips(Int_t iSensor);
   TClonesArray*      GetListOfStrips(Int_t iSensor) const;
   
   Int_t              GetStripsN(Int_t iSensor) const;
   
   TAMSDntuHit*       NewStrip(Int_t sensor, Double_t value, Int_t aView, Int_t aStrip);
   
   virtual void       SetupClones();
   
   virtual void       Clear(Option_t* opt="");
   
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
   
public:
   static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
   
   ClassDef(TAMSDntuRaw,1)
};

#endif

























