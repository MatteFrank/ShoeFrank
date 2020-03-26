#ifndef CCHANNELMAP_H
#define CCHANNELMAP_H

#include <string>
#include <map>
#include <TSystem.h>
#include "Parameters.h"

enum TLayer {NoLayer=-1,Front=0,Rear=1};

typedef std::map<Int_t,TLayer> TMapBarIdLayerId;
typedef std::tuple <Int_t,Int_t,Int_t> TChannelBoardTuple;
typedef std::map<Int_t,TChannelBoardTuple> TChannelPairMapType;


class CChannelMap
{
private:
	TMapBarIdLayerId fBarLayer;
   TChannelPairMapType fChannelBarMap;
   bool fChannelMapIsOk;
   
public:
   CChannelMap();
   bool Exists(Int_t BarId);
	void LoadChannelMap(std::string Filename, Int_t verbose=0);
	TLayer GetBarLayer(Int_t BarId);
	TChannelBoardTuple GetChannelABar(Int_t BarId);
	TChannelPairMapType::iterator begin();
	TChannelPairMapType::iterator end();
	Int_t GetNumberOfBars();
	std::vector<Int_t> GetBarIds();
};
#endif
