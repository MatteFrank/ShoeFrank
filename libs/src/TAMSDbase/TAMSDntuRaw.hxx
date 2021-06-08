#ifndef _TAMSDntuRaw_HXX
#define _TAMSDntuRaw_HXX

#include "TAGdata.hxx"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TString.h"

// --------------------------------------------------------------------------------------
/** TAMSDrawHit simple class containers for raw hit (pixel)
 
 \author Ch. Finck
 */

class TAMSDrawHit : public TObject {
   
protected:
   Int_t      fSensorId;
   Double32_t fCharge;
   Int_t      fIndex;
   Int_t      fView;
   Int_t      fStrip;
         
public:
   TAMSDrawHit();
   TAMSDrawHit( Int_t id, Int_t view, Int_t strip, double charge);
   virtual ~TAMSDrawHit() {;}

   //! Comapre method
   Int_t      Compare(const TObject* obj) const;
   //! Get input type
   Int_t      GetSensorId()     const    { return fSensorId;  }
   //! Get value (pulse height)
   Int_t      GetCharge()        const    { return fCharge;  }
   //! Get line number
   Int_t      GetView()         const    { return fView;   }
   //! Get column number
   Int_t      GetStrip()        const    { return fStrip;  }
   //! Get index
   Int_t      GetIndex()        const    { return fIndex;  }
   //! Is Sortable
   Bool_t     IsSortable()      const    { return kTRUE;   }

   //! Set input type
   void     SetSensorId(Int_t id)        { fSensorId = id; }
   //! Set value
   void     SetCharge(Int_t value)        { fCharge = value; }
   //! Set line number
   void     SetView(Int_t view)          { fView = view;   }
   //! Set column number
   void     SetStrip(Int_t strip)        { fStrip = strip; }
   //! Set index
   void     SetIndex(Int_t index)        { fIndex = index; }
   
   ClassDef(TAMSDrawHit,1)
};

//######################################################################################
/** TAMSDntuRaw class for raw data
 
 \author Ch. Finck
 */
class TAMSDparGeo;
class TAMSDntuRaw : public TAGdata {
   
protected:
   TObjArray*   fListOfStrips;
   TAMSDparGeo* fpGeoMap;
   
private:
   static TString fgkBranchName;    // Branch name in TTree

public:
   TAMSDntuRaw();
   ~TAMSDntuRaw();
   
   //! Get list of pixels
   TClonesArray*      GetStrips(Int_t iSensor)  const;
 
   //! Get number of pixels
   Int_t              GetStripsN(Int_t iSensor)  const;
   
   //! Get pixel
   TAMSDrawHit*       GetStrip(Int_t iSensor, Int_t iStrip);
   const TAMSDrawHit* GetStrip(Int_t iSensor, Int_t iStrip)   const;
   
   //! Add pixel to list
   void               AddStrip(Int_t sensor, Int_t view, Int_t aStrip, double value);
  
   //! Set up clones
   void               SetupClones();

   //! Clear pixel list
   virtual void       Clear(Option_t* opt="");
   
   //! To stream
   virtual void       ToStream(ostream& os=cout, Option_t* option="") const;

public:   
   static const Char_t* GetBranchName()   { return fgkBranchName.Data(); }

   ClassDef(TAMSDntuRaw,2)
};

# endif
