#ifndef _TAVTactDatRaw_HXX
#define _TAVTactDatRaw_HXX


#include "TAVTactBaseRaw.hxx"

/*!
 \file
 \version $Id: TAVTactDatRaw.hxx $
 \brief   Declaration of TAVTactDatRaw.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;
class TAVTactDatRaw : public TAVTactBaseRaw {
public:
   
   explicit TAVTactDatRaw(const char* name=0, TAGdataDsc* p_datraw=0, TAGdataDsc* p_datmbs=0, TAGparaDsc* p_pargeo=0, TAGparaDsc* p_parconf=0);
   virtual  ~TAVTactDatRaw();
   
   virtual Bool_t  Action();
  
private: 
   static const UChar_t fgkGeoId1;
   static const UChar_t fgkGeoId2;
   
public:

   typedef struct {
	  
	  UInt_t  Length  : 18;
	  UChar_t Free    : 1; 
	  UChar_t Type    : 3;
	  UChar_t Ovf     : 1;
	  UShort_t GeoId  : 5;
	  UChar_t MBO     : 4;
   } IR_Header; 
   
   TAGdataDsc*       fpDatMbs;		    // input data dsc

private:
   
   //! Find vertex data
   Bool_t FindData(const UInt_t* data, Int_t subEventSize);
   
   ClassDef(TAVTactDatRaw,0)
};

#endif
