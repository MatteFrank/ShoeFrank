#ifndef CCHANNELMAP_H
#define CCHANNELMAP_H

#include <string>
#include <map>
#include <TSystem.h>
#include "TAGobject.hxx"
#include "Parameters.h"

enum TLayer {NoLayer=-1,Front=0,Rear=1};

typedef std::map<Int_t,TLayer> TMapBarIdLayerId;
typedef std::tuple <Int_t,Int_t,Int_t> TChannelBoardTuple;
typedef std::map<Int_t,TChannelBoardTuple> TChannelPairMapType;


class TATWchannelMap : public TAGobject
{
private:
	TMapBarIdLayerId fBarLayer;
   TChannelPairMapType fChannelBarMap;
   bool fChannelMapIsOk;
   
public:
   TATWchannelMap();
   bool Exists(Int_t BarId);
	void LoadChannelMap(std::string Filename);
   std::vector<Int_t> GetBarIds();

   TLayer             GetBarLayer(Int_t BarId)    { return fBarLayer[BarId];       }
   TChannelBoardTuple GetChannelABar(Int_t BarId) { return fChannelBarMap[BarId];  }
   TChannelPairMapType::iterator begin()          { return fChannelBarMap.begin(); }
   TChannelPairMapType::iterator end()            { return fChannelBarMap.end();   }
   Int_t GetNumberOfBars()                  const { return fChannelBarMap.size();  }
   
   ClassDef(TATWchannelMap, 0)
};

#endif
