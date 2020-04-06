#ifndef _TATWdatRaw_HXX
#define _TATWdatRaw_HXX
/*!
  \file
  \version $Id: TATWdatRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TATWdatRaw.
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
class TATWrawHit : public TAGbaseWD {

public:
  TATWrawHit();
  virtual         ~TATWrawHit();

  virtual double ComputeTime( TWaveformContainer *w, double frac, double del, double tleft, double tright);
  virtual double ComputeCharge( TWaveformContainer *w);
  virtual double ComputeAmplitude( TWaveformContainer *w);
  virtual double ComputePedestal( TWaveformContainer *w);
  virtual double ComputeBaseline( TWaveformContainer *w);
  
  TATWrawHit( TWaveformContainer *w);

  ClassDef(TATWrawHit,2);
  //
};

//##############################################################################

class TATWdatRaw : public TAGdata {
public:

  TATWdatRaw();
  virtual         ~TATWdatRaw();
  Int_t             GetHitsN() const;
  TATWrawHit*       GetHit(Int_t i_ind);
  const TATWrawHit* GetHit(Int_t i_ind) const;
  void              NewHit(TWaveformContainer *w);


  virtual void    Clear(Option_t* opt="");
  void            SetupClones();
  virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
  void            UpdateRunTime(int value){fRunTime+=value;}

  ClassDef(TATWdatRaw,3);


  
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
  
private:
  static TString fgkBranchName;    // Branch name in TTree
  Int_t           fHitsN;		    // 
  TClonesArray*   fListOfHits;
  Int_t           fRunTime;
};

#endif
