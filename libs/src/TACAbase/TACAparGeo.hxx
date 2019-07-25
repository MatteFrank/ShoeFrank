#ifndef _TACAparGeo_HXX
#define _TACAparGeo_HXX

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAGparTools.hxx"

#include "GlobalPar.hxx"




class TGeoHMatrix;
class TGeoVolume;
//##############################################################################

class TACAparGeo : public TAGparTools {
 
public:

  TACAparGeo();
   virtual ~TACAparGeo();

   Bool_t FromFile(const TString& name = "");
   
   void DefineMaterial(); 
   
   TVector3       GetCaloSize()     const  { return fCaloSize;    }
   
   TVector3       GetCrystalSize()     const  { return fCrystalSize;    }
   Float_t        GetCrystalWidth()    const  { return fCrystalSize[0]; }
   Float_t        GetCrystalHeight()   const  { return fCrystalSize[1]; }
   Float_t        GetCrystalThick()    const  { return fCrystalSize[2]; }

   Int_t          GetCrystalsN()       const  { return fCrystalsN;      }
   
   TString        GetCrystalMat()      const  { return fCrystalMat;     }

   TVector3       GetCrystalPosition(Int_t iCrystal);

   //! Transform point from the global detector reference frame
   //! to the local sensor reference frame of the detection id
   TVector3        Detector2Sensor(Int_t idx, TVector3& glob) const;
   TVector3        Detector2SensorVect(Int_t idx, TVector3& glob) const;
   
   //! Transform point from the local reference frame
   //! of the detection id to the global reference frame
   TVector3        Sensor2Detector(Int_t idx, TVector3& loc) const;
   TVector3        Sensor2DetectorVect(Int_t idx, TVector3& loc) const;


   // to print fluka files
  string PrintRotations();
  string PrintBodies();
  string PrintRegions();
  string PrintAssignMaterial();
  string PrintSubtractBodiesFromAir();
  string PrintParameters();


  // TGeoVolume*     GetVolume();
  TGeoVolume*     BuildCalorimeter(const char *caName = "CA");
  TGeoVolume*     BuildModule(Int_t idx);

  void            SetCrystalColorOn(Int_t idx);
  void            SetCrystalColorOff(Int_t idx);

  virtual void    Clear(Option_t* opt="");
  virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
protected:
   vector<string> vBody;
   vector<string> vRegion;
  
public:
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   static Color_t GetDefaultModCol()     { return fgkDefaultModCol;      }
   static Color_t GetDefaultModColOn()   { return fgkDefaultModColOn;    }

private:
   static const TString fgkBaseName;
   static const TString fgkDefParaName;
   static const Color_t fgkDefaultModCol;     // default color of slat/module;
   static const Color_t fgkDefaultModColOn;  // default color of fired slat/module;
   static const TString fgkDefaultCrysName;  // default crystal name;

   static const Char_t* GetDefaultCrysName(Int_t idx) { return Form("%s_%d", fgkDefaultCrysName.Data(), idx); }

private:
   TVector3  fCaloSize;
   TVector3  fCrystalSize;
   TString   fCrystalMat;
   Float_t   fCrystalDensity;
   Int_t     fCrystalsN;
   TString   fkDefaultGeoName;  // default par geo file name

   vector<TVector3> vTilt;
  
  ClassDef(TACAparGeo,1)
};

#endif
