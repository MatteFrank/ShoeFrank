#include "TABMrawHit.hxx"

using namespace std;

ClassImp(TABMrawHit);

//------------------------------------------+-----------------------------------
//! Default constructor.
TABMrawHit::TABMrawHit()
  : TAGdata(),
   fiView(0),
   fiLayer(0),
   fiCell(0),
   fidCell(0),
   ftdTtime(0.)
{
}

//------------------------------------------+-----------------------------------
//! constructor.
TABMrawHit::TABMrawHit(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time)
: TAGdata(),
   fiView(view),
   fiLayer(lay),
   fiCell(cell),
   fidCell(id),
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
void TABMrawHit::SetData(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time)
{
  fiView  = view;
  fiLayer  = lay;
  fiCell   = cell;
  fidCell  = id;
  ftdTtime = time;
}
