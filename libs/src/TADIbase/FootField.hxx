#ifndef FOOTFIELD_H
#define FOOTFIELD_H

#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <map>

#include <TVector3.h>


#include "TADIparGeo.hxx"
#include "AbsBField.h"

using namespace std;
using namespace genfit;

typedef map< double, map< double, map< double, TVector3 > > > lattice3D;
typedef map< double, map< double, TVector3 > > lattice2D;

class FootField : public AbsBField {

public:

	FootField( string fileName =""  , TADIparGeo* diGeo=NULL);
	FootField( float constValue , TADIparGeo* diGeo=NULL);

	~FootField() {};

	TVector3 get(const TVector3& position) const;
	void get(const double& posX, const double& posY, const double& posZ, double& Bx, double& By, double& Bz) const ;	

 	TVector3 Interpolate( const TVector3 &position );
 	double IntegralField( int step, double start, double end );  // in cm


private:


	lattice3D  m_filedMap;

	string m_fieldSetting;
};


#endif























