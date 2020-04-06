#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <utility>
#include <vector>
#include <map>
#include <tuple>

#define WAVEFORMBINS 1024
#define NUMBEROFCHANNELS 18
#define MAXNUMBEROFBOARDS 16
#define NUMBEROFBARS 40
#define NUMBEROFLAYERS 2
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
typedef Int_t TChannelId;
typedef Int_t TBarId;
typedef Int_t TBoardId;

#define VOLTAGE_TS 0.5
#define CFD_THREHSOLD 0.3

#define NUMBEROFCALIBRATIONPARAMETERS 2
#define NUMBEROFPARTICLES 2

enum ParticleType {None=-1,Proton=0, Carbon=1};

static std::map<int,std::string> ParticleName={{-1,"None"},{0,"Proton"},{1,"Carbon"}};

static std::map<int,std::string> LayerName={{1,"Front"},{0,"Rear"}};


#endif
