#ifndef _TATWparGeo_HXX
#define _TATWparGeo_HXX
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TEveGeoShapeExtract.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
#include "TRotation.h"

#include "TAGparTools.hxx"
#include "TAGmaterials.hxx"

#include "TAGrecoManager.hxx"

class TGeoCombiTrans;
class TGeoVolume;
class TObjArray;
class TAGionisMaterials;

//##############################################################################

class TATWparGeo : public TAGparTools {

public:
    TATWparGeo();
    virtual ~TATWparGeo();

    Bool_t         FromFile(const TString& name = "");

    void           DefineMaterial();

   TVector3       GetBarSize()     const  { return fBarSize;     }
   Float_t        GetBarWidth()    const  { return fBarSize[0];  }
   Float_t        GetBarHeight()   const  { return fBarSize[1];  }
   Float_t        GetBarThick()    const  { return fBarSize[2];  }

   TString        GetBarMat()      const  { return fBarMat;      }
   Float_t        GetBarDensity()  const  { return fBarDensity;  }
   Float_t        GetBarIonisMat() const  { return fBarIonisMat; }
   Float_t        GetBarBirkMat()  const  { return fBarBirkMat;  }

   Int_t          GetBarId(Int_t layer, Float_t xGlob, Float_t yGlob);
   TVector3       GetBarPosition(Int_t layer, Int_t barId);
   TVector3       Detector2Sensor(Int_t layer, TVector3 vec);
   TVector3       GetLayerPosition(Int_t layer);
   
   //! Get matrix transformation
   TGeoCombiTrans* GetTransfo(Int_t layer, Int_t barId);

   
   //! Transform point from the global detector reference frame
   //! to the local sensor reference frame of the detection id
   TVector3        Detector2Sensor(Int_t iLayer, Int_t iBar, TVector3& glob) const;
   TVector3        Detector2SensorVect(Int_t iLayer, Int_t iBar, TVector3& glob) const;
   
   //! Transform point from the local reference frame
   //! of the detection id to the global reference frame
   TVector3        Sensor2Detector(Int_t iLayer, Int_t iBar, TVector3& loc) const;
   TVector3        Sensor2DetectorVect(Int_t iLayer, Int_t iBar, TVector3& loc) const;

  
   Float_t         GetCoordiante_sensorFrame(Int_t iLayer, Int_t iBar);
   Float_t         GetZ_sensorFrame(Int_t iLayer, Int_t iBar);
   
    // Return Scintillator full dimension.
    TVector3 GetDimension() { return fSize; };

    TVector3 GetBarDimension() { return fBarSize; };   // ( shorter dim, longer dim, z dim )

   
    int GetNBars() { return fBarsN; };
    int GetLayersN() { return fLayersN; };

   //crossing regions
   Int_t          GetRegStrip(Int_t lay, Int_t bar); //lay (0-1); bar(0-19)

    virtual string PrintBodies();
    virtual string PrintRegions();
    virtual string PrintAssignMaterial(TAGmaterials *Material);
    virtual string PrintSubtractBodiesFromAir();
    virtual string PrintParameters();
    virtual string PrintRotations();

    TGeoVolume*     BuildTofWall(const char *twName = GetBaseName());
    TGeoVolume*     BuildModule(Int_t iMod, Int_t iLayer);
   
    void            SetBarColorOn(Int_t slat,  Int_t view);
    void            SetBarColorOff(Int_t slat,  Int_t view);
   
    virtual void    Clear(Option_t* opt="");
    virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
private:
   TAGionisMaterials* fIonisation; //! pointer for ionisation property
   TVector3  fSize;
   TVector3  fBarSize;
   TString   fBarMat;
   Float_t   fBarDensity;
   Float_t   fBarIonisMat;
   Float_t   fBarBirkMat;

   Int_t     fLayersN;
   Int_t     fBarsN;

   TString    fkDefaultGeoName;  // default par geo file name

   vector<string> fvBody;
   vector<string> fvRegion;
   vector< vector<TVector3> > fvTilt;
   
private:
   static TString       fgkDefParaName;
   static const TString fgkBaseName;
   static Int_t         fgkLayerOffset;      // offset in salt id for 2nd layer
   static const Color_t fgkDefaultModCol;    // default color of slat/module;
   static const Color_t fgkDefaultModColOn;  // default color of fired slat/module;
   static const TString fgkDefaultBarName;  // default slat name;

public:
   static const Char_t* GetDefParaName() { return fgkDefParaName.Data(); }
   static const Char_t* GetBaseName()    { return fgkBaseName.Data();    }
   static Int_t   GetLayerOffset()       { return fgkLayerOffset;        }
   static Color_t GetDefaultModCol()     { return fgkDefaultModCol;      }
   static Color_t GetDefaultModColOn()   { return fgkDefaultModColOn;    }
   
   static const Char_t* GetDefaultBarName(Int_t slat, Int_t view) { return Form("%s_%d_%d", fgkDefaultBarName.Data(), slat, view); }

   
  ClassDef(TATWparGeo,1)
};

#endif
