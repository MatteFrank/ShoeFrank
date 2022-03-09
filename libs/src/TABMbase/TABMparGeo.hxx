
#ifndef _TABMparGeo_HXX
#define _TABMparGeo_HXX
/*!
  \file TABMpargeo.hxx
  \brief   Declaration of TABMparGeo, to handle the geometry of the Beam Monitor
*/
/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "TVector3.h"

#include "TAGparTools.hxx"
#include "TAGmaterials.hxx"

#include "TAGrecoManager.hxx"


//##############################################################################


class TGeoVolume;
class TABMparGeo : public TAGparTools {
public:

  TABMparGeo();
  virtual         ~TABMparGeo();

  //! Get the BM dimensions
  TVector3       GetSide()       const { return fBmSideDch;    }
  //! Get the BM lateral size (11.2 cm)
   Float_t        GetWidth()      const { return fBmSideDch[1]; }
  //! Get the BM longitudinal size (21 cm)
   Float_t        GetLength()     const { return fBmSideDch[2]; }
  //! Get the BM lateral size (11.2 cm)
   Float_t        GetHeigth()     const { return fBmSideDch[0]; }
  //! Get the Number of BM layers (6)
   Int_t          GetLayersN()    const { return fLayersN;      }
  //! Get the number of BM wires per layer (21)
   Int_t          GetWiresN()     const { return fWireLayersN;  }
  //! Get the number of BM cells per layer and per view (3)
   Int_t          GetCellsN()     const { return fSensesN;      }
   //! Get the Cell half-size along Z (0.5 cm)
   Float_t        GetCellHeight() const { return fBmStep;       }
   //! Get the Cell half-width  perpendicular to Z (0.8 cm)
   Float_t        GetCellWidth()  const { return fBmCellWide;   }
   //!Distance along Z between two views/planes (0.3 cm)
   Float_t        GetDeltaPlane() const { return fBmDplane;     }
   //! Sense wire radius (0.0015 cm)
   Float_t        GetSenseRad()   const { return fSenseRadius;  }
   //!sense wire material (gold-plated tungsten in real detector, only tungsten in FLUKA)
   TString        GetSenseMat()   const { return fSenseMat;     }
   //!Sense wire density (19.3 g/cm^3)
   Float_t        GetSenseRho()   const { return fSenseDensity; }
   //! Field wire radius (0.0045 cm)
   Float_t        GetFieldRad()   const { return fFieldRadius;  }
   //!Field wire material (Aluminium)
   TString        GetFieldMat()   const { return fFieldMat;     }
   //!Field wire density (2.6989 g/cm^3)
   Float_t        GetFieldRho()   const { return fFieldDensity; }
   //! Thickness of the beam entrance and exit windows (0.0025 cm)
   Float_t        GetFoilThick()  const { return fFoilThick;    }
   //!Material of the beam entrance and exit windows (Mylar)
   TString        GetFoilMat()    const { return fFoilMat;      }
   //!Thickness of the BM Aluminium cage (1.5 cm)
   Float_t        GetShieldThick()const { return fShieldThick;  }
   //!Density of the beam entrance and exit windows (1.4 g/cm^3)
   Float_t        GetFoilRho()    const { return fFoilDensity;  }
   //! Position of 1st wire with respect to the BM side
   TVector3       GetDelta()      const { return fBmDeltaDch;   }
   //! Position of the beam entrance window in local ref.
   TVector3       GetMylar1()     const { return fMylar1;       }
   //! Position of the beam exit window in local ref.
   TVector3       GetMylar2()     const { return fMylar2;       }
   //!Gas mixture (Ar/CO2)
   TString        GetGasMixture()   const { return fGasMixture;   }
   //!Gas proportion (Ar at 80 and CO2 at 20)
   TString        GetGasProp()      const { return fGasProp;      }
   //! Gas components densities ( Ar=0.001662 CO2=0.00187)
   TString        GetGasDensities() const { return fGasDensities; }
   //! Overall gas density (0.001677136)
   Float_t        GetGasRho()       const { return fGasDensity;   }
   //! Get the sense wire index given the cell index
   Int_t          GetSenseId(int cell) const { return fBmIdSense[cell]; }
   //! Get the X coordinate of the wire position
   Float_t        GetWireX(int wire, int layer, int view)  const { return fPosX[wire][layer][view]; }
   //! Get the Y coordinate of the wire position
   Float_t        GetWireY(int wire, int layer, int view)  const { return fPosY[wire][layer][view]; }
   //! Get the Z coordinate of the wire position
   Float_t        GetWireZ(int wire, int layer, int view)  const { return fPosZ[wire][layer][view]; }
   //! Get the X coordinate of the wire direction
   Float_t        GetWireCX(int wire, int layer, int view) const { return fPosCX[wire][layer][view]; }
   //! Get the Y coordinate of the wire direction
   Float_t        GetWireCY(int wire, int layer, int view) const { return fPosCY[wire][layer][view]; }
   //! Get the Z coordinate of the wire direction
   Float_t        GetWireCZ(int wire, int layer, int view) const { return fPosCZ[wire][layer][view]; }

   TVector3       GetWireAlign(Int_t i);
   TVector3       GetWireTilt(Int_t i);
   TVector3       GetWirePos(Int_t view, Int_t layer, Int_t wire) const;
   TVector3       GetWireDir(Int_t view) const;
   Int_t          GetCell(TVector3 pos, int layer, int view) ;

   void           Wire2Detector(Int_t cellid, Double_t xl, Double_t yl, Double_t zl, Double_t& xg, Double_t& yg, Double_t& zg) const;
   TVector3       Wire2Detector(Int_t cellid, TVector3& loc) const;
   TVector3       Wire2DetectorVect(Int_t cellid, TVector3& loc) const;
   void           Detector2Wire(Int_t cellid, Double_t xg, Double_t yg, Double_t zg, Double_t& xl, Double_t& yl, Double_t& zl) const;
   TVector3       Detector2Wire(Int_t cellid, TVector3& glob) const;
   TVector3       Detector2WireVect(Int_t cellid, TVector3& glob) const;

   TVector3       ProjectFromPversR0(TVector3 Pvers, TVector3 R0, Double_t z) const;
   TVector3       ProjectFromPversR0(Double_t PversXZ, Double_t PversYZ, Double_t R0X, Double_t R0Y, Double_t z) const;
   Double_t       FindRdrift(TVector3 pos, TVector3 dir, TVector3 A0, TVector3 Wvers, Bool_t isTrack) const;
   Bool_t         GetBMNlvc(Int_t cellid, Int_t& ilay, Int_t& iview, Int_t& icell) const;
   void           GetCellInfo(Int_t view, Int_t plane, Int_t cellID, Double_t& h_x, Double_t& h_y, Double_t& h_z, Double_t& h_cx, Double_t& h_cy, Double_t& h_cz) const;
   //! Get the cellid index [0-35] given the layer, view and cell indices
   Int_t          GetBMNcell(Int_t ilay, Int_t iview, Int_t icell) const {return icell+iview*3+ilay*6;};

   void           InitGeo();
   void           DefineMaterial();
   TVector3       GetPlaneInfo(TVector3 pos, Int_t& view, Int_t& layer, Int_t& wire, Int_t& senseId);
   Bool_t         FromFile(const TString& name = "");
   TGeoVolume*    BuildBeamMonitor(const char *bmName = GetBaseName());
   TGeoVolume*    BuildLayer(Int_t idx);

   virtual void   Clear(Option_t* opt="");
   virtual void   ToStream(ostream& os = cout, Option_t* option = "") const;

   void SetLayerColorOn(Int_t idx);
   void SetLayerColorOff(Int_t idx);

   //! get the region number of the beam entrance window
   Int_t          GetRegMylar1() {TString regname("BMN_MYL0");return GetCrossReg(regname);};
   //! get the region number of the beam exit window
   Int_t          GetRegMylar2() {TString regname("BMN_MYL1");return GetCrossReg(regname);};
   //! get the region number of the detector shielding
   Int_t          GetRegShield() {TString regname("BMN_SHI");return GetCrossReg(regname);};      //detector shielding
   //! get the region number of the gas outside cells
   Int_t          GetRegGas() {TString regname("BMN_GAS");return GetCrossReg(regname);};
   //! get the region number of the field wires
   Int_t          GetRegFieldWires() {TString regname("BMN_FWI");return GetCrossReg(regname);};
   //! get the region number of the sense wires
   Int_t          GetRegSenseWires() {TString regname("BMN_SWI");return GetCrossReg(regname);};
   //! get the region number of the cell for a given cellid [0-35] index
   Int_t          GetRegCell (Int_t cellid) ;
   //! get the region number of the cell given the lay, view and cell indices
   Int_t          GetRegCell (Int_t ilay, Int_t iview, Int_t icell) ;

   string PrintRotations();
   string PrintBodies();
   string PrintRegions();
   string PrintAssignMaterial(TAGmaterials *Material);
   string PrintParameters();
   string PrintSubtractBodiesFromAir();

   //! Get the name of this class (bmGeo)
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   //! Get the detector name of this class (BM)
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   //! Get the offset in layer id (6)
   static Int_t   GetLayerOffset()       { return fgkLayerOffset;        }

private:
   Int_t           fWireLayersN;  ///< Number of field and sense wires for each layer and for each view (21)
   Int_t           fLayersN;      ///< Number of Layers for each view (6)
   Int_t           fSensesN;      ///< Number of Sense wires for each layer and for each view (3)

   Float_t         fSenseRadius;  ///< Sense wire radius (0.0015 cm)
   TString         fSenseMat;     ///< sense wire material (gold-plated tungsten in real detector, only tungsten in FLUKA)
   Float_t         fSenseDensity; ///< Sense wire density (19.3 g/cm^3)
   Float_t         fFieldRadius;  ///< Field wire radius (0.0045 cm)
   TString         fFieldMat;     ///< Field wire material (Aluminium)
   Float_t         fFieldDensity; ///< Field wire density (2.6989 g/cm^3)

   Float_t         fFoilThick;    ///< Thickness of the beam entrance and exit windows (0.0025 cm)
   TString         fFoilMat;      ///< Material of the beam entrance and exit windows (Mylar)
   Float_t         fFoilDensity;  ///< Density of the beam entrance and exit windows (1.4 g/cm^3)

   Float_t         fShieldThick;  ///< Thickness of the BM Aluminium cage (1.5 cm)
   Float_t         fBmStep;       ///< Cell half-size along Z (0.5 cm)
   Float_t         fBmCellWide;   ///< Cell half-size along X and Y (0.8 cm)
   Float_t         fBmDplane;     ///< Distance along Z between two views/planes (0.3 cm)

   TString         fGasMixture;   ///< Gas mixture (usually Ar/CO2)
   TString         fGasProp;      ///< Gas proportion (usually 80/20)
   TString         fGasDensities; ///< Gas components densities (if Ar/CO2: 0.001662/0.00187)
   Float_t         fGasDensity;   ///< Overall gas density (If Ar/CO2 at 80/20: 0.001677136)

   TVector3        fMylar1;  ///< mylar1 center position in local coord.
   TVector3        fMylar2;  ///< mylar2 center position in local coord.

   Int_t           fBmIdSense[3]; ///< Indices of sense wires on each layer. Used to build the BM geometry

   TVector3        fBmSideDch;      ///< Chamber side dimensions
   TVector3        fBmDeltaDch;     ///< Position of 1st wire with respect to the BM side

   Double32_t      fPosX[21][6][2];  ///< X Positions of the BM wires
   Double32_t      fPosY[21][6][2];  ///< Y Positions of the BM wires
   Double32_t      fPosZ[21][6][2];  ///< Z Positions of the BM wires

   Double32_t      fPosCX[21][6][2]; ///< X Directions of the BM wires
   Double32_t      fPosCY[21][6][2]; ///< Y Directions of the BM wires
   Double32_t      fPosCZ[21][6][2]; ///< Z Diretions of the BM wires

   TString         fkDefaultGeoName;  ///< Default par geo file name (./geomaps/TABMdetector.geo)

   static const TString fgkDefParaName;  ///< Default par name (bmGeo)
   static const TString fgkBaseName;    ///< device base name
   static Int_t         fgkLayerOffset; ///< offset in layer id

  ClassDef(TABMparGeo,1)
};

#endif
