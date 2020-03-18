#ifndef _TASTdatRaw_HXX
#define _TASTdatRaw_HXX
/*!
  \file
  \version $Id: TASTdatRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TASTdatRaw.
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

class TASTdatRaw : public TAGdata {
public:

  TASTdatRaw();
  virtual         ~TASTdatRaw();

  TASTrawHit*       Hit(Int_t i_ind);
  const TASTrawHit* Hit(Int_t i_ind) const;

  TASTrawHit*       SuperHit(){return superhit;}  
  
  void NewHit(TWaveformContainer *W);
  void NewSuperHit(vector<TWaveformContainer*>);
  void SetupClones();

   
  virtual void    Clear(Option_t* opt="");
  virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

  inline void UpdateRunTime(int value){m_run_time+=value;}
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
  Int_t           nirhit;		    //
  TClonesArray*   hir;			// hits
  TASTrawHit* superhit;  //sum

  
  ClassDef(TASTdatRaw,3);
  
  
private:

  static TString fgkBranchName;    // Branch name in TTree
  int m_run_time;

  
};

#endif
