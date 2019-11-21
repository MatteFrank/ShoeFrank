
#ifndef _TAVTbaseNtuHit_HXX
#define _TAVTbaseNtuHit_HXX

// ROOT classes
#include "TObject.h"
#include "TArrayC.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TVector3.h"

// all 3 needed to take from gTagROOT
#include "TAGroot.hxx"
#include "TAVTparGeo.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"

// class TAVTrawHit;

/** TAVTbaseNtuHit class contains information respect to a pixel in cmos detectors
 index, position, noise, pulse height, size, etc...
 
    Revised in 2018 by Matteo Franchini franchinim@bo.infn.it
    Back to a class compliant with storing in a root file by Ch. Finck

    All the coordinates are in cm and in the detector reference frame, i.e. the center
    is the center of the detector.

*/
/*------------------------------------------+---------------------------------*/

class TAVTbaseNtuHit : public TAGobject {
   
protected:
	Int_t              fSensorId;                 // number of the sensor
	TVector3           fPosition;                 // pixel position in the detector frame
	// TVector3           fSize;                  // size in uvw directions

   Int_t              fPixelIndex;               // index of the pixel
	Int_t              fPixelLine;                // line in the matrix
	Int_t              fPixelColumn;              // column in the matrix
	Int_t              fLayer;

	Double32_t         fRawValue;                 // the rawvalue
	Double32_t         fPulseHeight;              // pulseheight on pixel
   Bool_t             fValidFrames;              // ok when 3 consecutive frame numbers 

   TArrayC            fMCindex;                  // Index of the hit created in the simulation
   TArrayC            fMcTrackIdx;               // Index of the track created in the simulation

public:

    TAVTbaseNtuHit() {};
    TAVTbaseNtuHit( Int_t iSensor, const Int_t aIndex, Double_t aValue);
    TAVTbaseNtuHit( Int_t iSensor, Double_t aValue, Int_t aLine, Int_t aColumn);
    virtual ~TAVTbaseNtuHit();

    void               Clear(Option_t* option = "C");

    Bool_t	           IsEqual(const TObject* obj) const;
   
    // Setter methods
    void               SetRawValue(Double_t aRV)       { fRawValue = aRV;         }
    void               SetPulseHeight(Double_t aPH)    { fPulseHeight = aPH;      }
    void               SetPosition(TVector3 aPosition) { fPosition = aPosition;   }
    void               SetValidFrames(Bool_t ok)       { fValidFrames = ok;       }

    // Getter methods
    Int_t              GetPixelIndex()   const         { return  fPixelIndex;     }
    Int_t              GetPixelLine()    const         { return  fPixelLine;      }
    Int_t              GetPixelColumn()  const         { return  fPixelColumn;    }
    Int_t              GetLayer()        const         { return  fLayer;          }
    Int_t              GetSensorID()     const         { return  fSensorId;       }
   
   
    Double_t           GetRawValue()     const         { return  fRawValue;       }
    Double_t           GetPulseHeight()  const         { return  fPulseHeight;    }

    Bool_t             IsValidFrames()   const         { return fValidFrames;     }
   
    TVector3&          GetPosition()                   { return fPosition;        }
   
    //! Compute distance from a given pixel
    Double_t           Distance( TAVTbaseNtuHit&         aPixel);
    //! Compute distance from a given position
    Double_t           Distance( const TVector3&     aPosition);
    //! Compute distance in U direction from a given pixel
    Double_t           DistanceU( TAVTbaseNtuHit&        aPixel);
    //! Compute distance in U direction from a given position
    Double_t           DistanceU( const TVector3&     aPosition);
    //! Compute distance in V direction from a given pixel
    Double_t           DistanceV( TAVTbaseNtuHit&         aPixel);
    //! Compute distance in V direction from a given position
    Double_t           DistanceV( const TVector3&     aPosition);


   // MC track id
   Int_t      GetMcIndex(Int_t index)    const   { return fMCindex[index];       }
   Int_t      GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
   Int_t      GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }
   
   // Add MC track Id
   void       AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);

    ClassDef(TAVTbaseNtuHit,4)                            // Pixel or Pixel of a Detector Plane
};

//##############################################################################



#endif























