#include "TABMrawHit.hxx"

using namespace std;

ClassImp(TABMrawHit);

//------------------------------------------+-----------------------------------
//! Destructor.

TABMrawHit::~TABMrawHit()
{}

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMrawHit::TABMrawHit()
  : fiView(0),    fiLayer(0),    fiCell(0),  ftdTtime(0.)
{
}

void TABMrawHit::SetData(Int_t id, Int_t lay, Int_t view, Int_t cell, Double_t time)
{
  fiView  = view;
  fiLayer  = lay;
  fiCell   = cell;
  fidCell  = id;
  ftdTtime = time;
  return;
}
