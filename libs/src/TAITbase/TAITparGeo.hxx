#ifndef _TAITparGeo_HXX
#define _TAITparGeo_HXX
/*!
  \file
  \brief   Declaration of TAITparGeo.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include "TAGmaterials.hxx"
#include "TAVTparGeo.hxx"

class TGeoHMatrix;
class TGeoVolume;
//##############################################################################

class TAITparGeo : public TAVTparGeo {
   
private:
   
   TVector3   fSupportSize;       // Support size
   TVector3   fSupportOffset;     // Support offset
   Float_t    fFoamThickness;     // Foam thickness
   TString    fFoamMat;           // Material of foam
   TString    fFoamMatDensities;  // Density of foam material for each component
   TString    fFoamMatProp;       // Material of foam component proportion
   Float_t    fFoamMatDensity;    // Density of foam material

   Float_t    fKaptonThickness;   // Kapton thickness
   TString    fKaptonMat;         // Material of kapton
   Float_t    fKaptonMatDensity;  // Density of kapton material
   
   Float_t    fEpoxyThickness;    // Epoxy thickness
   TString    fEpoxyMat;          // Material of expoxy
   Float_t    fEpoxyMatDensity;   // Density of epoxy material
   
   Float_t    fAlThickness;       // Aluminum thickness
   TString    fAlMat;             // Material of aluminum
   Float_t    fAlMatDensity;      // Density of aluminum material

protected:
   static const TString fgkBaseNameIt;   // IT base name
   static const TString fgkDefParaNameIt;

public:
   TAITparGeo();
    virtual ~TAITparGeo();
   
   //! Get Foam size
   TVector3 GetSupportSize()            const { return fSupportSize;      }
   //! Get Foam Offset
   TVector3 GetSupportOffset()          const { return fSupportOffset;    }
   //! Get Foam thickness
   Float_t GetFoamThickness()           const { return fFoamThickness;    }
   //! Get Foam material
   TString GetFoamMaterial()            const { return fFoamMat;          }
   //! Get Foam coponent densities
   TString GetFoamMatDensities()        const { return fFoamMatDensities; }
   //! Get Foam material proportion
   TString GetFoamMatProp()             const { return fFoamMatProp;      }
   //! Get Foam density
   Float_t GetFoamMatDensity()          const { return fFoamMatDensity;   }
   
   
   //! Get Kapton thickness
   Float_t GetKaptonThickness()         const { return fKaptonThickness;  }
   //! Get Kapton material
   TString GetKaptonMaterial()          const { return fKaptonMat;        }
   //! Get Kapton density
   Float_t GetKaptonMatDensity()        const { return fKaptonMatDensity; }

   
   //! Get Epoxy thickness
   Float_t GetEpoxyThickness()          const { return fEpoxyThickness;   }
   //! Get Epoxy material
   TString GetEpoxyMaterial()           const { return fEpoxyMat;         }
   //! Get Epoxy density
   Float_t GetEpoxyMatDensity()         const { return fEpoxyMatDensity;  }

   //! Get Al thickness
   Float_t GetAlThickness()             const { return fAlThickness;      }
   //! Get Al material
   TString GetAlMaterial()              const { return fAlMat;            }
   //! Get Al density
   Float_t GetAlMatDensity()            const { return fAlMatDensity;     }
   
   // Define materials
   void    DefineMaterial();

   // Read support
   void    ReadSupportInfo();

   //! Build Innert Tracker
   TGeoVolume* BuildInnerTracker(const char *name = "IT", const char* basemoduleName = "Module", Bool_t board = false,  Bool_t support = false);
   
   //! Build plume support
   TGeoVolume* BuildPlumeSupport(const char* basemoduleName = "Plume", const char *name = "ITSP");

   // to print fluka files
   virtual string PrintParameters();
   virtual string PrintRotations();
   virtual string PrintBodies();
   virtual string PrintRegions();
   virtual string PrintAssignMaterial(TAGmaterials *Material);
   virtual string PrintSubtractBodiesFromAir();
   
public:
   static const Char_t* GetItBaseName()    { return fgkBaseNameIt.Data();    }
   static const Char_t* GetItDefParaName() { return fgkDefParaNameIt.Data(); }

   ClassDef(TAITparGeo,1)
};

#endif
