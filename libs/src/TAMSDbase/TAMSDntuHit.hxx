
#ifndef _TAMSDntuHit_HXX
#define _TAMSDntuHit_HXX

// ROOT classes
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "TAGobject.hxX"
#include "TAGroot.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"



// class TAVTrawHit;

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
   static const Int_t fgkMAXTRACK = 100;

   Int_t     fSensorId;
   Float_t   fPosition;                 // strip position in the detector frame
   Int_t     fValue;
   Int_t     fIndex;
   Int_t     fView;
   Int_t     fStrip;
   Bool_t    fFound;

   Int_t     fMcTrackCount;                 // Variable that count the number of times a crystal is touched
   Int_t     fMcTrackId[fgkMAXTRACK];         // Id of the track created in the simulation

public:
   TAMSDntuHit();
    TAMSDntuHit( Int_t input, Int_t value, Int_t view, Int_t strip);

   virtual ~TAMSDntuHit() {};

   Bool_t	  IsEqual(const TObject* obj) const;
   
   //! Comapre method
   Int_t      Compare(const TObject* obj) const;
   
   //! Get input type
   Int_t      GetSensorId()     const    { return fSensorId; }
   //! Get value (pulse height)
   Int_t      GetValue()        const    { return fValue;    }
   //! Get line number
   Int_t      GetView()         const    { return fView;     }
   //! Get column number
   Int_t      GetStrip()        const    { return fStrip;    }
   //! Get index
   Int_t      GetIndex()        const    { return fIndex;    }
   //! Found
   Bool_t     Found()           const    { return fFound;    }
   // Get position
   Float_t    GetPosition()     const    { return fPosition;  }

   
   //! Is Sortable
   Bool_t     IsSortable()      const    { return kTRUE;   }
   
   // MC track id
   Int_t      GetTrackIdMc(Int_t index) const   { return fMcTrackId[index];  }
   Int_t      GetMcTrackCount()         const   { return fMcTrackCount;      }

   
   //! Set input type
   void     SetSensorId(Int_t input)     { fSensorId = input; }
   //! Set value
   void     SetValue(Int_t value)        { fValue = value;    }
   //! Set line number
   void     SetView(Int_t view)          { fView = view;      }
   //! Set column number
   void     SetStrip(Int_t strip)        { fStrip = strip;    }
   //! Set index
   void     SetIndex(Int_t index)        { fIndex = index;    }
   //! Set Found
   void     SetFound(Bool_t found)       { fFound = found;    }
   // Set position
   void     SetPosition(Float_t pos)     { fPosition = pos;   }

   // Add MC track Id
   void     AddMcTrackId(Int_t trackId);

   
    ClassDef(TAMSDntuHit,3)                            // Pixel or Pixel of a Detector Plane
};

//##############################################################################



#endif























