/* Class used for TW energy and time calibration
*
*/
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <map>
#include <TStyle.h>
#include <TROOT.h>
#include <TBranch.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TString.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TCanvas.h>
#include "../../../libs/src/TATWbase/Parameters.h"
#include <TMath.h>
#include <TGraphErrors.h>
#include <TSystemDirectory.h>
#include "TOFWallCalibration.h"
#include <fstream>


//Default constructor
TOFWallCalibration::TOFWallCalibration()
{
  _funcdECal = new TF1("funcdECal", "[0]*x/(1 + [1]*x)", DEMIN, DEMAX);
  _funcdECal->SetParLimits(0, 0., 2);
  _funcdECal->SetParLimits(1, 0., .1);

  CalculatePosIDtoBarsMap();
  _Debug = false;
};


/**Global function used to run the whole calibration of the dE-TOF system
* @par path to MC table file, path to input directory, path to output directory
*/
void TOFWallCalibration::RunCalibration(TString MCTableFile, TString InputDir, TString OutputDir)
{
  gROOT->SetBatch(kTRUE);

  LoadMCRefValues(MCTableFile);
  LoadRecData(InputDir);

  //Check if the OutputDir exists, othrwise throw error
  if(gSystem->AccessPathName(OutputDir.Data()))
  {
    Error("RunCalibration()", "Output directory %s does not exist", OutputDir.Data());
    throw -1;
  }

  EnergyCalibration(OutputDir);
  TOFCalibration(OutputDir);

  std::cout << "=> Calibration done!" << std::endl;
  std::cout << "=> Calibration maps written to output directory" << std::endl;

  _MCRefValues.clear();
  _ListOfFiles.clear();
  _PosIDtoXYBarsMap.clear();
  return;
}


/*Function that contains the routine used for the energy calibration of the TW
* The function saves a calibration map for each TW layer where every line contains:
* - Position ID
* - p0, "gain" factor of the Birks model [a.u./MeV]
* - p1, "saturation" parameter of the Birks model [1/MeV]
* - one flag indicating if the fit was successful. In the current version:
*   -> 0 = not good for low statistics/number of beams
*   -> 1 = good statistics, fit successful
*   -> 2 = good statistics, fit was unsuccessful anyway (high chi2)
* @par Output directory name, where maps are saved
*/
void TOFWallCalibration::EnergyCalibration(TString OutputDir)
{
  std::cout << "=> Performing Energy calibration" << std::endl;

  if(OutputDir.EndsWith("/")){OutputDir.Remove(TString::kTrailing, '/');}

  //Declare a file where raw histograms and calibration curves are saved in debug mode
  gStyle->SetOptStat(0);
  TFile* fOutDebug;
  if(_Debug){fOutDebug = new TFile(Form("%s/TWEnergyCalibrationCurvesDEBUG.root", OutputDir.Data()),"RECREATE");}

  std::ofstream* ofsSHOE = new std::ofstream(Form("%s/TATW_Energy_Calibration.cal",OutputDir.Data()));

  (*ofsSHOE) << "#position\tbarId_LayerX\tbarId_LayerY\tP0[a.u./MeV]\tP1[1/MeV]\tlayer(SHOE)\n";

  CalibrateLayerDeltaE(ofsSHOE, LayerX, OutputDir, fOutDebug);
  CalibrateLayerDeltaE(ofsSHOE, LayerY, OutputDir, fOutDebug);

  if(_Debug){fOutDebug->Close();}
  ofsSHOE->close();

  std::cout << "=> Done" << std::endl;
}


/*Function that performs the energy calibration of single TW layers
* @par pointer to energy calibration map ofstream, TW layer, output directory name, pointer to
*      output file used to store raw histograms, gaussian fits and calibration curves in debug mode
*/
void TOFWallCalibration::CalibrateLayerDeltaE(std::ofstream* ofsSHOE, TLayer layer,TString OutputDir, TFile* fOutDebug)
{
  //Setup the ofstream for single layer calibration maps --> DEBUG PURPOSES
  std::ofstream* ofsLayer;
  if(_Debug)
  {
    ofsLayer = new std::ofstream(Form("%s/TATW_Energy_Calibration_%s.cal", OutputDir.Data(), LayerName.at(layer).c_str()));

    (*ofsLayer) << "#position\tbarId_LayerX\tbarId_LayerY\tP0[a.u./MeV]\tP1[1/MeV]\tlayer(SHOE)\tflag\n";
  }

  //Variables used for the loop on beams and TW positions
  Double_t mean, sigma;
  Double_t p0, p1; //Birks parameters
  Int_t flag; //flag for calibration reliability

  //Declare a function for gaussian fits
  TF1* histfit = new TF1("histfit", "gaus", QMIN, QMAX);

  //Loop on TW positions and calibrate the 2 layers
  for(Int_t PosID = 0; PosID < nSlatCross; ++PosID)
  {
    //Create directories in the debug file and cd to the right one
    if(_Debug)
    {
      if(!fOutDebug->GetDirectory(Form("Pos%d", PosID)))
      {
        fOutDebug->mkdir(Form("Pos%d", PosID));
      }
      if(!fOutDebug->GetDirectory(Form("Pos%d/%s", PosID, LayerName.at(layer).c_str())))
      {
        fOutDebug->mkdir(Form("Pos%d/%s", PosID, LayerName.at(layer).c_str()));
      }

      fOutDebug->cd(Form("Pos%d/%s", PosID, LayerName.at(layer).c_str()));
    }

    //Declare the dE_MC vd Q_raw graphs
    TGraphErrors* grLayer = new TGraphErrors();

    //Loop on the files/MC values to fill the TGraph
    std::vector<TFile*>::iterator itFile;
    std::vector<MCRefType>::iterator itMC;
    for(itFile = _ListOfFiles.begin(), itMC = _MCRefValues.begin(); itFile != _ListOfFiles.end() && itMC != _MCRefValues.end(); ++itFile, ++itMC)
    {
      //Check if the file is present or not. If not, just skip the iteration
      if(*itFile == nullptr){continue;}
      TH1D* hist;
      (*itFile)->GetObject(Form("TW/hQ%s-PartID%d-En%.3lf-Pos%d", LayerName.at(layer).c_str(), (*itMC).PartID, (*itMC).BeamEn, PosID), hist);

      //Check if the position has enough data for that beam and perform the fit
      //to find the mean value of raw Q
      if(hist->GetEntries() > 50)
      {
        //Set the parameters of the gaussian function and fit the Qraw histogram
        mean = hist->GetBinCenter(hist->GetMaximumBin());
        sigma = hist->GetStdDev();

        histfit->SetParameters(hist->GetMaximum(), mean, sigma);
        hist->Fit(histfit, "Q0", "", mean - 4*sigma, mean + 4*sigma);

        //Increase the number of points and save mean value + error (rn sigma) in the tgraph
        grLayer->Set(grLayer->GetN() + 1);
        grLayer->SetPoint(grLayer->GetN()-1, (*itMC).dE_MC[layer], histfit->GetParameter(1));
        grLayer->SetPointError(grLayer->GetN()-1, 0., histfit->GetParameter(2));
      }

      //Save all the Q_{raw} histograms and gaussian fits if debug mode is activated
      if(_Debug)
      {
        TF1* fit = (TF1*) histfit->Clone();
        TH1D* histsave = (TH1D*) hist->Clone();
        TCanvas* c = new TCanvas("","",1000,800);
        histsave->SetTitle(Form("Qraw%s_Part%d_En%.3lf;Q_{raw} [V*ns];Entries", LayerName.at(layer).c_str(), (*itMC).PartID, (*itMC).BeamEn));
        histsave->SetMarkerStyle(20);
        histsave->SetMarkerSize(1.2);
        histsave->Draw();
        if(histsave->GetEntries() > 50)
        {
          fit->SetLineWidth(2);
          fit->SetLineColor(kRed);
          fit->Draw("same");
        }

        c->Write(Form("Qraw_Part%d_En%.3lf", (*itMC).PartID, (*itMC).BeamEn));
        delete fit;
        delete c;
      }

    } //End of loop on different runs


    //Initialize calibration parameters to 0 -> data written if no fit is performed
    flag = 0; p0 = 0; p1 = 0;

    //Perform the fit of the light output model
    //First, check if there's enough points to fit
    if(grLayer->GetN() >= 3)
    {
      //Sort TGraph data points
      grLayer->Sort();

      //Reset the parameters to likely values and fit
      _funcdECal->SetParameters(.5, 1E-2);
      grLayer->Fit(_funcdECal, "Q0", "", DEMIN, DEMAX);

      //Save the fit parameters
      p0 = _funcdECal->GetParameter(0);
      p1 = _funcdECal->GetParameter(1);
      flag = 1;

      //Check if the fit was successful
      if(_funcdECal->GetChisquare()/(grLayer->GetN()-2) > 3){flag =  2;}

    }

    //Save the canvas with the TGraph and calibration curves if in debug mode
    if(_Debug)
    {
      TF1* fit = (TF1*) _funcdECal->Clone();
      TH2D* histpave = new TH2D("", "", 10, -1., DEMAX, 10, -1., QMAX);
      TCanvas* c = new TCanvas("","",1000,800);
      histpave->SetTitle(Form("EnCalCurve%s_Pos%d_flag%d;#Delta E_{MC} [MeV];Q_{raw} [V*ns]", LayerName.at(layer).c_str(), PosID, flag));
      histpave->Draw();
      grLayer->SetMarkerStyle(20);
      grLayer->SetMarkerSize(1.2);
      grLayer->Draw("P same");
      if(grLayer->GetN() >= 3)
      {
        fit->SetLineWidth(2);
        fit->SetLineColor(kRed);
        fit->Draw("same");
      }

      c->Write(Form("EnCalCurve%s_flag%d", LayerName.at(layer).c_str(), flag));
      delete fit;
      delete c;
      delete histpave;

      //Write parameters to the single layer ofstream
      (*ofsLayer) << Form("%d\t%d\t%d\t%lf\t%lf\t%d\t%d\n", PosID, _PosIDtoXYBarsMap[PosID].first, _PosIDtoXYBarsMap[PosID].second, p0, p1, layer, flag);
    }

    //Write the parameters to the final calibration map
    (*ofsSHOE) << Form("%d\t%d\t%d\t%lf\t%lf\t%d\n", PosID, _PosIDtoXYBarsMap[PosID].first, _PosIDtoXYBarsMap[PosID].second, p0, p1, layer);

  } //End of loop on TW positions

  if(_Debug)
  {
    ofsLayer->close();
  }
}


/*Function that contains the routine used for the TOF calibration of the dE-TOF
* The maps contain:
* - Position ID
* - Bar ID for layer X
* - Bar ID for layer Y
* - Time difference between the raw TOF and the associated MC reference value [ns]
* - DeltaT2 [ns] -> TO BE DEFINED!!
* @par Output directory name, where maps are saved
*/
void TOFWallCalibration::TOFCalibration(TString OutputDir)
{
  std::cout << "=> Performing TOF calibration" << std::endl;

  if(OutputDir.EndsWith("/")){OutputDir.Remove(TString::kTrailing, '/');}

  //Declare pointer to file where debug histograms are saved
  gStyle->SetOptStat("neMR");
  TFile* fOutDebug;
  if(_Debug){fOutDebug = TFile::Open(Form("%s/TOFCalibrationDEBUG.root", OutputDir.Data()), "RECREATE");}

  //Loop in parallel over _MCRefValues and _ListOfFiles
  std::vector<TFile*>::iterator itFile;
  std::vector<MCRefType>::iterator itMC;
  for(itFile = _ListOfFiles.begin(), itMC = _MCRefValues.begin(); itFile != _ListOfFiles.end() && itMC != _MCRefValues.end(); ++itFile, ++itMC)
  {
    //If the file was not loaded, just skip the iteration
    if(*itFile == nullptr){continue;}

    std::ofstream* ofsSHOE = new std::ofstream(Form("%s/TATW_Tof_Calibration_%s_run%04d.cal", OutputDir.Data(), (*itMC).ExpName.Data(), (*itMC).runNb));

    (*ofsSHOE) << "#position\tbarId_LayerX\tbarId_LayerY\tdeltaT[ns]\tdeltaT2[ns]\tlayer(SHOE)\n";

    //Call the calibration routine for each layer
    CalibrateLayerTOF(*itFile, *itMC, ofsSHOE, LayerX, OutputDir, fOutDebug);
    CalibrateLayerTOF(*itFile, *itMC, ofsSHOE, LayerY, OutputDir, fOutDebug);

    ofsSHOE->close();
  }

  if(_Debug){fOutDebug->Close();}
  std::cout << "=> TOF calibration done" << std::endl;
}


/*Function that calibrates the TOF of a TW layer for a specific run
* @par pointer to data file, MC reference values of the run, pointer to ofstream of final caliration map,
*      TW layer, pointer to output debug file
*/
void TOFWallCalibration::CalibrateLayerTOF(TFile* f, MCRefType MCRef, std::ofstream* ofsSHOE, TLayer layer, TString OutputDir, TFile* fOutDebug)
{
  //Create a debug ofstream for single layer calibration maps
  //and make histograms directories in the debug root file
  std::ofstream* ofsLayer;
  if(_Debug)
  {
    ofsLayer = new std::ofstream(Form("%s/TATW_Tof_Calibration_%s_%s_run%04d.cal", OutputDir.Data(), LayerName.at(layer).c_str(), MCRef.ExpName.Data(), MCRef.runNb));

    (*ofsLayer) << "#position\tbarId_LayerX\tbarId_LayerY\tdeltaT[ns]\tdeltaT2[ns]\tlayer(SHOE)\tflag\n";

    if(!fOutDebug->GetDirectory(Form("Part%dEn%.3lf", MCRef.PartID, MCRef.BeamEn)))
    {
      fOutDebug->mkdir(Form("Part%dEn%.3lf", MCRef.PartID, MCRef.BeamEn));
    }
    if(!fOutDebug->GetDirectory(Form("Part%dEn%.3lf/%s", MCRef.PartID, MCRef.BeamEn, LayerName.at(layer).c_str())))
    {
      fOutDebug->mkdir(Form("Part%dEn%.3lf/%s", MCRef.PartID, MCRef.BeamEn, LayerName.at(layer).c_str()));
    }

    fOutDebug->cd(Form("Part%dEn%.3lf/%s", MCRef.PartID, MCRef.BeamEn, LayerName.at(layer).c_str()));
  }

  //Initialize the function used to find the mean TOF_raw values and some
  TF1* funcTOFCal = new TF1("funcTOFcal", "gaus", TOFMIN, TOFMAX);
  funcTOFCal->SetParLimits(1, TOFMIN, TOFMAX);
  funcTOFCal->SetParLimits(2, 0., 0.5); //Sigma of the gaussian from 0 to 500 ps

  //Useful variables for the cycle
  Int_t flag;
  Double_t mean, sigma, TOFcorrection, DeltaT2;

  //Cycle on the TW positions
  for(Int_t PosID = 0; PosID < nSlatCross; ++PosID)
  {
    //Set flag and TOFcorrection to 0
    flag = 0;
    TOFcorrection = 0.;
    DeltaT2 = 0.; //TO BE IMPLMENTED

    //Get the raw TOF histogram of the corresponding position and check if it was loaded correctly
    TH1D* hist;
    f->GetObject(Form("TW/hTOF%s-PartID%d-En%.3lf-Pos%d", LayerName.at(layer).c_str(), MCRef.PartID, MCRef.BeamEn, PosID), hist);
    if(hist == nullptr)
    {
      Error("CalibrateLayerTOF()","hist %s not found in file %s", Form("TW/hTOF%s-PartID%d-En%.3lf-Pos%d", LayerName.at(layer).c_str(), MCRef.PartID, MCRef.BeamEn, PosID), f->GetName());
      throw -1;
    }

    //Check if the TW position acquired enough events
    if(hist->GetEntries() > 50)
    {
      //Get the initial values for the fit
      mean = hist->GetBinCenter(hist->GetMaximumBin());
      sigma = hist->GetStdDev();

      //Set the initial values and fit TOF_raw distribution
      funcTOFCal->SetParameters(hist->GetMaximum(), mean, sigma);
      hist->Fit(funcTOFCal, "Q0", "", mean - 4*sigma, mean + 4*sigma);

      //Calculate the TOF correction as the difference between raw and MC mean values and set the flag
      TOFcorrection = funcTOFCal->GetParameter(1) - MCRef.TOF_MC[layer];
      flag = 1;
    }

    //Save histograms and gaussian fits if in debug mode
    if(_Debug)
    {
      TF1* fit = (TF1*) funcTOFCal->Clone();
      TH1D* histsave = (TH1D*) hist->Clone();
      TCanvas* c = new TCanvas("","",1000,800);
      histsave->SetTitle(Form("TOFraw%s_Pos%d_flag%d;TOF_{raw} [ns];Entries", LayerName.at(layer).c_str(), PosID, flag));
      histsave->SetMarkerStyle(20);
      histsave->SetMarkerSize(1.2);
      histsave->Draw();
      if(flag)
      {
        fit->SetLineWidth(2);
        fit->SetLineColor(kRed);
        fit->Draw("same");
      }

      c->Write(Form("TOFrawPos%d_flag%d", PosID, flag));
      delete fit;
      delete c;

      //Write to single layer calibration map
      (*ofsLayer) << Form("%d\t%d\t%d\t%lf\t%lf\t%d\t%d\n", PosID, _PosIDtoXYBarsMap[PosID].first, _PosIDtoXYBarsMap[PosID].second, TOFcorrection, DeltaT2, layer, flag);
    }

    //Write to final calibration map
    (*ofsSHOE) << Form("%d\t%d\t%d\t%lf\t%lf\t%d\n", PosID, _PosIDtoXYBarsMap[PosID].first, _PosIDtoXYBarsMap[PosID].second, TOFcorrection, DeltaT2, layer);
  }

  //Close single layer TOF calibration map for the current beam
  if(_Debug){ofsLayer->close();}
}


/*Function used to load the reconstructed data. The function fills the _ListOfFiles vector
* with the pointers to each data file
* @par name of the input file directory
*/
void TOFWallCalibration::LoadRecData(TString InputDir)
{
  //Check if the InputDir exists, otherwise throw an error
  if(gSystem->AccessPathName(InputDir.Data()))
  {
    Error("LoadRecData()", "Input directory %s does not exist", InputDir.Data());
    throw -1;
  }

  //Get the list of .rec.root files in InputDir and read them one by one
  if(InputDir.EndsWith("/")){InputDir.Remove(TString::kTrailing, '/');}
  Int_t FileCount = 0;

  for(auto it = _MCRefValues.begin(); it != _MCRefValues.end(); ++it)
  {
    TString fName = Form("%s/dE-TOF_Calib_%s_run%04d.root", InputDir.Data(), (*it).ExpName.Data(), (*it).runNb);

    //Check if the file exists: if not, append a nullptr to the vector of TFile*. This is done to keep the vectors
    //_MCRefValues and _ListOfFiles of the same size (useful for calibration cycles)
    if(gSystem->AccessPathName(fName)){_ListOfFiles.push_back(nullptr);}
    else
    {
      TFile* fp = TFile::Open(fName, "r");
      if(fp->IsOpen() && !fp->IsZombie()){FileCount++;}
      _ListOfFiles.push_back(fp);
    }
  }

  //End with an error if no data was found, otherwise print some information
  if(FileCount == 0)
  {
    Error("LoadRecData()","No ROOT (*.root) file found in %s with matching MC reference values", InputDir.Data());
    throw -1;
  }

  std::cout << "=> Found data for files:" << std::endl;
  for(auto it = _ListOfFiles.begin(); it != _ListOfFiles.end(); ++it)
  {
    if(*it != nullptr) {std::cout << (*it)->GetName() << std::endl;}
  }
}


/*Function needed to load the MC reference values for TW calibration
* - experiment name (string)
* - run number (int)
* - particle ID (string)
* - beam energy (Double_t, GeV/u)
* - mean dE_MC in the x and y layer (Double_t, MeV)
* - mean TOF_MC in the x and y layer (Double_t, ns)
* @par path to MC table. The file needs to contain:
*/
void TOFWallCalibration::LoadMCRefValues(TString MCTableFile)
{
  Char_t Exp[15];
  Int_t partType, run;
  Double_t BeamEn, dEx, dEy, TOFx, TOFy;

  //Open the MC table
  FILE* fMCRefValues = fopen(MCTableFile, "r");
  if(fMCRefValues == NULL)
  {
    Error("LoadMCRefValues()","MC Table not found! Path given: %s", MCTableFile.Data());
    throw -1;
  }

  std::cout << "=> MC reference values for dE-TOF Calibration\n";
  std::cout << "Exp\tRun\tpID\tBeamEn\tdEx\tdEy\tTOFx\tTOFy\n";
  std::cout << " \t \t \t[GeV/u]\t[MeV]\t[MeV]\t[ns]\t[ns]\n";
  std::cout << "--------------------------------------------------------------\n";

  Char_t line[200];
  MCRefType MCref;
  while(fgets(line, sizeof(line), fMCRefValues))
  {
    if(line[0] == '#' || line[0] == '\n'){continue;}
    std::sscanf(line, "%s %d %d %lf %lf %lf %lf %lf", Exp, &run, &partType, &BeamEn, &dEx, &dEy, &TOFx, &TOFy);

    MCref.ExpName = (TString)Exp;
    MCref.runNb = run;
    MCref.PartID = static_cast<ParticleType>(partType);
    MCref.BeamEn = BeamEn;
    MCref.dE_MC[LayerX] = dEx;
    MCref.dE_MC[LayerY] = dEy;
    MCref.TOF_MC[LayerX] = TOFx;
    MCref.TOF_MC[LayerY] = TOFy;
    _MCRefValues.push_back(MCref);

    //Print out the info from MC table
    std::cout << Form("%s\t%04d\t%d\t%.3lf\t%.3lf\t%.3lf\t%.3lf\t%.3lf", Exp, run, partType, BeamEn, dEx, dEy, TOFx, TOFy) << std::endl;
  }

  fclose(fMCRefValues);
}


/*Function that calcualtes the Position ID form bar numbers
* Modify if we change PosID convention
*/
Int_t TOFWallCalibration::CalculatePositionID(Int_t XBar, Int_t YBar)
{
  if(XBar < LAYERXBARMIN || XBar > LAYERXBARMAX)
  {
    Error("CalculatePositionID()","XBar index out of bounds (%d, %d)", LAYERXBARMIN, LAYERXBARMAX);
    throw -1;
  }
  if(YBar < LAYERYBARMIN || YBar > LAYERYBARMAX)
  {
    Error("CalculatePositionID()","YBar index out of bounds (%d, %d)", LAYERYBARMIN, LAYERYBARMAX);
    throw -1;
  }
  return (XBar - 20)*20 + YBar;
}


/*Function that fills the _PosIDtoXYBarsMap map with the actual values
* Needed to easily write the calibration maps in the format implemented in SHOE
*/
void TOFWallCalibration::CalculatePosIDtoBarsMap()
{
  //Cycle on the X-Y bars and calculate the PosID
  for(Int_t XBar = LAYERXBARMIN; XBar <= LAYERXBARMAX; ++XBar)
  {
    for(Int_t YBar = LAYERYBARMIN; YBar <= LAYERYBARMAX; ++YBar)
    {
      _PosIDtoXYBarsMap[CalculatePositionID(XBar, YBar)] = std::make_pair(XBar, YBar);
    }
  }
}


/*Function that sets the debug mode; this mode saves all the raw data histograms
* in a .root file inside the output directory
*/
void TOFWallCalibration::SetDebugMode()
{
  _Debug = true;
}
