/*!
 \file TAGFselectorLinear.hxx
 \brief  Header of GenFit track finding/selection class w/ linear forward extrapolation
 \author M. Franchini and R. Zarrella
*/

#ifndef TAGFselectorLinear_H
#define TAGFselectorLinear_H

#include "TAGFselectorStandard.hxx"

using namespace std;
using namespace genfit;

class TAGFselectorLinear : public TAGFselectorStandard{

public:

	TAGFselectorLinear();

	virtual void	Categorize() override;

protected:
	virtual void CategorizeMSD() override;
	virtual void CategorizeTW() override;

	ClassDef(TAGFselectorLinear,0);
};

#endif















