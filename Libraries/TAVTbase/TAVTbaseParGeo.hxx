#ifndef _TAVTbaseParGeo_HXX
#define _TAVTbaseParGeo_HXX
/*!
  \file TAVTbaseParGeo.hxx
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
   //! pointer for ionisation property
   TAGionisMaterials* fIonisation; //! pointer for ionisation property
   Int_t      fSensorsN;         ///< Number of sensors
   TString    fkDefaultGeoName;  ///< default par geo file name
   Int_t      fLayersN;          ///< Number of layer (planes/stations)
   Int_t      fSubLayersN;       ///< Number of sub layer (planes)

   TString    fTypeName;         ///< Type name
   Int_t      fTypeNumber;       ///< Type number
   Int_t      fPixelsNx;         ///< Number of pixels in U direction
   Int_t      fPixelsNy;         ///< Number of pixels in V direction
   Float_t    fPitchX;           ///< Pitch value in U direction
   Float_t    fPitchY;           ///< Pitch value in U direction
   TVector3   fTotalSize;        ///< Total size of sensor
   
   TVector3   fEpiSize;          ///< Sensitive size of sensor
   TVector3   fEpiOffset;        ///< Position offset of sensitive size
   TString    fEpiMat;           ///< Material of epitaxial
   Float_t    fEpiMatDensity;    ///< density of epitaxial material
   Float_t    fEpiMatExc;        ///< mean excitation energy of epitaxial material

   Float_t    fPixThickness;     ///< Pixel Thickness
   TString    fPixMat;           ///< Material of pixel
   TString    fPixMatDensities;  ///< density of pixel material for each component
   TString    fPixMatProp;       ///< Material of pixels component proportion
   Float_t    fPixMatDensity;    ///< density of pixel material

   Int_t      fSupportInfo;      ///< Boolean for support info (only for IT)
   Bool_t     fFlagMC;           ///< MC flag
   Bool_t     fFlagIt;           ///< IT flag
  
   /*!
    \struct SensorParameter_t
    \brief  Sensors parameters
   */
   struct SensorParameter_t : public  TObject {
	  Int_t     SensorIdx;   ///< sensor index
	  Int_t     TypeIdx;     ///< type index
	  TVector3  Position;    ///< current position
	  TVector3  Tilt;        ///< current tilt angles
	  Float_t   AlignmentU;  ///< U alignment
	  Float_t   AlignmentV;  ///< V alignment
	  Float_t   TiltW;       ///< Tilted angle around beam axis
     Bool_t    IsReverseX;  ///< Rotation of 180 around X axis
     Bool_t    IsReverseY;  ///< Rotation of 180 around Y axis
   };
   SensorParameter_t  fSensorParameter[128]; ///< sensor parameters

   TVector3   fMinPosition;  ///< minimum position
   TVector3   fMaxPosition;  ///< maximum position
   TVector3   fSizeBox;      ///< box size
   Int_t      fSensPerLayer; ///< number of sensor per layer
   std::size_t*    fSensorArray;  ///< Array of sensor
   std::map<float, std::vector<std::size_t>> fSensorMap; ///< map sensor
   
protected:
   //! Fill sensor map
   virtual void    FillSensorMap();

public:
   TAVTbaseParGeo();
    virtual ~TAVTbaseParGeo();

   // Transform point from the global detector reference frame
   // to the local sensor reference frame of the detection id
   void            Detector2Sensor(Int_t detID,  Double_t xg, Double_t yg, Double_t zg, 
                                   Double_t& xl, Double_t& yl, Double_t& zl) const;
   
   TVector3        Detector2Sensor(Int_t detID, TVector3& glob) const;
   TVector3        Detector2SensorVect(Int_t detID, TVector3& glob) const;
   
   // Transform point from the local reference frame
   // of the detection id to the global reference frame
   void            Sensor2Detector(Int_t detID,  Double_t xl, Double_t yl, Double_t zl, 
                                Double_t& xg, Double_t& yg, Double_t& zg) const;
   
   TVector3        Sensor2Detector(Int_t detID, TVector3& loc) const;
   TVector3        Sensor2DetectorVect(Int_t detID, TVector3& loc) const;
   
   // Get position sensor
   TVector3        GetSensorPosition(Int_t iSensor);
   
   //! Get number of Sensors
   Int_t GetSensorsN()                 const { return fSensorsN;       }
   //! Get number of Sensors per link
   Int_t GetSensPerDataLink()          const { return fSensorsN;       }
   //! Get number of layers
   Int_t GetLayersN()                  const { return fLayersN;        }
   //! Get number of sub layers
   Int_t GetSubLayersN()               const { return fSubLayersN;     }

   //! Get type name of sensor
   virtual const Char_t* GetTypeName()         const { return fTypeName.Data();}
   //! Get type of sensor
   virtual Int_t GetType()                     const { return fTypeNumber;     }
   
   //! Get number of pixel in X
   virtual Int_t GetPixelsNx()                 const { return fPixelsNx;       }
   //! Get number of pixel in Y
   virtual Int_t GetPixelsNy()                 const { return fPixelsNy;       }
   //! Get pitch size in X
   virtual Float_t GetPitchX()                 const { return fPitchX;         }
   //! Get pitch size in Y
   virtual Float_t GetPitchY()                 const { return fPitchY;         }
   
   //! Get total size of sensor
   virtual TVector3 GetTotalSize()             const { return fTotalSize;      }

   //! Get epitaxial size of sensor
   virtual TVector3 GetEpiSize()               const { return fEpiSize;        }
   //! Get epitaxial offset
   virtual TVector3 GetEpiOffset()             const { return fEpiOffset;      }
   //! Get epitaxial material
   virtual TString GetEpiMaterial()            const { return fEpiMat;         }
   //! Get epitaxial density
   virtual Float_t GetEpiMatDensity()          const { return fEpiMatDensity;  }
   //! Get epitaxial excitation
   virtual Float_t GetEpiMatExc()              const { return fEpiMatExc;      }

   //! Get pixel thickness
   virtual Float_t GetPixThickness()           const { return fPixThickness;   }
   //! Get pixel material
   virtual TString GetPixMaterial()            const { return fPixMat;         }
   //! Get pixel coponent densities
   virtual TString GetPixMatDensities()        const { return fPixMatDensities;}
   //! Get pixel material proportion
   virtual TString GetPixMatProp()             const { return fPixMatProp;     }
   //! Get pixel density
   virtual Float_t GetPixMatDensity()          const { return fPixMatDensity;  }
   
   //! Get MC flag
   Bool_t GetMcFlag()                  const { return fFlagMC;         }
   //! Set MC flag
   void   SetMcFlag(Bool_t flag = true)      { fFlagMC = flag;         }
   
   // Get position from pixel line/column
   virtual Float_t GetPositionU(Int_t column)         const;
   virtual Float_t GetPositionV(Int_t line)           const;
   virtual Int_t   GetIndex(Int_t line, Int_t column) const;
   
   // Get column/line from x/y position
   virtual Int_t   GetColumn(Float_t x) const;
   virtual Int_t   GetLine(Float_t y)   const;

   // return array of sensor id's for a given layer
   virtual std::size_t* GetSensorsPerLayer(Int_t iLayer);

   // Get layer position in Z
   virtual Float_t GetLayerPosZ(Int_t layer);

   // Read parameters from file
   virtual Bool_t   FromFile(const TString& name = "");

   //! Read support info if any
   virtual void     ReadSupportInfo() { return; }
   
   // Define material
   virtual void    DefineMaterial();

   // Define box of detector
   void            DefineMaxMinDimension();

   //! Getter Box size
   TVector3        GetBoxSize()     const { return fSizeBox;     }
   //! Getter minimum position
   TVector3        GetMinPoistion() const { return fMinPosition; }
   //! Getter maximum position
   TVector3        GetMaxPoistion() const { return fMaxPosition; }
   
   //! Get Sensor parameter
   SensorParameter_t& GetSensorPar(Int_t idx) { return fSensorParameter[idx]; }
   
   ClassDef(TAVTbaseParGeo,1)
};

#endif
