#ifndef _TAMSDntuHit_HXX
#define _TAMSDntuHit_HXX
/*!
 \file TAMSDntuHit.hxx
 \brief   Declaration of TAMSDntuHit.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include "TAMSDhit.hxx"
#include "TAGdata.hxx"

class TAMSDparGeo;
class TAMSDntuHit : public TAGdata {
   
protected:
   //using TObjArray here
   TObjArray*        fListOfStrips;    ///< list of strips
   TAMSDparGeo*      fpGeoMap;         //! geometry parameter, do not store
   
   std::map<pair<int, int>, int > fMap; //! map
   
public:
   TAMSDntuHit();
   virtual           ~TAMSDntuHit();
   
   TAMSDhit*          GetStrip(Int_t iSensor, Int_t iStrip);
   const TAMSDhit*    GetStrip(Int_t iSensor, Int_t iStrip) const;


   TClonesArray*      GetListOfStrips(Int_t iSensor);
   TClonesArray*      GetListOfStrips(Int_t iSensor) const;
   

   Int_t              GetStripsN(Int_t iSensor) const;
   
   TAMSDhit*          NewStrip(Int_t sensor, Double_t value, Int_t aView, Int_t aStrip);
   
   virtual void       SetupClones();
   
   virtual void       Clear(Option_t* opt="");
   
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;
   
   ClassDef(TAMSDntuHit,2)
};

#endif

























