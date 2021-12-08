
#ifndef _TAGeventDisplay_HXX_
#define _TAGeventDisplay_HXX_

/*!
 \file TAGeventDisplay.hxx
 \brief FOOT class to work on event display
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include <map>

#include "TAEDbaseInterface.hxx"

#include "TAEDcluster.hxx"
#include "TAEDtrack.hxx"
#include "TAEDwire.hxx"
#include "TAGbaseEventDisplay.hxx"

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
   static TAGeventDisplay* fgInstance; ///< static instance of class

public:
   //! Instance of class
   static TAGeventDisplay* Instance(const TString name = "", Int_t runNumber = -1, Int_t type = 0);
   
   virtual ~TAGeventDisplay();
   
   //! Set local reconstruction
   void SetLocalReco();
   
   //! skip event
   Bool_t SkipEvent();
   
   ClassDef(TAGeventDisplay, 1); ///< Base class for event display
};


#endif
