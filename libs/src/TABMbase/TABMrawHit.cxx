/*!
  \file    TABMrawHit.cxx
  \brief   Declaration of TABMrawHit, the class for the BM raw hits
*/
/*------------------------------------------+---------------------------------*/
#include "TABMrawHit.hxx"
using namespace std;
/*!
  \file    TABMrawHit.cxx
  \brief   Declaration of TABMrawHit, the class for the BM raw hits
*/
/*------------------------------------------+---------------------------------*/

ClassImp(TABMrawHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TABMrawHit::TABMrawHit()
  : TAGdata(),
   fidCell(0),
   fiPlane(0),
   fiView(0),
   fiCell(0),
   ftdTtime(0.)
{
}

//------------------------------------------+-----------------------------------
//! constructor with values
//!
//! \param[in] id hit idcell [0-35]
//! \param[in] plane hit plane [0-5]
//! \param[in] view hit view [0-1]
//! \param[in] cell hit cell [0-2]
//! \param[in] time hit tdc raw time measurement in ns
TABMrawHit::TABMrawHit(Int_t id, Int_t plane, Int_t view, Int_t cell, Double_t time)
: TAGdata(),
   fidCell(id),
   fiPlane(plane),
   fiView(view),
   fiCell(cell),
   ftdTtime(time)
{
}

//------------------------------------------+-----------------------------------
//! Destructor.
TABMrawHit::~TABMrawHit()
{
}

//------------------------------------------+-----------------------------------
//! set data
//!
//! \param[in] id hit idcell [0-35]
//! \param[in] plane hit plane [0-5]
//! \param[in] view hit view [0-1]
//! \param[in] cell hit cell [0-2]
//! \param[in] time hit tdc raw time measurement in ns
void TABMrawHit::SetData(Int_t id, Int_t plane, Int_t view, Int_t cell, Double_t time)
{
  fidCell  = id;
  fiPlane  = plane;
  fiView  = view;
  fiCell   = cell;
  ftdTtime = time;
}
