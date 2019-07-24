#ifndef _TAGbaseWD_HXX
#define _TAGbaseWD_HXX
/*!
  \file
  \version $Id: TAGbaseWD.hxx,v 1.0 2011/04/01 18:11:59 asarti Exp $
  \brief   Declaration of TAGbaseWD.
*/
/*------------------------------------------+---------------------------------*/
#include <vector>
using namespace std;
#include "TObject.h"
#include "TClonesArray.h"
#include "TAGroot.hxx"
#include "TAGdata.hxx"
#include "TAGdataDsc.hxx"
#include "TWaveformContainer.hxx"
//

/**
 * This class stores the params of a single channel waveform
 */
class TAGbaseWD : public TAGobject {
  public:
    TAGbaseWD();
    virtual         ~TAGbaseWD();
    TAGbaseWD( TWaveformContainer &w);
   
    void  SetData(Int_t cha ,Int_t board, Double_t charge,
		  Double_t ampl, Double_t apedestal,
		  Double_t atime, Int_t iscl,Double_t clock_t,Int_t TriggerType);
    //

  Double_t 		Clocktime();
  Int_t 			IsClock();
  //
  inline Int_t ChID() const {  return chid; }
  inline Int_t BoardId() const { return boardid; }
  inline Double_t Pedestal() const { return pedestal; }
  inline Double_t Amplitude() const { return amplitude; }
  inline Int_t TriggerType() const { return triggertype; }
  inline Double_t GetChiSquare(){return chisquare;}
  
  inline void SetAmplitdue(double amplitude) { amplitude=amplitude; }
  inline void SetPedestal(double pedestal) { pedestal=pedestal; }
  inline void SetBoardId(Int_t boardid) { boardid=boardid; }
  inline Int_t IDMC() const {  return mcid; }
  inline Double_t Time() const {  return time; }
  inline Double_t Charge() const {  return chg; }
  inline void SetChID(int id) {  chid = id;  return; }
  inline void SetMCID(int id) {  mcid = id;  return; }
  inline void SetTime(double atime) {  time = atime;  return; }
  inline void SetCharge(double achg) {  chg = achg;  return; }
  inline void SetChiSquare(double achisq) {  chisquare = achisq;  return; }
    
  ClassDef(TAGbaseWD,4);
    //
  private:
    Double_t time;    
    Double_t chg;
    Double_t pedestal;
    Double_t amplitude;
    Int_t chid;
    Int_t boardid;
    Int_t mcid;
    Int_t isclock;
    Int_t triggertype;
    Double_t clock_time;
    Double_t chisquare;

};

#endif
