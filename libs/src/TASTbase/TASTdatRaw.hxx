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
  TASTrawHit( TWaveformContainer &w);
  TASTrawHit(Int_t cha ,Int_t board, Double_t charge,
	     Double_t amplitude, Double_t pedestal,
	     Double_t time,Int_t isclock,Double_t clock_time,Int_t TriggerType);
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
  void       NewHit(TWaveformContainer &W);
  void       NewHit(int cha, int board, double pedestal, double amplitude, double charge, double time, int TriggerType, bool isclock, double ClockRaisingTime);
  virtual void    Clear(Option_t* opt="");
    void SetupClones();
    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    inline void UpdateRunTime(int value){m_run_time+=value;}
    static const Char_t* GetBranchName()   { return fgkBranchName.Data();   }
  
    Int_t           nirhit;		    // 
    TClonesArray*   hir;			// hits

  ClassDef(TASTdatRaw,3);

  private:
    static TString fgkBranchName;    // Branch name in TTree
     int m_run_time;

};

#endif
