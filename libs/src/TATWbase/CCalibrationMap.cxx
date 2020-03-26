#include "TError.h"

#include "CCalibrationMap.hxx"
#include "XmlParser.hxx"
#include "GlobalPar.hxx"


CCalibrationMap::CCalibrationMap(): fCalibrationMapIsOk(false)
{


}

void CCalibrationMap::LoadCalibrationMap(std::string FileName, int verbose)
{
	if (gSystem->AccessPathName(FileName.c_str()))
	{
      Error("LoadCalibrationMap()","File %s doesn't exist",FileName.c_str());
	}
	// reset channel map
	XmlParser x;
	x.ReadFile(FileName);
	std::vector<XMLNodePointer_t> BarVector;
	BarVector=x.GetChildNodesByName(x.GetMainNode(),"BAR");
   if (verbose > 0) {
      Info("LoadCalibrationMap()", " => Calibration ");
      Info("LoadCalibrationMap()", " Description: %s", x.GetContentAsString("DESCRIPTION",x.GetMainNode()).data());
      Info("LoadCalibrationMap()", " Creation date: %s", x.GetContentAsString("DATE",x.GetMainNode()).data());
   }
	for (std::vector<XMLNodePointer_t>::iterator it=BarVector.begin();it!=BarVector.end();++it)
	{
		Int_t BarId=x.GetContentAsInt("BAR_ID",*it);
		Double_t p0=x.GetContentAsDouble("P0",*it);
		Double_t p1=x.GetContentAsDouble("P1",*it);
		// the same bar should not appear twice
		if (fCalibrationMap.count(BarId)!=0)
		{
			Error("LoadCalibrationMap()","Calibration Map is malformed: BAR %d appear twice",BarId);
		}
		fCalibrationMap[BarId].push_back(p0);
		fCalibrationMap[BarId].push_back(p1);
      if (verbose > 0)
         Info("LoadCalibrationMap()", "BAR_ID %d  par0 %f par1 %f ",BarId,p0,p1);
	}
}

void CCalibrationMap::ExportToFile(std::string FileName)
{
	XmlParser x;
	XMLNodePointer_t main=x.CreateMainNode("CALIBRATION");
	x.AddElementWithContent(TString::Format("DATE").Data(),main," ");
	x.AddElementWithContent(TString::Format("DESCRIPTION").Data(),main," ");
	for (auto it=fCalibrationMap.begin();it!=fCalibrationMap.end();++it)
	{
		Int_t BarId=it->first;
		XMLNodePointer_t b=x.AddElement(TString::Format("BAR").Data(),main);
		x.AddElementWithContent("BAR_ID",b, TString::Format("%d",BarId).Data());
		x.AddElementWithContent("P0",b,TString::Format("%f",fCalibrationMap[BarId][0]).Data());
		x.AddElementWithContent("P1",b,TString::Format("%f",fCalibrationMap[BarId][1]).Data());
	}
	x.ExportToFile(FileName,main);
}

bool CCalibrationMap::Exists(Int_t BarId)
{
	if (fCalibrationMap.count(BarId)==0)
	{
		return false;
	}
	return true;
}

TCalibrationMapType::iterator CCalibrationMap::begin()
{
	return fCalibrationMap.begin();
}

TCalibrationMapType::iterator CCalibrationMap::end()
{
	return fCalibrationMap.end();
}


Int_t CCalibrationMap::GetNumberOfBars()
{
	return fCalibrationMap.size();
}

Double_t CCalibrationMap::GetBarParameter(Int_t BarId, UInt_t ParameterNumber)
{
	return fCalibrationMap[BarId][ParameterNumber];
}
void CCalibrationMap::AddBar(Int_t BarId)
{

	fCalibrationMap[BarId].resize(NUMBEROFCALIBRATIONPARAMETERS);
}

void CCalibrationMap::SetBarParameter(Int_t BarId, UInt_t ParameterNumber, Double_t p)
{
	fCalibrationMap[BarId][ParameterNumber]=p;
}
