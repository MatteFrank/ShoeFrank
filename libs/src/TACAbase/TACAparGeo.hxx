#ifndef _TACAparGeo_HXX
#define _TACAparGeo_HXX

/*!
  \file TACAparGeo.hxx
  \brief   Declaration of TACAparGeo
*/

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAGparTools.hxx"
#include "TAGmaterials.hxx"
#include "TGeoMatrix.h"

#include "TAGrecoManager.hxx"

class TAGionisMaterials;
class TGeoNode;
class TGeoMaterial;
class TGeoVolume;
class TACAcrystalGeo;


//##############################################################################

class TACAparGeo : public TAGparTools {
 
public:

                   TACAparGeo();
   virtual        ~TACAparGeo();

   Bool_t          FromFile(const TString& name = "");
   
   void            DefineMaterial(); 
   
   //! Get the Calorimeter dimensions
   TVector3        GetCaloSize()             const  { return fCaloSize;       }
   //! Get the number of Calorimeter modules  
   Int_t           GetModulesN()             const  { return fModulesN;       }
   //! Get the distance between crystal in a module
   Float_t         GetDelta()                const  { return fCrystalDelta;   }
   //! Get the Calorimeter setup configuration or  (FULL_DET, CENTRAL_DET, ONE_CRY, ONE_MOD, FIVE_MOD, SEVEN_MOD, FULL_DET_V1)
   TString         GetConfigTypeGeo()        const  { return fConfigTypeGeo;  }

   // Crystal
   //! Get the number of Calorimeter crystals  
   Int_t           GetCrystalsN()            const  { return fCrystalsN;      }
   //! Get the crystal truncate pyramid half dimensions 
   const Double_t* GetCrystalSize()          const  { return fCrystalSize;    }
   //! Get the crystal front width
   Float_t         GetCrystalWidthFront()    const  { return fCrystalSize[0]; }
   //! Get the crystal front height
   Float_t         GetCrystalHeightFront()   const  { return fCrystalSize[2]; }
   //! Get the crystal thickness
   Float_t         GetCrystalThick()         const  { return fCrystalSize[4]; }
   //! Get the crystal back width
   Float_t         GetCrystalWidthBack()     const  { return fCrystalSize[1]; }
   //! Get the crystal back height
   Float_t         GetCrystalHeightBack()    const  { return fCrystalSize[3]; }
   //! Get the crystal material
   TString         GetCrystalMat()           const  { return fCrystalMat;     }

   //! Get the support truncate pyramid half dimensions dimensions
   const Double_t* GetSupportSize()          const  { return fSupportSize;    }
   //! Get the support front width
   Float_t         GetSupportWidthFront()    const  { return fSupportSize[0]; }
   //! Get the support front height
   Float_t         GetSupportHeightFront()   const  { return fSupportSize[2]; }
   //! Get the support thickness
   Float_t         GetSupportThick()         const  { return fSupportSize[4]; }
   //! Get the support back width
   Float_t         GetSupportWidthBack()     const  { return fSupportSize[1]; }
   //! Get the support back height
   Float_t         GetSupportHeightBack()    const  { return fSupportSize[3]; }
   //! Get the support material
   TString         GetSupportMat()           const  { return fSupportMat;     }
   //! Get the support position in Z
   Float_t         GetSupportPositionZ()     const  { return fSupportPositionZ;}
   
   // 2D positions and dimensions fro clustering
   //! Get maximum of line number
   Int_t           GetMaxNumLines()          const  { return fNumLine;         }
   //! Get maximum of column number
   Int_t           GetMaxNumColumns()        const  { return fNumCol;          }

   //!Get dimension of truncate pyramid of air around the module
   const Double_t* GetModuleSize()           const  { return fModAirFlukaSize; }
   
   TVector3        GetCrystalPosition(Int_t iCrystal);
   TVector3        GetCrystalAngle(Int_t iCrystal);
   TVector3        GetModulePosition(Int_t iMod);
   TVector3        GetModuleAngle(Int_t iMod);

   //! of the detection id to the global reference frame
   TVector3        Detector2Crystal(Int_t idx, TVector3& glob) const;
   TVector3        Detector2CrystalVect(Int_t idx, TVector3& glob) const;
   
   //! to the local crystal reference frame of the detection id
   TVector3        Crystal2Detector(Int_t idx, TVector3& loc) const;
   TVector3        Crystal2DetectorVect(Int_t idx, TVector3& loc) const;

   virtual void    Clear(Option_t* opt="");
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;

   //crossing regions
   Int_t          GetRegCrystal(Int_t n); //n=number of crystal [0-fCrystalsN]

   // to print into FLUKA input files
   string          PrintRotations();
   string          PrintBodies();
   string          PrintRegions();
   string          PrintAssignMaterial(TAGmaterials *Material);
   string          PrintSubtractBodiesFromAir();
   string          PrintParameters();
   
   // 2D positions and dimensions for clustering
   Int_t           GetCrystalLine(Int_t iCrystal);
   Int_t           GetCrystalCol(Int_t iCrystal);

private:
   TString         SPrintCrystalBody(int id, TGeoCombiTrans * hm, TString bodyName, Double_t *trd2Size);
   TString         SPrintParallelPla(int id, TGeoCombiTrans * hm, TString bodyName, Double_t *trd2Size, int * dir);   
   TString         PrintModuleAirRegions();

   //! Build a row/col index to by used in clustering
   void            ComputeCrystalIndexes();
   
public:
   // Create ROOT geometry
   TGeoVolume*    BuildCalorimeter(const char *caName = GetBaseName());
   void           SetCrystalColorOn(Int_t idx);
   void           SetCrystalColorOff(Int_t idx);
  
public:
   //! Get the detector name of this class (CA)
   static const Char_t* GetBaseName()     { return fgkBaseName.Data();    }
   //! Get default crystal color
   static Color_t GetDefaultCryCol()      { return fgkDefaultCryCol;      }
   //! Get default on crystal color
   static Color_t GetDefaultCryColOn()    { return fgkDefaultCryColOn;    }
   //! Get default module color
   static Color_t GetDefaultModCol()      { return fgkDefaultModCol;      }
   //! Get number of crystal per module (9)
   static Int_t   GetCrystalsNperModule() { return fgkCrystalsNperModule; }
   
   //! Get default crystal name
   static const Char_t* GetDefaultCrysName(Int_t idx) { return Form("%s_%d", fgkDefaultCrysName.Data(), idx); }

private:
   static const TString fgkBaseName;            ///< Detector base name
   static const Color_t fgkDefaultCryCol;       ///< default color of slat/crystal;
   static const Color_t fgkDefaultCryColOn;     ///< default color of fired slat/crystal;
   static const Color_t fgkDefaultModCol;       ///< default color of module support;
   static const TString fgkDefaultCrysName;     ///< default crystal name;
   static const Int_t   fgkCrystalsNperModule;  ///< Number of crystals per module;
   
private:
   TVector3            fCaloSize;         ///< Size of a box contained the detector
   Float_t             fCaloBoxPositionZ; ///< Z position of the contained box 

   Int_t               fModulesN;         ///< Number of modules
   Int_t               fCrystalsN;        ///< Number of BGO crystals
   Double_t            fCrystalSize[5];   ///< Dimension of truncate pyramid, see TGeoTrd2
   Float_t             fCrystalDelta;     ///< delta between two crystal in a module
   TString             fCrystalMat;       ///< Definition of BGO material
   Float_t             fCrystalDensity;   ///< Crystal material density
   Float_t             fCrystalIonisMat;  ///< Crystal material
   TAGionisMaterials*  fIonisation;       ///<! pointer for ionisation property

   Double_t            fSupportSize[5];    ///< Dimension of truncate pyramid for the module support, see TGeoTrd2
   TString             fSupportMat;        ///<  Support material
   Float_t             fSupportDensity;    ///< Support material density
   Float_t             fSupportPositionZ;  ///< z shift respect to module origin

   Double_t            fModAirFlukaSize[5];   ///< Dimension of truncate pyramid of air around the module
   Float_t             fModAirFlukaPositionZ; ///< Fluka air module

   TString             fkDefaultGeoName;   ///< default par geo file name
   TString             fDetectorName;      ///< Detector name
   TString             fConfigTypeGeo;     ///< config/setup geometry of Calorimeter
   
   vector<TVector3>    fListOfCrysAng;     ///< list of angles for crystal
   vector<TVector3>    fListOfModAng;      ///< list of angles for module

   map<int, pair<int, int> > fMapIndexes;  ///< index map for line/column numbers
   Int_t               fNumLine;           ///< line index
   Int_t               fNumCol;            ///< column index

   ClassDef(TACAparGeo, 2)
};


#endif
