#include "TANLAnalysisManager.hxx"
#include <algorithm>

TANLAnalysisManager::TANLAnalysisManager(){
}


~TANLAnalysisManager::TANLAnalysisManager(){
  for(auto it = methods.rbegin(); it!= methods.rend(); it++){
    delete *it;
  }
  methods.clear();
}


void TANLAnalysisManager::Setup(){
  for(auto it = methods.begin(); it!= methods.end(); it++){
    it->Setup();
  }
}


void TANLAnalysisManager::BeforeEventLoop(){
  for(auto it = methods.begin(); it!= methods.end(); it++){
    it->BeforeEventLoop();
  }
}


void TANLAnalysisManager::ProcessEvent(){
  for(auto it = methods.begin(); it!= methods.end(); it++){
    it->ProcessEvent();
  }
}


void TANLAnalysisManager::AfterEventLoop(){
  for(auto it = methods.begin(); it!= methods.end(); it++){
    it->AfterEventLoop();
  }
}

// specific for the analysis manager

void TANLAnalysisManager::Register(TANLbaseAnalysis * anAnalysis){
  if( !methods.hasAnalysis(anAnalysis) )
    methods.push_back(anAnalysis);
}


void TANLAnalysisManager::Remove(TANLbaseAnalysis * anAnalysis){

  int n = methods.size();
  methods.remove(anAnalysis);
  if( n==methods.size() ){
    delete anAnalysis;  // methods OWNS the pointer!
  }

}


bool TANLAnalysisManager::hasAnalysis(TANLbaseAnalysis * anAnalysis) const{
  auto it = std::find(methods.begin(), methods.end(), anAnalysis);
  return it!=methods.end();
}

