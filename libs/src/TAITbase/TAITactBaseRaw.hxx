#ifndef _TAITactBaseRaw_HXX
#define _TAITactBaseRaw_HXX

#include "TAVTmi26Type.hxx"

#include "TAVTactBaseRaw.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"
#include <vector>


class TAITntuHit;
class TH2F;
class TH1F;

/*!
 \file
 \version $Id: TAITactBaseRaw.hxx $
 \brief   Declaration of TAITactBaseRaw.
 */
/*------------------------------------------+---------------------------------*/

using namespace std;

class TAITactBaseRaw : public TAVTactBaseRaw
 {
public:
   
   explicit TAITactBaseRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGparaDsc* p_geomap=0, TAGparaDsc* p_config=0, TAGparaDsc* pParMap=0);
   virtual  ~TAITactBaseRaw();
   
protected:

   //! Add pixel to list
   void  AddPixel( Int_t input, Int_t value, Int_t aLine, Int_t aColumn);
   
   ClassDef(TAITactBaseRaw,0)
};

#endif
