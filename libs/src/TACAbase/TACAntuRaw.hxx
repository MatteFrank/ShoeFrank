#ifndef _TACAntuRaw_HXX
#define _TACAntuRaw_HXX
/*!
  \file TACAntuRaw.hxx
  \brief   Declaration of TACAntuRaw.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;
#include "TObject.h"
#include "TClonesArray.h"
#include "TAGdata.hxx"
#include "TAGbaseWD.hxx"


/*!
 \class TACArawHit
 \brief class for raw hit.
 */

class TACArawHit : public TAGbaseWD {

protected:
   Double32_t fTemp;   ///< temperature

public:
   TACArawHit();
   TACArawHit(TWaveformContainer *w, double temp);
   virtual         ~TACArawHit();

   //! Get temperature
   Double_t GetTemperature()          const { return fTemp;  }   
   //! Set temperature
   void SetTemperature(double aTemp)        { fTemp = aTemp; }

   ClassDef(TACArawHit, 3);

//   virtual double ComputeTime( TWaveformContainer *w, double frac, double del, double tleft, double tright);
//   virtual double ComputeCharge( TWaveformContainer *w, double thr);
//   virtual double ComputeAmplitude( TWaveformContainer *w);
//   virtual double ComputePedestal( TWaveformContainer *w, double thr);
//   virtual double ComputeBaseline( TWaveformContainer *w);

};

//##############################################################################
/*!
 \class TACAntuRaw
 \brief Container for raw hit.
 */

class TACAntuRaw : public TAGdata {
public:

  TACAntuRaw();
  virtual         ~TACAntuRaw();

  Int_t             GetHitsN() const;
  TACArawHit*       GetHit(Int_t i_ind);
  const TACArawHit* GetHit(Int_t i_ind) const;
  
  void              NewHit(TWaveformContainer *W, double temp=25);
  void              SetupClones();
  
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;

public:
   //! Get branch name
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
private:
   Int_t           fHitsN;          ///< Number of hits (useless)
   TClonesArray*   fListOfHits;     ///< list of  hits

private:
   static TString fgkBranchName;    ///< Branch name in TTree
  
  ClassDef(TACAntuRaw,1);

};

#endif
