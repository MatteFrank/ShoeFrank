/*!
 \file TAGFselectorTrue.hxx
 \brief  Header of GenFit track finding/selection class based on MC truth info
 \author M. Franchini and R. Zarrella
*/

#ifndef TAGFselectorTrue_H
#define TAGFselectorTrue_H

#include "TAGFselectorBase.hxx"


using namespace std;
using namespace genfit;

class TAGFselectorTrue : public TAGFselectorBase {

public:

	TAGFselectorTrue();

	virtual void		Categorize();

};

#endif















