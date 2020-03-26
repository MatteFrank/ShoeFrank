#include "TError.h"

#include "CChannelMap.hxx"
#include "XmlParser.hxx"

CChannelMap::CChannelMap():_ChannelMapIsOk(false)
{

}

TLayer CChannelMap::GetBarLayer(TBarId BarId)
{
	return _BarLayer[BarId];
}

TChannelBoardTuple CChannelMap::GetChannelABar(TBarId BarId)
{
	return _ChannelBarMap[BarId];
}

void CChannelMap::LoadChannelMap(std::string FileName,int verbose)
{
	if (gSystem->AccessPathName(FileName.c_str()))
	{
		Error("LoadChannelMap()", "File %s doesn't exist",FileName.c_str());
	}
	// reset channel map
	_ChannelBarMap.clear();
	// parse xml
	XmlParser x;
	x.ReadFile(FileName);
	// create a verctor containing the "BAR" nodes
	std::vector<XMLNodePointer_t> BarVector=x.GetChildNodesByName(x.GetMainNode(),"BAR");
	// print some info about the channel map
	Info("LoadChannelMap()"," => Channel map ");
	Info("LoadChannelMap()"," Description: %s", x.GetContentAsString("DESCRIPTION",x.GetMainNode()).data());
   Info("LoadChannelMap()"," Creation date: %s", x.GetContentAsString("DATE",x.GetMainNode()).data());
	//
	for (std::vector<XMLNodePointer_t>::iterator it=BarVector.begin();it!=BarVector.end();++it)
	{
		TBarId BarId=x.GetContentAsInt("BAR_ID",*it);
		TLayer  Layer=(TLayer)x.GetContentAsInt("LAYER",*it);
		TChannelId ChannelA=x.GetContentAsInt("CHANNEL_A",*it);
		TChannelId ChannelB=x.GetContentAsInt("CHANNEL_B",*it);
		TChannelId BoardId=x.GetContentAsInt("BOARD_ID",*it);
		// the same bar should not appear twice
		if (_ChannelBarMap.count(BarId)!=0)
		{
			Error("LoadChannelMap()", "Channel Map is malformed: BAR %d appear twice",BarId);
		}
		_ChannelBarMap[BarId]=TChannelBoardTuple(BoardId,ChannelA,ChannelB);
		_BarLayer[BarId]=Layer;
		Info("LoadChannelMap()","BAR_ID %d BOARD ID %d Channel A %d Channel B %d Layer Id %d",BarId,BoardId,ChannelA,ChannelB,Layer);
	}
	// check if the same combination board,channel appears twice in the map
	std::map<std::pair<Int_t,Int_t>, Int_t> ChannelOccurrenceMap;
	for (TChannelPairMapType::iterator it=_ChannelBarMap.begin();it!=_ChannelBarMap.end();++it)
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
	// check if the number of bar found is bigger than NUMBEROFBARS found in Parameters.h
	Int_t ActualNumberOfBars=_ChannelBarMap.size();
	if (ActualNumberOfBars>NUMBEROFBARS)
	{
		Error("LoadChannelMap()", "Too many bars in channel map. Found %d expected %d",ActualNumberOfBars,NUMBEROFBARS);
	}
	_ChannelMapIsOk=true;
}

bool CChannelMap::Exists(TBarId BarId)
{
	if (_ChannelBarMap.count(BarId)==0)
	{
		return false;
	}
	return true;
}

TChannelPairMapType::iterator CChannelMap::begin()
{
	return _ChannelBarMap.begin();
}


TChannelPairMapType::iterator CChannelMap::end()
{
	return _ChannelBarMap.end();
}

Int_t CChannelMap::GetNumberOfBars()
{
	return _ChannelBarMap.size();
}

std::vector<TBarId> CChannelMap::GetBarIds()
{
	std::vector<TBarId> v;
	for(auto it = _ChannelBarMap.begin(); it != _ChannelBarMap.end(); ++it)
	{
		v.push_back(it->first);
	}
	return v;
}

