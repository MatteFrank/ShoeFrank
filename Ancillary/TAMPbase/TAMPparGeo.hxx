#ifndef _TAMPparGeo_HXX
#define _TAMPparGeo_HXX
/*!
  \file TAMPparGeo.hxx
  \brief   Declaration of TAMPparGeo.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include <map>
#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAVTbaseParGeo.hxx"


class TGeoHMatrix;
class TGeoVolume;
class TAGionisMaterials;
//##############################################################################

class TAMPparGeo : public TAVTbaseParGeo {
      
protected:
   //! pointer for ionisation property
   TAGionisMaterials*              fIonisation; //! pointer for ionisation property
   map<pair<Int_t, Int_t>, Int_t>  fMapIdxType;
   Int_t      fSensorsN;         ///< number of sensors
   Int_t      fSensorTypesN[10]; ///< number of sensors per type
   Int_t      fTypesN;           ///< Number of sensor types
   TString    fkDefaultGeoName;  ///< default par geo file name
   Int_t      fLayersN;          ///< Number of layer (planes/stations)
   Int_t      fSubLayersN;       ///< Number of sub layer (planes)

   struct TypeParameter_t : public  TObject {
      TString    TypeName;         ///< Type name
      Int_t      TypeNumber;       ///< Type number
      Int_t      PixelsNx;         ///< Number of pixels in U direction
      Int_t      PixelsNy;         ///< Number of pixels in V direction
      Float_t    PitchX;           ///< Pitch value in U direction
      Float_t    PitchY;           ///< Pitch value in U direction
      TVector3   TotalSize;        ///< Total size of sensor
      TVector3   EpiSize;          ///< Sensitive size of sensor
      TVector3   EpiOffset;        ///< Position offset of sensitive size
      TString    EpiMat;           ///< Material of epitaxial
      Float_t    EpiMatDensity;    ///< density of epitaxial material
      Float_t    EpiMatExc;        ///< mean excitation energy of epitaxial material
      Float_t    PixThickness;     ///< Pixel Thickness
      TString    PixMat;           ///< Material of pixel
      TString    PixMatDensities;  ///< density of pixel material for each component
      TString    PixMatProp;       ///< Material of pixels component proportion
      Float_t    PixMatDensity;    ///< density of pixel material

   };
   TypeParameter_t  fTypeParameter[10];
   
   
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
   std::size_t*    fSensorArray;  ///< [10] Array of sensor
   std::map<float, std::vector<std::size_t>> fSensorMap; ///< map sensor

protected:
   static const Int_t   fgkDefSensorsN;   ///< default number of sensors
   static const TString fgkBaseName;      ///< base mame
   static const TString fgkDefParaName;   ///< par name
   
protected:
   //! Fill sensor map
   virtual void    FillSensorMap();

public:
   TAMPparGeo();
    virtual ~TAMPparGeo();

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

   //! Get number of Sensors
   Int_t GetSensorTypesN(Int_t type)    const { return fSensorTypesN[type]; }
   //! Get number of types
   Int_t GetTypesN()                    const { return fTypesN; }
   
   //! Get type name of sensor
   const Char_t* GetTypeName(Int_t type)         const { return fTypeParameter[type].TypeName.Data();}
   //! Get type of sensor
   Int_t GetType(Int_t type)                     const { return fTypeParameter[type].TypeNumber;     }
   //! Get number of pixel in X
   Int_t GetPixelsNx(Int_t type)                 const { return fTypeParameter[type].PixelsNx;       }
   //! Get number of pixel in Y
   Int_t GetPixelsNy(Int_t type)                 const { return fTypeParameter[type].PixelsNy;       }
   //! Get pitch size in X
   Float_t GetPitchX(Int_t type)                 const { return fTypeParameter[type].PitchX;         }
   //! Get pitch size in Y
   Float_t GetPitchY(Int_t type)                 const { return fTypeParameter[type].PitchY;         }
   //! Get total size of sensor
   TVector3 GetTotalSize(Int_t type)             const { return fTypeParameter[type].TotalSize;      }
   //! Get epitaxial size of sensor
   TVector3 GetEpiSize(Int_t type)               const { return fTypeParameter[type].EpiSize;        }
   //! Get epitaxial offset
   TVector3 GetEpiOffset(Int_t type)             const { return fTypeParameter[type].EpiOffset;      }
   //! Get epitaxial material
   TString GetEpiMaterial(Int_t type)            const { return fTypeParameter[type].EpiMat;         }
   //! Get epitaxial density
   Float_t GetEpiMatDensity(Int_t type)          const { return fTypeParameter[type].EpiMatDensity;  }
   //! Get epitaxial excitation
   Float_t GetEpiMatExc(Int_t type)              const { return fTypeParameter[type].EpiMatExc;      }
   //! Get pixel thickness
   Float_t GetPixThickness(Int_t type)           const { return fTypeParameter[type].PixThickness;   }
   //! Get pixel material
   TString GetPixMaterial(Int_t type)            const { return fTypeParameter[type].PixMat;         }
   //! Get pixel coponent densities
   TString GetPixMatDensities(Int_t type)        const { return fTypeParameter[type].PixMatDensities;}
   //! Get pixel material proportion
   TString GetPixMatProp(Int_t type)             const { return fTypeParameter[type].PixMatProp;     }
   //! Get pixel density
   Float_t GetPixMatDensity(Int_t type)          const { return fTypeParameter[type].PixMatDensity;  }
   
   //! Get MC flag
   Bool_t GetMcFlag()                  const { return fFlagMC;         }
   //! Set MC flag
   void   SetMcFlag(Bool_t flag = true)      { fFlagMC = flag;         }
   
   // Get position from pixel line/column
   virtual Float_t GetPositionU(Int_t column, Int_t type = 0)         const;
   virtual Float_t GetPositionV(Int_t line, Int_t type = 0)           const;
   virtual Int_t   GetIndex(Int_t line, Int_t column, Int_t type = 0) const;
   
   // Get column/line from x/y position
   virtual Int_t   GetColumn(Float_t x, Int_t type = 0) const;
   virtual Int_t   GetLine(Float_t y, Int_t type = 0)   const;

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
   
   //! Get type parameter
   TypeParameter_t& GetTypePar(Int_t idx)     { return fTypeParameter[idx];   }

public:
   //! Get base name
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   //! Get default para name
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   //! Get default number of sensors
   static Int_t         GetDefSensorsN()      { return fgkDefSensorsN;        }
   
   ClassDef(TAMPparGeo,1)
};

#endif
