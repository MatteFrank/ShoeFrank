#ifndef _TAITparGeo_HXX
#define _TAITparGeo_HXX
/*!
  \file TAITparGeo.hxx
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
   Int_t      fSensPerBoard;      ///< number of sensors per Plume
   TVector3   fSupportSize;       ///< Support size
   TVector3   fSupportOffset;     ///< Support offset
   Float_t    fFoamThickness;     ///< Foam thickness
   TString    fFoamMat;           ///< Material of foam
   TString    fFoamMatDensities;  ///< Density of foam material for each component
   TString    fFoamMatProp;       ///< Material of foam component proportion
   Float_t    fFoamMatDensity;    ///< Density of foam material

   Float_t    fKaptonThickness;   ///< Kapton thickness
   TString    fKaptonMat;         ///< Material of kapton
   Float_t    fKaptonMatDensity;  ///< Density of kapton material
   
   Float_t    fEpoxyThickness;    ///< Epoxy thickness
   TString    fEpoxyMat;          ///< Material of expoxy
   Float_t    fEpoxyMatDensity;   ///< Density of epoxy material
   
   Float_t    fAlThickness;       ///< Aluminum thickness
   TString    fAlMat;             ///< Material of aluminum
   Float_t    fAlMatDensity;      ///< Density of aluminum material

private:
   //! Fluka foam body
   vector<string> fvFoamBody;     //! Fluka foam body
   //! Fluka kapton body
   vector<string> fvKaptonBody;   //! Fluka kapton body
   //! Fluka epoxy body
   vector<string> fvEpoxyBody;    //! Fluka epoxy body
    //! Fluka aluminum body
   vector<string> fvAlBody;       //! Fluka aluminum body
    //! Fluka foam region
   vector<string> fvFoamRegion;   //! Fluka foam region
   //! Fluka kapton region
   vector<string> fvKaptonRegion; //! Fluka kapton region
    //! Fluka epoxy body
   vector<string> fvEpoxyRegion;  //! Fluka epoxy region
   //! Fluka aluminum body
   vector<string> fvAlRegion;     //! Fluka aluminum region

protected:
   static const TString fgkBaseNameIt;      ///< IT base name
   static const Int_t   fgkDefSensPerLayer; ///< Def number of sensors per layer
   static const Int_t   fgkDefSensPerBoard; ///< Def number of sensors per plume module
   static const Int_t   fgkDefLayersN;      ///< Def number of layers
   static const Int_t   fgkDefSensorsN;     ///< default number of sensors

private:
   Float_t GetPassiveLayerPosZ(Int_t layer);

public:
   TAITparGeo();
    virtual ~TAITparGeo();
   
   //! Get number of Sensors per link
   Int_t GetSensPerDataLink()           const { return fSensPerBoard;     }
   //! Get sensors per Plume
   Int_t    GetSensPerBoard()           const { return fSensPerBoard;     }
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
   
   // return local Z positon of the layers
   // Get Foam layer thickness
   Float_t GetFoamLayer();
   // Get 1st Kapton layer thickness
   Float_t Get1stKaptonLayer();
   // Get 1st Aluminum layer thickness
   Float_t Get1stAlLayer();
   // Get 2nd Kapton layer thickness
   Float_t Get2ndKaptonLayer();
   // Get 2nd Aluminum layer thickness
   Float_t Get2ndAlLayer();
   // Get 3rd Kapton layer thickness
   Float_t Get3rdKaptonLayer();
   // Get Epxoy layer thickness
   Float_t GetEpoxyLayer();
   
   // Define materials
   void    DefineMaterial();

   // Read support
   void    ReadSupportInfo();

   // Build Innert Tracker
   TGeoVolume* BuildInnerTracker(const char *name = GetBaseName(), const char* basemoduleName = "Module", Bool_t board = false,  Bool_t support = false);
   
   // Build plume support
   TGeoVolume* BuildPlumeSupport(const char* basemoduleName = "Plume", const char *name = "ITSP");

   //!crossing regions
   Int_t          GetRegEpitaxial(Int_t n);
   Int_t          GetRegModule(Int_t n);                            //n=number of sensor (0<=n<=31)
   Int_t          GetRegPixel(Int_t n);                             //n=number of sensor (0<=n<=31)
                                                                    
   Int_t          GetRegFoam(Int_t n);                              //n=ladders (0<=n<=3)
   Int_t          GetRegKapton(Int_t n, Int_t lay, Bool_t side);    //n=ladders (0<=n<=3); side: true=positive, false=negative; lay=number of layer (lay=0,1,2)
   Int_t          GetRegAluminum(Int_t n, Int_t lay, Bool_t side);  //n=ladders (0<=n<=3); side: true=positive, false=negative; lay=number of layer (lay=0,1)
   Int_t          GetRegEpoxy(Int_t n, Bool_t side);                //n=ladders (0<=n<=3); side: true=positive, false=negative;

   // to print fluka files
   // Fluka Parameters
   virtual string PrintParameters();
   // Fluka rotations
   virtual string PrintRotations();
   // Fluka bodies
   virtual string PrintBodies();
   // Fluka regions
   virtual string PrintRegions();
   // Fluka assignment material
   virtual string PrintAssignMaterial(TAGmaterials *Material);
   // Fluka Transport settings
   virtual string PrintITPhysics();
   // Fluka bodies subtraction
   virtual string PrintSubtractBodiesFromAir();
   
public:
   //! Get base name
   static const Char_t* GetBaseName()    { return fgkBaseNameIt.Data();    }
   //! Get default number of sensors per board
   static Int_t GetDefSensPerBoard()     {return fgkDefSensPerBoard;       }
   //! Get default number of sensors
   static Int_t         GetDefSensorsN() { return fgkDefSensorsN;        }

   ClassDef(TAITparGeo,1)
};

#endif
