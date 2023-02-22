#ifndef TANLAnalysisManager_h
#define TANLAnalysisManager_h

#include "TANLbaseAnalysis.hxx"
#include <list>

class TANLAnalysisManager : public TANLbaseAnalysis {
public:

  TANLAnalysisManager();

  virtual ~TANLAnalysisManager();

 //
  virtual void Setup();

  virtual void BeforeEventLoop();

  virtual void ProcessEvent();

  virtual void AfterEventLoop();

  // specific for the analysis manager

  void Register(TANLbaseAnalysis * anAnalysis);
  void Remove(TANLbaseAnalysis * anAnalysis);
  bool hasAnalysis(TANLbaseAnalysis * anAnalysis) const;
  int  GetAnalysisMethods() const {return methods.size();};
  void Clean() { methods.clear();};

private:
  std::list<TANLbaseAnalysis*> methods;

};

#endif
