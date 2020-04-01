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

#include "TAGpara.hxx"
#include "TABMparGeo.hxx"
#include "TABMparCon.hxx"

using namespace std;

//##############################################################################

class TABMparMap : public TAGpara {
  public:

    //~ enum { n_wires = 36,		    // ugly, to make root happy ...
	   //~ dim_cha  =  64,
	   //~ dim_addr = dim_cha
    //~ };
  
  TABMparMap();
    virtual         ~TABMparMap();

    Bool_t          FromFile(const TString& name, TABMparGeo *bmgeo);
    bool            GetWireIDFromTDC(int channel, int board, int &senseID, int &plane, int &view);
    
    //getters
    Int_t GetTrefCh()    const {return fTrefCh;    }
    Int_t GetTdcMaxcha() const {return fTdcMaxCha; }
    Int_t GetSca830Ch()  const {return fSca830ch;  }
    Int_t GetAdc792Ch()  const {return fAdc792ch;  }
    Int_t GetHmEvRead()  const {return fhmEvRead;  }
    Int_t GetBoardNum()  const {return fboardNum;  }
   
    Int_t tdc2cell(Int_t tdccha){return (tdccha>=0 && tdccha<fTdcMaxCha) ? fTdc2CellVec[tdccha] : -1;};
    Int_t cell2tdc(Int_t cell){return (cell>=0 && cell<36) ? fCell2TdcVec[cell] : -1;};    
    
    virtual void    Clear(Option_t* opt="");
    
    ClassDef(TABMparMap,1)

  private:
    Int_t fTrefCh;  //trigger reference channel
    Int_t fTdcMaxCha;//tdc number of channel
    vector<Int_t> fTdc2CellVec;//each position of this vector correspond to a tdc channel, the value stored correspond to the bm cell index (0-35) or -1000 if is the trefCh, otherwise is -1
    vector<Int_t> fCell2TdcVec;//each position of this vector correspond to a bm cell index (0-35), the value stored correspond to the tdc channel if settled, otherwise =-1, the last element (index=36 stores the trefCh)
    Int_t fSca830ch;//number of the scaler channel read
    Int_t fAdc792ch;//number of the adc channel read
    Int_t fhmEvRead;//read the scaler each hm_ev_read
    Int_t fboardNum;//board_num of the BM acquisition software
};

#endif
