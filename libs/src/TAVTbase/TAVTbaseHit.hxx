
#ifndef _TAVTbaseHit_HXX
#define _TAVTbaseHit_HXX

/*!
 \file TAVTbaseHit.hxx
 \brief  contains information respect to a pixel in cmos detectors
 index, position, noise, pulse height, size, etc...
  
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
	Int_t              fSensorId;                 ///< number of the sensor
	TVector3           fPosition;                 ///< pixel position in the detector frame
	// TVector3           fSize;                  ///< size in uvw directions

   Int_t              fPixelIndex;               ///< index of the pixel
	Int_t              fPixelLine;                ///< line in the matrix
	Int_t              fPixelColumn;              ///< column in the matrix
	Int_t              fLayer;                    ///< layer number

	Double32_t         fRawValue;                 ///< the rawvalue
	Double32_t         fPulseHeight;              ///< pulseheight on pixel
   Bool_t             fValidFrames;              ///< ok when 3 consecutive frame numbers

   TArrayI            fMCindex;                  ///< Index of the hit created in the simulation
   TArrayI            fMcTrackIdx;               ///< Index of the track created in the simulation

public:

    //! Default constructor
    TAVTbaseHit() {};
    TAVTbaseHit( Int_t iSensor, const Int_t aIndex, Double_t aValue);
    TAVTbaseHit( Int_t iSensor, Double_t aValue, Int_t aLine, Int_t aColumn);
    virtual ~TAVTbaseHit();

    //! Clear
    void               Clear(Option_t* option = "C");
    //! Equal method
    Bool_t	           IsEqual(const TObject* obj) const;
   
    // Setter methods
    //! Set raw value
    void               SetRawValue(Double_t aRV)       { fRawValue = aRV;         }
    //! Set pulse height
    void               SetPulseHeight(Double_t aPH)    { fPulseHeight = aPH;      }
    //! Set position
    void               SetPosition(TVector3 aPosition) { fPosition = aPosition;   }
    //! Set valid frame
    void               SetValidFrames(Bool_t ok)       { fValidFrames = ok;       }

    // Getter methods
    //! Get pixel index
    Int_t              GetPixelIndex()   const         { return  fPixelIndex;     }
    //! Get pixel line
    Int_t              GetPixelLine()    const         { return  fPixelLine;      }
    //! Get pixel column
    Int_t              GetPixelColumn()  const         { return  fPixelColumn;    }
    //! Get layer
    Int_t              GetLayer()        const         { return  fLayer;          }
    //! Get sensor ID
    Int_t              GetSensorId()     const         { return  fSensorId;       }
   
    //! Get raw value
    Double_t           GetRawValue()     const         { return  fRawValue;       }
    //! Get pulse height
    Double_t           GetPulseHeight()  const         { return  fPulseHeight;    }
    //! Check valid frames
    Bool_t             IsValidFrames()   const         { return fValidFrames;     }
    //! Get position
    TVector3&          GetPosition()                   { return fPosition;        }
   
    // Compute distance from a given position
    Double_t           Distance( const TVector3&     aPosition);
    // Compute distance in U direction from a given position
    Double_t           DistanceU( const TVector3&     aPosition);
    // Compute distance in V direction from a given position
    Double_t           DistanceV( const TVector3&     aPosition);

   //! Get MC index
   Int_t      GetMcIndex(Int_t index)    const   { return fMCindex[index];       }
   //! Get MC track index
   Int_t      GetMcTrackIdx(Int_t index) const   { return fMcTrackIdx[index];    }
   //! Get MC track number
   Int_t      GetMcTracksN()             const   { return fMcTrackIdx.GetSize(); }
   
   // Add MC track Id
   void       AddMcTrackIdx(Int_t trackIdx, Int_t mcId = -1);

    ClassDef(TAVTbaseHit,4)                            // Pixel or Pixel of a Detector Plane
};

//##############################################################################



#endif























