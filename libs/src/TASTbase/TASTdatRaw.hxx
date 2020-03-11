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
  virtual         ~TASTrawHit();
  TASTrawHit(TWaveformContainer &w);
  ClassDef(TASTrawHit,2);
    //
};

//##############################################################################

class TASTdatRaw : public TAGdata {
public:
  TASTdatRaw();
  virtual         ~TASTdatRaw();
  Int_t             GetHitsN() const;
  TASTrawHit*       GetHit(Int_t i_ind);
  const TASTrawHit* GetHit(Int_t i_ind) const;
  void              NewHit(TWaveformContainer &W);
  virtual void      Clear(Option_t* opt="");
  void              SetupClones();
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;

  inline void UpdateRunTime(int value){m_run_time+=value;}
  static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
private:
  Int_t           fHitsN;		    // 
  TClonesArray*   fListOfHits;			// hits

  ClassDef(TASTdatRaw,3);

private:
  static TString fgkBranchName;    // Branch name in TTree
  int m_run_time;

};

#endif
