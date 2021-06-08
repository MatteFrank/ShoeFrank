#ifndef UpdatePDG_HXX
#define UpdatePDG_HXX


#include "TDatabasePDG.h"

#include "TAGrecoManager.hxx"

#include <vector>
#include <string>




using namespace std;
// using namespace genfit;



class UpdatePDG {

public:

  static UpdatePDG* Instance();
  static UpdatePDG* GetPDG();
  ~UpdatePDG() {};

  void MakePdgDatabase();

  bool IsParticleDefined( string partName );
  int GetPdgCode( string partName );
  int GetPdgCodeMainIsotope(int partCharge);

  int GetPdgMass( string partName );
  int GetPdgMass( int PdgCode );
  
  string GetPdgName( int code );
  
  // int GetPdgCode( int ch, int mass );


private:

  UpdatePDG();
  static UpdatePDG* m_pInstance;

  map<string, int> m_pdgCodeMap;


};

#endif










