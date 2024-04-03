/*!
 \file TAGFselectorStandard.hxx
 \brief  Header of GenFit track finding/selection class
 \author M. Franchini and R. Zarrella
*/

#ifndef TAGFselectorStandard_H
#define TAGFselectorStandard_H

#include "TAGFselectorBase.hxx"


using namespace std;
using namespace genfit;

class TAGFselectorStandard : public TAGFselectorBase {

public:
	TAGFselectorStandard();

	virtual void	Categorize();

protected:

	//Forward tracking
	virtual void	CategorizeVT();
	virtual void	CategorizeIT();
	virtual void	CategorizeMSD();
	virtual void	SetTrackSeedNoMSD();
	virtual void	CategorizeTW();

	ClassDef(TAGFselectorStandard,0);
};

#endif
