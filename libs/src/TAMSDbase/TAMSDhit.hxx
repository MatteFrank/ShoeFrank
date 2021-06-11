
#ifndef _TAMSDhit_HXX
#define _TAMSDhit_HXX

// ROOT classes
#include "TArrayI.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "TAGobject.hxx"
#include "TAGroot.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"



/*------------------------------------------+---------------------------------*/

class TAMSDhit : public TAGobject {
  
protected:
  Int_t     fSensorId;
  Float_t   fPosition;                 // strip position in the detector frame
  Float_t   fEnergyLoss;
  Int_t     fIndex;
  Int_t     fView;
  Int_t     fStrip;
  
  TArrayI   fMCindex;                  // Index of the hit created in the simulation
  TArrayI   fMcTrackIdx;               // Index of the track created in the simulation
  
public:
   TAMSDhit();
   TAMSDhit( Int_t input, Float_t value, Int_t view, Int_t strip);

  virtual ~TAMSDhit() {};

  Bool_t	  IsEqual(const TObject* obj) const;
   
  //! Comapre method
  Int_t      Compare(const TObject* obj) const;
   
  // Clear
  void       Clear(Option_t* option = "C");

  //! Get input type
  Int_t      GetSensorId()      const    { return fSensorId;   }
  //! Get energy loss
  Float_t    GetEnergyLoss()    const    { return fEnergyLoss; }
  //! Get line number
  Int_t      GetView()          const    { return fView;       }
  //! Get column number
  Int_t      GetStrip()         const    { return fStrip;      }
  //! Get index
  Int_t      GetIndex()         const    { return fIndex;      }
  // Get position
  Float_t    GetPosition()      const    { return fPosition;   }
  
  //! Is Sortable
  Bool_t     IsSortable()       const    { return kTRUE;     }
  
  // MC track id
  Int_t      GetMcIndex(Int_t index)    const   { return fMCindex[index];       }
  Int_t      GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
  Int_t      GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }
  
  
  //! Set input type
  void     SetSensorId(Int_t input)     { fSensorId = input; }
  //! Set value
  void     SetEnergyLoss(Float_t v)     { fEnergyLoss = v;   }
  //! Set line number
  void     SetView(Int_t view)          { fView = view;      }
  //! Set column number
  void     SetStrip(Int_t strip)        { fStrip = strip;    }
  //! Set index
   void     SetIndex(Int_t index)       { fIndex = index;    }
  // Set position
  void     SetPosition(Float_t pos)     { fPosition = pos;   }
  // Add MC track Id
  void     AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);
  
  
  ClassDef(TAMSDhit,4)                            // Pixel or Pixel of a Detector Plane
};

//##############################################################################



#endif























