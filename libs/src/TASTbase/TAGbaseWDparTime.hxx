#ifndef _TAGbaseWDparTime_HXX
#define _TAGbaseWDparTime_HXX
/*!
  \file
  \version $Id: TAGbaseWDparTime.hxx,v 1.1 2001/11/05 23:13:57 mueller Exp $
  \brief   Declaration of TAGbaseWDparTime.
*/
/*------------------------------------------+---------------------------------*/

#include <vector>
#include <map>
using namespace std;

#include "TString.h"
#include "TAGpara.hxx"
#include "TWaveformContainer.hxx"


#define NMAX_BO_ID 100
#define NMAX_CH_ID 18

//##############################################################################

class TAGbaseWDparTime : public TAGpara {
  public:

  
  TAGbaseWDparTime();
  virtual         ~TAGbaseWDparTime();

  virtual void    Clear(Option_t* opt="");
  virtual void    ToStream(ostream& os = cout, Option_t* option = "") const;
  
  vector<double> GetRawTimeArray(int iBo, int iCha, int TrigCell);
  void InitMap();
  bool FromFile(string expName = "", int iRunNumber = 0);
  void SetTimeCal(int iBo, int iCha, vector<float> tvec);

  ClassDef(TAGbaseWDparTime,1)

  const double sec2Nano = 1E9;
  
private:

  map<pair<int,int>, vector<double>> time_parcal;

  bool m_debug;
};

#endif
