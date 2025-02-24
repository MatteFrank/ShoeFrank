/*!
 \file TAGFselectorBack.hxx
 \brief  Header of GenFit backward track selection class
 \author M. Franchini and R. Zarrella
*/

#ifndef TAGFselectorBack_H
#define TAGFselectorBack_H

#include "TAGFselectorBase.hxx"

using namespace std;
using namespace genfit;

class TAGFselectorBack : public TAGFselectorBase {

public:
	TAGFselectorBack();

	virtual void Categorize();

private:
	//Backtracking
	void		BackTracklets();
	void		CategorizeIT_back();
	void		CategorizeVT_back();

	ClassDef(TAGFselectorBack,0);
};

#endif
