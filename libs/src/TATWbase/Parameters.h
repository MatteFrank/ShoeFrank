#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <utility>
#include <vector>
#include <map>
#include <tuple>

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

enum ParticleType {None=-1,Proton,Helium,Carbon,Oxygen};

static std::map<ParticleType,std::string> ParticleName={{None,"None"},{Proton,"Proton"},{Helium,"Helium"},{Carbon,"Carbon"},{Oxygen,"Oxygen"}};

enum TLayer {NoLayer=-1,LayerX=1,LayerY=0};  // layer1--> horizontal bars, layer0--> vertical bars

static std::map<TLayer,std::string> LayerName={{LayerX,"LayerX"},{LayerY,"LayerY"}};

// TW parameters
enum TWparam
  {
   nLayers = 2,
   nSlats = 40,
   nSlatsPerLayer = nSlats/nLayers,              // 20
   nSlatCross = nSlatsPerLayer*nSlatsPerLayer    // 400
  };

#endif
