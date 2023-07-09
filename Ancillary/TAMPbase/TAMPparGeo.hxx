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
   map<pair<Int_t, Int_t>, Int_t>  fMapIdxType;
   Int_t      fSensorTypesN[10]; ///< number of sensors per type
   Int_t      fTypesN;           ///< Number of sensor types

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
   

protected:
   static const TString fgkBaseName;      ///< base mame
   static const TString fgkDefParaName;   ///< par name

public:
   TAMPparGeo();
    virtual ~TAMPparGeo();

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
   
   //! Get type name of sensor
   const Char_t* GetTypeName()                   const override { return fTypeParameter[0].TypeName.Data();   }
   //! Get type of sensor
   Int_t GetType()                               const override { return fTypeParameter[0].TypeNumber;        }
   //! Get number of pixel in X
   Int_t GetPixelsNx()                           const override { return fTypeParameter[0].PixelsNx;          }
   //! Get number of pixel in Y
   Int_t GetPixelsNy()                           const override { return fTypeParameter[0].PixelsNy;          }
   //! Get pitch size in X
   Float_t GetPitchX()                           const override  { return fTypeParameter[0].PitchX;            }
   //! Get pitch size in Y
   Float_t GetPitchY()                           const override  { return fTypeParameter[0].PitchY;            }
   //! Get total size of sensor
   TVector3 GetTotalSize()                       const override  { return fTypeParameter[0].TotalSize;         }
   //! Get epitaxial size of sensor
   TVector3 GetEpiSize()                         const override  { return fTypeParameter[0].EpiSize;           }
   //! Get epitaxial offset
   TVector3 GetEpiOffset()                       const override  { return fTypeParameter[0].EpiOffset;         }
   //! Get epitaxial material
   TString GetEpiMaterial()                      const override { return fTypeParameter[0].EpiMat;            }
   //! Get epitaxial density
   Float_t GetEpiMatDensity()                    const override  { return fTypeParameter[0].EpiMatDensity;     }
   //! Get epitaxial excitation
   Float_t GetEpiMatExc()                        const override  { return fTypeParameter[0].EpiMatExc;         }
   //! Get pixel thickness
   Float_t GetPixThickness()                     const override  { return fTypeParameter[0].PixThickness;      }
   //! Get pixel material
   TString GetPixMaterial()                      const override  { return fTypeParameter[0].PixMat;            }
   //! Get pixel coponent densities
   TString GetPixMatDensities()                  const override  { return fTypeParameter[0].PixMatDensities;   }
   //! Get pixel material proportion
   TString GetPixMatProp()                       const override  { return fTypeParameter[0].PixMatProp;        }
   //! Get pixel density
   Float_t GetPixMatDensity()                    const override  { return fTypeParameter[0].PixMatDensity;     }
   
   //! Get MC flag
   Bool_t GetMcFlag()                  const { return fFlagMC;         }
   //! Set MC flag
   void   SetMcFlag(Bool_t flag = true)      { fFlagMC = flag;         }
   
   // Get position from pixel line/column
   virtual Float_t GetPositionU(Int_t column, Int_t type = 0)         const ;
   virtual Float_t GetPositionV(Int_t line, Int_t type = 0)           const;
   virtual Int_t   GetIndex(Int_t line, Int_t column, Int_t type = 0) const;
   
   // Get column/line from x/y position
   virtual Int_t   GetColumn(Float_t x, Int_t type = 0) const;
   virtual Int_t   GetLine(Float_t y, Int_t type = 0)   const;
   
   
   // Get position from pixel line/column
   virtual Float_t GetPositionU(Int_t column)         const override { return GetPositionU(column, 0);   }
   virtual Float_t GetPositionV(Int_t line)           const override { return GetPositionV(line, 0);     }
   virtual Int_t   GetIndex(Int_t line, Int_t column) const override { return GetIndex(line, column, 0); }
   
   // Get column/line from x/y position
   virtual Int_t   GetColumn(Float_t x)               const override { return GetColumn(x, 0);           }
   virtual Int_t   GetLine(Float_t y)                 const override { return GetLine(y, 0);             }


   // Read parameters from file
   virtual Bool_t   FromFile(const TString& name = "") override;
   
   // Define material
   virtual void    DefineMaterial() override;

   //! Get type parameter
   TypeParameter_t& GetTypePar(Int_t idx)     { return fTypeParameter[idx];   }

public:
   //! Get base name
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   //! Get default para name
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }

   
   ClassDefOverride(TAMPparGeo,1)
};

#endif
