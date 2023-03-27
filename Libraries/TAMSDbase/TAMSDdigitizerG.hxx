#ifndef _TAMSDdigitizerG_HXX
#define _TAMSDdigitizerG_HXX

#include <map>

#include "Riostream.h"
#include "TF1.h"
#include "TH2F.h"
#include "TMath.h"
#include "TObject.h"
#include "TRandom3.h"
#include "TString.h"

#include "TAGbaseDigitizer.hxx"

// --------------------------------------------------------------------------------------
class TAMSDparGeo;
class TAMSDdigitizerG : public TAGbaseDigitizer {
   
   // Class to digitize the energy into strips based on given patterns
public:
   TAMSDdigitizerG(TAMSDparGeo* parGeo);
   virtual ~TAMSDdigitizerG();
   
   virtual       Bool_t Process(Double_t edep, Double_t x0, Double_t y0, Double_t zin = 0, Double_t zout = 0, Double_t time = 0,
                                Int_t sensorId = 0, Int_t Z = -99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
   
   Bool_t        MakeCluster(Double_t x0, Double_t y0, Double_t zin, Double_t zout, Int_t sensorId);
   
   Double_t      FuncClusterHeight(Double_t* x, Double_t* par);
   Double_t      FuncClusterWidth(Double_t* x, Double_t* par);
   
   // Get strip from x position
   Int_t         GetStrip(Float_t x)     const;
   
   // Get remainder of strip
   Float_t       GetRemainder(Float_t x) const;
   
   // Get strip region
   Int_t         GetRegion(Float_t x)    const;
   
   //! Get parameter r_s for the cluster size function
   void          SetRsPar(Double_t par)           { fRsPar = par;              }
   //! Get parameter r_s error for the cluster size function
   void          SetRsParErr(Double_t par)        { fRsParErr = par;           }
   //! Get parameter threshold for the cluster size function
   void          SetThresPar(Double_t par)        { fThresPar = par;           }
   //! Get parameter threshold error for the cluster size function
   void          SetThresParErr(Double_t par)     { fThresParErr = par;        }
   
   //! Get height of the cluster for a given eloss
   Float_t       GetClusterHeight() const         { return fClusterHeight;     }
   //! Get Log parameter for the cluster height function
   void          SetLogHeightPar(Double_t par)    { fLogHeightPar = par;       }
   //! Get Log error parameter for the cluster height function
   void          SetLogHeightParErr(Double_t par) { fLogHeightParErr = par;    }
   //! Get constant parameter for cluster height function
   void          SetCstHeightPar(Double_t par)    { fCstHeightPar = par;       }
   //! Get constant error parameter for cluster height function
   void          SetCstHeightParErr(Double_t par) { fCstHeightParErr = par;    }
   
   //! Get width of the cluster for a given eloss
   Float_t       GetClusterWidth()  const         { return fClusterWidth;      }
   //! Get constant parameter for cluster width function
   void          SetCstWidthPar(Double_t par)     { fCstWidthPar = par;        }
   //! Get constant error parameter for cluster width function
   void          SetCstWidthParErr(Double_t par)  { fCstWidthParErr = par;     }
   
   //! Get number of strips
   Float_t       GetStripsN()       const         { return fStripsN;           }
   //! Get pitch
   Float_t       GetPitch ()        const         { return fPitch;             }
   
   //! Get cluster height function
   TF1*          GetFuncClusterHeight()           { return fFuncClusterHeight; }
   //! Get cluster width function
   TF1*          GetFuncClusterWidth()            { return fFuncClusterWidth;  }

   //! Get Map
   std::map<int, double>  GetMap() const         { return fMap;               }
   
private:
   TAMSDparGeo* fpParGeo;
   
   TF1*         fFuncClusterDis;    ///< cluster distribution function
   TF1*         fFuncClusterHeight; ///< cluster charge height function
   TF1*         fFuncClusterWidth;  ///< cluster charge width function
   TF1*         fFuncToDigital;     ///< time over threshold digital function
   
   std::map<int, double> fMap;      ///< map of found strips
   
   Double_t     fDe0Par;            ///< parameter shift in edep for the cluster size function
   Double_t     fDe0ParErr;         ///< parameter shift in edep for the cluster size function
   Double_t     fRsPar;             ///< parameter r_s for the cluster size function
   Double_t     fRsParErr;          ///< parameter r_s for the cluster size function
   Double_t     fThresPar;          ///< parameter threshold for cluster size function
   Double_t     fThresParErr;       ///< parameter threshold for cluster size function
   Double_t     fLinPar;            ///< parameter linear for cluster size function
   Double_t     fLinParErr;         ///< parameter linear for cluster size function
   
   Double_t     fClusterHeight;     ///< height of the cluster for a given eloss
   Double_t     fLogHeightPar;      ///< Log parameter for the cluster height function
   Double_t     fLogHeightParErr;   ///< Log parameter for the cluster height function
   Double_t     fCstHeightPar;      ///< constant parameter for cluster height function
   Double_t     fCstHeightParErr;   ///< constant parameter for cluster height function
   Double_t     fZcstHeightPar;     ///< z constant parameter for the cluster height function
   Double_t     fZgainHeightPar;    ///< z gain parameter for the cluster height function
   
   Double_t     fClusterWidth;      ///< width of the cluster for a given eloss
   Double_t     fCstWidthPar;       ///< constant parameter for cluster width function
   Double_t     fCstWidthParErr;    ///< constant parameter for cluster width function
   
   Double_t     fTotConversion;     ///< conversion factor for Time over threshold
   Double_t     fTotExponent;       ///< exponent factor for Time over threshold
   Double_t     fTotThres;          ///< threshold factor for Time over threshold
   
   Int_t        fStripsN;           ///< number of strips
   Float_t      fPitch;             ///< pitch

private:
   void         SetFunctions();
   Int_t        GetAdcValue(Float_t charge);

private:
   static Float_t  fgkThres;        ///< Default threshold value
   static Float_t  fgkFWTH;         ///< FWHT value
   static Float_t  fgDefSmearPos;   ///< Default position smearing value
   static Bool_t   fgSmearFlag;     ///< Smearing value
   static Float_t  fgkPairCreation; ///< Default value for pair creation for Si
   static Float_t  fgkFanoFactor;   ///< Fano factor for Si
   static Float_t  fgkNormFactor;   ///< Normalization factor
};
        

#endif

