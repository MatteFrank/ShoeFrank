

#ifndef TPGbasePeak_h
#define TPGbasePeak_h

#include "TNamed.h"


class TF1;
class TH1;
class TList;


/** A TPGbasePeak is defined by a height, intensity and a dimension of the peak

*/
class TPGbasePeak : public TNamed 
{
private:
	struct GateColor_t {
		Color_t k0;
		Color_t k1;
		Color_t k2;
		Color_t k3;
	};	
	
public:
	static GateColor_t fgGateColor; 
	
	static Bool_t  IsvalideGateColor(Color_t co);
	static Int_t   GetGateAxis(Color_t co);
	static Color_t GetGateColor(Int_t idx);
	
public:
	TPGbasePeak(const char* name, const char* title);
	virtual ~TPGbasePeak();
	
	TPGbasePeak(const TPGbasePeak& p);
	TPGbasePeak& operator=(const TPGbasePeak& p);
	
	//! Set height of peak
	virtual void  SetHeight(const Double_t height)	{ fHeight = height; }
	//! Set intensity of peak
	virtual void  SetIntensity(const Double_t intensity) 	{ fIntensity = intensity; }
	//! Set flag on to enable fit of the peak
	virtual void  EnableFit()  { fIsFitOn = true; }
	//! Set flag off to enable fit of the peak
	virtual void  DisableFit() 	{ fIsFitOn = false; }
	virtual void FitOn(Bool_t fit_on = true); //*TOGGLE* *GETTER=FitFlag
	
	//! Set flag for combined fit
	virtual void  SetCombinedFlag(Bool_t flag) { fIsCombined = flag; }

	//! Get height of peak 
	virtual Double_t GetHeight()    const 	{ return fHeight; }
	//! Get intensity of peak
	virtual Double_t GetIntensity() const 	{ return fIntensity; }
	//! Get dimension of peak
	virtual UShort_t GetDimension() const 	{ return fDimension; }
	//! Get flag for enable fit
	virtual Bool_t   FitFlag()      const 	{ return fIsFitOn; }
	//! Get flag for combined fit
	virtual Bool_t   CombinedFlag() const  { return fIsCombined; }

	//! print current peak
	virtual void Print(Option_t* opt = "") const;

	/* pure virtual functions */

    //! to determine if a point is in bg. 0 likely in peak, 1 in bg, 2 outside bg
    virtual Short_t IsPointInBkg(Double_t x, Double_t y = 0) = 0;

	//! Set pre-defined function to fit the signal
	virtual TF1 *SetSignalFunction(const char* nameFunc = "gaus") = 0; 

	//! Set function to fit the signal 
	virtual TF1 *SetSignalFunction(const TF1* func) = 0; 

	//! Access to bkg function ... don't delete it !
	virtual TF1 *SignalFunction() = 0;
	
	//! Set pre-defined function for background during fit
	virtual TF1 *SetBkgFunction(const char* nameFunc = "-") = 0; 
	
	//!  Set user defined function for background during fit
	virtual TF1 *SetBkgFunction(const TF1* func) = 0; 
	
	//! Access to bkg function ... don't delete it !
	virtual TF1 *BkgFunction() = 0;
	
	//! Fit peak with background
	virtual void Fit(TH1 *h, Option_t* optFit = "RN", Option_t* optBkg = "lin") = 0; 

	//! Fit multi-defined peak with a common background
	virtual void FitCombined(TH1 *h, TList* peakList, Option_t* optFit = "RN", Option_t* optBkg = "lin") = 0; 

	//! Get position of peak
	virtual Double_t GetPosition(Option_t* axis = "X") const = 0;

	//! Get FWHM of peak
	virtual Double_t GetFWHM(Option_t* axis = "X")     const = 0;

	//! Set position of peak
	virtual void SetPosition(const Double_t position, Option_t* axis = "X") = 0;

	//! Set FWHM of peak
	virtual void SetFWHM(const Double_t FWHM, Option_t* axis = "X") = 0;   
	
	//! Set vis attribute to be painted in the following frame
	virtual void PaintFor(Double_t xmin, Double_t xmax, Double_t ymin, Double_t ymax) = 0;         

protected:
	Double_t fHeight;      ///< height of the Peak
	Double_t fIntensity;   ///< intensity of the peak
	UShort_t fDimension;   ///< dimension of the peak
	Bool_t   fIsFitOn;     ///< flag for fitted peak
	Bool_t   fIsCombined;  ///< flag to stated peak is part of a combined fit
  
	ClassDef(TPGbasePeak,1); // Class definition for TPGbasePeak
};

#endif
