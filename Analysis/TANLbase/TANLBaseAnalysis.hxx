#ifndef TANLbaseAnalysis_h
#define TANLbaseAnalysis_h

#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"
#pragma GCC diagnostic warning "-Wformat-nonliteral"
#pragma GCC diagnostic warning "-Wcast-align"
#pragma GCC diagnostic warning "-Wpointer-arith"
#pragma GCC diagnostic warning "-Wmissing-declarations"
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Wshadow"

// Abstract base class TANLbaseAnalysis
// it provides the interface for a generic Analysis class
//  

class TANLbaseAnalysis {
public:

  TANLbaseAnalysis(){}

  virtual ~TANLbaseAnalysis();

  //
  virtual void Setup();

  virtual void BeforeEventLoop(){};

  virtual void ProcessEvent(){};

  virtual void AfterEventLoop(){};


private:
};

#endif
