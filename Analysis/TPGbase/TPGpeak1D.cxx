/***************************************************************************
 *   Copyright (C) 2004 by Olivier Stezowski, B. Rosse & Christian Finck   *
 *   stezow(AT)ipnl.in2p3.fr, cfinck(AT)iphc.cnrs.fr                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modif   *
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

#include "TF1.h"
#include "TH1.h"
#include "TList.h"
#include "TMarker.h"
#include "TMath.h"
#include "TPad.h"
#include "TMarker.h"
#include "TROOT.h"

#include "TPGpadManager.hxx"
#include "TPGpeak1D.hxx"

#include <iostream>

// to work with FWHM instead of sigma
#define SigmaToFWHM 2.35482

using namespace std;

ClassImp(TPGpeak1D);

const Int_t   TPGpeak1D::fgkPointsPeak   = 7;
const Int_t   TPGpeak1D::fgkPointsBkg    = 4;
const Int_t   TPGpeak1D::fgkPeakIdx      = 3;

Style_t TPGpeak1D::fgFillStylePeak = 3005;
Color_t TPGpeak1D::fgLineColorBkg  = 39;
Style_t TPGpeak1D::fgFillStyleBkg  = 3001;
Color_t TPGpeak1D::fgMarkerColor   = 36;
Color_t TPGpeak1D::fgFuncColor     = 26;


//__________________________________________________________
TPGpeak1D::TPGpeak1D() :
	TPGbasePeak("Peak","TPGpeak1D"),
	fPosition(0.0), 
	fFWHM(0.0),
	fBkgLeft1(0.0),
	fBkgLeft2(0.0),
	fBkgRight1(0.0),
	fBkgRight2(0.0),
	fBkgLevelLeft1(0.0),
	fBkgLevelLeft2(0.0),
	fBkgLevelRight1(0.0),
	fBkgLevelRight2(0.0),
   fBkgIntegral(0.0),
   fPeakIntegral(0.0),
   fSubPeakIntegral(0.0),
	fBkgFlag(false),
	fPolyLinePeak(new TPolyLine(fgkPointsPeak)),
	fPolyLineBkgL(new TPolyLine(fgkPointsBkg)),
	fPolyLineBkgR(new TPolyLine(fgkPointsBkg)),
	fCPolyline(0x0),
	fCopyPolyline(),
	fMarkerList(new TList()),
	fDrawAs(kPeak),
	fSigFunc(0x0),
	fBkgFunc(0x0),
	fPeakFunc(0x0)
{
	//Default Constructor
	fDimension = 1;
	
	//reset polyline
	Double_t x[fgkPointsPeak] = {0,0,0,0,0,0,0};
	Double_t y[fgkPointsPeak] = {0,0,0,0,0,0,0};
	fPolyLinePeak->SetPolyLine(fgkPointsPeak, x, y); 
	fPolyLinePeak->SetLineWidth(2);
	
	Double_t xb[fgkPointsBkg] = {0,0,0,0};
	Double_t yb[fgkPointsBkg] = {0,0,0,0};
	fPolyLineBkgL->SetPolyLine(fgkPointsBkg, xb, yb); 
	fPolyLineBkgL->SetLineWidth(2);
	
	fPolyLineBkgR->SetPolyLine(fgkPointsBkg, xb, yb); 
	fPolyLineBkgR->SetLineWidth(2);
	
	// set default line color
	SetLineColorPeak(fgGateColor.k0);
	SetLineColorBkg(fgLineColorBkg);
	
	//set default fill style
	SetFillStylePeak(fgFillStylePeak);
	SetFillStyleBkg(fgFillStyleBkg);
	
	// set default fill color
	SetFillColorPeak(fgGateColor.k0);
	SetFillColorBkg(fgLineColorBkg);
	
	// Add markers
	InitPeakMarkers();
}

//__________________________________________________________
TPGpeak1D::TPGpeak1D(const char* name, const char* title) :
	TPGbasePeak(name, title),
	fPosition(0.0), 
	fFWHM(0.0),
	fBkgLeft1(0.0),
	fBkgLeft2(0.0),
	fBkgRight1(0.0),
	fBkgRight2(0.0),
	fBkgLevelLeft1(0.0),
	fBkgLevelLeft2(0.0),
	fBkgLevelRight1(0.0),
	fBkgLevelRight2(0.0),
	fBkgFlag(false),
	fPolyLinePeak(new TPolyLine(fgkPointsPeak)),
	fPolyLineBkgL(new TPolyLine(fgkPointsBkg)),
	fPolyLineBkgR(new TPolyLine(fgkPointsBkg)),
	fCPolyline(0x0),
	fCopyPolyline(),
	fMarkerList(new TList()),
	fDrawAs(kPeak),
	fSigFunc(0x0),
	fBkgFunc(0x0),
	fPeakFunc(0x0)
{
  //Default Constructor
  fDimension = 1;
  
  //reset polyline
  Double_t x[fgkPointsPeak] = {0,0,0,0,0,0,0};
  Double_t y[fgkPointsPeak] = {0,0,0,0,0,0,0};
  fPolyLinePeak->SetPolyLine(fgkPointsPeak, x, y); 
  fPolyLinePeak->SetLineWidth(2);
  
  Double_t xb[fgkPointsBkg] = {0,0,0,0};
  Double_t yb[fgkPointsBkg] = {0,0,0,0};
  fPolyLineBkgL->SetPolyLine(fgkPointsBkg, xb, yb); 
  fPolyLineBkgL->SetLineWidth(2);
  
  fPolyLineBkgR->SetPolyLine(fgkPointsBkg, xb, yb); 
  fPolyLineBkgR->SetLineWidth(2);
  
  // set default line color
  SetLineColorPeak(fgGateColor.k0);
  SetLineColorBkg(fgLineColorBkg);
  
  //set default fill style
  SetFillStylePeak(fgFillStylePeak);
  SetFillStyleBkg(fgFillStyleBkg);
  
  // set default fill color
  SetFillColorPeak(fgGateColor.k0);
  SetFillColorBkg(fgLineColorBkg);
  
  // Add markers
  InitPeakMarkers();
}

//__________________________________________________________
TPGpeak1D::TPGpeak1D(TPolyLine* polyline) :
	TPGbasePeak("Peak","TPGpeak1D"),
	fPosition(0.0), 
	fFWHM(0.0),
	fBkgLeft1(0.0),
	fBkgLeft2(0.0),
	fBkgRight1(0.0),
	fBkgRight2(0.0),
	fBkgLevelLeft1(0.0),
	fBkgLevelLeft2(0.0),
	fBkgLevelRight1(0.0),
	fBkgLevelRight2(0.0),
	fBkgFlag(false),
	fPolyLinePeak(0x0),
	fPolyLineBkgL(new TPolyLine(fgkPointsBkg)),
	fPolyLineBkgR(new TPolyLine(fgkPointsBkg)),
	fCPolyline(0x0),
	fCopyPolyline(),
	fMarkerList(new TList()),
	fDrawAs(kPeak),
	fSigFunc(0x0),
	fBkgFunc(0x0),
	fPeakFunc(0x0)
{
	//Default Constructor
	fDimension = 1;

	//set peak polyline
	SetPeakPoints(polyline);
	UpdatePeak();

	Double_t xb[fgkPointsBkg] = {0,0,0,0};
	Double_t yb[fgkPointsBkg] = {0,0,0,0};
	fPolyLineBkgL->SetPolyLine(fgkPointsBkg, xb, yb); 
	fPolyLineBkgL->SetLineWidth(2);

	fPolyLineBkgR->SetPolyLine(fgkPointsBkg, xb, yb); 
	fPolyLineBkgR->SetLineWidth(2);

	// set default line color
	SetLineColorPeak(fgGateColor.k0);
	SetLineColorBkg(fgLineColorBkg);

	//set default fill style
	SetFillStylePeak(fgFillStylePeak);
	SetFillStyleBkg(fgFillStyleBkg);

	// set default fill color
	SetFillColorPeak(fgGateColor.k0);
	SetFillColorBkg(fgLineColorBkg);

	// Add markers
	InitPeakMarkers();
}

/*
//__________________________________________________________
TPGpeak1D::TPGpeak1D(const TPGpeak1D& p) : TPGbasePeak(p),
	fPosition(p.fPosition),
	fFWHM(p.fFWHM),
	fBkgLeft1(p.fBkgLeft1),
	fBkgLeft2(p.fBkgLeft2),
	fBkgRight1(p.fBkgRight1),
	fBkgRight2(p.fBkgRight2),
	fBkgLevelLeft1(p.fBkgLevelLeft1),
	fBkgLevelLeft2(p.fBkgLevelLeft2),
	fBkgLevelRight1(p.fBkgLevelRight1),
	fBkgLevelRight2(p.fBkgLevelRight2),
	fBkgFlag(p.fBkgFlag),
	fDrawAs(p.fDrawAs),
	fLog("TPGpeak1D")
{
	// copy constructor
	fPolyLinePeak   = new TPolyLine(p.fgkPointsPeak);
	fPolyLinePeak->Copy(*p.fPolyLinePeak);

	fPolyLineBkgL   = new TPolyLine(p.fgkPointsBkg);
	fPolyLineBkgL->Copy(*p.fPolyLineBkgL);

	fPolyLineBkgR   = new TPolyLine(p.fgkPointsBkg);
	fPolyLineBkgR->Copy(*p.fPolyLineBkgR);

	fSigFunc    = new TF1(*p.fSigFunc);
	fBkgFunc    = new TF1(*p.fBkgFunc);
	fPeakFunc   = new TF1(*p.fPeakFunc);
	fMarkerList = new TList(p.fMarkerList);
}

//__________________________________________________________
TPGpeak1D& TPGpeak1D::operator=(const TPGpeak1D& p)
{
	// assignment operator
	if (this == &p) 
		return *this;
	TPGbasePeak::operator=(p);

	fPolyLinePeak    = p.fPolyLinePeak;
	fPolyLineBkgL    = p.fPolyLineBkgL;
	fPolyLineBkgR    = p.fPolyLineBkgR;
	fMarkerList  = p.fMarkerList;
	fSigFunc    =  p.fSigFunc;
	fBkgFunc    =  p.fBkgFunc;
	fPeakFunc    = p.fPeakFunc;
	fPosition    = p.fPosition;
	fFWHM        = p.fFWHM;
	fBkgLeft1    = p.fBkgLeft1;
	fBkgLeft2    = p.fBkgLeft2;
	fBkgRight1   = p.fBkgRight1;
	fBkgRight2   = p.fBkgRight2;
	fBkgLevelLeft1    = p.fBkgLevelLeft1;
	fBkgLevelLeft2    = p.fBkgLevelLeft2;
	fBkgLevelRight1   = p.fBkgLevelRight1;
	fBkgLevelRight2   = p.fBkgLevelRight2;	
	fBkgFlag     = p.fBkgFlag; 
	fDrawAs      = p.fDrawAs;
	
	return *this;
}
 */

//__________________________________________________________
TPGpeak1D::~TPGpeak1D()
{
	// default destructor
	delete fPolyLinePeak;
	delete fPolyLineBkgL;
	delete fPolyLineBkgR;
	
	fMarkerList->Delete();
	delete fMarkerList;
	
	delete fSigFunc;
	delete fBkgFunc;
	delete fPeakFunc;
}


//__________________________________________________________
Double_t TPGpeak1D::Bkg(Double_t* x, Double_t* par)
{
	// reject point outside the backgorund limits
	if ( x[0] < fBkgLeft1 || x[0] > fBkgRight2 ) {
		TF1::RejectPoint();
		
		return 0;
	}
	if ( x[0] > fBkgLeft2 && x[0] < fBkgRight1 ) {
		TF1::RejectPoint();
		
		return 0;
	}	
	return fBkgFunc->EvalPar(x, par);
}

//__________________________________________________________
Double_t TPGpeak1D::SignalAndBkg(Double_t* x, Double_t* par)
{
	// Total function  
	Double_t f = fSigFunc->EvalPar(x, par); 
	
	if ( fBkgFunc ) 
		f += fBkgFunc->EvalPar(x, par + fSigFunc->GetNpar()); 
	
	return f;
}

//__________________________________________________________
TF1 *TPGpeak1D::SignalAndBkgFunction()
{
	// if already set 
	if (fPeakFunc)
		delete fPeakFunc;
	
	// now set the global function
   Double_t *allpar = new Double_t[fSigFunc->GetNpar()+fBkgFunc->GetNpar()];
	
	::memcpy(allpar, fSigFunc->GetParameters(), fSigFunc->GetNpar()*sizeof(Double_t));
	::memcpy(allpar + fSigFunc->GetNpar(), fBkgFunc->GetParameters(), fBkgFunc->GetNpar()*sizeof(Double_t));

	TString tmp = Form("SigAndBkg_%p",this);
	fPeakFunc = new TF1(tmp.Data(), this, &TPGpeak1D::SignalAndBkg, fBkgLeft1, fBkgRight2, fSigFunc->GetNpar()+fBkgFunc->GetNpar(), "TPGpeak1D", "SignalAndBkg");
   fPeakFunc->SetParameters(allpar);
	delete[] allpar;	
	
	fPeakFunc->SetBit(TObject::kCannotPick); 
	gROOT->GetListOfFunctions()->Remove(fPeakFunc);
	
	fPeakFunc->SetLineColor(fgFuncColor);
	return fPeakFunc;
}

//__________________________________________________________
void TPGpeak1D::WithBkg(Bool_t with_bg)
{
	fBkgFlag = with_bg;
}

//__________________________________________________________
void TPGpeak1D::SetPeak(Double_t position, Double_t height, Double_t FWHM, Double_t intensity)
{
	// constructor for members
	fPosition  = position;
	fFWHM      = FWHM;
	fHeight    = height;
	fIntensity = intensity;

	SetModified((*fPolyLinePeak));
}

//__________________________________________________________
void TPGpeak1D::SetPeakFromFit()
{
	fHeight    = fSigFunc->GetParameter(0);
	fPosition  = fSigFunc->GetParameter(1);
	fFWHM      = fSigFunc->GetParameter(2);
	fIntensity = fSigFunc->Integral(fBkgLeft1,fBkgRight2);
	
	// modified 
	SetPeakPoints();	
}

//__________________________________________________________
void TPGpeak1D::SetPosition(Double_t position, Option_t* /*axis*/)
{ 
	// set position
	fPosition = position; 

	SetModified((*fPolyLinePeak));
} 

//__________________________________________________________
void TPGpeak1D::SetFWHM(Double_t FWHM, Option_t* /*axis*/)
{ 
	if ( FWHM <= 0.0 )
		return;
	
	// set FWHM
	fFWHM = FWHM; 

	SetModified((*fPolyLinePeak));
} 

//__________________________________________________________
void TPGpeak1D::SetHeight(Double_t height)
{ 
	if ( height <= 0.0 )
		return;
	
	// set height
	TPGbasePeak::SetHeight(height); 

	SetModified((*fPolyLinePeak));
} 

//__________________________________________________________
void TPGpeak1D::SetIntensity(Double_t intensity)
{ 
	if ( intensity <= 0.0 )
		return;
	
	// set height
	TPGbasePeak::SetIntensity(intensity); 

	SetModified((*fPolyLinePeak));
} 

//__________________________________________________________
void TPGpeak1D::SetBackground( Double_t bgLeft1,  Double_t bgLeft2,  Double_t bgRight1,  Double_t bgRight2,
									 Double_t bgLevelLeft1, Double_t bgLevelLeft2, Double_t bgLevelRight1,Double_t bgLevelRight2)
{
	// set bkg limits left & right from the peak
	fBkgLeft1  = bgLeft1;
	fBkgLeft2  = bgLeft2;
	fBkgRight1 = bgRight1;
	fBkgRight2 = bgRight2;

	fBkgLevelLeft1  = bgLevelLeft1;
	fBkgLevelLeft2  = bgLevelLeft2;
	fBkgLevelRight1 = bgLevelRight1;
	fBkgLevelRight2 = bgLevelRight2;
	
	SetModified((*fPolyLineBkgL));
	SetModified((*fPolyLineBkgR));
}

//__________________________________________________________
void TPGpeak1D::SetBackground( Double_t bgLeft1, Double_t bgLeft2, Double_t bgRight1, Double_t bgRight2, const TH1 *h )
{
	SetBackground(bgLeft1, 
					  bgLeft2, 
					  bgRight1, 
					  bgRight2,
					  h->GetXaxis()->FindFixBin(bgLeft1),
					  h->GetXaxis()->FindFixBin(bgLeft2),
					  h->GetXaxis()->FindFixBin(bgRight1),
					  h->GetXaxis()->FindFixBin(bgRight2) );
}

//__________________________________________________________
void TPGpeak1D::ShiftPolyline(TPolyLine &poly, Double_t x, Double_t y)
{
	for (Int_t i = 0; i < poly.GetN(); i++) {
		poly.GetX()[i] += x;
		poly.GetY()[i] += y;
	}
}

//__________________________________________________________
void TPGpeak1D::SetAllPointsinPolyline(TPolyLine &poly, Double_t x, Double_t y)
{
	for (Int_t i = 0; i < poly.GetN(); i++) {
		poly.GetX()[i] = x;
		poly.GetY()[i] = y;
	}	
}

//__________________________________________________________
void TPGpeak1D::SetMarkerColor(Int_t color)
{
	// set marker color
	TMarker* m = 0x0;
	for (Int_t i = 0; i < fMarkerList->GetEntries(); ++i) {
		m  = static_cast<TMarker*> (fMarkerList->At(i));
		m->SetMarkerColor(color);
	}
}

//__________________________________________________________
TF1 *TPGpeak1D::SetSignalFunction(const TF1* func)
{
  // set user signal function
   
  if (func != 0x0) { //user defined function
    
	  Bool_t ok = true;

	  // check it is an expected kind of function
	  TString name = func->GetParName(0);
	  name.ToLower();
	  if (name != "height") {
		  Error("SetSignalFunction()", "Wrong name or bad index for parameter 0 (height expected)");
		  ok = false;
	  }
	  name = func->GetParName(1);
	  name.ToLower();
	  if (name != "position") {
		   Error("SetSignalFunction()", "Wrong name or bad index for parameter 1 (position expected)");
		  ok = false;		  
	  }
	  name = func->GetParName(2);
	  name.ToLower();
	  if (name != "fwhm") {
		  Error("SetSignalFunction()", "Wrong name or bad index for parameter 2 (FWHM expected)");
		  ok = false;
	  }
     if ( ok == false ) {        
        return fSigFunc;
     }
	
   // try to build the new function
   TF1 *tmp = new TF1(*func);

	  name = Form("%s_Sig_%p",func->GetName(),this);
	  tmp->SetName(name.Data());
	  
	  // check if formula is correct
//     if ( tmp->Compile() ) {
//        Error("SetSignalFunction()",  "Bad formula for function");
//       delete tmp;
//
//       return fSigFunc;
//     }
     
	  if (fSigFunc) // delete previous fit
		  delete fSigFunc;
	  
	  fSigFunc = tmp;
	  gROOT->GetListOfFunctions()->Remove(fSigFunc); 

  } else
 Error("SetSignalFunction()", "No function defined");
	

	return fSigFunc;
}
//__________________________________________________________
TF1 *TPGpeak1D::SetSignalFunction(const char* nameFunc)
{
	// set pre-defined signal function
	Double_t xmin = 0, xmax = 0;

	// check first it has not yet been set with the same type
	TString name = Form("%s_Sig_%p",nameFunc,this);

	// check if it is needed to creat a new function
	Bool_t do_new_func = true;
	if ( fSigFunc ) {
		if ( name == fSigFunc->GetName() ) {
			do_new_func = false;
		}
		else {
			delete fSigFunc; fSigFunc = 0x0;
		}
	}
	if  (do_new_func ) {
		if (name.Contains("gaus")) { // gaus function
			
			xmin = fPosition - 3*fFWHM; xmax = fPosition + 3*fFWHM;
			
			fSigFunc = new TF1(name.Data(), Form("[0]*exp(-0.5*((x-[1])/([2]/%f))**2)",SigmaToFWHM), xmin, xmax);
			fSigFunc->SetParName(0, "Height");
			fSigFunc->SetParName(1, "Position");
			fSigFunc->SetParName(2, "FWHM");
			
			fSigFunc->SetBit(TObject::kCannotPick);
			gROOT->GetListOfFunctions()->Remove(fSigFunc); 
		} 
		else {
			if (name.Contains("land")) { // landau function
			
				xmin = fPosition - 3*fFWHM; xmax = fPosition + 10*fFWHM;
			
				fSigFunc = new TF1(name.Data(), Form("(8*[0]*TMath::Log(2))*TMath::Landau(x, [1], [2]/%f, 0)",SigmaToFWHM), xmin, xmax);
				fSigFunc->SetParName(0, "Height");
				fSigFunc->SetParName(1, "MPV");
				fSigFunc->SetParName(2, "FWHM");  
			
				fSigFunc->SetBit(TObject::kCannotPick);
				gROOT->GetListOfFunctions()->Remove(fSigFunc); 
			} 
			else {
				 Error("SetSignalFunction()",  "Function not defined");
			}
		}
	}
	if ( fSigFunc ) {
			fSigFunc->SetParameter(0, fHeight);
			fSigFunc->SetParameter(1, fPosition);
			fSigFunc->SetParameter(2, fFWHM);
			
			fSigFunc->SetRange(xmin,xmax);
	}
	return fSigFunc;
}
	
//__________________________________________________________
TF1 *TPGpeak1D::SetBkgFunction(const TF1* func)
{
	// set user background function
	// the user has to provide the initial parameters
	// fit is done within the background limits
   
	if (func != 0x0) { //user defined function
			
			// try to build the new function
			TF1 *tmp = (TF1*)func->Clone();
			tmp->SetName(Form("%s_Bkg_%p",func->GetName(),this));
			
			// check if formula is correct
//         if ( tmp->Compile() ) {
//            Error("SetBkgFunction()","Bad formula for function");
//            delete tmp;
//
//            return fBkgFunc;
//         }
			
			if (fBkgFunc) // delete previous fit
				delete fBkgFunc;
			
			fBkgFunc = tmp;
			fBkgFunc->SetBit(TObject::kCannotPick);
			gROOT->GetListOfFunctions()->Remove(fBkgFunc); 
			
	} else  Error("SetBkgFunction()", "No function defined");
   
	return fBkgFunc;
}

//__________________________________________________________
TF1 *TPGpeak1D::SetBkgFunction(const char* nameFunc)
{		
	TString namef = Form("%s_Bkg_%p",nameFunc,this);
	
	// check first if the function has not yet been allocated with the right type 
	// or if it could be modified
	Bool_t do_new_func = true;
	if ( fBkgFunc ) {
		TString tmp = Form("%s_Bkg_%p",fBkgFunc->GetName(),this);

		if ( tmp == namef ) {
			do_new_func = false;
		}
		else { delete fBkgFunc; fBkgFunc = 0x0; }
	}
	// no need to change it !
	if ( do_new_func == false )
		return fBkgFunc;
	
	// list of pre defined functions
	if ( namef.Contains("cst") || namef.Contains("pol0") || namef == "" || namef.Contains("-") )  {
		fBkgFunc = new TF1(namef.Data(), "pol0", fBkgLeft1, fBkgRight2);
		fBkgFunc->SetParameter(0,0.0); // so that is can be used in case no background substraction is required
	}
	if ( namef.Contains("lin") || namef.Contains("pol1") )  
		fBkgFunc = new TF1(namef.Data(), "pol1", fBkgLeft1, fBkgRight2);   
	
	if ( namef.Contains("quad") || namef.Contains("pol2") ) 
		fBkgFunc = new TF1(namef.Data(), "pol2", fBkgLeft1, fBkgRight2);
	// to do ... step function
	
	// try directly to compile the given expression
	if ( fBkgFunc == 0x0 ) {
		fBkgFunc = new TF1(namef.Data(), nameFunc, fBkgLeft1, fBkgRight2);	
		// check if formula is correct
//      if ( fBkgFunc->Compile() ) {
//         Error("SetBkgFunction()", "Bad formula for function");
//         delete fBkgFunc; fBkgFunc = 0x0;
//      }
	}
	if ( fBkgFunc == 0x0 ) {
		fBkgFunc = new TF1("pol0_Bkg", "pol0", fBkgLeft1, fBkgRight2);
		fBkgFunc->SetParameter(0,0.0); 
		Warning("SetBkgFunction", "Cannot define %s , pol0 is set instead", namef.Data());
	}
	fBkgFunc->SetBit(TObject::kCannotPick);
	gROOT->GetListOfFunctions()->Remove(fBkgFunc); 
 
	return fBkgFunc;
}		

//__________________________________________________________
Double_t TPGpeak1D::GetPosition(Option_t* /*axis*/) const
{ 
  // return position for given axis
  return fPosition; 
}

//__________________________________________________________
Double_t TPGpeak1D::GetFWHM(Option_t* /*axis*/) const
{ 
  // return FWHM for given axis
  return fFWHM; 
}

//__________________________________________________________
TMarker* TPGpeak1D::GetMarker(Int_t markerId)
{
  // get marker
  if ( markerId >= fMarkerList->GetEntries() ) {
    Error("GetMarker()", "Index out of range");
  }
  
  return ( static_cast<TMarker*> (fMarkerList->At(markerId)) );
}

//__________________________________________________________
void TPGpeak1D::FitMenu(const char* nameFunc, Option_t* optFit, Option_t* optBkg)
{
	
	// check the current pad owns the peak
	if ( TVirtualPad::Pad() == 0x0 )
		return;
	// check the current pad owns the peak
	if ( TVirtualPad::Pad()->FindObject(this) == 0x0 )
		return;
	
	TH1 *h = TPGpadManager::GetHisto(TVirtualPad::Pad());
	if (h == 0x0) {
		Error("FitMenu()", "Histogram not defined");
		return;
	}		
	// set signal function
	SetSignalFunction(nameFunc);

	// look for other peaks in the pad in case fit combined is required
	std::vector<TPGpeak1D *> allpeaks;
	TPGpadManager::Collect<TPGpeak1D>(allpeaks,TVirtualPad::Pad());
	
	TList peaks; peaks.SetOwner(false);
	//
	for (size_t i = 0; i < allpeaks.size(); i++) {
		if ( allpeaks[i] == this )
			continue;
		if ( IsPointInBkg(allpeaks[i]->GetPosition()) < 1  ) {
			allpeaks[i]->SetSignalFunction(nameFunc);
			peaks.Add(allpeaks[i]); 
		}
	}
	if ( peaks.GetSize() ) {
		Warning("FitMenu()",  "COMBINE called because several peaks are between this peaks limits : #%d ", peaks.GetSize());
		FitCombined(h, &peaks, optFit, optBkg);
	}
	else 
		Fit(h, optFit, optBkg);
	//
	TIter next(&peaks); TObject *obj;
	while ( (obj = next()) ) {
		peaks.Remove(obj);
	}
}

//__________________________________________________________
void TPGpeak1D::Area(Option_t * opt)
{
	TString Opt(opt); TH1 *h; Short_t choice = 0;
		
	if ( Opt.Contains("h") ) {
		if ( ( h = TPGpadManager::GetHisto(TVirtualPad::Pad()) ) ) // only if there is an histogram in the current pad
			 choice = 1;
	}
   
   if ( Opt.Contains("f") ) {
	  if (fSigFunc != 0x0 && fBkgFunc != 0x0)
		 choice = 2;	
   }
	
   if ( Opt.Contains("g") ) { // get area when peak is drawn as a gate
	  if ( ( h = TPGpadManager::GetHisto(TVirtualPad::Pad()) ) ) // only if there is an histogram in the current pad
		 choice = 3;
   }
	Double_t position = 0.0, sig = 0.0, sig_bg = 0.0, bg1 = 0.0, bg2 = 0.0, bg_sig = 0.0;

	switch ( choice ) {
		case 1:
	   {	//
		    sig_bg = h->Integral(h->GetXaxis()->FindFixBin(fBkgLeft2),h->GetXaxis()->FindFixBin(fBkgRight1));
			//
			bg1 = h->Integral(h->GetXaxis()->FindFixBin(fBkgLeft1),h->GetXaxis()->FindFixBin(fBkgLeft2));
			bg2 = h->Integral(h->GetXaxis()->FindFixBin(fBkgRight1),h->GetXaxis()->FindFixBin(fBkgRight2));
			
			// loop on bins into bg limits to compute sig by substracting an extrapolation of bg
			Double_t slope  = (fBkgLevelLeft2-fBkgLevelLeft1) / (fBkgLeft2-fBkgLeft1);
			Double_t offset = fBkgLevelLeft1 - slope*fBkgLeft1;
			//
			Double_t sum = 0.0;
			position = 0.0; 
			sig = 0.0;
			for (Int_t i = h->GetXaxis()->FindFixBin(fBkgLeft2); i < h->GetXaxis()->FindFixBin(fBkgRight1); i++) {
				
				Double_t bin_cont = h->GetBinContent(i);
				Double_t bin_cent = h->GetBinCenter(i);
				
				sig      += ( bin_cont - bin_cent*slope - offset );
			    bg_sig   += bin_cent*slope + offset;
				position += bin_cont*bin_cent;
				sum		 += bin_cont;
				
			}
			if ( sum )
				position /= sum;
			else {
				position = 0.0;
			}
	   }
		  break;
		  
		case 2:
	   {
		  sig = fSigFunc->Integral(fPosition-4*fFWHM, fPosition+4*fFWHM);
		  bg1 = fBkgFunc->Integral(fBkgLeft1, fBkgLeft2);
		  bg2 = fBkgFunc->Integral(fBkgRight1, fBkgRight2);
		  position = fPosition;
	   }
		  break;
		  
	   case 3:
	   {
		  //
		  sig_bg = h->Integral(h->GetXaxis()->FindFixBin(fPosition-fFWHM/2.),h->GetXaxis()->FindFixBin(fPosition+fFWHM/2.));
		  //
		  bg1 = h->Integral(h->GetXaxis()->FindFixBin(fBkgLeft1),h->GetXaxis()->FindFixBin(fBkgLeft2));
		  bg2 = h->Integral(h->GetXaxis()->FindFixBin(fBkgRight1),h->GetXaxis()->FindFixBin(fBkgRight2));
		  
		  // loop on bins into bg limits to compute sig by substracting an extrapolation of bg
		  Double_t slope  = (fBkgLevelLeft2-fBkgLevelLeft1) / (fBkgLeft2-fBkgLeft1);
		  Double_t offset = fBkgLevelLeft1 - slope*fBkgLeft1;
		  //
		  Double_t sum = 0.0;
		  position = 0.0; 
		  sig = 0.0;
		  for (Int_t i = h->GetXaxis()->FindFixBin(fPosition-fFWHM/2.); i < h->GetXaxis()->FindFixBin(fPosition+fFWHM/2.); i++) {
			 
			 Double_t bin_cont = h->GetBinContent(i);
			 Double_t bin_cent = h->GetBinCenter(i);
			 
			 sig      += ( bin_cont - bin_cent*slope - offset );
			 bg_sig   += bin_cent*slope + offset;
			 position += bin_cont*bin_cent;
			 sum	  += bin_cont;
		  }
		  if ( sum )
			 position /= sum;
		  else {
			 position = 0.0;
		  }
	   }
		  break;
		  
		default:	 
		  break;
	}
	if ( Opt.Contains("v") ) 
		cout <<  " Mean Area_SIG Area_SIG+BG Area_BG_LEFT Area_BG_RIGHT " << endl;
	cout << position << " " << sig << " " << sig_bg << " " << bg1 << " " << bg2 << endl;
	
	fBkgIntegral     = bg1+bg2;
	fPeakIntegral    = sig;
	fSubPeakIntegral = bg_sig;
}


//__________________________________________________________
void TPGpeak1D::FitCombined(TH1 *h, TList* peakList, Option_t* optFit, Option_t* optBkg)
{	
	// just in case
	if ( peakList->GetSize() == 0 ) 
		return;
	
	// be sure the signal function is defined for all peaks 
	Bool_t is_signals = true; TPGpeak1D *peak;
	
	TIter next(peakList);
	while ( (peak = (TPGpeak1D *)next()) ) {
		if ( peak->SignalFunction() == 0x0 ) {
			is_signals = false;
			break;
		}
	}
	if ( fSigFunc == 0x0 ) 
		is_signals = false;
	// cannot fit
	if ( is_signals == false ) {
		Error("FitCombined()", "At least one fitting function not defined, use SetSignalFunction() to set it");
		return;
	}
  	
	// deal with bg. if fit of bg required, the histogram is cloned to apply background substraction
	TString obkg(optBkg); TF1 *bkg = 0x0; TH1 *lochist = h; 
   //Int_t nPar = fSigFunc->GetNpar();
			 
	// set all peaks with the same background function
	next.Reset(); 
	while ( (peak = (TPGpeak1D *)next() ) ) {
		peak->SetBkgFunction(optBkg); 
	}
	bkg = SetBkgFunction(optBkg); // set the background function : no bg means pol0
	
	Bool_t do_bg_fit = true;
	if ( obkg == "" || obkg.Contains("-") ) {
		do_bg_fit = false;
		bkg->SetParameter(0,0.0); // in principle already done in SetBkgFunction
	}
	else {
		// a clone is needed to realize the background substraction
		lochist = (TH1*)h->Clone();
	}
	
	// local histo to perform the fit of the signal part in case of background substraction
	if ( do_bg_fit ) {
		// real function to fit excluding points outside the limits of the background 
		bkg = new TF1("Peak_bkg", this, &TPGpeak1D::Bkg, h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax(), fBkgFunc->GetNpar(), "TPGpeak1D", "Bkg");
		// todo : add option for bkg fit i.e. optBkg = "lin[opt_fit==>pass to root]"
		//	cout << " Backgroud fit" << endl;
		lochist->Fit(bkg,"RNQ");
		//	cout << " Backgroud fit" << endl;
		// set parameter back to background function and set background level
		fBkgFunc->SetParameters(bkg->GetParameters());
		// no needed any more
		delete bkg;
		
		// substract bkg so that lochist is ready to be fitted by the signal
		lochist->Add(fBkgFunc, -1);
	}
	// update bkg part
	SetBackground(fBkgLeft1, fBkgLeft2, fBkgRight1, fBkgRight2, 
					  fBkgFunc->Eval(fBkgLeft1),
					  fBkgFunc->Eval(fBkgLeft2),
					  fBkgFunc->Eval(fBkgRight1),
					  fBkgFunc->Eval(fBkgRight2)); 
	// update bkg part of other peaks
	next.Reset(); 
	while ( (peak = (TPGpeak1D *)next() ) ) {
		peak->BkgFunction()->SetParameters(fBkgFunc->GetParameters()); 
		peak->SetBackground(fBkgLeft1, fBkgLeft2, fBkgRight1, fBkgRight2, 
									fBkgFunc->Eval(fBkgLeft1),
									fBkgFunc->Eval(fBkgLeft2),
									fBkgFunc->Eval(fBkgRight1),
									fBkgFunc->Eval(fBkgRight2)); 
	}
	
	// now perform global fit
	// first build the global function 
	TString formula_fit, formula_draw;  
	// set formulas
	formula_fit   = fSigFunc->GetName();
	formula_draw  = fBkgFunc->GetName();
	formula_draw += "+"; 
	formula_draw += fSigFunc->GetName(); 
	//
	next.Reset();
	while ( (peak = (TPGpeak1D *)next() ) ) {
		formula_fit  += "+"; 
		formula_fit  += peak->SignalFunction()->GetName();
		formula_draw += "+"; 
		formula_draw += peak->SignalFunction()->GetName();		
		
		gROOT->GetListOfFunctions()->Add(peak->SignalFunction()); // add temporarly to be able to build envelop
	}	
	gROOT->GetListOfFunctions()->Add( fSigFunc );
	gROOT->GetListOfFunctions()->Add( fBkgFunc );
	
	// functions to fit and draw 
	TF1 *envelop_fit  = new TF1("envelop_fit", formula_fit.Data(), fBkgLeft1, fBkgRight2);
	
	TF1 *envelop = new TF1(Form("CombinedPeak_%p",this), formula_draw.Data(), fBkgLeft1, fBkgRight2);
	envelop->SetLineColor(30);

	next.Reset();
	while ( (peak = (TPGpeak1D *)next() ) ) {
		gROOT->GetListOfFunctions()->Remove(peak->SignalFunction()); // add temporarly to be able to build envelop
	}
	gROOT->GetListOfFunctions()->Remove( fSigFunc );
	gROOT->GetListOfFunctions()->Remove( fBkgFunc );
	
	// deal with fit option
	TString roptFit(optFit);
	if ( ! roptFit.Contains("N") ) {
		roptFit += "N";
	}
	if ( ! roptFit.Contains("Q") ) {
		roptFit += "Q";
	}
	// do fit
	lochist->Fit(envelop_fit, roptFit.Data());
	
	/*
	fLog  << GetName() << ": " 
	<< fSigFunc->GetParameter(1) << " [" << 2*fSigFunc->GetParError(1) << "], "
	<< fSigFunc->GetParameter(2) << " [" << 2*fSigFunc->GetParError(2) << "], "
	<< fSigFunc->GetParameter(0) << " [" << 2*fSigFunc->GetParError(0) << "], "
	<< fSigFunc->GetIntensity() << endl;
	 */
	
	// now copy parameters to the different functions.
	for (Int_t i = 0; i < fBkgFunc->GetNpar(); i++) {
		// 
		envelop->SetParameter(i,fBkgFunc->GetParameter(i));
		envelop->SetParError(i,fBkgFunc->GetParError(i));
		envelop->SetParName(i,fBkgFunc->GetParName(i));
		//
		next.Reset();
		while ( (peak = (TPGpeak1D *)next() ) ) {
			peak->BkgFunction()->SetParameter(i,fBkgFunc->GetParameter(i));
			peak->BkgFunction()->SetParError(i,fBkgFunc->GetParError(i));
		}
	}
	Int_t which_peak_parameter = 0;
	next.Reset(); 
	peak = this;
	for (Int_t i = 0; i < envelop_fit->GetNpar(); i++) {
		// 
		envelop->SetParameter(fBkgFunc->GetNpar()+i,envelop_fit->GetParameter(i));
		envelop->SetParError(fBkgFunc->GetNpar()+i,envelop_fit->GetParError(i));
		envelop->SetParName(fBkgFunc->GetNpar()+i,Form("%s_%s",peak->GetName(),peak->SignalFunction()->GetParName(which_peak_parameter)));
		//
		peak->SignalFunction()->SetParameter(which_peak_parameter,envelop_fit->GetParameter(i));
		peak->SignalFunction()->SetParError(which_peak_parameter,envelop_fit->GetParError(i));
		//
		which_peak_parameter++;
		// next peak to be set
		if ( which_peak_parameter == peak->SignalFunction()->GetNpar() ) {
			peak->SetPeakFromFit();
			peak->EnableFit();
			peak->SignalAndBkgFunction();
			
			// print on log
			roptFit = optFit;
			if ( !roptFit.Contains("Q") )
				peak->Print("f");

			// next peak to be filled
			which_peak_parameter = 0;
			peak = (TPGpeak1D *)next();
		}
	}	

	// clone;envelop_fit not needed anymore
	if ( lochist != h )
		delete lochist;
	delete envelop_fit;
	
	envelop->Draw("same");
}

//__________________________________________________________
void TPGpeak1D::Fit(TH1 *h, Option_t*  optFit, Option_t* optBkg)
{
	
	// be sure the signal function is defined
	if ( fSigFunc == 0x0 ) {
		Error("Fit()", "Fitting function not defined, use SetSignalFunction() to set it");
		return;
	}
  	
	// deal with bg. if fit of bg required, the histogram is cloned to apply background substraction
	TString obkg(optBkg); TF1 *bkg = 0x0; TH1 *lochist = h; Int_t nPar = fSigFunc->GetNpar();

	bkg = SetBkgFunction(optBkg); // set the background function : no bg means pol0

	Bool_t do_bg_fit = true;
	if ( obkg == "" || obkg.Contains("-") ) {
		do_bg_fit = false;
		bkg->SetParameter(0,0.0); // in principle already done in SetBkgFunction
	}
	else {
		// a clone is needed to realize the background substraction
		lochist = (TH1*)h->Clone();
	}
	
	// total number of parameters 
	nPar += fBkgFunc->GetNpar();
	
	// local histo to perform the fit of the signal part in case of background substraction
	if ( do_bg_fit ) {
		// real function to fit excluding points outside the limits of the background 
		bkg = new TF1("Peak_bkg", this, &TPGpeak1D::Bkg, h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax(), fBkgFunc->GetNpar(), "TPGpeak1D", "Bkg");
		// todo : add option for bkg fit i.e. optBkg = "lin[opt_fit==>pass to root]"
		//	cout << " Backgroud fit" << endl;
		lochist->Fit(bkg,"RNQ");
		//	cout << " Backgroud fit" << endl;
		// set parameter back to background function and set background level
		fBkgFunc->SetParameters(bkg->GetParameters());
		// no needed any more
		delete bkg;
		
		// substract bkg so that lochist is ready to be fitted by the signal
		lochist->Add(fBkgFunc, -1);
	}
		
	// update bg part
	SetBackground(fBkgLeft1, fBkgLeft2, fBkgRight1, fBkgRight2, 
					  fBkgFunc->Eval(fBkgLeft1),
					  fBkgFunc->Eval(fBkgLeft2),
					  fBkgFunc->Eval(fBkgRight1),
					  fBkgFunc->Eval(fBkgRight2)); 
				
	// deal with fit option
	TString roptFit(optFit);
	if ( ! roptFit.Contains("N") ) {
		roptFit += "N";
	}
	if ( ! roptFit.Contains("Q") ) {
		roptFit += "Q";
	}
	lochist->Fit(fSigFunc, roptFit.Data());
	
	// clone not needed anymore
	if ( lochist != h )
		delete lochist;
	
	// set signal+bkg function. Takes care of copy of the parameters
	SetPeakFromFit();
	SignalAndBkgFunction();
	
	// fit performed
	EnableFit();
	
	// print on log
	roptFit = optFit;
	if ( !roptFit.Contains("Q") )
		Print("f");
	
	// display polyline showing diff between h and fit
	/*
	TPolyLine *l = TPGpadManager::ShowDiff(h,fPeakFunc,fBkgLeft1,fBkgRight2);
	l->SetLineStyle(2);
	l->SetLineColor(29); */

}

//___________________________________________________________
void TPGpeak1D::FitIsWithBkg(TH1* h, Option_t* optFit)
{
	// fit with signal function removing bkg
		
	Int_t nPar = fBkgFunc->GetNpar();
	TF1* bkg =new TF1("Bkg", this, &TPGpeak1D::Bkg, fBkgLeft1, fBkgRight2, nPar+2, "TPGpeak1D", "Bkg");
	bkg->FixParameter(nPar, fBkgLeft2);
	bkg->FixParameter(nPar+1, fBkgRight1);
		
	// transfert initial parameter to bkg function
	for (Int_t i = 0; i < nPar; ++i)
		bkg->SetParameter(i, fBkgFunc->GetParameter(i));
		
	h->Fit(bkg, optFit);
		
	// set parameter back to background function
	fBkgFunc->SetParameters(bkg->GetParameters());
	fBkgLevelLeft2  = fBkgFunc->Eval(fBkgLeft2);
	fBkgLevelRight1 = fBkgFunc->Eval(fBkgRight1);
	delete bkg;
		
	// substract bkg and fit
	TH1* lochist = (TH1*)h->Clone();
	lochist->Add(fBkgFunc, -1);
	lochist->Fit(fSigFunc, optFit);
	delete lochist;  
}
	
//__________________________________________________________
void TPGpeak1D::Print(Option_t* opt) const
{
	//
	TString Opt(opt);
	if ( Opt.Contains("f") ) {
		if ( fSigFunc ) {
			cout  << GetName() << ": " 
					<< fSigFunc->GetParameter(1) << " [" << 2*fSigFunc->GetParError(1) << "], "
					<< fSigFunc->GetParameter(2) << " [" << 2*fSigFunc->GetParError(2) << "], "
					<< fSigFunc->GetParameter(0) << " [" << 2*fSigFunc->GetParError(0) << "], "
					<< fIntensity << endl;
		}
	}
	else {
		printf("\n");
		TNamed::Print(opt);
		TPGbasePeak::Print(opt);
		printf("Position: %5.1f  FWHM: %5.1f\n", fPosition, fFWHM);
		if ( IsWithBkg() ) // set bkg
			printf("Bkg left1: %5.1f Bkg left2: %5.1f  Bkg right1: %5.1f Bkg right1: %5.1f\n", 
					 fBkgLeft1, fBkgLeft2, fBkgRight1, fBkgRight2);
	}
}


//__________________________________________________________
void TPGpeak1D::SetLineColorPeak(Color_t color)
{
  // set line color for peak
  fPolyLinePeak->SetLineColor(color); 
}

//__________________________________________________________
Color_t TPGpeak1D::GetLineColorPeak()
{
  return fPolyLinePeak->GetLineColor();
}

//__________________________________________________________
void TPGpeak1D::SetLineWidthPeak(Width_t width)
{
  // set line width for peak
  fPolyLinePeak->SetLineWidth(width);
}

//__________________________________________________________
void TPGpeak1D::SetFillColorPeak(Color_t color)
{
  // set fill color for peak
  fPolyLinePeak->SetFillColor(color);
}

//__________________________________________________________
void TPGpeak1D::SetFillStylePeak(Style_t style)
{
  // set fill sryle for peak
  fPolyLinePeak->SetFillStyle(style);
}

//__________________________________________________________
void TPGpeak1D::SetLineColorBkg(Color_t color)
{
  // set line color for peak
  fPolyLineBkgL->SetLineColor(color);
  fPolyLineBkgR->SetLineColor(color);
}

//__________________________________________________________
void TPGpeak1D::SetLineWidthBkg(Width_t width)
{
  // set line width for peak
  fPolyLineBkgL->SetLineWidth(width);
  fPolyLineBkgR->SetLineWidth(width);
}

//__________________________________________________________
void TPGpeak1D::SetFillColorBkg(Color_t color)
{
  // set fill color for peak
  fPolyLineBkgL->SetFillColor(color);
  fPolyLineBkgR->SetFillColor(color);
}

//__________________________________________________________
void TPGpeak1D::SetFillStyleBkg(Style_t style)
{
  // set fill sryle for peak
  fPolyLineBkgL->SetFillStyle(style);
  fPolyLineBkgR->SetFillStyle(style);
}

//__________________________________________________________
void TPGpeak1D::SetDrawAs(EDrawAs d)
{ 
	if ( fDrawAs == d )
		return;
   fDrawAs = d; 
	
	// peak <==> gate
	SetModified((*fPolyLinePeak));
}

//__________________________________________________________
void TPGpeak1D::Paint(Option_t* /*opt*/)
{
	// first background
	if ( IsWithBkg() ) {
		if ( IsModified((*fPolyLineBkgL)) || IsModified((*fPolyLineBkgR)) )
			SetBkgPoints();
		
		fPolyLineBkgL->Paint("f");  
		fPolyLineBkgR->Paint("f"); 
		fPolyLineBkgL->Paint();  
		fPolyLineBkgR->Paint(); 		
	}
	
	// paint peak but first recalculate the polyline
	if ( IsModified((*fPolyLinePeak)) )
		 SetPeakPoints();
	if ( fDrawAs == kGate)
		fPolyLinePeak->Paint("f");
	fPolyLinePeak->Paint();

	if (fDrawAs == kPeak) {
		// if fit, functions
		if ( fPeakFunc && FitFlag() ) {
			fPeakFunc->Paint("same");
			if ( fBkgFunc && FitFlag() && IsWithBkg() ) 
				fBkgFunc->Paint("same");
		}
		// synchronize markers
		TMarker* m = 0x0;
		for (Int_t i = 0; i < fgkPointsPeak; ++i) {
			m  = static_cast<TMarker*> (fMarkerList->At(i));
			m->SetX(fPolyLinePeak->GetX()[i]);  
			m->SetY(fPolyLinePeak->GetY()[i]);
		}
		fMarkerList->Paint();
	}
}

//__________________________________________________________
void TPGpeak1D::Draw(Option_t* opt)
{
	// draw
	if (!gPad) {
		Error("Draw()", "Pad not existing");
		return;
	}
	TString tmp(opt);
	tmp.ToLower();
	if (tmp.Contains("gate")) {
		SetDrawAs(kGate);
	} else { SetDrawAs(kPeak); }
	
	AppendPad("");  
}

void TPGpeak1D::SetDrawOption(Option_t* opt)
{
	TString tmp(opt);
	tmp.ToLower();
	if (tmp.Contains("gate")) {
		SetDrawAs(kGate);
	} else { SetDrawAs(kPeak); }	
}


//__________________________________________________________
Int_t TPGpeak1D::DistancetoPrimitive(Int_t px, Int_t py)
{
	Int_t dP = fPolyLinePeak->DistancetoPrimitive(px, py);
	Int_t dL = fPolyLineBkgL->DistancetoPrimitive(px, py);
	Int_t dR = fPolyLineBkgR->DistancetoPrimitive(px, py);

	Int_t d1 = TMath::Min(dP, dL);
	Int_t d2 = TMath::Min(d1, dR); 
	return d2;
}

//___________________________________________________________
void TPGpeak1D::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{	
	if (!gPad->IsEditable()) 
		return;

	Int_t dP = 0;
	Int_t dL = 0;
	Int_t dR = 0;

	if (fCPolyline == 0x0) {
		dP = fPolyLinePeak->DistancetoPrimitive(px, py);
		dL = fPolyLineBkgL->DistancetoPrimitive(px, py);
		dR = fPolyLineBkgR->DistancetoPrimitive(px, py);

		if (dL <= dP) {
			if (dR <= dL) {
				fCPolyline = fPolyLineBkgR;
			} else {
				fCPolyline = fPolyLineBkgL;
			}
		} else {
			if (dR <= dP) {
				fCPolyline = fPolyLineBkgR;
			} else {
				fCPolyline = fPolyLinePeak;
			}
		}
		// keep copy of the polyline to be modified
		fCopyPolyline.SetPolyLine(fCPolyline->GetN(),fCPolyline->GetX(),fCPolyline->GetY());
//		SetModified((*fCPolyline),false);
	}

	fCPolyline->ExecuteEvent(event, px, py);

	Int_t how_many, which, sign; Double_t deltax, deltay, val;
	switch (event) {

		case kMouseMotion:
			fCPolyline = 0x0;
			break;
	  
		case kButton1Up:
			// do your test before modifications !
			how_many = which = 0;
			for (Int_t i = 0; i < fCPolyline->GetN() ; i++ ) {
				if ( fCPolyline->GetX()[i] != fCopyPolyline.GetX()[i] ) {
					which = i;
					how_many++;
				}
				if ( fCPolyline->GetY()[i] != fCopyPolyline.GetY()[i] ) {
					which = i;
					how_many++;
				}				
//			  printf("%d %d %d :  %f -> %f \n",i,which,how_many,fCPolyline->GetX()[i],fCopyPolyline.GetX()[i]);
			}
		  
			if ( fCPolyline == fPolyLinePeak ) {
				// the full peak has been moved, change postion & translate the polyline (to avoid re-calculations)
				if ( how_many > 2 ) {
					// just move on X
					deltax = fCPolyline->GetX()[fgkPeakIdx] - fCopyPolyline.GetX()[fgkPeakIdx];
					val = GetPosition() + deltax;
					// back the previous polyline before application of the shift
					fPolyLinePeak->SetPolyLine(fCopyPolyline.GetN(),fCopyPolyline.GetX(),fCopyPolyline.GetY());
					// check it does not go beyond background in case there is one
					if ( IsWithBkg() ) {
						if ( val > fBkgLeft2 && val < fBkgRight1 ) {
							fPosition = val;
							ShiftPolyline((*fPolyLinePeak),deltax,0.0);
							SetModified((*fPolyLinePeak),false);
						}
					}
					else { 
						fPosition = val;
						ShiftPolyline((*fPolyLinePeak),deltax,0.0);
						SetModified((*fPolyLinePeak),false);
					}
				}
				else { // one of the point has moved only
					switch (which) {
						// change only the height
						case fgkPeakIdx:
							// compute modifications
							deltay = fCPolyline->GetY()[fgkPeakIdx] - fCopyPolyline.GetY()[fgkPeakIdx];
							val = GetHeight() + deltay;
							// apply modifications
							SetHeight(val);
							break;
						// width
						case fgkPeakIdx-1:
						case fgkPeakIdx+1:
							// compute modifications
							deltax = fCPolyline->GetX()[which] - fCopyPolyline.GetX()[which];
							if ( which == fgkPeakIdx-1 )
								sign = -1;
							else
								sign = +1;
								
							val = GetFWHM() + 2*sign*deltax;
							SetFWHM(val);
							break;							
						default:
							break;
					} 
					// modifications only of the peak values. polyline are delayed at paint
					fPolyLinePeak->SetPolyLine(fCopyPolyline.GetN(),fCopyPolyline.GetX(),fCopyPolyline.GetY());
				}
			// background 
			} else {
				// the full bg has been moved, just translate the polyline
				if ( how_many > 2 ) {
					// just move on X
					deltax = fCPolyline->GetX()[0] - fCopyPolyline.GetX()[0];
					// check if move possible
					Bool_t do_move = false;
					if ( fCPolyline == fPolyLineBkgL ) { // 
						if ( fBkgLeft2 + deltax < fPolyLinePeak->GetX()[fgkPeakIdx-1] )
							do_move = true;
					}
					else {
						if ( fBkgRight1 + deltax > fPolyLinePeak->GetX()[fgkPeakIdx+1] )
							do_move = true;						
					}
					// back the previous polyline before application of the shift
					fCPolyline->SetPolyLine(fCopyPolyline.GetN(),fCopyPolyline.GetX(),fCopyPolyline.GetY());
					// check it does not overlap with peak						
					if ( do_move ) {
						// global shift
						ShiftPolyline((*fCPolyline),deltax,0.0);
						// compute new values of bg from polyline 
						UpdateBkg();
					}
				}
				else { // one of the point has moved only
					switch (which) {
						// background level+position
						case 1:
						case 2:
							deltax = fCPolyline->GetX()[which] - fCopyPolyline.GetX()[which];
							// check it does not overlap with peak						
							if ( fCPolyline->GetX()[which] < fPolyLinePeak->GetX()[fgkPeakIdx-1] || fCPolyline->GetX()[which] > fPolyLinePeak->GetX()[fgkPeakIdx+1] ) {
								// compute new values of bg from polyline 
								UpdateBkg();
							    SetPeakPoints();
							}
							// back the previous polyline before application of the shift
							else 
								fCPolyline->SetPolyLine(fCopyPolyline.GetN(),fCopyPolyline.GetX(),fCopyPolyline.GetY());
							break;
						default:
							fCPolyline->SetPolyLine(fCopyPolyline.GetN(),fCopyPolyline.GetX(),fCopyPolyline.GetY());
							break;
					} 
				}
			}
			fCPolyline = 0x0;
			break;
	}
}

//___________________________________________
void TPGpeak1D::PaintFor(Double_t /*xmin*/, Double_t /*xmax*/, Double_t ymin, Double_t /*ymax*/)
{
	if ( fDrawAs == kGate ) {
		fPolyLinePeak->GetY()[0] = fPolyLinePeak->GetY()[fgkPointsPeak - 1] = ymin;
	}
	fPolyLineBkgL->GetY()[0] = fPolyLineBkgL->GetY()[fgkPointsBkg-1] = ymin;
	fPolyLineBkgR->GetY()[0] = fPolyLineBkgR->GetY()[fgkPointsBkg-1] = ymin;
}


//___________________________________________
Int_t TPGpeak1D::Compare(const TObject *obj) const
{
  // sort peak by position 
  TPGpeak1D* event = (TPGpeak1D*) obj;
  return (fPosition > event->GetPosition()) ? 1 : -1;
}

//___________________________________________________________
void  TPGpeak1D::SetPeakPoints(TPolyLine* polyline)
{
  
  Double_t x[fgkPointsPeak] = {0,0,0,0,0,0,0};
  Double_t y[fgkPointsPeak] = {0,0,0,0,0,0,0};
  
  if (polyline->GetN() == fgkPointsPeak) {
	fPolyLinePeak = (TPolyLine*)polyline->Clone();
	
  } else if (polyline->GetN() == fgkPointsPeak-2) {
	fPolyLinePeak = new TPolyLine(fgkPointsPeak);
	x[fgkPeakIdx] = polyline->GetX()[fgkPeakIdx-1]; 
	y[fgkPeakIdx] = polyline->GetY()[fgkPeakIdx-1]; 
	for (Int_t i = 1; i < 3; ++i) {
	  x[fgkPeakIdx-i] = polyline->GetX()[fgkPeakIdx-1-i]; 
	  y[fgkPeakIdx-i] = polyline->GetY()[fgkPeakIdx-1-i]; 
	  
	  x[fgkPeakIdx+i] = polyline->GetX()[fgkPeakIdx-1+i]; 
	  y[fgkPeakIdx+i] = polyline->GetY()[fgkPeakIdx-1+i]; 
	}
	x[fgkPeakIdx+3] = x[fgkPeakIdx+2];
	y[fgkPeakIdx+3] = y[fgkPeakIdx+2];
	x[fgkPeakIdx-3] = x[fgkPeakIdx-2];
	y[fgkPeakIdx-3] = y[fgkPeakIdx-2];
	fPolyLinePeak->SetPolyLine(fgkPointsPeak, x, y); 
	
  } else {
    Error("SetPeakPoints()", "Wrong number of points");
    return;
  }
  
  fPolyLinePeak->SetLineWidth(2);
}

//___________________________________________________________

void TPGpeak1D::SetModified(TPolyLine &p, Bool_t modif)
{
	p.SetBit(0x1000,modif); // in principle this bit is not used by Polyline	
}

//___________________________________________________________
Bool_t TPGpeak1D::IsModified(TPolyLine &p)
{
	return p.TestBit(0x1000);
}

//___________________________________________________________
void  TPGpeak1D::SetPeakPoints()
{
   // set points of polyline from peak members
   Int_t n = fgkPointsPeak;
   
   Double_t x[n], y[n];
   Double_t sigma = fFWHM/SigmaToFWHM; Double_t base = ( fBkgLevelLeft2 + fBkgLevelRight1 )/2.;
   
   // centroid
   x[fgkPeakIdx] = fPosition;
   y[fgkPeakIdx] = fHeight+base;

   // width
   x[fgkPeakIdx-1] = fPosition - fFWHM/2.;
   y[fgkPeakIdx-1] = fHeight/2 + base; // default model is a gaussian shape
   
   x[fgkPeakIdx+1] = fPosition + fFWHM/2.;
   y[fgkPeakIdx+1] = fHeight/2 + base; // default model is a gaussian shape
   
   if ( fDrawAs == kPeak ) {
		
		if ( FitFlag() && fSigFunc && fBkgFunc ) {
			
			// centroid
			x[fgkPeakIdx] = fPosition;
			y[fgkPeakIdx] = fHeight + fBkgFunc->Eval(x[fgkPeakIdx]);
			
			x[fgkPeakIdx-1] = fPosition - fFWHM/2.;
			y[fgkPeakIdx-1] = fSigFunc->Eval(x[fgkPeakIdx-1]) + fBkgFunc->Eval(x[fgkPeakIdx-1]);
			
			x[fgkPeakIdx+1] = fPosition + fFWHM/2.;
			y[fgkPeakIdx+1] = fSigFunc->Eval(x[fgkPeakIdx+1]) + fBkgFunc->Eval(x[fgkPeakIdx+1]);
			
			x[fgkPeakIdx-2] = fPosition - 2.5*sigma;
			x[fgkPeakIdx+2] = fPosition + 2.5*sigma;
			x[fgkPeakIdx-3] = fPosition - 3.5*sigma;
			x[fgkPeakIdx+3] = fPosition + 3.5*sigma;
			
			y[fgkPeakIdx-2] = fSigFunc->Eval(x[fgkPeakIdx-2]) + fBkgFunc->Eval(x[fgkPeakIdx-2]);
			y[fgkPeakIdx+2] = fSigFunc->Eval(x[fgkPeakIdx+2]) + fBkgFunc->Eval(x[fgkPeakIdx+2]);
			y[fgkPeakIdx-3] = fSigFunc->Eval(x[fgkPeakIdx-3]) + fBkgFunc->Eval(x[fgkPeakIdx-3]);
			y[fgkPeakIdx+3] = fSigFunc->Eval(x[fgkPeakIdx+3]) + fBkgFunc->Eval(x[fgkPeakIdx+3]);			
		}
		else {  // default model is a gaussian shape
	  
			x[fgkPeakIdx-2] = fPosition - 2.5*sigma;
			x[fgkPeakIdx+2] = fPosition + 2.5*sigma;
			x[fgkPeakIdx-3] = fPosition - 3.5*sigma;
			x[fgkPeakIdx+3] = fPosition + 3.5*sigma;

			y[fgkPeakIdx-2] = fHeight*4.39e-02 + base;
			y[fgkPeakIdx+2] = fHeight*4.39e-02 + base;
			y[fgkPeakIdx-3] = fHeight*2.187e-03 + base;
			y[fgkPeakIdx+3] = fHeight*2.187e-03 + base;			
		}
		
   } else if ( fDrawAs == kGate ) {
	  
		x[fgkPeakIdx-3] = x[fgkPeakIdx-2] = x[fgkPeakIdx-1];
		x[fgkPeakIdx+3] = x[fgkPeakIdx+2] = x[fgkPeakIdx+1];
		y[fgkPeakIdx-1] = y[fgkPeakIdx+1] = y[fgkPeakIdx];
		y[fgkPeakIdx-2] = y[fgkPeakIdx+2] = y[fgkPeakIdx] - fHeight;
		
   } else {
	  Error("SetPeakPoints()", "No Style for Drawing peak");
	  return;
   }
   //
   fPolyLinePeak->SetPolyLine(n, x, y);  
	// the polyline is synchronised with it ==> set it to modify false 
	SetModified((*fPolyLinePeak),false);
}

//___________________________________________________________
void  TPGpeak1D::SetBkgPoints()
{
	// set points of polyline from peak members
	Int_t n = fgkPointsBkg;
	if ( !IsWithBkg() ) 
		return;
	 
	Double_t xL[n], yL[n];
	Double_t xR[n], yR[n];

	// bkg left
	xL[0] = fBkgLeft1;
	xL[1] = fBkgLeft1;
	xL[2] = fBkgLeft2;
	xL[3] = fBkgLeft2;
	
	yL[0] = yL[3] = 0.;
	yL[1] = fBkgLevelLeft1;
	yL[2] = fBkgLevelLeft2; 

	// bkg right
	xR[0] = fBkgRight1;
	xR[1] = fBkgRight1;
	xR[2] = fBkgRight2;
	xR[3] = fBkgRight2;

	yR[0] = yR[3] = 0.;
	yR[1] = fBkgLevelRight1;
	yR[2] = fBkgLevelRight2;

	fPolyLineBkgL->SetPolyLine(n, xL, yL);  
	fPolyLineBkgR->SetPolyLine(n, xR, yR);  
	
	// this line could be drawn as it they are consistent with the background definition
	SetModified((*fPolyLineBkgL),false);
	SetModified((*fPolyLineBkgR),false);
}
	  
	  
//___________________________________________________________
void  TPGpeak1D::UpdatePeak()
{
  // update peak members from polyline
  
  // centroid  
  fPosition = fPolyLinePeak->GetX()[fgkPeakIdx];
  
  // width
  fFWHM = fPolyLinePeak->GetX()[fgkPeakIdx+1] - fPolyLinePeak->GetX()[fgkPeakIdx-1];
      
  // height
   Double_t base = 0;
   
   if (fBkgFlag) {
	  base = (fPolyLineBkgL->GetY()[2] + fPolyLineBkgR->GetY()[1])/2.;
   }
   
  fHeight = fPolyLinePeak->GetY()[fgkPeakIdx] - base;

   if (fDrawAs == kPeak) {
	// area triangle + trapeze
	fIntensity = (2*fFWHM + fPolyLinePeak->GetX()[fgkPeakIdx+2]-fPolyLinePeak->GetX()[fgkPeakIdx-2])*fHeight/4.;
  }
}

//___________________________________________________________
void  TPGpeak1D::UpdateBkg()
{
  // update peak members from polyline
  
 	fBkgLeft1  = fPolyLineBkgL->GetX()[0] = fPolyLineBkgL->GetX()[1];
	fBkgLeft2  = fPolyLineBkgL->GetX()[3] = fPolyLineBkgL->GetX()[2];
	fBkgRight1 = fPolyLineBkgR->GetX()[0] = fPolyLineBkgR->GetX()[1];
	fBkgRight2 = fPolyLineBkgR->GetX()[3] = fPolyLineBkgR->GetX()[2];
	
 	fBkgLevelLeft1  = fPolyLineBkgL->GetY()[1];
	fBkgLevelLeft2  = fPolyLineBkgL->GetY()[2];
	fBkgLevelRight1 = fPolyLineBkgR->GetY()[1];
	fBkgLevelRight2 = fPolyLineBkgR->GetY()[2];  
	
	// this line could be drawn as it they are consistent with the background definition
	SetModified((*fPolyLineBkgL),false);
	SetModified((*fPolyLineBkgR),false);
}
	  
// private functions

//___________________________________________________________
void TPGpeak1D::InitPeakMarkers()
{
	
	for (Int_t i = 0; i < fgkPointsPeak ; i++) {
		TMarker *m = new TMarker(); 
		if ( i == fgkPeakIdx )
			m->SetMarkerStyle(23); 
		else 
			m->SetMarkerStyle(20);
		m->SetMarkerColor(fgMarkerColor);
		
		fMarkerList->Add(m);		
	}
}

