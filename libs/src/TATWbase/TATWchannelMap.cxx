
/*!
 \file TATWchannelMap.cxx
 \brief   Implementation of TATWcalibrationMap.
 */

#include "TError.h"

#include "TATWchannelMap.hxx"
#include "TAGxmlParser.hxx"
#include "TAGrecoManager.hxx"

/*!
 \class TATWchannelMap
 \brief mapping class for TW
 */

//! Class Imp
ClassImp(TATWchannelMap)

TATWchannelMap::TATWchannelMap()
: TAGobject(),
  fChannelMapIsOk(false)
{
}

void TATWchannelMap::LoadChannelMap(std::string FileName)
{
	if (gSystem->AccessPathName(FileName.c_str()))
	{
		Error("LoadChannelMap()", "File %s doesn't exist",FileName.c_str());
	}
	// reset channel map
	fChannelBarMap.clear();
	// parse xml
	TAGxmlParser x;
	x.ReadFile(FileName);
	// create a verctor containing the "BAR" nodes
	std::vector<XMLNodePointer_t> BarVector=x.GetChildNodesByName(x.GetMainNode(),"BAR");
	// print some info about the channel map
   if (FootDebugLevel(1)) {
      Info("LoadChannelMap()"," => Channel map ");
      Info("LoadChannelMap()"," Description: %s", x.GetContentAsString("DESCRIPTION",x.GetMainNode()).data());
      Info("LoadChannelMap()"," Creation date: %s", x.GetContentAsString("DATE",x.GetMainNode()).data());
   }
   
	for (std::vector<XMLNodePointer_t>::iterator it=BarVector.begin();it!=BarVector.end();++it)
	{
		Int_t BarId=x.GetContentAsInt("BAR_ID",*it);
		TLayer  Layer=(TLayer)x.GetContentAsInt("LAYER",*it);
		Int_t ChannelA=x.GetContentAsInt("CHANNEL_A",*it);
		Int_t ChannelB=x.GetContentAsInt("CHANNEL_B",*it);
		Int_t BoardId=x.GetContentAsInt("BOARD_ID",*it);
		// the same bar should not appear twice
		if (fChannelBarMap.count(BarId)!=0)
		{
			Error("LoadChannelMap()", "Channel Map is malformed: BAR %d appear twice",BarId);
		}
		fChannelBarMap[BarId]=TChannelBoardTuple(BoardId,ChannelA,ChannelB);
		fBarLayer[BarId]=Layer;
      if (FootDebugLevel(1))
         Info("LoadChannelMap()","BAR_ID %d BOARD ID %d Channel A %d Channel B %d Layer Id %d",BarId,BoardId,ChannelA,ChannelB,Layer);
	}
	// check if the same combination board,channel appears twice in the map
	std::map<std::pair<Int_t,Int_t>, Int_t> ChannelOccurrenceMap;
	for (TChannelPairMapType::iterator it=fChannelBarMap.begin();it!=fChannelBarMap.end();++it)
	{
		Int_t BoardId=it->first;
		Int_t ChannelA=std::get<1>(it->second);
		Int_t ChannelB=std::get<2>(it->second);
		std::pair<Int_t,Int_t> pair1 (BoardId,ChannelA);
		std::pair<Int_t,Int_t> pair2 (BoardId,ChannelB);

		if (ChannelOccurrenceMap.count(pair1)!=0)
		{
			Error("LoadChannelMap()", "Combination Board %d Channel %d appears twice in the channel map",BoardId,ChannelA);
		}
		ChannelOccurrenceMap[pair1]=1;
		if (ChannelOccurrenceMap.count(pair2)!=0)
		{
			Error("LoadChannelMap()", "Combination Board %d Channel %d appears twice in the channel map",BoardId,ChannelB);
		}
		ChannelOccurrenceMap[pair2]=1;
	}
	// check if the number of bar found is bigger than nSlats found in Parameters.h
	Int_t ActualNumberOfBars=fChannelBarMap.size();
	if (ActualNumberOfBars>nSlats)
	{
		Error("LoadChannelMap()", "Too many bars in channel map. Found %d expected %d",ActualNumberOfBars,nSlats);
	}
	fChannelMapIsOk=true;
}

bool TATWchannelMap::Exists(Int_t BarId)
{
	if (fChannelBarMap.count(BarId)==0)
	{
		return false;
	}
	return true;
}

std::vector<Int_t> TATWchannelMap::GetBarIds()
{
	std::vector<Int_t> v;
	for(auto it = fChannelBarMap.begin(); it != fChannelBarMap.end(); ++it)
	{
		v.push_back(it->first);
	}
	return v;
}

