#ifndef _TACAparGeo_HXX
#define _TACAparGeo_HXX

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAGparTools.hxx"
#include "TAGmaterials.hxx"
#include "TGeoMatrix.h"

#include "GlobalPar.hxx"

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
   
   TVector3        GetCaloSize();
   Int_t           GetModulesN()             const  { return fModulesN;       }
   Float_t         GetDelta()                const  { return fCrystalDelta;   }
   TString         GetConfigTypeGeo()        const  { return fConfigTypeGeo;  }

   // Crystal
   Int_t           GetCrystalsN()            const  { return fCrystalsN;      }
   const Double_t* GetCrystalSize()          const  { return fCrystalSize;    }
   Float_t         GetCrystalWidthFront()    const  { return fCrystalSize[0]; }
   Float_t         GetCrystalHeightFront()   const  { return fCrystalSize[2]; }
   Float_t         GetCrystalThick()         const  { return fCrystalSize[4]; }
   Float_t         GetCrystalWidthBack()     const  { return fCrystalSize[1]; }
   Float_t         GetCrystalHeightBack()    const  { return fCrystalSize[3]; }
   TString         GetCrystalMat()           const  { return fCrystalMat;     }

   // Support
   const Double_t* GetSupportSize()          const  { return fSupportSize;    }
   Float_t         GetSupportWidthFront()    const  { return fSupportSize[0]; }
   Float_t         GetSupportHeightFront()   const  { return fSupportSize[2]; }
   Float_t         GetSupportThick()         const  { return fSupportSize[4]; }
   Float_t         GetSupportWidthBack()     const  { return fSupportSize[1]; }
   Float_t         GetSupportHeightBack()    const  { return fSupportSize[3]; }
   TString         GetSupportMat()           const  { return fSupportMat;     }
   Float_t         GetSupportPositionZ()     const  { return fSupportPositionZ;}

   const Double_t* GetModuleSize()           const  { return fModAirFlukaSize; }
   
   TVector3        GetCrystalPosition(Int_t iCrystal);
   TVector3        GetCrystalAngle(Int_t iCrystal);

   //! of the detection id to the global reference frame
   TVector3        Detector2Crystal(Int_t idx, TVector3& glob) const;
   TVector3        Detector2CrystalVect(Int_t idx, TVector3& glob) const;
   
   //! to the local crystal reference frame of the detection id
   TVector3        Crystal2Detector(Int_t idx, TVector3& loc) const;
   TVector3        Crystal2DetectorVect(Int_t idx, TVector3& loc) const;

   virtual void    Clear(Option_t* opt="");
   virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;

   // to print into FLUKA input files
   string          PrintRotations();
   string          PrintBodies();
   string          PrintRegions();
   string          PrintAssignMaterial(TAGmaterials *Material);
   string          PrintSubtractBodiesFromAir();
   string          PrintParameters();
private:
   TString         SPrintCrystalBody(int id, TGeoCombiTrans * hm, TString bodyName, Double_t *trd2Size);
   TString         SPrintParallelPla(int id, TGeoCombiTrans * hm, TString bodyName, Double_t *trd2Size, int * dir);   
   TString         PrintModuleAirRegions();
   
public:
   // Create ROOT geometry
   TGeoVolume*    BuildCalorimeter(const char *caName = GetBaseName());
   void           SetCrystalColorOn(Int_t idx);
   void           SetCrystalColorOff(Int_t idx);
  
public:
   static const Char_t* GetBaseName()     { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName()  { return fgkDefParaName.Data(); }
   static Color_t GetDefaultCryCol()      { return fgkDefaultCryCol;      }
   static Color_t GetDefaultCryColOn()    { return fgkDefaultCryColOn;    }
   static Color_t GetDefaultModCol()      { return fgkDefaultModCol;      }
   static Int_t   GetCrystalsNperModule() { return fgkCrystalsNperModule; }
   static Int_t   GetModuleId(Int_t idx)  { return idx / fgkCrystalsNperModule; }


private:
   static const TString fgkBaseName;         // Detector base name
   static const TString fgkDefParaName;
   static const Color_t fgkDefaultCryCol;    // default color of slat/crystal;
   static const Color_t fgkDefaultCryColOn;  // default color of fired slat/crystal;
   static const Color_t fgkDefaultModCol;    // default color of module support;
   static const TString fgkDefaultCrysName;  // default crystal name;
   static const Int_t   fgkCrystalsNperModule;  // Number of crystals per module;

   static const Char_t* GetDefaultCrysName(Int_t idx) { return Form("%s_%d", fgkDefaultCrysName.Data(), idx); }

private:
   TVector3            fCaloSize;         // Size of a box contained the detector
   Float_t             fCaloBoxPositionZ; // contained box Z pos

   Int_t               fModulesN;         // Number of modules
   Int_t               fCrystalsN;        // Number of BGO crystals
   Double_t            fCrystalSize[5];   // Dimension of truncate piramid, see TGeoTrd2
   Float_t             fCrystalDelta;     // delta between two crystal in a module
   TString             fCrystalMat;       // 
   Float_t             fCrystalDensity;
   Float_t             fCrystalIonisMat;
   TAGionisMaterials*  fIonisation;        //! pointer for ionisation property

   Double_t            fSupportSize[5];    // Dimension of truncate piramid, see TGeoTrd2
   TString             fSupportMat;        // 
   Float_t             fSupportDensity;
   Float_t             fSupportPositionZ;  // z shift respect to module origin

   Double_t            fModAirFlukaSize[5];   //
   Float_t             fModAirFlukaPositionZ;

   TString             fkDefaultGeoName;  // default par geo file name
   TString             fDetectorName;
   TString             fConfigTypeGeo;   // config geometry of calo
   
   vector<TVector3>    fListOfCrysAng; // list of angles for module

  
   ClassDef(TACAparGeo, 2)
};


#endif
