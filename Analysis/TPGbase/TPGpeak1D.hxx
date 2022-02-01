/***************************************************************************
 *   Copyright (C) 2004 by Olivier Stezowski, B. Rosse & Christian Finck   *
 *   stezow(AT)ipnl.in2p3.fr                                               *
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

/*!
 \file TPGpeak1D.hxx
 \brief   Declaration of TPGpeak1D.
 */

#ifndef TPGpeak1D_h
#define TPGpeak1D_h

#include "TPGbasePeak.hxx"

#include "TPolyLine.h"
#include "TVector2.h"

class TF1;
class TH1;
class TList;
class TMarker;

class TPGpeak1D : public TPGbasePeak {

public:
   //! peak enum
	enum EDrawAs { kPeak, kGate };
  
public:
	//! Get default marker color
	static Color_t GetDefaultMarkerColor()       	{ return fgMarkerColor; }
	//! Get default function color
	static Color_t GetDefaultFuncColor()            { return fgFuncColor; }
	//! Set default marker color
	static void    SetDefaultMarkerColor(Color_t c)	{ fgMarkerColor = c; }
	//! Set default function color
	static void    SetDefaultFuncColor(Color_t c)   { fgFuncColor = c; }
  
public:
	TPGpeak1D();
	TPGpeak1D(const char* name, const char* title);
	TPGpeak1D(TPolyLine* polyline);
	virtual ~TPGpeak1D();

	//! Copy ctr
	//TPGpeak1D(const TPGpeak1D& p);
	//! Assignment operator
	//TPGpeak1D& operator=(const TPGpeak1D& p);

   // Actions : Area, Fit
	// Fit peak with background
	virtual void Fit(TH1 *histo, Option_t* optFit = "RN", Option_t* optBkg = "pol1"); 
	
	// Fit multi-defined peak with a common background
	virtual void FitCombined(TH1 *histo, TList* peakList, Option_t* optFit = "RN", Option_t* optBkg = "pol1"); 
	
	// Fit with  given function and background
	void FitMenu(const char* nameFunc = "gaus", Option_t* optFit = "RN", Option_t* optBkg = "pol1"); //*MENU*

	// Print current peak
	virtual void Print(Option_t* opt ="") const; //*MENU*

	// to get the Area (+bkg) of this peak
	virtual void Area(Option_t *opt = "h"); //*MENU*
	
   // Set/Get methods
	// Set peak members
	void SetPeak(Double_t pos,  Double_t height,  Double_t fwhm, Double_t intensity=0); //*MENU={Hierarchy="SetPeak/All"}*
	// Set peak members + polyline
	void SetPeakFromFit(); 
	// Set height of peak
	void SetHeight(const Double_t height);       // *MENU={Hierarchy="SetPeak/Height"}* *GETTER=GetHeight
	// Set Intensity of peak
	void SetIntensity(const Double_t intensity); 
	// Set Position of peak
	virtual void SetPosition(const Double_t position, Option_t* axis = "X");  // *MENU={Hierarchy="SetPeak/Position"}* *GETTER=GetPosition  
	// Set FWHM of peak
	virtual void SetFWHM(const Double_t FWHM, Option_t* axis = "X");          // *MENU={Hierarchy="SetPeak/FWHM"}* *GETTER=GetFWHM
	// Set background limits
	void SetBackground(Double_t bgLeft1, Double_t bgLeft2, Double_t bgRight1,Double_t bgRight2,
							 Double_t bgLevelLeft1, Double_t bgLevelLeft2, Double_t bgLevelRight1,Double_t bgLevelRight2); 
	// Set background limits
	// void SetBackground( Double_t bgLeft1, Double_t bgLeft2, Double_t bgRight1, Double_t bgRight2, const TVirtualPad *pad = 0x0); 
	void SetBackground( Double_t bgLeft1, Double_t bgLeft2, Double_t bgRight1, Double_t bgRight2, const TH1* ); 
	//! Set marker color of the peak polyline
	void SetMarkerColor(Int_t color);

	// Get position of peak
	virtual Double_t GetPosition(Option_t* axis = "X") const;
	// Get FWHM of peak
	virtual Double_t GetFWHM(Option_t* axis = "X") const;
	// Set pre-defined function to fit the signal
	virtual TF1 *SetSignalFunction(const char* nameFunc = "gaus"); 
	// Set function to fit the signal
	virtual TF1 *SetSignalFunction(const TF1* func); 
	//! Access to signal function ... don't delete it !
	virtual TF1 *SignalFunction()	{ return fSigFunc; }
	// Set pre-defined function for background during fit
	virtual TF1 *SetBkgFunction(const char* nameFunc = "-"); 
	// Set user defined function for background during fit
	virtual TF1 *SetBkgFunction(const TF1* func); 
	//! Access to signal function ... don't delete it !
	virtual TF1 *BkgFunction()	{ return fBkgFunc; }
	//! Access to signal+Bkg function. Signa + Bkg must be set first, parameter are copied !
	virtual TF1 *SignalAndBkgFunction();
	
	// toggle background (fit and display)
	void WithBkg(Bool_t with_bg = true); //*TOGGLE* *GETTER=IsWithBkg
	
	//! Test if peak defined with background
	Bool_t IsWithBkg() const  { return fBkgFlag; }
	
	// Set line attribute for peak
	virtual void	SetFillColorPeak(Color_t color);
	virtual void	SetFillStylePeak(Style_t style);  
	virtual void	SetLineColorPeak(Color_t color); 
	virtual void	SetLineWidthPeak(Width_t width); 
	
	// Set line attribute for bkg
	virtual void	SetFillColorBkg(Color_t color);
	virtual void	SetFillStyleBkg(Style_t style);  
	virtual void	SetLineColorBkg(Color_t color);  
	virtual void	SetLineWidthBkg(Width_t width); 
	
	// Get line peak color
	virtual Color_t GetLineColorPeak();
	
	// Get marker for a given Id
	virtual TMarker* GetMarker(Int_t markerId);
	
	//! Get signal function
	virtual TF1* GetSigFunction() { return fSigFunc; }
	
	// Set drawing flag
	virtual void SetDrawAs(EDrawAs d);
	//! to know the current display mode
	virtual EDrawAs GetDrawAs() const { return fDrawAs; }
	virtual void SetDrawOption(Option_t* option = ""); //*MENU*
	
	//! Sort
	virtual Bool_t  IsSortable() const  {return kTRUE;}
	virtual Int_t Compare(const TObject *obj) const;

	//! Get base of peak
	Double_t GetBase() const { return fPolyLinePeak->GetY()[0]; }

	//! Get background left limits
	const TVector2 GetBkgLeft()  const { return TVector2(fBkgLeft1,  fBkgLeft2); }
   //! Get background right limits
	const TVector2 GetBkgRight() const { return TVector2(fBkgRight1, fBkgRight2); }
	
	//! to determine if a point is in bg. 0 likely in peak, 1 in bg, 2 outside bg
	Short_t IsPointInBkg(Double_t x, Double_t /*y*/ = 0)
	{
		if ( x < fBkgLeft1 || x > fBkgRight2 )
			return 2;
		if ( x <= fBkgLeft2 || x >= fBkgRight1 )
			return 1;
		return 0;
	}
	 
   //! Get integral of background after calling area method 
   Double_t GetBkgIntegral()     const { return fBkgIntegral;}
   //! Get integral of peak after calling area method 
   Double_t GetPeakIntegral()    const { return fPeakIntegral;}
   //! Get integral of background under peak after calling area method 
   Double_t GetSubPeakIntegral() const { return fSubPeakIntegral;}
   
   // Draw/Paint methods
	virtual void Paint(Option_t* opt ="peak");
	virtual void Draw(Option_t* opt ="peak");
	// set attribute to paint this gate on the given histogram
	virtual void PaintFor(Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax);         

	// Handle event in pad
	virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);

	// Compute distance to object
	virtual Int_t	DistancetoPrimitive(Int_t px, Int_t py);

	// functions used to really fit
	Double_t SignalAndBkg(Double_t* x, Double_t* par);
	Double_t Bkg(Double_t* x, Double_t* par);
	
private:
	// Set peak point for members
	void SetPeakPoints();

	// Set peak point from polyline
	void SetPeakPoints(TPolyLine* polyline);

	// Set bkg point for members
	void SetBkgPoints();

	// Set members for polyline
	void UpdatePeak();

	// Set members for polyline
	void UpdateBkg();
	
	// set the following polyline to modified ==> use one of the TObject status bit
	void SetModified(TPolyLine &p, Bool_t modif = true);
	// to know if the polyline should be updated
	Bool_t IsModified(TPolyLine &p);

	// Add marker to polyline
	void InitPeakMarkers();
	// to shift a given polyline
	void ShiftPolyline(TPolyLine &, Double_t, Double_t);
	// change all values in
	void SetAllPointsinPolyline(TPolyLine &, Double_t x = 0.0, Double_t y = 0.0);

	// Fit with background
	void FitIsWithBkg(TH1* h, Option_t* optFit);

private:
	// peak's definition
	Double_t fPosition;          ///< Position of the Peak in energy
	Double_t fFWHM;              ///< FWHM of the Peak
	
	Double_t fBkgLeft1;          ///< lower limit for left side background
	Double_t fBkgLeft2;          ///< upper limit for left side background
	Double_t fBkgRight1;         ///< lower limit for right side background
	Double_t fBkgRight2;         ///< upper limit for right side background
	Double_t fBkgLevelLeft1;     ///< level lower limit for left side background
	Double_t fBkgLevelLeft2;     ///< level upper limit for left side background
	Double_t fBkgLevelRight1;    ///< level lower limit for right side background
	Double_t fBkgLevelRight2;    ///< level upper limit for right side background
   Double_t fBkgIntegral;       ///< Total counts in background
   Double_t fPeakIntegral;      ///< Total counts in peak (bg substracted)
   Double_t fSubPeakIntegral;   ///< Integral of background under peak
  
	Bool_t   fBkgFlag;           ///< flag w/o background
	
	// peak's display
	TPolyLine*  fPolyLinePeak;   ///<! pointer to polyline peak
	TPolyLine*  fPolyLineBkgL;   ///<! pointer to polyline left background
	TPolyLine*  fPolyLineBkgR;   ///<! pointer to polyline right background
	
	TPolyLine*  fCPolyline;      ///<! current polyline
	TPolyLine   fCopyPolyline;   ///<! current polyline before modification
	
	TList*      fMarkerList;	  ///<! list of different markers
	EDrawAs     fDrawAs;         ///< flag for drawing polyline as

	// peak's fit
	TF1*        fSigFunc;        ///< pointer to signal function
	TF1*        fBkgFunc;        ///< pointer to bkg function
	TF1*        fPeakFunc;       ///<! pointer to signal+ bkg function
	
protected:
	static const Int_t fgkPointsPeak;     ///< nb of polyline points for the peak
	static const Int_t fgkPointsBkg;      ///< nb of polyline points for background
	static const Int_t fgkPeakIdx;        ///< index of the peak central point

	static Color_t     fgLineColorPeak[]; ///< default line color for peak
	static Style_t     fgFillStylePeak;   ///< default fill style for peak
	static Color_t     fgLineColorBkg;    ///< default line color for background
	static Style_t     fgFillStyleBkg;    ///< default fill style for background
	static Color_t     fgMarkerColor;     ///< default marker color
	static Color_t     fgFuncColor;       ///< default function color

public:
	ClassDef(TPGpeak1D,1) // A 1D Peak
};

#endif
