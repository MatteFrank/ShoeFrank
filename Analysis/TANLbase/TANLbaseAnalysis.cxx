
#include "TANLbaseAnalysis.hxx"
#include "TAGgeoTrafo.hxx"


TANLbaseAnalysis::~TANLbaseAnalysis(){
}

void TANLbaseAnalysis::Setup(TAGgeoTrafo* aTrafo){
  fpFootGeo = aTrafo;
}

void TANLbaseAnalysis::BeforeEventLoop(){
}

void TANLbaseAnalysis::ProcessEvent(){
}

void TANLbaseAnalysis::AfterEventLoop(){
}
