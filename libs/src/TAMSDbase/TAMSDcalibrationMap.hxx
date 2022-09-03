#ifndef TAMSDCALIBRATIONMAP_H
#define TAMSDCALIBRATIONMAP_H

/*!
 \file TAMSDcalibrationMap.hxx
 \brief   Declaration of TAMSDcalibrationMap.
 */
/*------------------------------------------+---------------------------------*/

#include <map>
#include <TSystem.h>
#include <TString.h>
#include "TAGobject.hxx"

using namespace std;

class TAMSDcalibrationMap : public TAGobject
{
public:
    //! \struct PedParameter_t
    struct PedParameter_t {
        double mean;   ///< mean value
        double sigma;  ///< sigma value
        bool   status; ///< status
    };
   
    //! \struct ElossParameter_t
    struct ElossParameter_t {
        vector<double> eta;         ///< eta vector
        vector<double> correction;  ///< correction vector
    };

private:
   vector<Double_t>         fSigmaNoiseLevelSeed; ///< Sigma noise level seed vector
   vector<Double_t>         fSigmaNoiseLevelHit;  ///< Sigma noise level hit vector
   vector<PedParameter_t>   fPedestal;            ///< pedestal vector
   ElossParameter_t         fEloss;               ///< Energy loss structure
   Int_t                    fStripsN;             ///< Number of strips

public:
  TAMSDcalibrationMap(int strip_number_p);
    
  void                      LoadEnergyCalibrationMap(TString Filename);
  void                      LoadPedestalMap(TString Filename);
  Double_t                  GetElossParam(Float_t eta);

  //! Get energy loss parameters
  ElossParameter_t          GetElossParameters()                             { return fEloss;                                 }
  //! Get pedestal parameters
  //! \param[in] sensorId plane id
  //! \param[in] stripId strip id
  PedParameter_t            GetPedestal(Int_t sensorId, Int_t stripId)       { return fPedestal[sensorId*fStripsN + stripId]; }
  //! Get pedestal seed level
  //! \param[in] sensorId plane id
  Double_t                  GetPedestalSeedLevel(Int_t sensorId)             { return fSigmaNoiseLevelSeed[sensorId];         }
  //! Get pedestal hit level
  //! \param[in] sensorId plane id
  Double_t                  GetPedestalHitLevel(Int_t sensorId)              { return fSigmaNoiseLevelHit[sensorId];          }
   
  ClassDef(TAMSDcalibrationMap, 0)
};

#endif
