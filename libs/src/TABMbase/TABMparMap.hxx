#ifndef _TABMparMap_HXX
#define _TABMparMap_HXX
/*!
  \file
  \version $Id: TABMparMap.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
  \brief   Declaration of TABMparMap.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>

#include "TString.h"
#include <map>

#include "TAGparTools.hxx"
#include "TABMparGeo.hxx"

using namespace std;

//##############################################################################

class TABMparMap : public TAGparTools {
  public:

  TABMparMap();
    virtual         ~TABMparMap();
    Bool_t          FromFile(const TString& name, TABMparGeo *bmgeo);

    //getters
    Int_t GetTdcMaxCh() const {return fTdcCha[0];}
    Int_t GetBmTrefCh()  const {return fTdcCha[1];}
    Int_t GetDaqTrefCh() const {return fTdcCha[2];}
    Int_t GetScaMaxCh()  const {return fScaCha[0];}
    Int_t GetAdcMaxCh()  const {return fAdcCha[0];}
    Int_t GetBoardNum()  const {return fboardNum; }

    Int_t GetTdcCh(Int_t ch)     const {return (fTdcCha.GetSize()>ch && ch>=0) ? fTdcCha[ch] : -1;}
    Int_t GetScaCh(Int_t ch)     const {return (fScaCha.GetSize()>ch && ch>=0) ? fScaCha[ch] : -1;}
    Int_t GetAdcCh(Int_t ch)     const {return (fAdcCha.GetSize()>ch && ch>=0) ? fAdcCha[ch] : -1;}

    Int_t tdc2cell(Int_t tdccha){return (tdccha>=0 && tdccha<fTdcCha[1]) ? fTdc2CellVec[tdccha] : -1;};
    Int_t cell2tdc(Int_t cell){return (cell>=0 && cell<36) ? fCell2TdcVec[cell] : -1;};

    virtual void    Clear(Option_t* opt="");
   
  private:
    TArrayI fTdcCha;                //tdc relevant channels: 0=Maxcha, 1=bm ref ch, 2=daq ref ch
    TArrayI fScaCha;                //scaler relevant channels: 0=Maxcha
    TArrayI fAdcCha;                //Adc relevant channels: 0=Maxcha
    map<Int_t,Int_t> fTdc2CellVec;       //each position of this vector correspond to a tdc channel, the value stored correspond to the bm cell index (0-35) or -1000 if is the bmtrefCh, and -1001 if is the daqtrefch, otherwise is -1
    map<Int_t,Int_t> fCell2TdcVec;       //each position of this vector correspond to a bm cell index (0-35), the value stored correspond to the tdc channel if settled, otherwise =-1
    Int_t fboardNum;                  //board_num of the BM acquisition software (adopted in TABMactVmeReader)
   
   ClassDef(TABMparMap,1)
};

#endif
