#ifndef _TAVTbaseDigitizer_HXX
#define _TAVTbaseDigitizer_HXX
/*!
 \file
 \version $Id: TAVTbaseDigitizer.hxx,v $
 \brief   Declaration of TAVTbaseDigitizer.
 */
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"
#include <map>

#include "TH2F.h"
#include "TObject.h"
#include "TString.h"
#include "TList.h"

#include "TAGbaseDigitizer.hxx"
#include "TAGparaDsc.hxx"


class TF1;
class TAVTbaseParGeo;
class TAVTbaseDigitizer : public TAGbaseDigitizer {
   
public:
   TAVTbaseDigitizer(TAVTbaseParGeo* parGeo);
   virtual ~TAVTbaseDigitizer();
   
   Double_t     FuncClusterSize(Double_t* x, Double_t* par);
   Double_t     FuncClusterHeight(Double_t* x, Double_t* par);
   Double_t     FuncClusterWidth(Double_t* x, Double_t* par);

   virtual Bool_t Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin = 0, Double_t zout = 0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99);

   virtual Bool_t MakeCluster(Double_t /*x0*/, Double_t /*y0*/, Double_t /*zin*/, Double_t /*zout*/) { return false; }

   Int_t        GetPixelsN()       const         { return fPixelsN;        }
   void         SetRsPar(Double_t par)           { fRsPar = par;           }
   void         SetRsParErr(Double_t par)        { fRsParErr = par;        }
   void         SetThresPar(Double_t par)        { fThresPar = par;        }
   void         SetThresParErr(Double_t par)     { fThresParErr = par;     }
   
   Float_t      GetClusterHeight() const         { return fClusterHeight;  }
   void         SetLogHeightPar(Double_t par)    { fLogHeightPar = par;    }
   void         SetLogHeightParErr(Double_t par) { fLogHeightParErr = par; }
   void         SetCstHeightPar(Double_t par)    { fCstHeightPar = par;    }
   void         SetCstHeightParErr(Double_t par) { fCstHeightParErr = par; }
   
   Float_t      GetClusterWidth()  const        { return fClusterWidth;    }
   void         SetLogWidthPar(Double_t par)    { fLogWidthPar = par;      }
   void         SetLogWidthParErr(Double_t par) { fLogWidthParErr = par;   }
   void         SetCstWidthPar(Double_t par)    { fCstWidthPar = par;      }
   void         SetCstWidthParErr(Double_t par) { fCstWidthParErr = par;   }
   
   Float_t      GetNPixelX()       const        { return fPixelsNx;        }
   Float_t      GetNPixelY()       const        { return fPixelsNy;        }
   
   Float_t      GetPitchX()        const        { return fPitchX;          }
   Float_t      GetPitchY()        const        { return fPitchX;          }

   TF1*         GetFuncClusterSize()            { return fFuncClusterSize;   }
   TF1*         GetFuncClusterHeight()          { return fFuncClusterHeight; }
   TF1*         GetFuncClusterWidth()           { return fFuncClusterWidth;  }

   //! Get Map
   std::map<int, double>  GetMap() const        { return fMap;             }
   
   //! Get column/line from x/y position
   Int_t        GetColumn(Float_t x) const;
   Int_t        GetLine(Float_t y)   const;
   Int_t        GetIndex(Int_t line, Int_t column) const;
   
   //! Get position from pixel line/column
   Float_t      GetPositionU(Int_t column)         const;
   Float_t      GetPositionV(Int_t line)           const;
   
   //! Get remainder of column/line
   Float_t      GetColRemainder(Float_t x)  const;
   Float_t      GetLineRemainder(Float_t y) const;
   
   //! Get pixel region of column/line
   Int_t        GetColRegion(Float_t x)  const;
   Int_t        GetLineRegion(Float_t y) const;

protected:
   TAVTbaseParGeo* fpParGeo;
   TF1*        fFuncClusterSize;   // cluster size function
   TF1*        fFuncClusterHeight; // cluster charge height function
   TF1*        fFuncClusterWidth;  // cluster charge width function

   std::map<int, double> fMap;   // map of found pixels
   
   Int_t       fPixelsN;         // number of pixels for a given eloss
   Double_t    fDe0Par;          // parameter shift in edep for the cluster size function
   Double_t    fDe0ParErr;       // parameter shift in edep for the cluster size function
   Double_t    fRsPar;           // parameter r_s for the cluster size function
   Double_t    fRsParErr;        // parameter r_s for the cluster size function
   Double_t    fThresPar;        // parameter threshold for cluster size function
   Double_t    fThresParErr;     // parameter threshold for cluster size function

   Double_t    fClusterHeight;   // height of the cluster for a given eloss
   Double_t    fLogHeightPar;    // Log parameter for the cluster height function
   Double_t    fLogHeightParErr; // Log parameter for the cluster height function
   Double_t    fCstHeightPar;    // constant parameter for cluster height function
   Double_t    fCstHeightParErr; // constant parameter for cluster height function
   Double_t    fZcstHeightPar;   // z constant parameter for the cluster height function
   Double_t    fZgainHeightPar;  // z gain parameter for the cluster height function

   Double_t    fClusterWidth;    // width of the cluster for a given eloss
   Double_t    fLogWidthPar;     // log parameter for the cluster width function
   Double_t    fLogWidthParErr;  // log parameter for the cluster width function
   Double_t    fCstWidthPar;     // constant parameter for cluster width function
   Double_t    fCstWidthParErr;  // constant parameter for cluster width function
   Double_t    fZcstWidthPar;    // z constant parameter for the cluster width function
   Double_t    fZgainWidthPar;   // z gain parameter for the cluster width function

   
   Int_t       fPixelsNx;        // number of pixels in X (colummn)
   Int_t       fPixelsNy;        // number of pixels in Y (line)
   
   Float_t     fPitchX;          // pitch in X
   Float_t     fPitchY;          // pitch in Y
   
   Int_t*      fShel;
   
protected:
   void        SetFunctions();
   Int_t       GetLastShell(Int_t* shell, Int_t maxTurn) const;
   
public:
   static Bool_t  GetSmearFlag()                    { return fgSmearFlag;        }
   static void    SetSmearFlag(Bool_t flag)         { fgSmearFlag = flag;        }
   
   static Float_t GetDefSmearPos()                  { return fgDefSmearPos;      }
   static void    SetDefSmearPos(Float_t pos)       { fgDefSmearPos = pos;       }
   
   static Int_t   GetRandom (Int_t i)               { return std::rand() % i;    }
   
protected:
   static Float_t  fgDefSmearPos;
   static Bool_t   fgSmearFlag;

   ClassDef(TAVTbaseDigitizer,0)
};

#endif
