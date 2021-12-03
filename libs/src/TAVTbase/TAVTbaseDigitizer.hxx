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
   Double_t     FuncTotAnalog(Double_t* qin, Double_t* par);
   Double_t     FuncTotDigital(Double_t* qin, Double_t* par);
   
   Int_t        DigitizeAdc(Double_t value, Int_t nbits, Double_t maxValue);

   virtual Bool_t Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin = 0, Double_t zout = 0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);

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
   
   Float_t      GetPixelsNx()       const        { return fPixelsNx;        }
   Float_t      GetPixelsNy()       const        { return fPixelsNy;        }
   
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
   TAVTbaseParGeo* fpParGeo;       /// pointer on par geo
   TF1*        fFuncClusterSize;   /// cluster size function
   TF1*        fFuncClusterHeight; /// cluster charge height function
   TF1*        fFuncClusterWidth;  /// cluster charge width function
   TF1*        fFuncTotAnalog;     /// time over threshold analog function
   TF1*        fFuncTotDigital;    /// time over threshold digital function

   std::map<int, double> fMap;   /// map of found pixels
   
   Int_t       fPixelsN;         /// number of pixels for a given eloss
   Double_t    fDe0Par;          /// parameter shift in edep for the cluster size function
   Double_t    fDe0ParErr;       /// parameter shift in edep for the cluster size function
   Double_t    fRsPar;           /// parameter r_s for the cluster size function
   Double_t    fRsParErr;        /// parameter r_s for the cluster size function
   Double_t    fThresPar;        /// parameter threshold for cluster size function
   Double_t    fThresParErr;     /// parameter threshold for cluster size function
   Double_t    fLinPar;          /// parameter linear for cluster size function
   Double_t    fLinParErr;       /// parameter linear for cluster size function

   Double_t    fClusterHeight;   /// height of the cluster for a given eloss
   Double_t    fLogHeightPar;    /// Log parameter for the cluster height function
   Double_t    fLogHeightParErr; /// Log parameter for the cluster height function
   Double_t    fCstHeightPar;    /// constant parameter for cluster height function
   Double_t    fCstHeightParErr; /// constant parameter for cluster height function
   Double_t    fZcstHeightPar;   /// z constant parameter for the cluster height function
   Double_t    fZgainHeightPar;  /// z gain parameter for the cluster height function

   Double_t    fClusterWidth;    /// width of the cluster for a given eloss
   Double_t    fLogWidthPar;     /// log parameter for the cluster width function
   Double_t    fLogWidthParErr;  /// log parameter for the cluster width function
   Double_t    fCstWidthPar;     /// constant parameter for cluster width function
   Double_t    fCstWidthParErr;  /// constant parameter for cluster width function
   Double_t    fZcstWidthPar;    /// z constant parameter for the cluster width function
   Double_t    fZgainWidthPar;   /// z gain parameter for the cluster width function

   Double_t     fTotConversion;  /// conversion factor for Time over threshold
   Double_t     fTotExponent;    /// exponent factor for Time over threshold
   Double_t     fTotThres;       /// threshold factor for Time over threshold

   Int_t       fPixelsNx;        /// number of pixels in X (colummn)
   Int_t       fPixelsNy;        /// number of pixels in Y (line)
   
   Float_t     fPitchX;          /// pitch in X
   Float_t     fPitchY;          /// pitch in Y
   
   Int_t*      fShel;            /// pointer to current shell
   
protected:
   //! Set functoins
   void        SetFunctions();
   //! Compute last shell
   Int_t       GetLastShell(Int_t* shell, Int_t maxTurn) const;
   
public:
   //! Get smearing flag
   static Bool_t  GetSmearFlag()                    { return fgSmearFlag;        }
   //! Set smearing flag
   static void    SetSmearFlag(Bool_t flag)         { fgSmearFlag = flag;        }
   
   //! Get default smear parameter
   static Float_t GetDefSmearPos()                  { return fgDefSmearPos;      }
   //! Set default smear parameter
   static void    SetDefSmearPos(Float_t pos)       { fgDefSmearPos = pos;       }
   //! Get random function
   static Int_t   GetRandom (Int_t i)               { return std::rand() % i;    }
   
   //! Get time over threshold adc depth
   static Int_t   GetTotAdcDepth()                  { return fgTotAdcDepth;      }
   //! Set time over threshold adc depth
   static void    SetTotAdcDepth(Int_t adc)         { fgTotAdcDepth = adc;       }
   //! Get time over threshold maximum value
   static Float_t GetTotMaxValue()                  { return fgTotMaxValue;      }
   //! Set time over threshold maximum value
   static void    SetTotMaxValue(Float_t value)     { fgTotMaxValue = value;     }
   
protected:
   static Float_t  fgDefSmearPos;   /// default smear parameter
   static Bool_t   fgSmearFlag;     /// smearing flag
   static Float_t  fgkPairCreation; /// pair creation energy
   static Float_t  fgkFanoFactor;   /// fano factor
   static Float_t  fgkNormFactor;   /// normalization factor
   static Float_t  fgTotMaxValue;   /// Time over threshold maximum value
   static Int_t    fgTotAdcDepth;   /// Time over threshold adc depth

};

#endif
