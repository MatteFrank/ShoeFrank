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

#include "TAVTbaseParGeo.hxx"

class TGeoHMatrix;
class TGeoVolume;
//##############################################################################

class TAMSDparGeo : public TAVTbaseParGeo {


public:
  TAMSDparGeo();
  virtual ~TAMSDparGeo();
  
  // Define materials
  //   void        DefineMaterial();
  
  //! Build MSD
  TGeoVolume* BuildMultiStripDetector(const char* basemoduleName = "Module", const char *name = "MSD");
  Bool_t      FromFile(const TString& name = "");
  
  // to keep interace for compilation
  virtual string PrintParameters();
  virtual string PrintRotations();
  virtual string PrintBodies();
  virtual string PrintRegions();
  virtual string PrintAssignMaterial();
  virtual string PrintSubtractBodiesFromAir();

protected:
  map<string, vector<string> > m_regionName;
  map<string, vector<string> > m_bodyName;
  // map<string, vector<string> > m_regionPrintOut;
  map<string, vector<string> > m_bodyPrintOut;
  vector<string> vMetalBody, vModBody, vStripBody;
  vector<string> vMetalRegion, vModRegion, vStripRegion;
  
  Int_t      fStripN;          // Number of strips
  Float_t    fPitch;           // Pitch value
  
  TVector3   fMetalSize;        // Sensitive size of metallization
  Float_t    fMetalThickness;   // Thickness of metallization
  TString    fMetalMat;         // Material of metallization
  Float_t    fMetalDensity;     // density of metallization
  
private:
  static const TString fgkBaseName;   // MSD base name
  static const TString fgkDefParaName;
  
public:
  static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
  static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }

  
  ClassDef(TAMSDparGeo,1)
};

#endif
