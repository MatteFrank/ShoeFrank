#ifndef _TAIRparTools_HXX
#define _TAIRparTools_HXX
/*!
  \file
  \brief   Declaration of TAIRparTools.
 
  \author Ch. Finck
*/
/*------------------------------------------+---------------------------------*/

#include "Riostream.h"

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "TAGpara.hxx"


//##############################################################################

class TAIRparTools : public TAGpara {
      
protected:
   ifstream fFileStream; // parameter file
   TString  fFileName;   // name of the file
   Int_t    fDebugLevel; // debug level
   
public:
   
   TAIRparTools();
   virtual ~TAIRparTools();
   
   Bool_t  Open(const TString& name);
   void    ReadItem(TString& item);
   void    ReadItem(Int_t &arg);
   void    ReadItem(Float_t &arg);
   void    ReadItem(Float_t* coeff, Int_t size, const Char_t delimiter = ' ');
   void    ReadVector3(TVector3 &arg);
   void    ReadStrings(Char_t *aString, Int_t aLength);
   TString Normalize(const char* line);

   //! Get file name
   TString GetFileName() const { return fFileName; }

   ClassDef(TAIRparTools,1)
};

#endif
