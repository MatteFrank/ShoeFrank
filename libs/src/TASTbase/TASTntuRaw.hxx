#ifndef _TASTntuRaw_HXX
#define _TASTntuRaw_HXX
/*!
  \file
  \version $Id: TASTntuRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TASTntuRaw.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;
#include "TObject.h"
#include "TClonesArray.h"
#include "TAGdata.hxx"
#include "TAGbaseWD.hxx"
//

/**
 * This class stores the params of a single channel waveform
 */
class TASTrawHit : public TAGbaseWD {

public:
  TASTrawHit();
  TASTrawHit(TWaveformContainer *w);
  virtual         ~TASTrawHit();
  
  virtual double ComputeTime( TWaveformContainer *w, double frac, double del, double tleft, double tright);
  virtual double ComputeCharge( TWaveformContainer *w);
  virtual double ComputeAmplitude( TWaveformContainer *w);
  virtual double ComputePedestal( TWaveformContainer *w);
  virtual double ComputeBaseline( TWaveformContainer *w);
  
  ClassDef(TASTrawHit,2);
  //
};

//##############################################################################

class TASTntuRaw : public TAGdata {
public:

  TASTntuRaw();
  virtual         ~TASTntuRaw();

  Int_t             GetHitsN() {return fHistN;}
  
  TASTrawHit*       GetHit(Int_t i_ind);
  const TASTrawHit* GetHit(Int_t i_ind) const;

  TASTrawHit*       GetSuperHit(){return fSuperHit;}
  
  void              NewHit(TWaveformContainer *W);
  void              NewSuperHit(vector<TWaveformContainer*>);
  void              SetupClones();

   
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;

  void              UpdateRunTime(int value){fRunTime+=value;}
   
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
  ClassDef(TASTntuRaw,3);
  
private:
   Int_t           fHistN;          //
   TClonesArray*   fListOfHits;         // hits
   TASTrawHit*     fSuperHit;  //sum
   Int_t           fRunTime;

   static TString fgkBranchName;    // Branch name in TTree
};

#endif
