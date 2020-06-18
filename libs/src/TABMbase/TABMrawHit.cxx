#include "TABMrawHit.hxx"

using namespace std;

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
//! constructor.
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
void TABMrawHit::SetData(Int_t id, Int_t plane, Int_t view, Int_t cell, Double_t time)
{
  fidCell  = id;
  fiPlane  = plane;
  fiView  = view;
  fiCell   = cell;
  ftdTtime = time;
}
