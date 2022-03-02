#ifndef TPGSPlayer_h
#define TPGSPlayer_h

/*!
 \file TPGSPlayer.hxx
 \brief   Declaration of TPGSPlayer.
 */

#include "TPGspectrumPlayer.hxx"

class TPolyMarker;
class TSpline3;

class TPGSPlayer : public TPGspectrumPlayer
{
public:
   TPGSPlayer();
   virtual ~TPGSPlayer();
   
   // Find peak in current histo graphically
   virtual Int_t FindPeaks(TH1 *histo, Option_t* opt = "");
   
   // Compute the background for that histogram
   virtual TH1 *Background(const TH1 *histo, Option_t* opt = "");
   
   //! Find peak in current histo graphically 
   virtual Int_t FindPeaks(Option_t* opt = "") { return TPGspectrumPlayer::FindPeaks(opt); }
   
   //! Compute the background for the histogram in the current pad
   virtual TH1 *Background(Option_t* opt = "")  { return TPGspectrumPlayer::Background(opt); }
   
   // To change the parameters for that algorithm
   virtual Bool_t SetParameter(const char* name, Double_t value);
   //! To change the parameters for that algorithm
   virtual Bool_t SetParameter(const char* name, Int_t value) { return TPGspectrumPlayer::SetParameter(name,value); }
   //! To change the parameters for that algorithm
   virtual Bool_t SetParameter(const char* name, const TObject *value) { return TPGspectrumPlayer::SetParameter(name,value); }	
   
   ClassDef(TPGSPlayer, 0);  ///< some standard/graphical approach
};

#endif
