#ifndef MATERIALS_HXX
#define MATERIALS_HXX


#include <TROOT.h>

#include <array>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <limits>

#include <TGeoManager.h>

// #include <TGeoMaterialInterface.h>

using namespace std;


#define build_string(expr) \
    (static_cast<ostringstream*>(&(ostringstream().flush() << expr))->str());


class Materials {

public:

	
	Materials( ) {};
	~Materials() {};

	

	TGeoMaterial* GetMaterial( string nome ) {
		// if (not find nome)	....
		return store[nome];
	};

	void Print() {};


private:
	
	void FillStore() {};
	
	map<string, TGeoMaterial*> store;

};





#endif























