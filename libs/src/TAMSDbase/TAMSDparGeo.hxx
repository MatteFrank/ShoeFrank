#ifndef _TAMSDparGeo_HXX
#define _TAMSDparGeo_HXX
/*!
  \file TAMSDparGeo.hxx
  \brief   Declaration of TAMSDparGeo.
*/
/*------------------------------------------+---------------------------------*/

#include <map>
#include <sstream>

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAGmaterials.hxx"
#include "TAVTbaseParGeo.hxx"

class TGeoHMatrix;
class TGeoVolume;
//##############################################################################

class TAMSDparGeo : public TAVTbaseParGeo {

public:
  TAMSDparGeo();
  virtual ~TAMSDparGeo();
  
  // Define materials
   void DefineMaterial();
  
  //! Add SSSD module geometry to strip detector
  TGeoVolume* AddModule(const char* basemoduleName = "SSSD", const char *name = "MSD");
   
  //! Build MSD
  TGeoVolume* BuildMicroStripDetector(const char* basemoduleName = "Module", const char *name = GetBaseName());
   
  //! Reading from file
  Bool_t      FromFile(const TString& name = "");
  
  Float_t     GetPosition(Int_t strip) const;
   
  //! Get number of strip
  Int_t       GetStripsN()             const { return fStripsN; }
   
  //! Get pitch
  Float_t     GetPitch()               const { return fPitch;   }
   
  //! Get stationsN
  Int_t       GetStationsN()           const { return (fSensorsN % 2 == 0) ? fSensorsN/2 : fSensorsN/2 + 1;  }
   
  //crossing regions
  Int_t       GetRegStrip(Int_t n);   //n=sensor (0-5)
  Int_t       GetRegModule(Int_t n);  //n=sensor (0-5)
  Int_t       GetRegMetal(Int_t n);   //n=sensor (0-5)
  
  // to keep interace for compilation
  string      PrintParameters();
  string      PrintRotations();
  string      PrintBodies();
  string      PrintRegions();
  string      PrintAssignMaterial(TAGmaterials *Material);
  string      PrintSubtractBodiesFromAir();

private:
  Int_t          fStripsN;          ///< Number of strips
  Float_t        fPitch;            ///< Pitch value
  TVector3       fMetalSize;        ///< Sensitive size of metallization
  Float_t        fMetalThickness;   ///< Thickness of metallization
  TString        fMetalMat;         ///< Material of metallization
  Float_t        fMetalDensity;     ///< density of metallization
   
  vector<string> fvMetalBody;       ///< Fluka metal body vector
  vector<string> fvModBody;         ///< Fluka body vector
  vector<string> fvStripBody;       ///< Fluka strip  vector
  vector<string> fvMetalRegion;     ///< Fluka metal region vector
  vector<string> fvModRegion;       ///< Fluka  mod region vector
  vector<string> fvStripRegion;     ///< Fluka strip region vector
  
private:
  static const TString fgkBaseName;    ///< MSD base name
  static const TString fgkDefParaName; //< default parameter name
  
public:
  //! Get base name
  static const Char_t* GetBaseName()                    { return fgkBaseName.Data();    }
  //! Get default parameter name
  static const Char_t* GetDefParaName()                 { return fgkDefParaName.Data(); }
  //! Get sensor id from board and side id
  static Int_t GetSensorId(Int_t boardId, Int_t sideId) { return 2*(boardId-1)+sideId;  }

  ClassDef(TAMSDparGeo,3)
};

#endif
