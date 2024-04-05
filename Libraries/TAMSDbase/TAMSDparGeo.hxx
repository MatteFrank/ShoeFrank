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

  //! @brief Get thickness of metallic material
  Float_t     GetMetalThickness()  const {return fMetalThickness;}
  
  // to keep interace for compilation
  string      PrintParameters();
  string      PrintRotations();
  string      PrintBodies();
  string      PrintRegions();
  string      PrintPassiveRegions();  
  string      PrintAssignMaterial(TAGmaterials *Material);
  string      PrintSubtractBodiesFromAir();
   // Fluka Transport settings
  string      PrintMSDPhysics();

   void    ReadSupportInfo();


private:
  Int_t          fStripsN;          ///< Number of strips
  Float_t        fPitch;            ///< Pitch value
  TVector3       fMetalSize;        ///< Sensitive size of metallization
  Float_t        fMetalThickness;   ///< Thickness of metallization
  TString        fMetalMat;         ///< Material of metallization
  Float_t        fMetalDensity;     ///< density of metallization
  TString        fEpiMat;           ///< Material of epitaxial layer
  Float_t        fEpiMatDensity;    ///< density of epitaxial layer
  Float_t        fBoardThickness;   ///< Thickness of pcb
  TString        fBoardMat;         ///< Material of pcb
  Float_t        fBoardDensity;     ///< density of pcb
  Float_t        fBoardSizeX;       ///< size X of pcb   
  Float_t        fBoardSizeY;       ///< size Y of pcb   
  Float_t        fBoardSizeZ;       ///< size Z of pcb   
  Float_t        fBoardOffsetX;     ///< Offset size X of pcb   
  Float_t        fBoardOffsetY;     ///< Offset size Y of pcb   
  Float_t        fBoardOffsetZ;     ///< Offset size Z of pcb   
  Float_t        fBdHoleSizeX;     ///< size X of pcb hole  
  Float_t        fBdHoleSizeY;     ///< size Y of pcb hole   
  Float_t        fBdHoleSizeZ;     ///< size Z of pcb hole  
  TVector3       fBoardSize;        ///< Total size of pcb   
  TVector3       fBdHoleSize;       ///< Hole in pcb
  TVector3       fBoardOffset;      ///< Position offset of pcb
  TVector3       fOutBoxSize;       ///< Outer Box size
  TVector3       fInBoxSize;        ///< Inner Box size
  TVector3       fBoxOff;           ///< Box offset
  TVector3       fBoxHoleSize;      ///< Box hole size
  TVector3       fBoxHoleOff;       ///< Box hole offset
  TString        fBoxMat;           ///< Box material
  Float_t        fBoxDensity;       ///< Box density
 
  vector<string> fvMetalBody;       ///< Fluka metal body vector
  vector<string> fvModBody;         ///< Fluka body vector
  vector<string> fvStripBody;       ///< Fluka strip  vector
  vector<string> fvAirBody;         ///< Fluka air box vector
  vector<string> fvBoxBody;         ///< Fluka metal box vector
  vector<string> fvBoardBody;       ///< Fluka pcb vector
  vector<string> fvHoleBody;        ///< Fluka pcb hole vector
  vector<string> fvMetalRegion;     ///< Fluka metal region vector
  vector<string> fvModRegion;       ///< Fluka mod region vector
  vector<string> fvStripRegion;     ///< Fluka strip region vector
  vector<string> fvAirRegion;       ///< Fluka air box region vector
  vector<string> fvBoxRegion;       ///< Fluka box region vector
  vector<string> fvBoardRegion;     ///< Fluka pcb region vector

private:
  static const TString fgkBaseName;    ///< MSD base name
  static const Int_t   fgkDefSensorsN;   ///< default number of sensors

public:
  //! Get base name
  static const Char_t* GetBaseName()                    { return fgkBaseName.Data();    }
  //! Get sensor id from board and side id
  static Int_t GetSensorId(Int_t boardId, Int_t sideId) { return 2*(boardId-1)+sideId;  }
   //! Get default number of sensors
  static Int_t GetDefSensorsN()                         { return fgkDefSensorsN;        }
   
  ClassDef(TAMSDparGeo,3)
};

#endif
