#ifndef _TAWDparTime_HXX
#define _TAWDparTime_HXX
/*!
  \file TAWDparTime.hxx
  \brief   Declaration of TAWDparTime.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
#include <map>
using namespace std;

#include "TString.h"
#include "TAGpara.hxx"
#include "TATWparameters.hxx"
#include "TWaveformContainer.hxx"

//##############################################################################

class TAWDparTime : public TAGpara {
   
  public:
  TAWDparTime();
  virtual         ~TAWDparTime();

  virtual void    Clear(Option_t* opt="");
  virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
  
  vector<double> GetRawTimeArray(int iBo, int iCha, int TrigCell);
  void InitMap();
  bool FromFileCFD(TString fileName = "");
  bool FromFileTcal(TString fileName = "");
  void GetTimeInfo(FILE*);
  void SetTimeCal(int iBo, int iCha, vector<float> tvec);

  const double sec2Nano = 1E9;

  inline string GetCFDalgo(string det) { return fCfdAlgo.find(det)->second; }
  inline double GetCFDfrac(string det) { return fCfdFrac.find(det)->second; }
  inline double GetCFDdel(string det)  { return fCfdDel.find(det)->second;  }

private:
  map<pair<int,int>, vector<double>> fTimeParCal;
  map<string,string> fCfdAlgo;
  map<string,double> fCfdFrac;
  map<string,double> fCfdDel;

   ClassDef(TAWDparTime,1)
};

#endif
