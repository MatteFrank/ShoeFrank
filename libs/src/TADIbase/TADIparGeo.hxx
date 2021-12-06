#ifndef _TADIparGeo_HXX
#define _TADIparGeo_HXX
/*!
  \file TADIparGeo.hxx
  \brief   Declaration of TADIparGeo.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include <map>
#include "Riostream.h"

#include "TObject.h"
#include "TString.h"

#include "TAGparTools.hxx"
#include "TAGmaterials.hxx"


class TGeoHMatrix;
class TGeoVolume;
//##############################################################################

class TADIparGeo : public TAGparTools {
  
private:
  Int_t     fMagnetsN;      /// number of magnets
  Int_t     fType;          /// Magnet type
  TString   fMapName;       /// Map filename
  
  Float_t   fMapMesh;       /// size of the map mesh
  TVector3  fMapLimLo;      /// lower bound of the field map
  TVector3  fMapLimUp;      /// upper bound of the field map
  
  TVector3  fMagCstValue;   /// if type 0, value of constant field

  Float_t   fShieldRadius;  /// cover radius
  TString   fShieldMat;     /// cover material
  Float_t   fShieldDensity; /// cover material density
  Float_t   fShieldThick;   /// cover material density
  
  TString   fMagMat;        /// Magnet material
  Float_t   fMagDensity;    /// Magnet material density
  
  struct MagnetParameter_t : public  TObject {
    Int_t     MagnetIdx;    /// Magnet index
    TVector3  Size;         /// current size
    TVector3  ShieldSize;   /// shield size
    TVector3  Position;     /// current position
    TVector3  Tilt;         /// current tilt angles
  };
  MagnetParameter_t  fMagnetParameter[10]; /// magnet parameters
  
  
  TVector3   fMinPosition;   /// min position
  TVector3   fMaxPosition;   /// max position
  TVector3   fSizeBox;       /// box size
  
  vector<string> fvReg;      /// Fluka region
  vector<string> fvRegShield;/// Fluka shield region
  vector<string> fvBodyOut;  /// Fluka body in
  vector<string> fvBodyIn;   /// Fluka body out

private:
  static const TString fgkDefParaName;   /// defaylt parameter name
  static       TString fgDefaultGeoName; /// default detector geomap file
  static const TString fgkDevBaseName;   /// device base name
  static const Int_t fgkDefMagnetsN;     /// default number of Magnets
  
public:
   //! Get default number of magnets
  static Int_t         GetDefMagnetsN()      { return fgkDefMagnetsN;        }
   //! Get device base name
  static const Char_t* GetBaseName()         { return fgkDevBaseName.Data(); }
   //! Get default paramater name
  static const Char_t* GetDefParaName()      { return fgkDefParaName.Data(); }
  
public:
  TADIparGeo();
  virtual ~TADIparGeo();
  
   //! Get Magnet materialo
  TString        GetMagMat()    const  { return fMagMat;    }
   //! Get magnet shield material
  TString        GetShieldMat() const  { return fShieldMat; }
  
  //! Transform point from the global reference frame
  //! to the local reference frame of the detection id
  void            Global2Local(Int_t detID,  Double_t xg, Double_t yg, Double_t zg, 
			       Double_t& xl, Double_t& yl, Double_t& zl) const;
  //! Global to local
  TVector3        Global2Local(Int_t detID, TVector3& glob) const;
   //! Global to local vector
  TVector3        Global2LocalVect(Int_t detID, TVector3& glob) const;
  
  //! Transform point from the local reference frame
  //! of the detection id to the global reference frame 
  void            Local2Global(Int_t detID,  Double_t xl, Double_t yl, Double_t zl, 
			       Double_t& xg, Double_t& yg, Double_t& zg) const;
  //! Local to global
  TVector3        Local2Global(Int_t detID, TVector3& loc) const;
   //! Local to global vector
  TVector3        Local2GlobalVect(Int_t detID, TVector3& loc) const;
  
  //! Read parameters from file
  Bool_t          FromFile(const TString& name = "");
  
  //! Define materials
  void            DefineMaterial();
  
  //! Get position Magnet
  TVector3        GetPosition(Int_t iMagnet);
  
  //! Get number of Magnets
  Int_t GetMagnetsN()                  const { return fMagnetsN; }
  
  //! Get type of Magnets
  Int_t GetType()                      const { return fType;     }
  
  //! Get map file name
  TString GetMapName()                 const { return fMapName;  }
  
  //! Get map mesh size
   Float_t         GetMapMesh()        const { return fMapMesh; }
   
  //! Get map lower bound
  TVector3        GetMapLimLo()        const { return fMapLimLo; }
   
  //! Get map upper bound
  TVector3        GetMapLimUp()        const { return fMapLimUp; }
  
  //! Get field const value
  TVector3        GetMagCstValue()     const { return fMagCstValue; }
   
  //! Build Magnet
  TGeoVolume* BuildMagnet(const char* basemoduleName = "Module", const char *name = GetBaseName());
  
  //! Get Magnet parameter
  MagnetParameter_t& GetMagnetPar(Int_t idx) { return fMagnetParameter[idx]; }
  
  // Getter
  //! Get box size
  TVector3    GetBoxSize()     const { return fSizeBox;  }
  //! Get min position
  TVector3    GetMinPoistion() const { return fMinPosition; }
  //! Get Max position
  TVector3    GetMaxPoistion() const { return fMaxPosition; }

  //crossing regions
   //! Get magnet region
  Int_t       GetRegMagnet(Int_t n); //n=0,1
  //! Get shield region
  Int_t       GetRegShield(Int_t n); //n=0,1
  
  // to print fluka files
  //! Print Fluka rotations
  string PrintRotations();
  //! Print Fluka rotations
  string PrintBodies();
  //! Print Fluka bodies
  string PrintRegions();
  //! Print Fluka assignment materials
  string PrintAssignMaterial(TAGmaterials *Material);
   //! Print Fluka subtract bodies
  string PrintSubtractBodiesFromAir();
  //! Print Fluka parameters
  string PrintParameters();
  
private:
   //! compute min/max position
  void DefineMaxMinDimension();
  
  ClassDef(TADIparGeo,1)
};

#endif
