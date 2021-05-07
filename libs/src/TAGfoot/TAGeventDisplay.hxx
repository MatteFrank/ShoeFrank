
#ifndef _TAGeventDisplay_HXX_
#define _TAGeventDisplay_HXX_

#include "TAEDbaseInterface.hxx"

/** TAGeventDisplay FOOT class to work on event display
 
 */
#include "TAEDcluster.hxx"
#include "TAEDtrack.hxx"
#include "TAEDwire.hxx"
#include "TAGbaseEventDisplay.hxx"

/** TAFOeventnDisplay FOOT class to work on event display
 
 */
#include <map>

#include "TVirtualMagField.h"

#include "TADIeveField.hxx"
#include "TADIgeoField.hxx"

#include "BaseReco.hxx"

class TEveDigitSet;
class TGCheckButton;

class TAGeventDisplay : public TAGbaseEventDisplay
{
   
protected:
   //! default constructor
   TAGeventDisplay(const TString expName, Int_t runNumber, Int_t type);
   
protected:
   static TAGeventDisplay* fgInstance; // static instance of class

public:
   //! Instance of class
   static TAGeventDisplay* Instance(const TString name = "", Int_t runNumber = -1, Int_t type = 0);
   
   virtual ~TAGeventDisplay();
   
   //! Set local reconstruction
   void SetLocalReco();
   
   ClassDef(TAGeventDisplay, 1); // Base class for event display
};


#endif
