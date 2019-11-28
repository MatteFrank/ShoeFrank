#ifndef _TAVTbaseParGeo_HXX
#define _TAVTbaseParGeo_HXX
/*!
  \file
  \brief   Declaration of TAVTbaseParGeo.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include <map>
#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAGparTools.hxx"


class TGeoHMatrix;
class TGeoVolume;
class TAGionisMaterials;
//##############################################################################

class TAVTbaseParGeo : public TAGparTools {
      
protected:
   TAGionisMaterials* fIonisation; //! pointer for ionisation property
   Int_t      fSensorsN;         // Number of sensors
   TString    fkDefaultGeoName;  // default par geo file name
   Int_t      fLayersN;          // Number of layer (planes)

   TString    fTypeName;         // Type name
   Int_t      fTypeNumber;       // Type number
   Int_t      fPixelsNx;         // Number of pixels in U direction
   Int_t      fPixelsNy;         // Number of pixels in V direction
   Float_t    fPitchX;           // Pitch value in U direction
   Float_t    fPitchY;           // Pitch value in U direction
   TVector3   fTotalSize;        // Total size of sensor
   
   TVector3   fEpiSize;          // Sensitive size of sensor
   TVector3   fEpiOffset;        // Position offset of sensitive size
   TString    fEpiMat;           // Material of epitaxial
   Float_t    fEpiMatDensity;    // density of epitaxial material
   Float_t    fEpiMatExc;        // mean excitation energy of epitaxial material

   Float_t    fPixThickness;     // Pixel Thickness
   TString    fPixMat;           // Material of pixel
   TString    fPixMatDensities;  // density of pixel material for each component
   TString    fPixMatProp;       // Material of pixels component proportion
   Float_t    fPixMatDensity;    // density of pixel material

   Int_t      fSupportInfo;      // Boolean for support info (only for IT)
   Bool_t     fFlagMC;           // MC flag
   Bool_t     fFlagIt;           // IT flag
   
   struct SensorParameter_t : public  TObject {
	  Int_t     SensorIdx;   // sensor index
	  Int_t     TypeIdx;     // type index
	  TVector3  Position;    // current position
	  TVector3  Tilt;        // current tilt angles 
	  Float_t   AlignmentU;  // U alignment
	  Float_t   AlignmentV;  // V alignment
	  Float_t   TiltW;       // Tilted angle around beam axis
     Bool_t    IsReverseX;  // Rotation of 180 around X axis
     Bool_t    IsReverseY;  // Rotation of 180 around Y axis
   };
   SensorParameter_t  fSensorParameter[128];

   TVector3   fMinPosition;
   TVector3   fMaxPosition;
   TVector3   fSizeBox;

protected:
   static const Int_t   fgkDefSensorsN;   // default number of sensors
   
public:
   TAVTbaseParGeo();
    virtual ~TAVTbaseParGeo();

   //! Transform point from the global detector reference frame
   //! to the local sensor reference frame of the detection id
   void            Detector2Sensor(Int_t detID,  Double_t xg, Double_t yg, Double_t zg, 
                                   Double_t& xl, Double_t& yl, Double_t& zl) const;
   
   TVector3        Detector2Sensor(Int_t detID, TVector3& glob) const;
   TVector3        Detector2SensorVect(Int_t detID, TVector3& glob) const;
   
   //! Transform point from the local reference frame
   //! of the detection id to the global reference frame 
   void            Sensor2Detector(Int_t detID,  Double_t xl, Double_t yl, Double_t zl, 
                                Double_t& xg, Double_t& yg, Double_t& zg) const;
   
   TVector3        Sensor2Detector(Int_t detID, TVector3& loc) const;
   TVector3        Sensor2DetectorVect(Int_t detID, TVector3& loc) const;
   
   //! Get position sensor
   TVector3        GetSensorPosition(Int_t iSensor);
 
   virtual int GetSensorID( int layer, int /*col*/, int /*row*/ )    { return layer; };

   //! Get Sensor idx for a given type and sensor in type
   Int_t GetSensorIdx(Int_t iSensor, Int_t type);

   //! Get number of Sensors
   Int_t GetSensorsN()                 const { return fSensorsN;       }
   //! Get number of layers
   Int_t GetNLayers()                  const { return fLayersN;        }

   //! Get type name of sensor
   const Char_t* GetTypeName()         const { return fTypeName.Data();}
   //! Get type of sensor
   Int_t GetType()                     const { return fTypeNumber;     }
   
   //! Get number of pixel in X
   Int_t GetNPixelX()                  const { return fPixelsNx;       }
   //! Get number of pixel in Y
   Int_t GetNPixelY()                  const { return fPixelsNy;       }
   //! Get pitch size in X
   Float_t GetPitchX()                 const { return fPitchX;         }
   //! Get pitch size in Y
   Float_t GetPitchY()                 const { return fPitchY;         }
   
   //! Get total size of sensor
   TVector3 GetTotalSize()             const { return fTotalSize;      }

   //! Get epitaxial size of sensor
   TVector3 GetEpiSize()               const { return fEpiSize;        }
   //! Get epitaxial offset
   TVector3 GetEpiOffset()             const { return fEpiOffset;      }
   //! Get epitaxial material
   TString GetEpiMaterial()            const { return fEpiMat;         }
   //! Get epitaxial density
   Float_t GetEpiMatDensity()          const { return fEpiMatDensity;  }
   //! Get epitaxial excitation
   Float_t GetEpiMatExc()              const { return fEpiMatExc;      }

   //! Get pixel thickness
   Float_t GetPixThickness()           const { return fPixThickness;   }
   //! Get pixel material
   TString GetPixMaterial()            const { return fPixMat;         }
   //! Get pixel coponent densities
   TString GetPixMatDensities()        const { return fPixMatDensities;}
   //! Get pixel material proportion
   TString GetPixMatProp()             const { return fPixMatProp;     }
   //! Get pixel density
   Float_t GetPixMatDensity()          const { return fPixMatDensity;  }
   
   //! Get MC flag
   Bool_t GetMcFlag()                  const { return fFlagMC;         }
   //! Set MC flag
   void   SetMcFlag(Bool_t flag = true)      { fFlagMC = flag;         }
   
   //! Get position from pixel line/column
   virtual Float_t GetPositionU(Int_t column)         const;
   virtual Float_t GetPositionV(Int_t line)           const;
   virtual Int_t   GetIndex(Int_t line, Int_t column) const;
   
   //! Get column/line from x/y position
   virtual Int_t   GetColumn(Float_t x) const;
   virtual Int_t   GetLine(Float_t y)   const;

   
   //! Read parameters from file
   virtual Bool_t   FromFile(const TString& name = "");

   // Read support info if any
   virtual void     ReadSupportInfo() { return; }
   
   // Define material
   virtual void    DefineMaterial();

   // Define box of detector
   void            DefineMaxMinDimension();

   // Getter
   TVector3        GetBoxSize()     const { return fSizeBox;  }
   TVector3        GetMinPoistion() const { return fMinPosition; }
   TVector3        GetMaxPoistion() const { return fMaxPosition; }
   
   //! Get Sensor parameter
   SensorParameter_t& GetSensorPar(Int_t idx) { return fSensorParameter[idx]; }
   
public:
   static Int_t         GetDefSensorsN()      { return fgkDefSensorsN;        }
   
   ClassDef(TAVTbaseParGeo,1)
};

#endif
