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
#include "TH1D.h"

#define WAVE_CFD_ID 999
#define WAVE_ID 998


class TASTrawHit : public TObject {
  public:
  
  TASTrawHit(int ch_num , vector<double> time, vector<double> amplitude);
  virtual         ~TASTrawHit();

  virtual void Clear(Option_t* /*option*/);

  
  inline int GetChannel(){return m_ch_num;}
  inline int GetBoard(){return m_board_id;}
  inline vector<double> GetTimeArray(){return m_time;}
  inline vector<double> GetAmplitudeArray(){return m_amplitude;}
  inline void SetArrivalTime(double value){m_tarr = value;}
  inline void SetCharge(double value){m_charge = value;}

    ClassDef(TASTrawHit,1)

  private:
    vector<double> m_time, m_amplitude;
    int m_ch_num, m_board_id;
    double m_tarr;
    double m_charge;
  
};




//##############################################################################

class TASTdatRaw : public TAGdata {
  public:

 TASTdatRaw();
  virtual         ~TASTdatRaw();
  
  
 
  void AddWaveform(int ch_num, vector<double> time, vector<double> amplitude);
  void SumWaveforms();
  inline TASTrawHit* GetWaveCFD(){return fSumWaves_cfd;}
  inline TASTrawHit* GetWaveSum(){return fSumWaves;}
  //bool IsSTChannel();


  inline void SetTriggerTime(double value){fdTrgTime = value;}
  inline void SetCharge(double value){fdCharge = value;}

  inline double     TrigTime(){return fdTrgTime;}
  inline double     Charge(){return fdCharge;}
  inline vector<TASTrawHit*>GetHits(){return  fListOfWaveforms;}
  inline vector<TASTrawHit*>GetHitsCFD(){return  fListOfWaveforms_cfd;}  
  
  virtual void      Clear(Option_t* opt="");
  virtual void      ToStream(ostream& os=cout, Option_t* option="") const;
  
  

  
    ClassDef(TASTdatRaw,1)
      
  private:


    vector<TASTrawHit*> fListOfWaveforms; // hits
    vector<TASTrawHit*> fListOfWaveforms_cfd; // hits
    TASTrawHit*     fSumWaves; // hits
    TASTrawHit*     fSumWaves_cfd; // hits
    double          fdTrgTime;   // SC trigger time
    double          fdCharge;   // SC total charge
   
 

  
};

#include "TASTdatRaw.icc"

#endif
