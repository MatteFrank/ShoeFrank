
#ifndef _TAFOeventDisplay_HXX_
#define _TAFOeventDisplay_HXX_

#include "TAEDbaseInterface.hxx"

/** TAFOeventDisplay FOOT class to work on event display
 
 */
#include "TAEDcluster.hxx"
#include "TAEDtrack.hxx"
#include "TAEDwire.hxx"
#include "TAEDglbTrack.hxx"
#include "TAEDglbTrackList.hxx"
#include "TADIeveTrackPropagator.hxx"
#include "TAFObaseEventDisplay.hxx"

/** TAFOeventnDisplay FOOT class to work on event display
 
 */
#include <map>

#include "TVirtualMagField.h"

#include "TADIeveField.hxx"
#include "TADIgeoField.hxx"

#include "BaseReco.hxx"

class TEveDigitSet;
class TGCheckButton;

class TAFOeventDisplay : public TAFObaseEventDisplay
{
   
protected:
   //! default constructor
   TAFOeventDisplay(const TString expName, Int_t runNumber, Int_t type);
   
protected:
   static TAFOeventDisplay* fgInstance; // static instance of class

public:
   //! Instance of class
   static TAFOeventDisplay* Instance(const TString name = "", Int_t runNumber = -1, Int_t type = 0);
   
   virtual ~TAFOeventDisplay();
   
   //! Set local reconstruction
   void SetLocalReco();
   
   ClassDef(TAFOeventDisplay, 1); // Base class for event display
};


#endif
