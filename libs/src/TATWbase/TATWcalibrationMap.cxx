#include "TError.h"

#include "TATWcalibrationMap.hxx"
#include "TAGxmlParser.hxx"
#include "TAGrecoManager.hxx"

ClassImp(TATWcalibrationMap)

TATWcalibrationMap::TATWcalibrationMap()
: TAGobject(),
  fCalibrationMapIsOk(false)
{
}

//_____________________________________________________________________

void TATWcalibrationMap::LoadEnergyCalibrationMap(std::string FileName)
{

  if (gSystem->AccessPathName(FileName.c_str()))
    {
    Error("LoadEnergyCalibrationMap()","File %s doesn't exist",FileName.c_str());
    }	

  ///////// read the file with Charge calibration

  ifstream fin_Q;
  fin_Q.open(FileName,std::ifstream::in);
  
  int posQ[nSlatCross*nLayers];  // Id of the bars crossing (0-399)
  int layQ[nSlatCross*nLayers];  // layer (0 for Ybars and 1 for Xbars)
  int barX[nSlatCross*nLayers];  // Id of the bars in the layerX
  int barY[nSlatCross*nLayers];  // Id of the bars in the layerY

  // Birks parameters for energy calibration p0 and p1
  double Q_corrp0[nSlatCross*nLayers], Q_corrp1[nSlatCross*nLayers]; 


  if(fin_Q.is_open()){

    int  cnt(0);
    char line[200];
    
    // loop over all the slat crosses ( nSlatCross*nLayers ) for two TW layers
    while (fin_Q.getline(line, 200, '\n')) {

      if(strchr(line,'#')) {
	if(FootDebugLevel(1))
	  Info("LoadEnergyCalibrationMap()","Skip comment line:: %s\n",line);
	continue;
      }

      sscanf(line, "%d %d %d %lf %lf %d",&posQ[cnt],&barX[cnt],&barY[cnt],&Q_corrp0[cnt],&Q_corrp1[cnt],&layQ[cnt]);
      if(FootDebugLevel(1))
	Info("LoadEnergyCalibrationMap()","%d %d %d %.5f %.7f %d\n",posQ[cnt],barX[cnt],barY[cnt],Q_corrp0[cnt],Q_corrp1[cnt],layQ[cnt]);
      
      cnt++;
    }

  }
  else
    Info("LoadEnergyCalibrationMap()","File Calibration Energy %s not open!!",FileName.data());

  fin_Q.close();

  
  // store in a Map the two Birks parameter p0 and p1 for energy calibration with key their position Id (Id of the bars crossing)  
  for(int i=0; i<nSlatCross*nLayers; i++) {

    if(layQ[i]==LayerX){

      fCalibElossMapLayX[posQ[i]].push_back(Q_corrp0[i]);
      fCalibElossMapLayX[posQ[i]].push_back(Q_corrp1[i]);

      if(FootDebugLevel(1))
	cout<<"Position: "<<posQ[i]<<" Layer: "<<layQ[i]<<" P0: "<<fCalibElossMapLayX[posQ[i]][0]<<" P1: "<<fCalibElossMapLayX[posQ[i]][1]<<endl;

    } else if (layQ[i]==LayerY) {

      fCalibElossMapLayY[posQ[i]].push_back(Q_corrp0[i]);
      fCalibElossMapLayY[posQ[i]].push_back(Q_corrp1[i]);

      if(FootDebugLevel(1))
	cout<<"Position: "<<posQ[i]<<" Layer: "<<layQ[i]<<" P0: "<<fCalibElossMapLayY[posQ[i]][0]<<" P1: "<<fCalibElossMapLayY[posQ[i]][1]<<endl;
      
    }
    else
      Error("LoadEnergyCalibrationMap()","Layer %d doesn't exist",layQ[i]);
    
  }

}

//_____________________________________________________________________

void TATWcalibrationMap::LoadTofCalibrationMap(std::string FileName)
{
  if (gSystem->AccessPathName(FileName.c_str()))
    {
      Error("LoadTofCalibrationMap()","File %s doesn't exist",FileName.c_str());
    }
  
  
  /////////// read the file with TOF corrections
  ifstream fin_TOF;
  fin_TOF.open(FileName,std::ifstream::in);
  
  int posT[nSlatCross*nLayers];  // Id of the bars crossing (0-399)
  int layT[nSlatCross*nLayers];  // layer (0 for Ybars and 1 for Xbars)
  int barX[nSlatCross*nLayers];  // Id of the bars in the layerX
  int barY[nSlatCross*nLayers];  // Id of the bars in the layerY
  
  // Delta Time parameters for Tof calibration DeltaT1 and DeltaT2
  double deltaT[nSlatCross*nLayers], deltaT2[nSlatCross*nLayers];
  
  if(fin_TOF.is_open()) {
    
    int  cnt(0);
    char line[200];

    // loop over all the slat crosses ( nSlatCross*nLayers ) for two TW layers
    while (fin_TOF.getline(line, 200, '\n')) {
      
      if(strchr(line,'#')) {
	if(FootDebugLevel(1))
	  Info("LoadTofCalibrationMap()","Skip comment line:: %s\n",line);
	continue;
      }
      
      sscanf(line, "%d %d %d %lf %lf %d",&posT[cnt],&barX[cnt],&barY[cnt],&deltaT[cnt],&deltaT2[cnt],&layT[cnt]);
      if(FootDebugLevel(1))
	Info("LoadTofCalibrationMap()","%d %d %d %.6f %.6f %d\n",posT[cnt],barX[cnt],barY[cnt],deltaT[cnt],deltaT2[cnt],layT[cnt]);
      
      cnt++;
    }
  }

  else
    Info("LoadTofCalibrationMap()","File Calibration Energy %s not open!!",FileName.data());
  
  fin_TOF.close();
  
  
    // store in a Map the two Delta Time parameter DeltaT1 and DeltaT2 for energy calibration with key their Bar Id (Id of the bars in Pisa notation)
  for(int i=0; i<nSlatCross*nLayers; i++) {
    
    if(layT[i]==LayerX){
      
      fCalibTofMapLayX[posT[i]].push_back(deltaT[i]);
      fCalibTofMapLayX[posT[i]].push_back(deltaT2[i]);
      
      if(FootDebugLevel(1))
	cout<<"Position: "<<posT[i]<<" Layer: "<<layT[i]<<" q: "<<fCalibTofMapLayX[posT[i]][0]<<" m: "<<fCalibTofMapLayX[posT[i]][1]<<endl;

    } else if (layT[i]==LayerY){
      
      fCalibTofMapLayY[posT[i]].push_back(deltaT[i]);
      fCalibTofMapLayY[posT[i]].push_back(deltaT2[i]);

    if(FootDebugLevel(1))
      cout<<"Position: "<<posT[i]<<" Layer: "<<layT[i]<<" q: "<<fCalibTofMapLayY[posT[i]][0]<<" m: "<<fCalibTofMapLayY[posT[i]][1]<<endl;

    }
    else
      Error("LoadTofCalibrationMap()","Layer %d doesn't exist",layT[i]);
    
    
  }

}

//_____________________________________________________________________

void TATWcalibrationMap::LoadBarEnergyCalibrationMap(std::string FileName)
{
  if (gSystem->AccessPathName(FileName.c_str()))
    {
      Error("LoadBarEnergyCalibrationMap()","File %s doesn't exist",FileName.c_str());
    }
   
    ///////// read the file with Charge calibration

    ifstream fileBarE;
    fileBarE.open(FileName,std::ifstream::in);
    
    int BarId[nSlats];  // Id of the bars (Pisa notation 0-40)
    int lay[nSlats];  // layer (0 for Ybars and 1 for Xbars)

    // Birks parameters for energy calibration p0 and p1
    double P0[nSlats], P1[nSlats];


    if(fileBarE.is_open()){

      int  cnt(0);
      char line[200];
      
      // loop over all the slats (0-40) of the two TW layers
      while (fileBarE.getline(line, 200, '\n')) {

        if(strchr(line,'#')) {
      if(FootDebugLevel(1))
        Info("LoadBarEnergyCalibrationMap()","Skip comment line:: %s\n",line);
      continue;
        }

        sscanf(line, "%d %lf %lf %d",&BarId[cnt],&P0[cnt],&P1[cnt],&lay[cnt]);
        if(FootDebugLevel(1))
	  Info("LoadBarEnergyCalibrationMap()","%d %.5f %.7f %d\n",BarId[cnt],P0[cnt],P1[cnt],lay[cnt]);
        
        cnt++;
      }

    }
    else
      Info("LoadBarEnergyCalibrationMap()","File Bar Calibration Energy %s not open!!",FileName.data());

    fileBarE.close();

    
    // store in a Map the two Birks parameter p0 and p1 for energy calibration with key their Bar Id (Id of the bars in Pisa notation)
    for(int i=0; i<nSlats; i++) {

      if(lay[i]==LayerX){

        fCalibElossBarMapLayX[BarId[i]].push_back(P0[i]);
        fCalibElossBarMapLayX[BarId[i]].push_back(P1[i]);

        if(FootDebugLevel(1))
	  cout<<"BarId: "<<BarId[i]<<" Layer: "<<lay[i]<<" P0: "<<fCalibElossBarMapLayX[BarId[i]][0]<<" P1: "<<fCalibElossBarMapLayX[BarId[i]][1]<<endl;
	
      } else if (lay[i]==LayerY) {
	
        fCalibElossBarMapLayY[BarId[i]].push_back(P0[i]);
        fCalibElossBarMapLayY[BarId[i]].push_back(P1[i]);
	
        if(FootDebugLevel(1))
	  cout<<"BarId: "<<BarId[i]<<" Layer: "<<lay[i]<<" P0: "<<fCalibElossBarMapLayY[BarId[i]][0]<<" P1: "<<fCalibElossBarMapLayY[BarId[i]][1]<<endl;
        
      }
      else
        Error("LoadBarEnergyCalibrationMap()","Layer %d doesn't exist",lay[i]);
      
    }
}

void TATWcalibrationMap::LoadBarTofCalibrationMap(std::string FileName)
{
  if (gSystem->AccessPathName(FileName.c_str()))
    {
      Error("LoadBarTofCalibrationMap()","File %s doesn't exist",FileName.c_str());
    }
   
    ///////// read the file with Charge calibration

    ifstream fileBarT;
    fileBarT.open(FileName,std::ifstream::in);
    
    int BarId[nSlats];  // Id of the bars (Pisa notation 0-40)
    int lay[nSlats];  // layer (0 for Ybars and 1 for Xbars)

    // Delta Time parameters for Tof calibration DeltaT1 and DeltaT2
    double DeltaT1[nSlats], DeltaT2[nSlats];


    if(fileBarT.is_open()){

      int  cnt(0);
      char line[200];
      
      // loop over all the slats (0-40) of the two TW layers
      while (fileBarT.getline(line, 200, '\n')) {

        if(strchr(line,'#')) {
      if(FootDebugLevel(1))
        Info("LoadBarTofCalibrationMap()","Skip comment line:: %s\n",line);
      continue;
        }

        sscanf(line, "%d %lf %lf %d",&BarId[cnt],&DeltaT1[cnt],&DeltaT2[cnt],&lay[cnt]);
        if(FootDebugLevel(1))
	  Info("LoadBarTofCalibrationMap()","%d %.5f %.7f %d\n",BarId[cnt],DeltaT1[cnt],DeltaT2[cnt],lay[cnt]);
        
        cnt++;
      }

    }
    else
      Info("LoadBarTofCalibrationMap()","File Bar Calibration Tof %s not open!!",FileName.data());
    
    fileBarT.close();

    
    // store in a Map the two Delta Time parameter DeltaT1 and DeltaT2 for energy calibration with key their Bar Id (Id of the bars in Pisa notation)
    for(int i=0; i<nSlats; i++) {

      if(lay[i]==LayerX){

        fCalibTofBarMapLayX[BarId[i]].push_back(DeltaT1[i]);
        fCalibTofBarMapLayX[BarId[i]].push_back(DeltaT2[i]);

        if(FootDebugLevel(1))
	  cout<<"BarId: "<<BarId[i]<<" Layer: "<<lay[i]<<" Delta Time 1: "<<fCalibTofBarMapLayX[BarId[i]][0]<<" Delta Time 2: "<<fCalibTofBarMapLayX[BarId[i]][1]<<endl;
	
      } else if (lay[i]==LayerY) {
	
        fCalibTofBarMapLayY[BarId[i]].push_back(DeltaT1[i]);
        fCalibTofBarMapLayY[BarId[i]].push_back(DeltaT2[i]);
	
        if(FootDebugLevel(1))
	  cout<<"BarId: "<<BarId[i]<<" Layer: "<<lay[i]<<" Delta Time 1: "<<fCalibTofBarMapLayY[BarId[i]][0]<<" Delta Time 2: "<<fCalibTofBarMapLayY[BarId[i]][1]<<endl;
        
      }
      else
        Error("LoadBarTofCalibrationMap()","Layer %d doesn't exist",lay[i]);
      
    }
}

void TATWcalibrationMap::LoadBarElossTuning(std::string FileName)
{
  if (gSystem->AccessPathName(FileName.c_str()))
    {
      Error("LoadBarElossTuning()","File %s doesn't exist",FileName.c_str());
    }
   
    ///////// read the file with Charge calibration

    ifstream fileTune;
    fileTune.open(FileName,std::ifstream::in);
    
    int BarId[nSlats];  // Id of the bars (Pisa notation 0-40)
    int lay[nSlats];  // layer (0 for Ybars and 1 for Xbars)

    // A B C parabola parameters for Energy Tuning
    double A[nSlats], B[nSlats], C[nSlats];


    if(fileTune.is_open()){

      int  cnt(0);
      char line[200];
      
      // loop over all the slats (0-40) of the two TW layers
      while (fileTune.getline(line, 200, '\n')) {

        if(strchr(line,'#')) {
      if(FootDebugLevel(1))
        Info("LoadBarElossTuning()","Skip comment line:: %s\n",line);
      continue;
        }
	
        sscanf(line, "%d %lf %lf %lf %d",&BarId[cnt],&A[cnt],&B[cnt],&C[cnt],&lay[cnt]);
        if(FootDebugLevel(1))
	  Info("LoadBarElossTuning()","%d %.5f %.7f %.5f %d\n",BarId[cnt],A[cnt],B[cnt],C[cnt],lay[cnt]);
        
        cnt++;
      }
      
    }
    else
      Info("LoadBarElossTuning()","File Bar Tuning Energyloss %s not open!!",FileName.data());
    
    fileTune.close();
    
    
    // store in a Map the three parameter A,B and C for energy tuning with key their Bar Id (Id of the bars in Pisa notation)
    for(int i=0; i<nSlats; i++) {
      
      if(lay[i]==LayerX){

        fElossTuningBarMapLayX[BarId[i]].push_back(A[i]);
        fElossTuningBarMapLayX[BarId[i]].push_back(B[i]);
        fElossTuningBarMapLayX[BarId[i]].push_back(C[i]);
          
        if(FootDebugLevel(1))
	  cout<<"BarId: "<<BarId[i]<<" Layer: "<<lay[i]<<" A: "<<fElossTuningBarMapLayX[BarId[i]][0]<<" B: "<<fElossTuningBarMapLayX[BarId[i]][1]<<" C: "<<fElossTuningBarMapLayX[BarId[i]][2]<<endl;

      } else if (lay[i]==LayerY) {

        fElossTuningBarMapLayY[BarId[i]].push_back(A[i]);
        fElossTuningBarMapLayY[BarId[i]].push_back(B[i]);
        fElossTuningBarMapLayY[BarId[i]].push_back(C[i]);

        if(FootDebugLevel(1))
	  cout<<"BarId: "<<BarId[i]<<" Layer: "<<lay[i]<<" A: "<<fElossTuningBarMapLayY[BarId[i]][0]<<" B: "<<fElossTuningBarMapLayY[BarId[i]][1]<<" C: "<<fElossTuningBarMapLayY[BarId[i]][2]<<endl;
          
      }
      else
        Error("LoadBarElossTuning()","Layer %d doesn't exist",lay[i]);
      
    }
}

void TATWcalibrationMap::LoadCalibrationMap(std::string FileName)
{
	if (gSystem->AccessPathName(FileName.c_str()))
	{
      Error("LoadCalibrationMap()","File %s doesn't exist",FileName.c_str());
	}
	// reset channel map
	TAGxmlParser x;
	x.ReadFile(FileName);
	std::vector<XMLNodePointer_t> BarVector;
	BarVector=x.GetChildNodesByName(x.GetMainNode(),"BAR");
   if (FootDebugLevel(1)) {
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
      if (FootDebugLevel(1))
          Info("LoadCalibrationMap()", "BAR_ID %d  par0 %f par1 %f ",BarId,p0,p1);
	}
}

//_____________________________________________________________________

Double_t TATWcalibrationMap::GetPosElossParameter(Int_t PosId, Int_t layer, UInt_t ParameterNumber)
{

  if(layer==LayerX)

    return fCalibElossMapLayX[PosId][ParameterNumber];

  else if(layer==LayerY)

    return fCalibElossMapLayY[PosId][ParameterNumber];

  else {
    Error("LoadElossCalibrationMap()","Layer %d doesn't exist",layer);
    return -1;
  }
}

//_____________________________________________________________________

Double_t TATWcalibrationMap::GetPosTofParameter(Int_t PosId, Int_t layer, UInt_t ParameterNumber)
{

  if(layer==LayerX) {

    return fCalibTofMapLayX[PosId][ParameterNumber];

  }
  else if(layer==LayerY) {

    return fCalibTofMapLayY[PosId][ParameterNumber];

  }
  else {
    Error("LoadTofCalibrationMap()","Layer %d doesn't exist",layer);
    return -1;
  }
}

//_____________________________________________________________________

Double_t TATWcalibrationMap::GetBarElossParameter(Int_t BarId, Int_t layer, UInt_t ParameterNumber)
{

  if(layer==LayerX)

    return fCalibElossBarMapLayX[BarId][ParameterNumber];

  else if(layer==LayerY)

    return fCalibElossBarMapLayY[BarId][ParameterNumber];

  else {
    Error("LoadElossBarCalibrationMap()","Layer %d doesn't exist",layer);
    return -1;
  }
}

//_____________________________________________________________________

Double_t TATWcalibrationMap::GetBarTofParameter(Int_t BarId, Int_t layer, UInt_t ParameterNumber)
{

  if(layer==LayerX) {

    return fCalibTofBarMapLayX[BarId][ParameterNumber];

  }
  else if(layer==LayerY) {

    return fCalibTofBarMapLayY[BarId][ParameterNumber];

  }
  else {
    Error("LoadTofBarCalibrationMap()","Layer %d doesn't exist",layer);
    return -1;
  }
}


Double_t TATWcalibrationMap::GetBarElossTuningParameter(Int_t BarId, Int_t layer, UInt_t ParameterNumber)
{

  if(layer==LayerX) {

    return fElossTuningBarMapLayX[BarId][ParameterNumber];

  }
  else if(layer==LayerY) {

    return fElossTuningBarMapLayY[BarId][ParameterNumber];

  }
  else {
    Error("LoadElossTuningBarMap()","Layer %d doesn't exist",layer);
    return -1;
  }
}
//_____________________________________________________________________

Int_t TATWcalibrationMap::GetNumberOfElossPos() const
{
  if(fCalibElossMapLayX.size() == fCalibElossMapLayX.size())
    return fCalibElossMapLayX.size();
  else {
    Error("LoadTofCalibrationMap()","Different number of position btw LayerX (%lu) and LayerY (%lu)",fCalibElossMapLayX.size() , fCalibElossMapLayX.size());
    return -1;
  }
}
//_____________________________________________________________________
Int_t TATWcalibrationMap::GetNumberOfTofPos() const
{
  if(fCalibTofMapLayX.size() == fCalibTofMapLayX.size())
    return fCalibTofMapLayX.size();
  else {
    Error("LoadTofCalibrationMap()","Different number of position btw LayerX (%lu) and LayerY (%lu)",fCalibTofMapLayX.size() , fCalibTofMapLayX.size());
    return -1;
  }
}
//_____________________________________________________________________
void TATWcalibrationMap::ExportToFile(std::string FileName)
{
	TAGxmlParser x;
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

bool TATWcalibrationMap::Exists(Int_t BarId)
{
	if (fCalibrationMap.count(BarId)==0)
	{
		return false;
	}
	return true;
}
