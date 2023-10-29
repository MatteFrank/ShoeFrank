#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <utility>
#include <vector>
#include <map>
#include <tuple>

#define NMAX_BO_ID 200
#define NMAX_CH_ID 18

#define WAVEFORMBINS 1024
#define NUMBEROFCHANNELS 18
#define MAXNUMBEROFBOARDS 16
#define PEDESTALSTARTBIN 10
#define PEDESTALSTOPBIN 60

#define AMPLITUDESTARTBIN 61
#define AMPLITUDESTOPBIN  800

#define CHARGESTARTBIN 0
#define CHARGESTOPBIN  1023

#define TIMESTAMPSTARTBIN 61
#define TIMESTAMPSTOPBIN  800
#define DEFAULTCFDTHRESHOLD 0.3

#define SANIFICATION_TS 10000
#define EMPTYTS 1e-2

#define VOLTAGE_TS 0.5
#define CFD_THREHSOLD 0.3

#define NUMBEROFCALIBRATIONPARAMETERS 2
#define NUMBEROFPARTICLES 2

//Parameters for dE-TOF calibration: ADJUST FOR EACH CAMPAIGN!!
#define LAYERXBARMIN 20
#define LAYERXBARMAX 39
#define LAYERYBARMIN 0
#define LAYERYBARMAX 19
#define QMIN 0.
#define QMAX 50. //[V*ns]
#define QBINS 300
#define TOFMIN 0. //[ns]
#define TOFMAX 50. //[ns]
#define TOFBINS 5000
#define DEMIN 0. //[MeV]
#define DEMAX 100. //[MeV]

enum ParticleType {None=-1,Proton,Helium,Carbon,Oxygen};

static std::map<ParticleType,std::string> ParticleName={{None,"None"},{Proton,"Proton"},{Helium,"Helium"},{Carbon,"Carbon"},{Oxygen,"Oxygen"}};

enum TLayer {NoLayer=-1,LayerX=1,LayerY=0};  // LayerX--> horizontal bars - front r layer, LayerY--> vertical bars - rear layer

static std::map<TLayer,std::string> LayerName={{LayerX,"LayerX"},{LayerY,"LayerY"}};

// TW parameters
enum TWparam
  {
   nLayers = 2,
   nCentralBars = 3,
   nBars = 40,
   nBarsPerLayer = nBars/nLayers,              // 20
   nBarCross = nBarsPerLayer*nBarsPerLayer    // 400
  };

const Int_t   CentralBarsID[nCentralBars] = {8,9,10}; // for both the layers

#endif
