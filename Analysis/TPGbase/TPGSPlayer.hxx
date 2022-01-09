


#ifndef TPGSPlayer_h
#define TPGSPlayer_h

#include "TPGspectrumPlayer.hxx"

class TPolyMarker;
class TSpline3;


/** Most of the methods relies on graphical approach
 
 Below an example of fitting in a 152Eu source spectrum:
 \code 
 TCanvas* c1 = new TCanvas("c1","titre",10,10,700,500);
 c1->Draw();
 TH1F* h = new TH1F();
 h->SetAxisRange(2100, 2400);
 h->Draw(); // draw the spectrum of 152Eu in the given range.
 
 TPGSPlayer* s = new TPGSPlayer();
 // place the peaks using 'a' key 
 // or use FindPeaks() method: press 's' whenever the line pointed a peak you want to add
 s->FitAll(); // fits the peaks with gaussian shape with a linear background
 \endcode
 
 The FitAll(..) method fits all the positionned peaks in the spectrum. Peaks separated less than 2*fwhm 
 (default wise fwhm = 6) are fitting with the CombinedFit(...) method.
 
 \code 
 // You can also defined background by pressing key 'b' to set each point of the background spline
 s->Background() // fit the background point with a TSpline
 s->DoBackground // fit the background point with a TSpline & removed background from current histogramm
 \endcode
 
 */
class TPGSPlayer : public TPGspectrumPlayer
{
public:
   TPGSPlayer();
   virtual ~TPGSPlayer();
   
   //! Find peak in current histo graphically  
   virtual Int_t FindPeaks(TH1 *histo, Option_t* opt = "");
   
   //! Find peak in current histo graphically 
   virtual Int_t FindPeaks(Option_t* opt = "") { return TPGspectrumPlayer::FindPeaks(opt); }
   
   //! Compute the background for that histogram
   virtual TH1 *Background(const TH1 *histo, Option_t* /*opt = ""*/);
   
   //! Compute the background for the histogram in the current pad
   virtual TH1 *Background(Option_t* opt = "")  { return TPGspectrumPlayer::Background(opt); }
   
   //! To change the parameters for that algorithm
   virtual Bool_t SetParameter(const char* name, Double_t value);
   //! To change the parameters for that algorithm
   virtual Bool_t SetParameter(const char* name, Int_t value) { return TPGspectrumPlayer::SetParameter(name,value); }
   virtual Bool_t SetParameter(const char* name, const TObject *value) { return TPGspectrumPlayer::SetParameter(name,value); }	
   
   ClassDef(TPGSPlayer, 0);  // some standard/graphical approach
};

#endif
