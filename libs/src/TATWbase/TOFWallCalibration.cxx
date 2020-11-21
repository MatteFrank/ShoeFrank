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
#include <TBranch.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TString.h>
#include <TH1.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include "CTags.h"
#include "Parameters.h"
#include <TMath.h>
#include <TGraphErrors.h>
#include <TSystemDirectory.h>
#include "TOFWallCalibration.hxx"
#include <fstream>


//Default constructor
TOFWallCalibration::TOFWallCalibration()
{
  _funcdECal = new TF1("funcdECal", "[0]*x/(1 + [1]*x)", DEMIN, DEMAX);
  _funcdECal->SetParLimits(0, 0., 4);
  _funcdECal->SetParLimits(1, 0., .1);
  _Debug = false;
};


/*Global function that calls the calibration routines
*
*/
void TOFWallCalibration::RunCalibration(TString OutputDir)
{
  EnergyCalibration(OutputDir);
  TOFCalibration(OutputDir);

  Message::DisplayMessageWithEmphasys("Calibration done!");
  Message::DisplayMessageWithEmphasys("Calibration maps written to output directory");
}


/*Function that contains the routine used for the energy calibration of the TW
* The function saves a calibration map for each TW layer where every line contains:
* - Position ID
* - p0, "gain" factor of the Birks model [a.u./MeV]
* - p1, "saturation" parameter of the Birks model [1/MeV]
* - one flag indicating if the fit was successful. In the current version:
*   -> 0 = not good for low statistics/number of beams
*   -> 1 = good statistics, both CNAO and GSI data
*   -> 2 = good statistics, only CNAO data
*   -> 3 = good statistics, fit was unsuccessful anyway (high chi2)
* @par Output directory name, where maps are saved
*/
void TOFWallCalibration::EnergyCalibration(TString OutputDir)
{
  Message::DisplayMessageWithEmphasys("Performing Energy calibration");

  //Declare a file where raw histograms and calibration curves are saved in debug mode
  TFile* fOut;
  if(_Debug){fOut = TFile::Open(Form("%s/EnergyCalibrationDEBUG.root",OutputDir.Data()),"RECREATE");}

  //Variables used for the loop on beams and TW positions
  std::pair<Int_t, Double_t> PartEnPair;
  Double_t mean, sigma;
  Double_t p0, p1; //Birks parameters
  Bool_t gsiF, gsiR; //flags that check if a position has GSI data or not
  Int_t flag; //flag for calibration reliability

  //Open the files where energy cal maps are stored.
  //The energy calibration is position-dependent
  ofstream ofsF(Form("%s/EnergyCalMapFront.txt", OutputDir.Data()));
  ofstream ofsR(Form("%s/EnergyCalMapRear.txt", OutputDir.Data()));

  //Declare a function for gaussian fits
  TF1* histfit = new TF1("histfit", "gaus", QMIN, QMAX);

  //Cycle on TW positions and calibrate the 2 layers
  for(Int_t PosID = 0; PosID < NUMBEROFTWPOSITIONS; ++PosID)
  {
    //Create some directories in the debug file
    if(_Debug)
    {
      gDirectory->mkdir(Form("Pos%d",PosID));
      gDirectory->cd(Form("Pos%d",PosID));
      gDirectory->mkdir("FrontLayer");
      gDirectory->mkdir("RearLayer");
    }

    //Declare the dE_MC vd Q_raw graphs
    TGraphErrors* grFront = new TGraphErrors();
    TGraphErrors* grRear = new TGraphErrors();

    //Initialize the GSI flags to false
    gsiF = false;
    gsiR = false;
    for(auto itBeam = _PartEnPairsData.begin(); itBeam != _PartEnPairsData.end(); ++itBeam)
    {
      //Check if the position has enough data for that beam and perform the fit
      //to find the mean value of raw Q
      if(_hQFrontLayer.at(*itBeam).at(PosID)->GetEntries() > 50)
      {
        //Set the parameters of the gaussian function and fit the Qraw histogram
        mean = _hQFrontLayer.at(*itBeam).at(PosID)->GetBinCenter(_hQFrontLayer.at(*itBeam).at(PosID)->GetMaximumBin());
        sigma = _hQFrontLayer.at(*itBeam).at(PosID)->GetStdDev();

        histfit->SetParameters(_hQFrontLayer.at(*itBeam).at(PosID)->GetMaximum(), mean, sigma);
        _hQFrontLayer.at(*itBeam).at(PosID)->Fit(histfit, "Q0", "", mean - 4*sigma, mean + 4*sigma);

        //Increase the number of points and save mean value + error (rn sigma) in the tgraph
        grFront->Set(grFront->GetN() + 1);
        grFront->SetPoint(grFront->GetN()-1, _dERefValues.at(*itBeam).first, histfit->GetParameter(1));
        grFront->SetPointError(grFront->GetN()-1, 0., histfit->GetParameter(2));

        //Set GSI flag to true if the beam is found in that position
        if((*itBeam).first == 2){gsiF = true;}
      }

      //Save all the histograms and fits if debug mode is activated
      if(_Debug)
      {
        TF1* fit = (TF1*) histfit->Clone();
        TCanvas* c1 = new TCanvas("","",1000,800);
        _hQFrontLayer.at(*itBeam).at(PosID)->SetTitle(Form("QrawFront_Part%d_En%.1lf;Q_{raw} [a.u.];Entries", (*itBeam).first, (*itBeam).second));
        _hQFrontLayer.at(*itBeam).at(PosID)->SetMarkerStyle(20);
        _hQFrontLayer.at(*itBeam).at(PosID)->SetMarkerSize(1.2);
        _hQFrontLayer.at(*itBeam).at(PosID)->Draw();
        fit->SetLineWidth(2);
        fit->SetLineColor(kRed);
        if(_hQFrontLayer.at(*itBeam).at(PosID)->GetEntries() > 50) {fit->Draw("same");}

        gDirectory->cd("FrontLayer");
        c1->Write(Form("Qraw_Part%d_En%1.lf", (*itBeam).first, (*itBeam).second));
        gDirectory->cd("..");
        delete fit;
        delete c1;
      }

      //Repeat the same steps for the rear layer
      if(_hQRearLayer.at(*itBeam).at(PosID)->GetEntries() > 50)
      {
        mean = _hQRearLayer.at(*itBeam).at(PosID)->GetBinCenter(_hQRearLayer.at(*itBeam).at(PosID)->GetMaximumBin());
        sigma = _hQRearLayer.at(*itBeam).at(PosID)->GetStdDev();

        histfit->SetParameters(_hQRearLayer.at(*itBeam).at(PosID)->GetMaximum(), mean, sigma);
        _hQRearLayer.at(*itBeam).at(PosID)->Fit(histfit, "Q0", "", mean - 4*sigma, mean + 4*sigma);

        grRear->Set(grRear->GetN() + 1);
        grRear->SetPoint(grRear->GetN() - 1, _dERefValues.at(*itBeam).second, histfit->GetParameter(1));
        grRear->SetPointError(grRear->GetN() - 1, 0., histfit->GetParameter(2));

        if((*itBeam).first == 2){gsiR = true;}
      }

      if(_Debug)
      {
        TF1* fit = (TF1*) histfit->Clone();
        TCanvas* c1 = new TCanvas("","",1000,800);
        _hQRearLayer.at(*itBeam).at(PosID)->SetTitle(Form("QrawRear_Part%d_En%.1lf;Q_{raw} [a.u.];Entries", (*itBeam).first, (*itBeam).second));
        _hQRearLayer.at(*itBeam).at(PosID)->SetMarkerStyle(20);
        _hQRearLayer.at(*itBeam).at(PosID)->SetMarkerSize(1.2);
        _hQRearLayer.at(*itBeam).at(PosID)->Draw();
        fit->SetLineWidth(2);
        fit->SetLineColor(kRed);
        if(_hQRearLayer.at(*itBeam).at(PosID)->GetEntries() > 50) {fit->Draw("same");}

        gDirectory->cd("RearLayer");
        c1->Write(Form("Qraw_Part%d_En%1.lf", (*itBeam).first, (*itBeam).second));
        gDirectory->cd("..");
        delete fit;
        delete c1;
      }
    }

    //Initialize to 0 -> data written if no fit is performed
    flag = 0;
    p0 = 0;
    p1 = 0;

    //Sort TGraph data points
    grFront->Sort();

    //Perform the fit of the light output model for both layers
    //First, check if there's enough points to fit
    if(grFront->GetN() >= 3)
    {
      //Reset the parameters to likely values and fit
      _funcdECal->SetParameters(2., 1E-2);
      grFront->Fit(_funcdECal, "Q0", "", DEMIN, DEMAX);

      //Save the fit parameters
      p0 = _funcdECal->GetParameter(0);
      p1 = _funcdECal->GetParameter(1);

      //Check if the fit was successful
      if(_funcdECal->GetChisquare()/(grFront->GetN()-2) > 3){flag =  3;}
      //Check if GSI data have been acquired in the position
      else{flag = gsiF ? 1 : 2;}
    }

    //Save the canvas with the TGraph and calibration curves if in debug mode
    if(_Debug)
    {
      TF1* fit = (TF1*) _funcdECal->Clone();
      TCanvas* c1 = new TCanvas("","",1000,800);
      grFront->SetTitle(Form("EnCalCurveFront_Pos%d_flag%d;#Delta E_{MC} [MeV];Q_{raw} [a.u.]", PosID, flag));
      grFront->SetMarkerStyle(20);
      grFront->SetMarkerSize(1.2);
      grFront->Draw("AP");
      fit->SetLineWidth(2);
      fit->SetLineColor(kRed);
      if(grFront->GetN() >= 3) {fit->Draw("same");}

      gDirectory->cd("FrontLayer");
      c1->Write(Form("EnCalCurveFront_flag%d", flag));
      gDirectory->cd("..");
      delete fit;
      delete c1;
    }

    //Write to Front Layer file
    ofsF << Form("%d\t%lf\t%lf\t%d\n", PosID, p0, p1, flag);

    //Repeat the same steps for the other layer
    flag = 0; p0 = 0; p1 = 0;

    grRear->Sort();

    if(grRear->GetN() >= 3)
    {
      _funcdECal->SetParameters(2.,1E-2);
      grRear->Fit(_funcdECal, "Q0", "", DEMIN, DEMAX);
      p0 = _funcdECal->GetParameter(0);
      p1 = _funcdECal->GetParameter(1);

      if(_funcdECal->GetChisquare()/(grRear->GetN()-2) > 2){flag =  3;}
      else{flag = gsiF ? 1 : 2;}
    }

    if(_Debug)
    {
      TF1* fit = (TF1*) _funcdECal->Clone();
      TCanvas* c1 = new TCanvas("","",1000,800);
      grRear->SetTitle(Form("EnCalCurveRear_Pos%d_flag%d;#Delta E_{MC} [MeV];Q_{raw} [a.u.]", PosID, flag));
      grRear->SetMarkerStyle(20);
      grRear->SetMarkerSize(1.2);
      grRear->Draw("AP");
      fit->SetLineWidth(2);
      fit->SetLineColor(kRed);
      if(grRear->GetN() >= 3){fit->Draw("same");}

      gDirectory->cd("RearLayer");
      c1->Write(Form("EnCalCurveRear_flag%d", flag));
      gDirectory->cd("..");
      delete fit;
      delete c1;
    }

    ofsR << Form("%d\t%lf\t%lf\t%d\n", PosID, p0, p1, flag);

    if(_Debug){gDirectory->cd("..");}

    //Delete the graphs at the end of the cycle on the position
    delete grFront;
    delete grRear;
  }

  //Close all the output files
  if(_Debug){fOut->Close();}

  ofsF.close();
  ofsR.close();

  Message::DisplayMessageWithEmphasys("Done");
}


/*Function that contains the routine used for the TOF calibration of the dE-TOF
* The function saves two TOF calibration maps (one per TW layer) for each beam found. The maps contain:
* - Position ID
* - Time difference between the raw TOF and the associated MC reference value [ns]
* - Flag: 0 -> not calibrated, 1 -> calibrated
* @par Output directory name, where maps are saved
*/
void TOFWallCalibration::TOFCalibration(TString OutputDir)
{
  Message::DisplayMessageWithEmphasys("Performing TOF calibration");

  //Declare pointer to file where debug histograms are saved
  TFile* fOut;
  if(_Debug){fOut = TFile::Open(Form("%s/TOFCalibrationDEBUG.root",OutputDir.Data()),"RECREATE");}

  //Initialize the function used to find the mean TOF_raw values and some
  //Useful variables for the cycle
  TF1* funcTOFCal = new TF1("funcTOFcal", "gaus", TOFMIN, TOFMAX);
  funcTOFCal->SetParLimits(1, TOFMIN, TOFMAX);
  funcTOFCal->SetParLimits(2, 0., 0.4); //Sigma of the gaussian from 0 to 400 ps
  Int_t flag;
  Double_t mean, sigma, TOFcorrection;

  //Cycle on each beam found in data -> TOF calibration is beam-dependent
  for(auto itBeam = _PartEnPairsData.begin(); itBeam != _PartEnPairsData.end(); ++itBeam)
  {
    //Open the files where TOF calibration will be stored
    ofstream ofsF(Form("%s/TOFCalMapFrontPart%dEn%.1lf.txt", OutputDir.Data(), (*itBeam).first, (*itBeam).second));
    ofstream ofsR(Form("%s/TOFCalMapRearPart%dEn%.1lf.txt", OutputDir.Data(), (*itBeam).first, (*itBeam).second));

    //Make histograms directories in the debug file
    if(_Debug)
    {
      gDirectory->mkdir(Form("Part%dEn%.1lf.txt", (*itBeam).first, (*itBeam).second));
      gDirectory->cd(Form("Part%dEn%.1lf.txt", (*itBeam).first, (*itBeam).second));
      gDirectory->mkdir("FrontLayer");
      gDirectory->mkdir("RearLayer");
    }

    //Cycle on the TW positions
    for(Int_t PosID = 0; PosID < NUMBEROFTWPOSITIONS; ++PosID)
    {
      //Set flag and TOFcorrection to 0
      flag = 0;
      TOFcorrection = 0.;
      //Check if the position has enough statistics to perform the calibration
      if(_hTOFFrontLayer.at(*itBeam).at(PosID)->GetEntries() > 50)
      {
        //Get the initial values for the fit
        mean = _hTOFFrontLayer.at(*itBeam).at(PosID)->GetBinCenter(_hTOFFrontLayer.at(*itBeam).at(PosID)->GetMaximumBin());
        sigma = _hTOFFrontLayer.at(*itBeam).at(PosID)->GetStdDev();

        //Set the initial values and fit TOF_raw distribution
        funcTOFCal->SetParameters(_hTOFFrontLayer.at(*itBeam).at(PosID)->GetMaximum(), mean, sigma);
        _hTOFFrontLayer.at(*itBeam).at(PosID)->Fit(funcTOFCal, "Q0", "", mean - 4*sigma, mean + 4*sigma);

        //Calculate the TOF correction as the difference between raw and MC mean values and set the flag
        TOFcorrection = funcTOFCal->GetParameter(1) - _TOFRefValues.at(*itBeam).first;
        flag = 1;
      }

      //Save histograms and gaussian fits if in debug mode
      if(_Debug)
      {
        TF1* fit = (TF1*) funcTOFCal->Clone();
        TCanvas* c1 = new TCanvas("","",1000,800);
        _hTOFFrontLayer.at(*itBeam).at(PosID)->SetTitle(Form("TOFrawFront_Pos%d_flag%d;TOF_{raw} [ns];Entries", PosID, flag));
        _hTOFFrontLayer.at(*itBeam).at(PosID)->SetMarkerStyle(20);
        _hTOFFrontLayer.at(*itBeam).at(PosID)->SetMarkerSize(1.2);
        _hTOFFrontLayer.at(*itBeam).at(PosID)->Draw();
        fit->SetLineWidth(2);
        fit->SetLineColor(kRed);
        if(flag){fit->Draw("same");}

        gDirectory->cd("FrontLayer");
        c1->Write(Form("TOFrawPos%d_flag%d", PosID, flag));
        gDirectory->cd("..");
        delete fit;
        delete c1;
      }

      //Write to output file
      ofsF << Form("%d\t%lf\t%d", PosID, TOFcorrection, flag) << endl;

      //Repeat exactly for the other layer
      flag = 0;
      TOFcorrection = 0.;
      if(_hTOFRearLayer.at(*itBeam).at(PosID)->GetEntries() > 50)
      {
        mean = _hTOFRearLayer.at(*itBeam).at(PosID)->GetBinCenter(_hTOFRearLayer.at(*itBeam).at(PosID)->GetMaximumBin());
        sigma = _hTOFRearLayer.at(*itBeam).at(PosID)->GetStdDev();

        funcTOFCal->SetParameters(_hTOFRearLayer.at(*itBeam).at(PosID)->GetMaximum(), mean, sigma);
        _hTOFRearLayer.at(*itBeam).at(PosID)->Fit(funcTOFCal, "Q0", "", mean - 4*sigma, mean + 4*sigma);

        TOFcorrection = funcTOFCal->GetParameter(1) - _TOFRefValues.at(*itBeam).second;
        flag = 1;
      }

      if(_Debug)
      {
        TF1* fit = (TF1*) funcTOFCal->Clone();
        TCanvas* c1 = new TCanvas("","",1000,800);
        _hTOFRearLayer.at(*itBeam).at(PosID)->SetTitle(Form("TOFrawRear_Pos%d_flag%d;TOF_{raw} [ns];Entries", PosID, flag));
        _hTOFRearLayer.at(*itBeam).at(PosID)->SetMarkerStyle(20);
        _hTOFRearLayer.at(*itBeam).at(PosID)->SetMarkerSize(1.2);
        _hTOFRearLayer.at(*itBeam).at(PosID)->Draw();
        fit->SetLineWidth(2);
        fit->SetLineColor(kRed);
        if(flag){fit->Draw("same");}

        gDirectory->cd("RearLayer");
        c1->Write(Form("TOFrawPos%d_flag%d", PosID, flag));
        gDirectory->cd("..");
        delete fit;
        delete c1;
      }

      ofsR << Form("%d\t%lf\t%d", PosID, TOFcorrection, flag) << endl;
    }

    if(_Debug){gDirectory->cd("..");}

    //Close TOF calibration maps for the current beam
    ofsF.close();
    ofsR.close();
  }

  //Close the debug file
  if(_Debug){fOut->Close();}

  Message::DisplayMessageWithEmphasys("Done");
}


/*Function used to load the reconstructed data
* @par name of the input file directory
*/
void TOFWallCalibration::LoadRecData(TString InputDataDir)
{
  //Initialize the histograms here using the list of beams found in the MC table
  InitHists();

  //Get the list of .rec.root files in InputDataDir and read them one by one
  if(InputDataDir.EndsWith("/")){InputDataDir.Remove(TString::kTrailing, '/');}
  Int_t FileCount = 0;
  TString ext = ".rec.root";
  TSystemDirectory dir(InputDataDir, InputDataDir);
  TList *files = dir.GetListOfFiles();
  if(files)
  {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while((file=(TSystemFile*)next()))
    {
      fname = Form("%s/%s",dir.GetName(), file->GetName());
      if (!file->IsDirectory() && fname.EndsWith(ext))
      {
        //If the file is found, read it and increase counter
        FileCount++;
        LoadSingleFileData(fname);
      }
    }
  }

  //Throw an error if no data was found, otherwise print some information
  if(!FileCount){Message::DisplayFatalError(Form("No ROOT (*.rec.root) file found in %s", dir.GetName()));}

  Message::DisplayMessageWithEmphasys("Found data for the beams:");
  for(auto itBeam = _PartEnPairsData.begin(); itBeam != _PartEnPairsData.end(); ++itBeam)
  {
    Message::DisplayMessage(Form("Primary ID: %d\tEnergy: %.1lf MeV/u", (*itBeam).first, (*itBeam).second));
  }
}


/*Function that loads the raw data from a single root file and stores them in the histograms of the class
* @par ROOT file name
*/
void TOFWallCalibration::LoadSingleFileData(TString FileName)
{
  //Open file
  TFile* InputFile = TFile::Open(FileName);
  if(InputFile->IsZombie() || !InputFile->IsOpen())
  {
    Message::DisplayFatalError(Form("Cannot open input file %s\n",FileName.Data()));
  }

  //Get data tree
  TTree* InputTree;
  InputFile->GetObject("rec_tree", InputTree);
  if(InputTree==nullptr)
  {
    Message::DisplayFatalError(Form("Error opening file: %s\n",FileName.Data()));
  }

  //Activate only the needed branches
  InputTree->SetBranchStatus("*",0);
  InputTree->SetBranchStatus("BAR_Charge",1);
  InputTree->SetBranchStatus("BAR_TOF_Uncal",1);
  InputTree->SetBranchStatus("Tags",1);

  //Set the address for some variables
  Double_t Qtree[NUMBEROFBARS], TOFtree[NUMBEROFBARS];
  TTag Tags;
  InputTree->SetBranchAddress("BAR_Charge", &Qtree[0]);
  InputTree->SetBranchAddress("BAR_TOF_Uncal", &TOFtree[0]);
  InputTree->SetBranchAddress("Tags", &Tags);

  //Variabes used to save the data in the correct histogram
  Int_t Frontbar, Rearbar, PosID, NHits;
  std::pair<Int_t, Double_t> PartEnPair;

  Message::DisplayMessage(Form("Found %lld events in file %s", InputTree->GetEntries(), FileName.Data()));

  //Set the beam type for the opened file and check if it was found in the MC table
  //If not, throw an error
  InputTree->GetEntry(0);
  PartEnPair = std::make_pair(Tags.PrimaryParticle, Tags.BeamEnergy);
  if(std::find(_PartEnPairsMC.begin(), _PartEnPairsMC.end(), PartEnPair) == _PartEnPairsMC.end())
  {
    Message::DisplayFatalError(Form("Found data for beam missing in the MC table: PartID %d\t En = %.1lf MeV/u", PartEnPair.first, PartEnPair.second));
  }

  //Store the beam type in the _PartEnPairsData if it was not found before
  //Used only to print out the beams found in data at run time
  if(std::find(_PartEnPairsData.begin(), _PartEnPairsData.end(), PartEnPair) == _PartEnPairsData.end())
  {
    _PartEnPairsData.push_back(PartEnPair);
  }


  for(Int_t entry=0; entry < InputTree->GetEntries(); ++entry)
  {
    InputTree->GetEntry(entry);

    NHits = 0;
    for(Int_t fb = FRONTBARMIN; fb <= FRONTBARMAX; ++fb)
    {
      //Skip if either the front or rear bar was not hit
      if(Qtree[fb] <= 2 || TOFtree[fb] <= 0){continue;}
      for(Int_t rb = REARBARMIN; rb <= REARBARMAX; ++rb)
      {
        if(Qtree[rb] <= 2 || TOFtree[rb] <= 0){continue;}

        //The position has been hit -> store bar number and increase number of hits
        NHits ++;
        Frontbar = fb;
        Rearbar = rb;
      }
    }

    //Save data to histograms if one position was hit -> clean events only!
    if(NHits == 1)
    {
      PosID = CalculatePositionID(Frontbar, Rearbar);
      _hQFrontLayer.at(PartEnPair).at(PosID)->Fill(Qtree[Frontbar]);
      _hQRearLayer.at(PartEnPair).at(PosID)->Fill(Qtree[Rearbar]);
      _hTOFFrontLayer.at(PartEnPair).at(PosID)->Fill(TOFtree[Frontbar]);
      _hTOFRearLayer.at(PartEnPair).at(PosID)->Fill(TOFtree[Rearbar]);
    }
  }

  InputFile->Close();
}


/*Function used to initialize the histograms of a new Particle-BeamEnergy pair
* @par PartID-BeamEnergy pair
*/
void TOFWallCalibration::InitHists()
{
  //Cycle on the beams found in the MC table
  for(auto itBeam = _PartEnPairsMC.begin(); itBeam != _PartEnPairsMC.end(); ++itBeam)
  {
    //Cycle on the TW positions
    for(Int_t PosID=0; PosID < NUMBEROFTWPOSITIONS; ++PosID)
    {
      //Allocate memory for all the histograms
      _hQFrontLayer[*itBeam].push_back(new TH1D(Form("hQFront-PartID%d-En%.1lf-Pos%d", (*itBeam).first, (*itBeam).second, PosID), Form("hQFront-PartID%d-En%.1lf-Pos%d", (*itBeam).first, (*itBeam).second, PosID), QBINS, QMIN, QMAX));

      _hQRearLayer[*itBeam].push_back(new TH1D(Form("hQRear-PartID%d-En%.1lf-Pos%d", (*itBeam).first, (*itBeam).second, PosID), Form("hQRear-PartID%d-En%.1lf-Pos%d", (*itBeam).first, (*itBeam).second, PosID), QBINS, QMIN, QMAX));

      _hTOFFrontLayer[*itBeam].push_back(new TH1D(Form("hTOFFront-PartID%d-En%.1lf-Pos%d", (*itBeam).first, (*itBeam).second, PosID), Form("hTOFFront-PartID%d-En%.1lf-Pos%d", (*itBeam).first, (*itBeam).second, PosID), TOFBINS, TOFMIN, TOFMAX));

      _hTOFRearLayer[*itBeam].push_back(new TH1D(Form("hTOFRear-PartID%d-En%.1lf-Pos%d", (*itBeam).first, (*itBeam).second, PosID), Form("hTOFRear-PartID%d-En%.1lf-Pos%d", (*itBeam).first, (*itBeam).second, PosID), TOFBINS, TOFMIN, TOFMAX));
    }
  }
}


/*Function needed to load the MC reference values for TW calibration
* @par MC table with fields:
* - campaign name (string) (to be added?)
* - particle ID (string)
* - beam energy (Double_t, MeV/u)
* - mean dE_MC in the front and rear layer (Double_t, MeV)
* - mean TOF_MC in the front and rear layer (Double_t, ns)
*/
void TOFWallCalibration::LoadMCRefValues(TString MCRefValuesFile)
{
  Int_t partType;
  Double_t BeamEn, dEf, dEr, TOFf, TOFr;

  //Open the MC table
  FILE* fMCRefValues = fopen(MCRefValuesFile, "r");
  if(fMCRefValues == NULL)
  {
    Message::DisplayFatalError("MC Table not found!");
  }

  Message::DisplayMessageWithEmphasys("MC reference values for Calibration");
  Message::DisplayMessage("pID\tBeamEn\tdEf\tdEr\tTOFf\tTOFr");
  Message::DisplayMessage(" \t[MeV/u]\t[MeV]\t[MeV]\t[ns]\t[ns]");
  Message::DisplayMessage("----------------------------------------------------");

  Char_t line[100];
  std::pair<Int_t, Double_t> PartEnPair;
  while(fgets(line, sizeof(line), fMCRefValues))
  {
    if(line[0] == '#' || line[0] == '\n'){continue;}
    std::sscanf(line, "%d %lf %lf %lf %lf %lf", &partType, &BeamEn, &dEf, &dEr, &TOFf, &TOFr);
    PartEnPair = std::make_pair(partType, BeamEn);
    _PartEnPairsMC.push_back(PartEnPair);
    _dERefValues[PartEnPair] = std::make_pair(dEf, dEr);
    _TOFRefValues[PartEnPair] = std::make_pair(TOFf, TOFr);

    //Print out the info from MC table
    cout << PartEnPair.first << "\t" << PartEnPair.second << "\t" << _dERefValues.at(PartEnPair).first << "\t" << _dERefValues.at(PartEnPair).second << "\t" << _TOFRefValues.at(PartEnPair).first << "\t" << _TOFRefValues.at(PartEnPair).second << endl;
  }

  fclose(fMCRefValues);
}


/*Additional function that can be used to save the raw data histograms to a
* .root file in the outuput directory
* -> CURRENTLY NOT USED! HISTOGRAMS ARE ALREADY SAVED IN DEBUG MODE
*/
void TOFWallCalibration::SaveRawHistograms(TString OutputDir)
{
  Int_t PosID;
  TString FileName = Form("%s/RawHistogramsDebug.root",OutputDir.Data());
  TFile* File = TFile::Open(FileName,"RECREATE");
  std::pair<Int_t, Double_t> PartEnPair;
  for(auto itBeam = _PartEnPairsData.begin(); itBeam != _PartEnPairsData.end(); ++itBeam)
  {
    gDirectory->mkdir(Form("PartID%d-En%.1lf", (*itBeam).first, (*itBeam).second));
    gDirectory->cd(Form("PartID%d-En%.1lf", (*itBeam).first, (*itBeam).second));
    for(PosID = 0; PosID < NUMBEROFTWPOSITIONS; ++PosID)
    {
      gDirectory->mkdir(Form("Pos-%d",PosID));
      gDirectory->cd(Form("Pos-%d",PosID));
      _hQFrontLayer.at(*itBeam).at(PosID)->Write("Q_F");
      _hQRearLayer.at(*itBeam).at(PosID)->Write("Q_R");
      _hTOFFrontLayer.at(*itBeam).at(PosID)->Write("TOF_F");
      _hTOFRearLayer.at(*itBeam).at(PosID)->Write("TOF_R");
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }
  File->Close();
}


/*Function that calcualtes the Position ID form bar numbers
* Modify if we change PosID convention
*/
Int_t TOFWallCalibration::CalculatePositionID(Int_t Frontbar, Int_t Rearbar)
{
  if(Frontbar < FRONTBARMIN || Frontbar > FRONTBARMAX)
  {
    Message::DisplayFatalError(Form("Frontbar index out of bounds (%d, %d)", FRONTBARMIN, FRONTBARMAX));
  }
  if(Rearbar < REARBARMIN || Rearbar > REARBARMAX)
  {
    Message::DisplayFatalError(Form("Rearbar index out of bounds (%d, %d)", REARBARMIN, REARBARMAX));
  }
  return (Frontbar - 20)*20 + Rearbar;
}


/*Function used to clear histogram data
*
*/
void TOFWallCalibration::ClearData()
{
  Message::DisplayMessageWithEmphasys("Clearing data");

  for(auto itBeam = _PartEnPairsMC.begin(); itBeam != _PartEnPairsMC.end(); ++itBeam)
  {
    for(Int_t PosID = 0; PosID < NUMBEROFTWPOSITIONS; ++PosID)
    {
      delete _hQFrontLayer.at(*itBeam).at(PosID);
      delete _hQRearLayer.at(*itBeam).at(PosID);
      delete _hTOFFrontLayer.at(*itBeam).at(PosID);
      delete _hTOFRearLayer.at(*itBeam).at(PosID);
    }

    _hQFrontLayer.at(*itBeam).clear();
    _hQRearLayer.at(*itBeam).clear();
    _hTOFFrontLayer.at(*itBeam).clear();
    _hTOFRearLayer.at(*itBeam).clear();
  }

  _hQFrontLayer.clear();
  _hQRearLayer.clear();
  _hTOFFrontLayer.clear();
  _hTOFRearLayer.clear();

  Message::DisplayMessageWithEmphasys("Done");
}


/*Function that sets the debug mode; this mode saves all the raw data histograms
* in a .root file inside the output directory
*/
void TOFWallCalibration::SetDebugMode()
{
  _Debug = true;
}
