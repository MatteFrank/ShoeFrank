
#ifndef _TAMSDntuHit_HXX
#define _TAMSDntuHit_HXX

// ROOT classes
#include "TArrayC.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "TAGobject.hxx"
#include "TAGroot.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"


/** TAMSDntuHit class contains information respect to a pixel in cmos detectors
 index, position, noise, pulse height, size, etc...
 
    Revised in 2018 by Matteo Franchini franchinim@bo.infn.it
    Back to a class compliant with storing in a root file by Ch. Finck

    All the coordinates are in cm and in the detector reference frame, i.e. the center
    is the center of the detector.

*/
/*------------------------------------------+---------------------------------*/

class TAMSDntuHit : public TAGobject {
  
protected:
  Int_t     fSensorId;
  Float_t   fPosition;                 // strip position in the detector frame
  Float_t   fValue;
  Int_t     fIndex;
  Int_t     fView;
  Int_t     fStrip;
  
  TArrayC   fMCindex;                  // Id of the hit created in the simulation
  TArrayC   fMcTrackId;                // Id of the track created in the simulation
  
public:
   TAMSDntuHit();
    TAMSDntuHit( Int_t input, Float_t value, Int_t view, Int_t strip);

   virtual ~TAMSDntuHit() {};

   Bool_t	  IsEqual(const TObject* obj) const;
   
   //! Comapre method
   Int_t      Compare(const TObject* obj) const;
   
   // Clear
   void       Clear(Option_t* option = "C");

   //! Get input type
   Int_t      GetSensorId()     const    { return fSensorId; }
   //! Get value (pulse height)
   Float_t    GetValue()        const    { return fValue;    }
   //! Get line number
   Int_t      GetView()         const    { return fView;     }
   //! Get column number
  Int_t      GetStrip()        const    { return fStrip;    }
  //! Get index
  Int_t      GetIndex()        const    { return fIndex;    }
  // Get position
  Float_t    GetPosition()     const    { return fPosition;  }
  
  
  //! Is Sortable
  Bool_t     IsSortable()      const    { return kTRUE;   }
  
  // MC track id
  Int_t      GetMcIndex(Int_t index)   const   { return fMCindex[index];      }
  Int_t      GetMcTrackI(Int_t index)  const   { return fMcTrackId[index];    }
  Int_t      GetMcTrackCount()         const   { return fMcTrackId.GetSize(); }
  
  
  //! Set input type
  void     SetSensorId(Int_t input)     { fSensorId = input; }
  //! Set value
  void     SetValue(Float_t value)      { fValue = value;    }
  //! Set line number
  void     SetView(Int_t view)          { fView = view;      }
  //! Set column number
  void     SetStrip(Int_t strip)        { fStrip = strip;    }
  //! Set index
   void     SetIndex(Int_t index)        { fIndex = index;    }
  // Set position
  void     SetPosition(Float_t pos)     { fPosition = pos;   }
  
  // Add MC track Id
  void     AddMcTrackId(Int_t trackId, Int_t mcId = -1);
  
  
  ClassDef(TAMSDntuHit,3)                            // Pixel or Pixel of a Detector Plane
};

//##############################################################################



#endif























