#ifndef _TABMparGeo_HXX
#define _TABMparGeo_HXX
/*!
  \file
  \version $Id: TABMparGeo.hxx,v 1.2 2003/06/22 19:33:36 mueller Exp $
  \brief   Declaration of TABMparGeo.
*/
/*------------------------------------------+---------------------------------*/

#include "TString.h"
#include "TVector3.h"

#include "TAGparTools.hxx"

#include "GlobalPar.hxx"


//##############################################################################


class TGeoVolume;
class TABMparGeo : public TAGparTools {
public:

  TABMparGeo();
  virtual         ~TABMparGeo();

   Float_t        GetWidth()      const { return fBmSideDch[1]; }
   Float_t        GetLength()     const { return fBmSideDch[2]; }
   Float_t        GetHeigth()     const { return fBmSideDch[0]; }
   Int_t          GetLayersN()    const { return fLayersN;      }
   Int_t          GetWiresN()     const { return fWireLayersN;  }
   Int_t          GetCellsN()     const { return fSensesN;      }
   
   Float_t        GetCellHeight() const { return fBmStep;       }
   Float_t        GetCellWidth()  const { return fBmCellWide;   }
   Float_t        GetDeltaPlane() const { return fBmDplane;     }
   
   Float_t        GetSenseRad()   const { return fSenseRadius;  }
   TString        GetSenseMat()   const { return fSenseMat;     }
   Float_t        GetSenseRho()   const { return fSenseDensity; }
   Float_t        GetFieldRad()   const { return fFieldRadius;  }
   TString        GetFieldMat()   const { return fFieldMat;     }
   Float_t        GetFieldRho()   const { return fFieldDensity; }

   Float_t        GetFoilThick()  const { return fFoilThick;    }
   TString        GetFoilMat()    const { return fFoilMat;      }
   Float_t        GetShieldThick()const { return fShieldThick;  }
   Float_t        GetFoilRho()    const { return fFoilDensity;  }

   TVector3       GetSide()       const { return fBmSideDch;    }
   TVector3       GetDelta()      const { return fBmDeltaDch;   }
   
   TString        GetGasMixture()   const { return fGasMixture;   }
   TString        GetGasProp()      const { return fGasProp;      }
   TString        GetGasDensities() const { return fGasDensities; }
   Float_t        GetGasRho()       const { return fGasDensity;   }

   TVector3       GetMylar1()     const { return fMylar1;       }
   TVector3       GetMylar2()     const { return fMylar2;       }
   TVector3       GetTarget()     const { return fTarget;       }
   void           SetTarget(TVector3 v) { fTarget = v;          }
   
   //Id sense as function of cell
   Int_t          GetSenseId(int cell) { return fBmIdSense[cell]; }
   
   //X,Y,Z as a function of wire, layer, view
   Float_t        GetWireX(int wire, int layer, int view)  const { return fPosX[wire][layer][view]; }
   Float_t        GetWireY(int wire, int layer, int view)  const { return fPosY[wire][layer][view]; }
   Float_t        GetWireZ(int wire, int layer, int view)  const { return fPosZ[wire][layer][view]; }
   
   Float_t        GetWireCX(int wire, int layer, int view) const { return fPosCX[wire][layer][view]; }
   Float_t        GetWireCY(int wire, int layer, int view) const { return fPosCY[wire][layer][view]; }
   Float_t        GetWireCZ(int wire, int layer, int view) const { return fPosCZ[wire][layer][view]; }

   TVector3       GetWireAlign()                           const { return fWireAlign;                }
   TVector3       GetWireTilt()                            const { return fWireTilt;                 }

   TVector3       GetWirePos(Int_t view, Int_t layer, Int_t wire) const;
   TVector3       GetWireDir(Int_t view) const;

   int         GetCell(TVector3 pos, int layer, int view) ;
   
   void           GetCellInfo(Int_t view, Int_t plane, Int_t cellID,
                              Double_t& h_x, Double_t& h_y, Double_t& h_z,
                              Double_t& h_cx, Double_t& h_cy, Double_t& h_cz);
   
   //for a given cellid, it sets the ilay (0-5), view (0 or1) and icell (0-2)
   Bool_t GetBMNlvc(const Int_t cellid, Int_t& ilay, Int_t& iview, Int_t& icell);

   //get a number from 0 to 35 to identify any cell (ivew=0 or 1)
   Int_t          GetBMNcell(Int_t ilay, Int_t iview, Int_t icell){return icell+iview*3+ilay*6;};
   //get a number from 0 to 12 to identify real wire plane (iview=0 or 1)
   Int_t          GetWirePlane(Int_t ilay, Int_t iview){return iview + ilay*2;};   
   // transformation from BM to wire and vice versa
   void           Wire2Detector(Double_t xl, Double_t yl, Double_t zl,
                                Double_t& xg, Double_t& yg, Double_t& zg) const;
   TVector3       Wire2Detector(TVector3& loc) const;
   TVector3       Wire2DetectorVect(TVector3& loc) const;

   void           Detector2Wire(Double_t xg, Double_t yg, Double_t zg,
                                Double_t& xl, Double_t& yl, Double_t& zl) const;
   TVector3       Detector2Wire(TVector3& glob) const;
   TVector3       Detector2WireVect(TVector3& glob) const;

   //other methods
   TVector3       ProjectFromPversR0(TVector3 Pvers, TVector3 R0, Double_t z);
   TVector3       ProjectFromPversR0(Double_t PversXZ, Double_t PversYZ, Double_t R0X, Double_t R0Y, Double_t z);
   
   //~ void           SetWireAlignment(Bool_t reverse = false);
   
   void           InitGeo();
   
   void           DefineMaterial();

   TVector3       GetPlaneInfo(TVector3 pos, Int_t& view, Int_t& layer, Int_t& wire, Int_t& senseId);
   
   Bool_t         FromFile(const TString& name = "");
   
   virtual void   Clear(Option_t* opt="");
   
   virtual void   ToStream(ostream& os = cout, Option_t* option = "") const;
   
   TGeoVolume*    BuildBeamMonitor(const char *bmName = "BM");
    TGeoVolume*   BuildLayer(Int_t idx);
    
    void SetLayerColorOn(Int_t idx);
    void SetLayerColorOff(Int_t idx);
   
   string PrintRotations();
   string PrintBodies();
   string PrintRegions();
   string PrintAssignMaterial();
   string PrintParameters();
   string PrintSubtractBodiesFromAir();


  ClassDef(TABMparGeo,1)

private:
   Int_t           fWireLayersN;
   Int_t           fLayersN;
   Int_t           fSensesN;
   
   Float_t         fSenseRadius;
   TString         fSenseMat;
   Float_t         fSenseDensity;
   Float_t         fFieldRadius;
   TString         fFieldMat;
   Float_t         fFieldDensity;

   Float_t         fFoilThick; //foil==mylar
   TString         fFoilMat;
   Float_t         fFoilDensity;
   
   Float_t         fShieldThick;
   Float_t         fBmStep;
   Float_t         fBmCellWide;
   Float_t         fBmDplane;
   
   TString         fGasMixture;
   TString         fGasProp;
   TString         fGasDensities;
   Float_t         fGasDensity;

   /*  Johdot siirtyvät kammion sivuille */
   Float_t         fBmDeltaZ;
   Float_t         fBmDeltaY;
   Float_t         fBmDeltaX;
   
   TVector3        fWireAlign;
   TVector3        fWireTilt;
   TVector3        fMylar1;  // mylar1 center position in local coord.
   TVector3        fMylar2;  // mylar2 center position in local coord.
   TVector3        fTarget;  // target center position in local coord. set outside the class !!
   
   Int_t           fBmIdSense[3];
   
   TVector3        fBmSideDch;      /* Chamber side dimensions */
   TVector3        fBmDeltaDch;     /* displacement of 1st wire wrt chmb side */
   
   //x,y,z center positions of the wires and dimensions
   Double32_t      fPosX[50][6][2];
   Double32_t      fPosY[50][6][2];
   Double32_t      fPosZ[50][6][2];
   
   Double32_t      fPosCX[50][6][2];
   Double32_t      fPosCY[50][6][2];
   Double32_t      fPosCZ[50][6][2];

   TString         fkDefaultGeoName;  // default par geo file name

private:
   static const TString fgkDefParaName;
   static const TString fgkBaseName;    // device base name
   static Int_t         fgkLayerOffset; // offset in layer id

public:
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static Int_t   GetLayerOffset()       { return fgkLayerOffset;        }

};

#endif
