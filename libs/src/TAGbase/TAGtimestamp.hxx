#ifndef _TAGtimestamp_HXX
#define _TAGtimestamp_HXX
/*!
  \file
  \version $Id: TAGtimestamp.hxx,v 1.1 2001/12/09 14:25:29 mueller Exp $
  \brief   Declaration of TAGtimestamp.
*/
/*------------------------------------------+---------------------------------*/

#include "Rtypes.h"

class TAGtimestamp {
  public:
                    TAGtimestamp();
    explicit        TAGtimestamp(Int_t i_time);
    virtual         ~TAGtimestamp();

    void            FromDouble(Double_t d_time);
    void            FromUnixTime(Int_t i_time) { fiSec = i_time; fiUSec = 0;}
    void            SetCurrent();

    Double_t        ToDouble()   const { return (Double_t)fiSec + 1.e-6 * (Double_t)fiUSec;}
    Int_t           ToUnixTime() const { return fiSec;}
    Double_t        Age() const;

    Bool_t          Valid() const { return fiSec >= 0;}
    void            Clear() { fiSec = -1; fiUSec = 0;}

    TAGtimestamp&     operator+(Double_t d_deltatime);
    TAGtimestamp&     operator-(Double_t d_deltatime);

    friend Double_t   operator-(const TAGtimestamp& lhs,
				const TAGtimestamp& rhs);

    ClassDef(TAGtimestamp,1)

  private:				    // NOTE: CUSTOM STREAMER
    Int_t           fiSec;		    //!
    Int_t           fiUSec;		    //!
};

#endif
