

#ifndef _TAGwireDisplay_HXX_
#include "TAGwireDisplay.hxx"
#endif

#ifndef ROOT_TEveChunkManager
#include "TEveChunkManager.h"
#endif

//
ClassImp(TAGwireDisplay)

//__________________________________________________________
TAGwireDisplay::TAGwireDisplay(const Text_t* name) 
  : TEveStraightLineSet(name)
{ 
  // default constructor
}

//__________________________________________________________
TAGwireDisplay::~TAGwireDisplay()
{
  // default destructor
}

//__________________________________________________________
void TAGwireDisplay::AddWire(Float_t x1, Float_t y1, Float_t z1, 
                                    Float_t x2, Float_t y2, Float_t z2)
{
   AddLine(x1, y1, z1, x2, y2, z2);
}

//__________________________________________________________
void TAGwireDisplay::ResetWires()
{
   TEveChunkManager& p = GetLinePlex();
   p.Reset(sizeof(TEveStraightLineSet::Line_t), p.N());
}

//__________________________________________________________
void TAGwireDisplay::RefitPlex()
{
   TEveChunkManager& p = GetLinePlex();
   p.Refit();
}
