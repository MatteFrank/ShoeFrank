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
   Int_t fInput;
   Int_t fValue;
   Int_t fIndex;
   Int_t fView;
   Int_t fStrip;
         
public:
   TAMSDrawHit();
   TAMSDrawHit( Int_t input, Int_t value, Int_t view, Int_t strip );
   virtual ~TAMSDrawHit() {;}

   //! Comapre method
   Int_t      Compare(const TObject* obj) const;
   //! Get input type
   Int_t      GetInput()        const    { return fInput;  }
   //! Get value (pulse height)
   Int_t      GetValue()        const    { return fValue;  }
   //! Get line number
   Int_t      GetView()         const    { return fView;   }
   //! Get column number
   Int_t      GetStrip()        const    { return fStrip;  }
   //! Get index
   Int_t      GetIndex()        const    { return fIndex;  }
   //! Is Sortable
   Bool_t     IsSortable()      const    { return kTRUE;   }

   //! Set input type
   void     SetInput(Int_t input)        { fInput = input; }
   //! Set value
   void     SetValue(Int_t value)        { fValue = value; }
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

class TAMSDntuRaw : public TAGdata {
   
protected:
   TObjArray* fListOfPixels;

private:
   static TString fgkBranchName;    // Branch name in TTree

public:
   TAMSDntuRaw();
   ~TAMSDntuRaw();
   
   //! Get list of pixels
   TClonesArray*     GetPixels(Int_t iSensor)  const;
 
   //! Get number of pixels
   Int_t             GetPixelsN(Int_t iSensor)  const;      
   
   //! Get pixel
   TAMSDrawHit*       GetPixel(Int_t iSensor, Int_t iPixel);        
   const TAMSDrawHit* GetPixel(Int_t iSensor, Int_t iPixel)   const;
   
   //! Add pixel to list
   void              AddPixel(Int_t sensor, Int_t value, Int_t aLine, Int_t aColumn);
  
   //! Set up clones
   void             SetupClones();

   //! Clear pixel list
   virtual void     Clear(Option_t* opt="");
   
   //! To stream
   virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

public:   
   static const Char_t* GetBranchName()   { return fgkBranchName.Data(); }

   ClassDef(TAMSDntuRaw,2)
};

# endif
