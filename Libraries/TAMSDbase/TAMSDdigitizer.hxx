#ifndef _TAMSDdigitizer_HXX
#define _TAMSDdigitizer_HXX

/*!
 \file TAMSDdigitizer.hxx
 \brief Declaration of TAMSDdigitizer
 */
/*------------------------------------------+---------------------------------*/

#include <map>

#include "TAGbaseDigitizer.hxx"

// --------------------------------------------------------------------------------------
class TAMSDparGeo;
class TF1;
class TAMSDdigitizer : public TAGbaseDigitizer {
   
   // Class to digitize the energy into strips based on given patterns
public:
   TAMSDdigitizer(TAMSDparGeo* parGeo);
   virtual ~TAMSDdigitizer();

   void  FillMap(Int_t strip, Double_t value);
   Bool_t Process( Double_t edep, Double_t x0, Double_t y0, Double_t zin = 0, Double_t zout = 0, Double_t time = 0, Int_t sensorId = 0, Int_t Z = -99, Double_t px0 = 0, Double_t py0 = 0, Double_t pz0 = 0);
   
   Int_t GetStrip(Float_t pos) const;

   Double_t       EtaX0(Double_t* x, Double_t* par);
   Double_t       GetEta(Double_t pos);

   Double_t       ResEnergy(Double_t* x, Double_t* par);
   Double_t       GetResEnergy(Float_t edep);
   
   void           SetFunctions();
   void           SetParFunction();

   
private:
   TAMSDparGeo*    fpParGeo;       ///< geometry parameter
   TF1*            fFuncEta;       ///< eta function
   Int_t           fStripsN;       ///< number of strips for a given eloss
   Float_t         fPitch;         ///< pitch
   Int_t           fView;          ///< plane view
   
   Double_t        fEtaLimLo;      ///< eta limit low parameter
   Double_t        fEtaLimUp;      ///< eta limit up parameter
   Float_t         fEtaCst;        ///< eta constant parameter
   Float_t         fEtaErrCst;     ///< eta constant error parameter
   
   TF1*            fDeResE;        ///< energy loss
   Float_t         fResPar0;       ///< resolution parameter 0
   Float_t         fResErrPar0;    ///< resolution error parameter 0
   Float_t         fResPar1;       ///< resolution parameter 1
   Float_t         fResErrPar1;    ///< resolution error parameter 1
   Float_t         fResPar2;       ///< resolution parameter 2
   Float_t         fResErrPar2;    ///< resolution error parameter 2
   
   std::map<int, double> fMap;     ///< map of found strips

private:
   static Float_t  fgChargeGain;   ///< gain factor for despoted charge
   static Float_t  fgDefSmearPos;  ///< default value for smearing position
   static Bool_t   fgSmearFlag;    ///< smearing flag

public:
   //! Get number of strips
   Int_t                  GetStripsN() const { return fStripsN; }
   //! Get pitch
   Int_t                  GetPitch()   const { return fPitch;   }
   //! Get plane view
   Int_t                  GetView()    const { return fView;    }
   //! Get map
   std::map<int, double>  GetMap()     const { return fMap;     }

public:
   //! Get gain charge
   static Float_t GetChargeGain()                   { return fgChargeGain;       }
   //! Set gain charge
   static void    SetChargeGain(Float_t gain)       { fgChargeGain = gain;       }
   //! Get smearing flag
   static Bool_t  GetSmearFlag()                    { return fgSmearFlag;        }
   //! Set smearing flag
   static void    SetSmearFlag(Bool_t flag)         { fgSmearFlag = flag;        }
   //! Get smearing position value
   static Float_t GetDefSmearPos()                  { return fgDefSmearPos;      }
   //! Set smearing position value
   static void    SetDefSmearPos(Float_t pos)       { fgDefSmearPos = pos;       }
};

#endif

