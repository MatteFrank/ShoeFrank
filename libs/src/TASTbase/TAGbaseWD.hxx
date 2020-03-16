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
  TAGbaseWD(TWaveformContainer *w);
  virtual         ~TAGbaseWD();
  
  virtual double ComputeTime(TWaveformContainer *w, double frac, double del, double tleft, double tright);
  virtual double ComputeTimeSimpleCFD(TWaveformContainer *w, double frac);
  virtual double ComputePedestal( TWaveformContainer *w);
  virtual double ComputeBaseline( TWaveformContainer *w);
  virtual double ComputeCharge(TWaveformContainer *w);
  virtual double ComputeAmplitude(TWaveformContainer *w);


  inline Int_t GetChID() const {  return chid; }
  inline Int_t GetBoardId() const { return boardid; }
  inline Int_t GetTriggerType() const { return triggertypeId; }
  inline Int_t GetTriggerCellID() const { return triggercellId; }
  inline Double_t GetTime() const {  return time; }
  inline Double_t GetTimeOth() const {  return time_oth; }
  inline Double_t GetCharge() const {  return chg; }
  inline Double_t GetAmplitude() const { return amplitude; }
  inline Double_t GetPedestal() const { return pedestal; }
  inline Double_t GetBaseline() const { return baseline; }
  inline Int_t GetIDMC() const {  return mcid; }
   
  inline void SetChID(int id) {  chid = id;  return; }
  inline void SetBoardId(Int_t boardid) { boardid=boardid; }
  inline void SetTriggerType(int value){ triggertypeId = value; }
  inline void SetTriggerCellID(int value){ triggercellId = value; }
  inline void SetTime(double atime) {  time = atime;  return; }
  inline void SetTimeOth(double atime) {  time_oth = atime;  return; }
  inline void SetCharge(double achg) {  chg = achg;  return; }
  inline void SetMCID(int id) {  mcid = id;  return; }
  inline void SetAmplitude(double value) { amplitude=value; }
  inline void SetPedestal(double value) { pedestal=value; }

   
  ClassDef(TAGbaseWD,4);
    //
  protected:

  Double_t time;
  Double_t time_oth;
  Double_t chg;
  Double_t amplitude;
  Double_t baseline;
  Double_t pedestal;
  Int_t chid;
  Int_t boardid;
  Int_t mcid;
  Int_t triggertypeId;
  Int_t triggercellId;
  
   
};

#endif
