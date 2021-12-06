
#ifndef _TAVTbaseHit_HXX
#define _TAVTbaseHit_HXX

/*!
 \file TAVTbaseHit.hxx
 \version $Id: TAVTbaseHit
 \brief  contains information respect to a pixel in cmos detectors
 index, position, noise, pulse height, size, etc...
 
 Revised in 2018 by Matteo Franchini franchinim@bo.infn.it
 Back to a class compliant with storing in a root file by Ch. Finck
 
 All the coordinates are in cm and in the detector reference frame, i.e. the center
 is the center of the detector.
 
 \author Ch. Finck
 */

// ROOT classes
#include "TObject.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TVector3.h"

#include "TAGroot.hxx"
#include "TAVTparGeo.hxx"
#include "TAGparaDsc.hxx"
#include "TAGdataDsc.hxx"

/*------------------------------------------+---------------------------------*/

class TAVTbaseHit : public TAGobject {
   
protected:
	Int_t              fSensorId;                 /// number of the sensor
	TVector3           fPosition;                 /// pixel position in the detector frame
	// TVector3           fSize;                  /// size in uvw directions

   Int_t              fPixelIndex;               /// index of the pixel
	Int_t              fPixelLine;                /// line in the matrix
	Int_t              fPixelColumn;              /// column in the matrix
	Int_t              fLayer;

	Double32_t         fRawValue;                 /// the rawvalue
	Double32_t         fPulseHeight;              /// pulseheight on pixel
   Bool_t             fValidFrames;              /// ok when 3 consecutive frame numbers

   TArrayI            fMCindex;                  /// Index of the hit created in the simulation
   TArrayI            fMcTrackIdx;               /// Index of the track created in the simulation

public:

    TAVTbaseHit() {};
    TAVTbaseHit( Int_t iSensor, const Int_t aIndex, Double_t aValue);
    TAVTbaseHit( Int_t iSensor, Double_t aValue, Int_t aLine, Int_t aColumn);
    virtual ~TAVTbaseHit();

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
    Double_t           Distance( TAVTbaseHit&         aPixel);
    //! Compute distance from a given position
    Double_t           Distance( const TVector3&     aPosition);
    //! Compute distance in U direction from a given pixel
    Double_t           DistanceU( TAVTbaseHit&        aPixel);
    //! Compute distance in U direction from a given position
    Double_t           DistanceU( const TVector3&     aPosition);
    //! Compute distance in V direction from a given pixel
    Double_t           DistanceV( TAVTbaseHit&         aPixel);
    //! Compute distance in V direction from a given position
    Double_t           DistanceV( const TVector3&     aPosition);


   //! Get MC index
   Int_t      GetMcIndex(Int_t index)    const   { return fMCindex[index];       }
   //! Get MC track index
   Int_t      GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
   //! Get MC track number
   Int_t      GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }
   
   //! Add MC track Id
   void       AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);

    ClassDef(TAVTbaseHit,4)                            // Pixel or Pixel of a Detector Plane
};

//##############################################################################



#endif























