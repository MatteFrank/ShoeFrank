/***************************************************************************
 *   Copyright (C) 2004-2006 by Olivier Stezowski & Christophe Theisen     *
 *   stezow(AT)ipnl.in2p3.fr, christophe.theisen(AT)cea.fr                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "TPGspectrumCalib.hxx"

#include "TMath.h"
#include "TAxis.h"
#include "TLine.h"

#include <iostream>
#include <fstream>

using namespace std;

// ROOT dictionnary
ClassImp(TPGspectrumCalib);

//__________________________________________________________________________________________________
TPGspectrumCalib::TPGspectrumCalib()
: fTabulated(0),
fRaw(0),
fCalibrator()
{
   //Defaut Constructor
}

//__________________________________________________________________________________________________
TPGspectrumCalib::~TPGspectrumCalib()
{  //Destructor
   
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::AddPoint(const Double_t channel, const Double_t energy)
{
   fRaw.Set(fRaw.GetSize()+1);
   fTabulated.Set(fTabulated.GetSize()+1);
   fRaw.AddAt(channel,fRaw.GetSize()-1);
   fTabulated.AddAt(energy,fTabulated.GetSize()-1);
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::Calibrate(TF1 *function) const
{
   if (function == 0) {
      printf("Info in <TPGspectrumCalib::Calibrate> function NOT defined !!\n");
      return;
   }
   if (function->GetNpar() > fRaw.GetSize()) {
      printf("Info in <TPGspectrumCalib::Calibrate> Nb of points < Nb of fit parameters, abort calibration !!\n");
      return;
   }
   TGraphErrors *graph = new TGraphErrors(fRaw.GetSize(),fRaw.GetArray(),fTabulated.GetArray());
   graph->Fit(function,"","",graph->GetXaxis()->GetXmin(),graph->GetXaxis()->GetXmax());
   graph->Draw("AP");
   DrawResults(function);
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::Calibrate(const char* filename, TF1 *function)
{
   if (function == 0) {
      printf("Info in <TPGspectrumCalib::Calibrate> function NOT defined !!\n");
      return;
   }
   TGraphErrors *graph = new TGraphErrors(filename);
   fTabulated.Set(graph->GetN(),graph->GetY());
   fRaw.Set(graph->GetN(),graph->GetX());
   if (function->GetNpar() > fRaw.GetSize()) {
      printf("Info in <TPGspectrumCalib::Calibrate> Nb of points < Nb of fit parameters, abort calibration !!\n");
      return;
   }
   graph->Fit(function,"","",graph->GetXaxis()->GetXmin(),graph->GetXaxis()->GetXmax());
   graph->Draw("AP");
   DrawResults(function);
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::Calibrate(TF1 *function, const Int_t nbpoints, const Double_t *raw, const Double_t *tabulated)
{
   if (function == 0) {
      printf("Info in <TPGspectrumCalib::Calibrate> function NOT defined !!\n");
      return;
   }
   TGraphErrors *graph = new TGraphErrors(nbpoints,raw,tabulated);
   fTabulated.Set(graph->GetN(),graph->GetY());
   fRaw.Set(graph->GetN(),graph->GetX());
   if (function->GetNpar() > fRaw.GetSize()) {
      printf("Info in <TPGspectrumCalib::Calibrate> Nb of points < Nb of fit parameters, abort calibration !!\n");
      return;
   }
   graph->Fit(function,"","",graph->GetXaxis()->GetXmin(),graph->GetXaxis()->GetXmax());
   graph->Draw("AP");
   DrawResults(function);
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::Calibrate(Int_t dimension)
{
   TMath::Abs(dimension);
   TString str = "[0]";
   for (Int_t i=1 ; i<dimension ; i++) {
      str += "["; str += i; str += "*"; str += "TMath::Power(x,"; str += i; str +=")";
   }
   TF1 *ftemp = new TF1("ftemp",str.Data());
   if (ftemp->GetNpar() > fRaw.GetSize()) {
      printf("Info in <TPGspectrumCalib::Calibrate> Nb of points < Nb of fit parameters, abort calibration !!\n");
      return;
   }
   TGraphErrors *graph = new TGraphErrors(fRaw.GetSize(),fRaw.GetArray(),fTabulated.GetArray());
   graph->Fit(ftemp,"","",graph->GetXaxis()->GetXmin(),graph->GetXaxis()->GetXmax());
   graph->Draw("AP");
   DrawResults(ftemp);
   
   if (ftemp) delete ftemp;
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::AlignMax(TH1 *histo,TF1 *function, const Double_t AlignValue,
                                const Bool_t invert, const Double_t xmin, const Double_t xmax)
{
   //Perform the alignment of the maximum of a TH1 on AlignValue
   //The search for maximum if performed in the range [xmin,xmax] of the histogram
   //If xmin=0, xmax=0 (by default), the search for maximum if performed in the total range of the histogram
   
   //if invert=true, all the histogram is inverted (can be usefull for TAC spectrum)
   
   //A polynomial function must be used
   // par 0 -> shift
   // par 1 -> proportional coef
   
   if (function == 0) {
      printf("Info in <TPGspectrumCalib::AlignMax> function NOT defined !!\n");
      return;
   }
   if (histo == 0) {
      printf("Info in <TPGspectrumCalib::AlignMax> histo NOT defined !!\n");
      return;
   }
   
   if (histo->GetDimension() > 1) {
      printf("Info in <TPGspectrumCalib::AlignMax> Method not yet defined for Histogram Dimension >= 2 !!\n");
      return;
   }
   
   Double_t maxvalue=0.0, shift=0.0;
   histo->GetXaxis()->SetRangeUser(xmin,xmax);
   maxvalue = histo->GetBinCenter(histo->GetMaximumBin());
   if (invert == false) {
      shift = AlignValue - maxvalue;
      function->SetParameter(0,shift);
      function->SetParameter(1,1.0);
   } else {
      shift = AlignValue + maxvalue;
      function->SetParameter(0,shift);
      function->SetParameter(1,-1.0);
   }
   DrawResults(function);
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::AlignMax(TH1 *histo,TF1 *function, const Double_t AlignValue,
                                const Double_t ProportionalCoef, const Bool_t invert, const Double_t xmin, const Double_t xmax)
{
   //Perform the alignment of the maximum of a TH1 on AlignValue with a proportional coefficient
   //The search for maximum if performed in the range [xmin,xmax] of the histogram
   //If xmin=0, xmax=0 (by default), the search for maximum if performed in the total range of the histogram
   
   //if invert=true, all the histogram is inverted (can be usefull for TAC spectrum)
   
   //A polynomial function must be used
   // par 0 -> shift
   // par 1 -> proportional coef
   
   if (function == 0) {
      printf("Info in <TPGspectrumCalib::AlignMax> function NOT defined !!\n");
      return;
   }
   if (histo == 0) {
      printf("Info in <TPGspectrumCalib::AlignMax> histo NOT defined !!\n");
      return;
   }
   
   if (histo->GetDimension() > 1) {
      printf("Info in <TPGspectrumCalib::AlignMax> Method not yet defined for Histogram Dimension >= 2 !!\n");
      return;
   }
   
   Double_t maxvalue=0.0, shift=0.0;
   histo->GetXaxis()->SetRangeUser(xmin,xmax);
   maxvalue = histo->GetBinCenter(histo->GetMaximumBin());
   if (invert == false) {
      shift = AlignValue - ProportionalCoef*maxvalue;
      function->SetParameter(0,shift);
      function->SetParameter(1,ProportionalCoef);
   } else {
      shift = AlignValue + ProportionalCoef*maxvalue;
      function->SetParameter(0,shift);
      function->SetParameter(1,-ProportionalCoef);
   }
   DrawResults(function);
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::CheckCalibration(TH1 *histo, const Double_t value, Double_t xmin, Double_t xmax, const Int_t color)
{
   // Display a histogram between the user range defined by xmin and xmax
   // A vertical TLine is drawn at value
   
   if (histo == 0)  {
      printf("Info in <TPGspectrumCalib::CheckCalibration> histo NOT defined !!\n");
      return;
   }
   if (histo->GetDimension() > 1) {
      printf("Info in <TPGspectrumCalib::CheckCalibration> Method not yet defined for Histogram Dimension >= 2 !!\n");
      return;
   }
   
   if (xmin == -1111) xmin = histo->GetXaxis()->GetXmin();
   if (xmax == -1111) xmax = histo->GetXaxis()->GetXmax();
   if (xmin > xmax) histo->GetXaxis()->SetRangeUser(xmax,xmin); else histo->GetXaxis()->SetRangeUser(xmin,xmax);
   
   //Create a new TLine
   TLine *line = new TLine();
   line->SetLineColor(color); line->SetLineStyle(2);
   histo->Draw();
   line->DrawLine(value,0.0,value,histo->GetMaximum());
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::CheckCalibration(TH1 *histo, const Int_t nbvalue, const Double_t *values,
                                        Double_t xmin, Double_t xmax, const Int_t color)
{
   //Display a histogram between the user range defined by xmin and xmax
   // nbvalue vertical TLines are drawn at values
   
   if (histo == 0) {
      printf("Info in <TPGspectrumCalib::CheckCalibration> histo NOT defined !!\n");
      return;
   }
   if (histo->GetDimension() > 1) {
      printf("Info in <TPGspectrumCalib::CheckCalibration> Method not yet defined for Histogram Dimension >= 2 !!\n");
      return;
   }
   
   if (xmin == -1111) xmin = histo->GetXaxis()->GetXmin();
   if (xmax == -1111) xmax = histo->GetXaxis()->GetXmax();
   if (xmin > xmax) histo->GetXaxis()->SetRangeUser(xmax,xmin); else histo->GetXaxis()->SetRangeUser(xmin,xmax);
   
   TArrayD array(nbvalue,values);
   histo->Draw();
   
   for (Int_t i=0 ; i<array.GetSize() ; i++) {
      TLine *line = new TLine();
      line->SetLineColor(color); line->SetLineStyle(2);
      line->DrawLine(array[i],0.0,array[i],histo->GetMaximum());
   }
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::CheckCalibration(TH1 *histo, const char *SourceName, Double_t xmin, Double_t xmax, const Int_t color)
{
   // Display a histogram between the user range defined by xmin and xmax
   // vertical TLines are drawn corresponding to SourceName (most intense peaks)
   // For the moment the sources are : Co60, Eu152, Ba133, Na22, K40
   
   TString opt = SourceName;
   opt.ToLower();
   
   Double_t *energies = 0;
   
   //Create new TLines according to the SourceName
   if ( (!opt.CompareTo("co60")) or (!opt.CompareTo("60co")) ) {
      energies = new Double_t[2];
      energies[0] = 1173.228;
      energies[1] = 1332.492;
      CheckCalibration(histo,2,energies,xmin,xmax,color);
      
   } else if ( (!opt.CompareTo("eu152")) or (!opt.CompareTo("152eu")) ) {
      energies = new Double_t[7];
      energies[0] = 121.7830;
      energies[1] = 244.6920;
      energies[2] = 344.2760;
      energies[3] = 778.9030;
      energies[4] = 964.1310;
      energies[5] = 1112.1160;
      energies[6] = 1408.0110;
      CheckCalibration(histo,7,energies,xmin,xmax,color);
   } else if ( (!opt.CompareTo("ba133")) or (!opt.CompareTo("133ba")) ) {
      energies = new Double_t[4];
      energies[0] = 80.9971;
      energies[1] = 276.3997;
      energies[2] = 302.8510;
      energies[3] = 356.0134;
      CheckCalibration(histo,4,energies,xmin,xmax,color);
   } else if ( (!opt.CompareTo("na22")) or (!opt.CompareTo("22na")) ) {
      energies = new Double_t[2];
      energies[0] = 511.0;
      energies[1] = 1274.537;
      CheckCalibration(histo,2,energies,xmin,xmax,color);
   } else if ( (!opt.CompareTo("k40")) or (!opt.CompareTo("40k")) ) {
      energies = new Double_t[1];
      energies[0] = 1311.07;
      CheckCalibration(histo,1,energies,xmin,xmax,color);
   } else if ( (!opt.CompareTo("y88")) or (!opt.CompareTo("88y")) ) {
      energies = new Double_t[2];
      energies[0] = 898.042;
      energies[1] = 1836.063;
      CheckCalibration(histo,2,energies,xmin,xmax,color);
   } else {
      printf("Info in <TPGspectrumCalib::CheckCalibration> This source is not yet defined - use the manual method !!\n");
   }
   
   if (energies != 0) delete [] energies;
   
}

//__________________________________________________________________________________________________
void TPGspectrumCalib::DrawResults(const TF1 *function) const
{
   if (function == 0) {
      printf("Info in <TPGspectrumCalib::DrawResults> function NOT defined !!\n");
      return;
   }
   
   printf("***************************************\n");
   printf("***     RESULTS FROM CALIBRATION    ***\n");
   printf("***************************************\n");
   for (Int_t i=0 ; i<function->GetNpar() ; i++) {
      printf("* par #%3i = %10.5f +- %10.5f *\n",i,function->GetParameter(i),function->GetParError(i));
   }
   printf("***************************************\n");
}


