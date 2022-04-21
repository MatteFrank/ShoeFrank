#ifndef _TABMparMap_HXX
#define _TABMparMap_HXX
/*!
\file    TABMparMap.hxx
\brief   Beam Monitor map parameters
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

    //default
    TABMparMap();
    virtual         ~TABMparMap();
    Bool_t          FromFile(const TString& name, TABMparGeo *bmgeo);

    //getters
    //! Get the number of channels read by the TDC
    Int_t GetTdcMaxCh() const {return fTdcCha[0];}
    //! Get the trigger reference channel number needed for the BM cell measurements
    Int_t GetBmTrefCh()  const {return fTdcCha[1];}
    //! Get the DAQ additional trigger channel number used in GSI2019
    Int_t GetDaqTrefCh() const {return fTdcCha[2];}
    //! Get the number of channels read by the scaler
    Int_t GetScaMaxCh()  const {return fScaCha[0];}
    //! Get the number of channels read by the ADC
    Int_t GetAdcMaxCh()  const {return fAdcCha[0];}
    //! Get the Beam Monitor boardnum, used in the BM stand alone acquisitions
    Int_t GetBoardNum()  const {return fboardNum; }

    //! Get the TDC relevant channel number for a given ch position in the TDC fTdcCha vector
    Int_t GetTdcCh(Int_t ch)     const {return (fTdcCha.GetSize()>ch && ch>=0) ? fTdcCha[ch] : -1;}
    //! Get the scaler relevant channel number for a given ch position in the Scaler  fScaCha vector
    Int_t GetScaCh(Int_t ch)     const {return (fScaCha.GetSize()>ch && ch>=0) ? fScaCha[ch] : -1;}
    //! Get the ADC relevant channel number for a given ch position in the ADC fAdcCha vector
    Int_t GetAdcCh(Int_t ch)     const {return (fAdcCha.GetSize()>ch && ch>=0) ? fAdcCha[ch] : -1;}

    //! Get the cellid index [0-35] for a given tdc channel value
    Int_t tdc2cell(Int_t tdccha){return (tdccha>=0 && tdccha<fTdcCha[1]) ? fTdc2CellVec[tdccha] : -1;};
    //! Get the tdc channel for a given cellid [0-35] index value
    Int_t cell2tdc(Int_t cell){return (cell>=0 && cell<36) ? fCell2TdcVec[cell] : -1;};

    virtual void    Clear(Option_t* opt="");

  private:
    TArrayI fTdcCha;                //tdc relevant channels: 0=Maxcha, 1=bm ref ch, 2=daq ref ch
    TArrayI fScaCha;                //scaler relevant channels: 0=Maxcha
    TArrayI fAdcCha;                //Adc relevant channels: 0=Maxcha
    map<Int_t,Int_t> fTdc2CellVec;  //each position of this vector correspond to a tdc channel, the value stored corresponds to the bm cell index [0-35] or -1000 if is the bmtrefCh, and -1001 if is the daqtrefch, otherwise is -1
    map<Int_t,Int_t> fCell2TdcVec;  //each position of this vector correspond to a bm cell index (0-35), the value stored correspond to the tdc channel if settled, otherwise =-1
    Int_t fboardNum;                //board_num of the BM acquisition software (adopted in TABMactVmeReader)

   ClassDef(TABMparMap,1)
};

#endif
