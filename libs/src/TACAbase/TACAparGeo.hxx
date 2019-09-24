#ifndef _TACAparGeo_HXX
#define _TACAparGeo_HXX

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAGparTools.hxx"

#include "GlobalPar.hxx"

class TGeoHMatrix;
class TGeoVolume;
class TAGionisMaterials;

//##############################################################################

class TACAparGeo : public TAGparTools {
 
public:

  TACAparGeo();
   virtual ~TACAparGeo();

   Bool_t FromFile(const TString& name = "");
   
   void DefineMaterial(); 
   
   TVector3       GetCaloSize()        const  { return fCaloSize;       }
   Int_t          GetModulesN()        const  { return fModulesN;       }
   Float_t        GetDelta()           const  { return fCrystalDelta;    }

   // Crystal
   Int_t          GetCrystalsN()       const  { return fCrystalsN;      }
   TVector3       GetCrystalSize()     const  { return fCrystalSize;    }
   Float_t        GetCrystalWidth()    const  { return fCrystalSize[0]; }  // keep for backward compatibility
   Float_t        GetCrystalHeight()   const  { return fCrystalSize[1]; }
   Float_t        GetCrystalThick()    const  { return fCrystalSize[2]; }
   
   Float_t        GetCrystalBotBase()  const  { return fCrystalSize[0]; }
   Float_t        GetCrystalTopBase()  const  { return fCrystalSize[1]; }
   Float_t        GetCrystalLength()   const  { return fCrystalSize[2]; }
   TString        GetCrystalMat()      const  { return fCrystalMat;     }

   // Support
   TVector3       GetSupportSize()     const  { return fSupportSize;    }
   Float_t        GetSupportBotBase()  const  { return fSupportSize[0]; }
   Float_t        GetSupportTopBase()  const  { return fSupportSize[1]; }
   Float_t        GetSupportLength()   const  { return fSupportSize[2]; }
   TString        GetSupportMat()      const  { return fSupportMat;     }

   // Position inside the detector framework
   TVector3       GetGlobalCrystalPosition(Int_t iCrystal, Int_t iModule);
   TVector3       GetGlobalCrystalFrontPosition(Int_t iCrystal, Int_t iModule);

   // Position/angle inside the module
   TVector3       GetCrystalPosition(Int_t iCrystal);
   TVector3       GetCrystalAngle(Int_t iCrystal);
   
   // Position/angle of the module
   TVector3       GetModulePosition(Int_t iModule);
   TVector3       GetModuleAngle(Int_t iModule);

   //! Transform point from the global detector reference frame
   //! to the local Module reference frame of the detection id
   TVector3        Detector2Module(Int_t idx, TVector3& glob) const;
   TVector3        Detector2ModuleVect(Int_t idx, TVector3& glob) const;
   
   //! to the local crystal reference frame of the detection id
   TVector3        Detector2Crystal(Int_t idx, Int_t iMod, TVector3& glob) const;
   TVector3        Detector2CrystalVect(Int_t idx, Int_t iMod, TVector3& glob) const;
   
   //! Transform point from the local reference frame
   //! of the detection id to the global reference frame
   TVector3        Module2Detector(Int_t idx, TVector3& loc) const;
   TVector3        Module2DetectorVect(Int_t idx, TVector3& loc) const;

   //! of the detection id to the global reference frame
   TVector3        Crystal2Detector(Int_t idx, Int_t iMod, TVector3& loc) const;
   TVector3        Crystal2DetectorVect(Int_t idx, Int_t iMod, TVector3& loc) const;

   // to print fluka files
  string PrintRotations();
  string PrintBodies();
  string PrintRegions();
  string PrintAssignMaterial();
  string PrintSubtractBodiesFromAir();
  string PrintParameters();


  // TGeoVolume*     GetVolume();
  TGeoVolume*     BuildCalorimeter(const char *caName = "CA");
  TGeoVolume*     BuildModule(Int_t iMod);

  void            SetCrystalColorOn(Int_t idx, Int_t iMod);
  void            SetCrystalColorOff(Int_t idx, Int_t iMod);

  virtual void    Clear(Option_t* opt="");
  virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
protected:
   vector<string> vBody;
   vector<string> vRegion;
  
public:
   static const Char_t* GetBaseName()     { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName()  { return fgkDefParaName.Data(); }
   static Color_t GetDefaultModCol()      { return fgkDefaultModCol;      }
   static Color_t GetDefaultModColOn()    { return fgkDefaultModColOn;    }
   static Int_t   GetDefaultModulesN()    { return fgkDefaultModulesN;    }
   static Int_t   GetCrystalsNperModule() { return fgkCrystalsNperModule; }

private:
   static const TString fgkBaseName;
   static const TString fgkDefParaName;
   static const Color_t fgkDefaultModCol;     // default color of slat/module;
   static const Color_t fgkDefaultModColOn;   // default color of fired slat/module;
   static const TString fgkDefaultCrysName;   // default crystal name;
   static const TString fgkDefaultModName;    // default module name;

   static const Int_t fgkDefaultModulesN;     // default number of modules;
   static const Int_t fgkCrystalsNperModule;  // Number of crystals per module;

   static const Char_t* GetDefaultCrysName(Int_t idx, Int_t iMod) { return Form("%s_%d_%d", fgkDefaultCrysName.Data(), idx, iMod); }
   static const Char_t* GetDefaultModName(Int_t idx) { return Form("%s_%d", fgkDefaultModName.Data(), idx); }

private:
   TAGionisMaterials* fIonisation; //! pointer for ionisation property
   TObjArray* fCrystalMatrixList;       //! list of transformation matrices  (rotation+translation for each crystal)
   TVector3  fCaloSize;
   TVector3  fCrystalSize;
   TString   fCrystalMat;
   Float_t   fCrystalDensity;
   Float_t   fCrystalIonisMat;
   Int_t     fCrystalsN;
   Int_t     fModulesN;
   
   TVector3  fSupportSize;
   TString   fSupportMat;
   Float_t   fSupportDensity;
   
   Float_t   fCrystalDelta; // delta between two crystal in a module

   TVector3  fCrystalPos[9];  // position of crystals in module
   TVector3  fCrystalAng[9];  // angle of crystals in module

   TString   fkDefaultGeoName;  // default par geo file name

   vector<TVector3> fListOfModAng; // list of angles for module
   
private:
   void      ComputeCrystalPos();
   //! Add matrxi transformation
   void      AddCrystalMatrix(TGeoHMatrix* mat, Int_t idx = -1);
   
  ClassDef(TACAparGeo,1)
};

#endif
