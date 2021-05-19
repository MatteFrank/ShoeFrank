#ifndef _TAMSDparGeo_HXX
#define _TAMSDparGeo_HXX
/*!
  \file
  \version $Id: TAMSDparGeo.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
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
  TGeoVolume* BuildMultiStripDetector(const char* basemoduleName = "Module", const char *name = GetBaseName());
   
  //! Reading from file
  Bool_t      FromFile(const TString& name = "");
  
  Float_t     GetPosition(Int_t strip) const;
   
  //! Get number of strip
  Int_t       GetStripsN()             const { return fStripsN; }
   
   //! Get pitch
  Float_t     GetPitch()               const { return fPitch;   }
   
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
  vector<string> fvMetalBody;
  vector<string> fvModBody;
  vector<string> fvStripBody;
  vector<string> fvMetalRegion;
  vector<string> fvModRegion;
  vector<string> fvStripRegion;

  Int_t          fStripsN;         // Number of strips
  Float_t        fPitch;           // Pitch value
  
  TVector3       fMetalSize;        // Sensitive size of metallization
  Float_t        fMetalThickness;   // Thickness of metallization
  TString        fMetalMat;         // Material of metallization
  Float_t        fMetalDensity;     // density of metallization
  
private:
  static const TString fgkBaseName;   // MSD base name
  static const TString fgkDefParaName;
  
public:
  static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
  static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }

  ClassDef(TAMSDparGeo,3)
};

#endif
