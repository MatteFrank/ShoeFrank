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
  TATWrawHit( TWaveformContainer &w);
  TATWrawHit(Int_t cha ,Int_t board, Double_t charge,
	     Double_t amplitude, Double_t pedestal,
			   Double_t time,Int_t isclock,Double_t clock_time,Int_t TriggerType);
    ClassDef(TATWrawHit,2);
    //
};

//##############################################################################

class TATWdatRaw : public TAGdata {
  public:
                    TATWdatRaw();
    virtual         ~TATWdatRaw();
    TATWrawHit*       Hit(Int_t i_ind);
    const TATWrawHit* Hit(Int_t i_ind) const;
  void       NewHit(TWaveformContainer &W);
  void       NewHit(int cha, int board, double pedestal, double amplitude, double charge, double time, int TriggerType, bool isclock, double ClockRaisingTime);
  virtual void    Clear(Option_t* opt="");
    void SetupClones();
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;
    ClassDef(TATWdatRaw,3);

    static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
  public:
    Int_t           nirhit;		    // 
    TClonesArray*   hir;			// hits
  private:
    static TString fgkBranchName;    // Branch name in TTree

};

#endif
