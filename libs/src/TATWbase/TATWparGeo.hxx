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

#include "FootBox.hxx"
#include "GlobalPar.hxx"

#include <FieldManager.h>

#include "LightSabre.hxx"

class TGeoHMatrix;
class TGeoVolume;
class TObjArray;
//##############################################################################

class TATWparGeo : public TAGparTools {

public:
    TATWparGeo();
    virtual ~TATWparGeo();

    Bool_t         FromFile(const TString& name = "");

    void           DefineMaterial();

   Float_t        GetBarWidth()    const  { return fBarSize[0]; }
   Float_t        GetBarHeight()   const  { return fBarSize[1]; }
   Float_t        GetBarThick()    const  { return fBarSize[2]; }

   Int_t          GetBarId(Int_t layer, Float_t xGlob, Float_t yGlob);
   TVector3       GetBarPosition(Int_t layer, Int_t barId);
   TVector3       Detector2Sensor(Int_t layer, TVector3 vec);

   
   //! Add matrix transformation
   void            AddTransMatrix(TGeoHMatrix* mat, Int_t idx = -1);
   //! Remove matrix transformation
   void            RemoveTransMatrix(TGeoHMatrix* mat);
   //! Get matrix transformation
   TGeoHMatrix*    GetTransfo(Int_t idx);
   TGeoHMatrix*    GetTransfo(Int_t layer, Int_t barId);

   
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
    int GetNLayers() { return fLayersN; };

    string PrintBodies();
    string PrintRegions();
    string PrintAssignMaterial();
    string PrintSubtractBodiesFromAir();
    string PrintParameters();

    TGeoVolume*     BuildTofWall(const char *twName = "TW");
    TGeoVolume*     BuildTofWallXY(const char *twName = "TW", Int_t iLayer = 0);
    TGeoVolume*     BuildModule(Int_t iMod, Int_t iLayer);
   
    void            SetBarColorOn(Int_t slat,  Int_t view);
    void            SetBarColorOff(Int_t slat,  Int_t view);
   
    virtual void    Clear(Option_t* opt="");
    virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
   
private:
   TObjArray* fMatrixList;       //! list of transformation matrices  (rotation+translation for each crystal)
   TVector3   fCurrentPosition;  // current position

   TVector3  fSize;
   TVector3  fBarSize;
   TString fBarMat;
   Float_t fBarDensity;

  int fLayersN;
   int fBarsN;

  map<string, vector<string> > m_regionPrintOut;
  map<string, vector<string> > m_bodyPrintOut;
  map<string, vector<string> > m_regionName;
  map<string, vector<string> > m_bodyName;
  map<string, int > m_magneticRegion;
   
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
