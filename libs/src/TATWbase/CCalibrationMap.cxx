#include "CCalibrationMap.hxx"
#include "Message.hxx"
#include "XmlParser.hxx"


CCalibrationMap::CCalibrationMap(): _CalibrationMapIsOk(false)
{


}

void CCalibrationMap::LoadCalibrationMap(std::string FileName, int verbose)
{
	if (gSystem->AccessPathName(FileName.c_str()))
	{
		Message::DisplayFatalError(TString::Format("File %s doesn't exist",FileName.c_str()));
	}
	// reset channel map
	XmlParser x;
	x.ReadFile(FileName);
	std::vector<XMLNodePointer_t> BarVector;
	BarVector=x.GetChildNodesByName(x.GetMainNode(),"BAR");
	Message::DisplayMessageWithEmphasys(" Calibration ");
	Message::DisplayMessage(" Description: "+x.GetContentAsString("DESCRIPTION",x.GetMainNode()));
	Message::DisplayMessage(" Creation date: "+x.GetContentAsString("DATE",x.GetMainNode()));
	for (std::vector<XMLNodePointer_t>::iterator it=BarVector.begin();it!=BarVector.end();++it)
	{
		TBarId BarId=x.GetContentAsInt("BAR_ID",*it);
		Double_t p0=x.GetContentAsDouble("P0",*it);
		Double_t p1=x.GetContentAsDouble("P1",*it);
		// the same bar should not appear twice
		if (_CalibrationMap.count(BarId)!=0)
		{
			Message::DisplayFatalError(TString::Format("Calibration Map is malformed: BAR %d appear twice",BarId));
		}
		_CalibrationMap[BarId].push_back(p0);
		_CalibrationMap[BarId].push_back(p1);
		Message::DisplayMessage(TString::Format("BAR_ID %d  par0 %f par1 %f ",BarId,p0,p1),std::cout,verbose);
	}
}

void CCalibrationMap::ExportToFile(std::string FileName)
{
	XmlParser x;
	XMLNodePointer_t main=x.CreateMainNode("CALIBRATION");
	x.AddElementWithContent(TString::Format("DATE").Data(),main," ");
	x.AddElementWithContent(TString::Format("DESCRIPTION").Data(),main," ");
	for (auto it=_CalibrationMap.begin();it!=_CalibrationMap.end();++it)
	{
		TBarId BarId=it->first;
		XMLNodePointer_t b=x.AddElement(TString::Format("BAR").Data(),main);
		x.AddElementWithContent("BAR_ID",b, TString::Format("%d",BarId).Data());
		x.AddElementWithContent("P0",b,TString::Format("%f",_CalibrationMap[BarId][0]).Data());
		x.AddElementWithContent("P1",b,TString::Format("%f",_CalibrationMap[BarId][1]).Data());
	}
	x.ExportToFile(FileName,main);
}

bool CCalibrationMap::Exists(TBarId BarId)
{
	if (_CalibrationMap.count(BarId)==0)
	{
		return false;
	}
	return true;
}

TCalibrationMapType::iterator CCalibrationMap::begin()
{
	return _CalibrationMap.begin();
}

TCalibrationMapType::iterator CCalibrationMap::end()
{
	return _CalibrationMap.end();
}


Int_t CCalibrationMap::GetNumberOfBars()
{
	return this->_CalibrationMap.size();
}

CalibrationParType CCalibrationMap::GetBarParameter(TBarId BarId,unsigned int ParameterNumber)
{
	return _CalibrationMap[BarId][ParameterNumber];
}
void CCalibrationMap::AddBar(TBarId BarId)
{

	_CalibrationMap[BarId].resize(NUMBEROFCALIBRATIONPARAMETERS);
}

void CCalibrationMap::SetBarParameter(TBarId BarId,unsigned int ParameterNumber,CalibrationParType p)
{
	_CalibrationMap[BarId][ParameterNumber]=p;
}
