#ifndef TANLVTAnalysis_h
#define TANLVTAnalysis_h

#include "TANLbaseAnalysis.hxx"
#include <list>

class TH1D;
class TH2D;

class TANLVTAnalysis : public TANLbaseAnalysis {
public:

  TANLVTAnalysis();

  virtual ~TANLVTAnalysis();

  //
  virtual void Setup();

  virtual void BeforeEventLoop();

  virtual void ProcessEvent();

  virtual void AfterEventLoop();

private:

  enum hist1Name {,
		  last1d};
  enum hist2Name {,
		  last2d};

  TH1D * h1v[last1d];
  TH1D * h2v[last2d];

};

#endif;
