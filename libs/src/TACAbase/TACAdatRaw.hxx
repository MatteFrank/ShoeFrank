#ifndef _TACAdatRaw_HXX
#define _TACAdatRaw_HXX
/*!
  \file
  \version $Id: TACAdatRaw.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TACAdatRaw.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
using namespace std;

#include "TObject.h"
#include "TClonesArray.h"

#include "TAGdata.hxx"

class TACArawHit : public TObject {
  public:
    TACArawHit();
    TACArawHit(int typ, int cha, double charge, double time);
    virtual         ~TACArawHit();

    void            SetData(Int_t type, Int_t id, Double_t time, Double_t charge);
    Double_t        Time() const;
    Double_t        Charge() const;
    Int_t           ChID() const;
    Int_t           Type() const;

    void            SetTime(double time);
    void            SetCharge(double charge);
    void            SetChID(int id);  //SC channel ID
    void            SetType(int typ); //meaningless for now.

    ClassDef(TACArawHit,1)

  private:
    Double_t fTime;    
    Double_t fCharge;    
    Int_t    fType;
    Int_t    fChannelID;
};

//##############################################################################

class TACAdatRaw : public TAGdata {
  public:

                    TACAdatRaw();
    virtual         ~TACAdatRaw();

    void            SetCounter(Int_t i_ntdc, Int_t i_nadc, Int_t i_ndrop);

    Int_t             GetHitsN() const;
    TACArawHit*       GetHit(Int_t i_ind);
    const TACArawHit* GetHit(Int_t i_ind) const;

    void            SetTrigTime(double time);
    Double_t        TrigTime() const;

    Int_t           NTdc() const;
    Int_t           NAdc() const;
    Int_t           NDrop() const;

    virtual void    Clear(Option_t* opt="");

    void SetupClones();

    virtual void    ToStream(ostream& os=cout, Option_t* option="") const;

    ClassDef(TACAdatRaw,1)

  private:
    Int_t           fHitsN;		    // 
    TClonesArray*   fListOfHits;	    // hits
    Double_t        fTrigTime;       // SC trigger time
    Int_t           fiNAdc;		    // 
    Int_t           fiNTdc;		    // 
    Int_t           fiNDrop;		    // 

};

#include "TACAdatRaw.icc"

#endif
