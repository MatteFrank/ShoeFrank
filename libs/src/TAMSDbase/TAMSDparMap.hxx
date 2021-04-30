#ifndef _TAMSDparMap_HXX
#define _TAMSDparMap_HXX
/*!
 \file
 \version $Id: TAMSDparMap.hxx,v 1.1
 \brief   Declaration of TAMSDparMap.
 */
/*------------------------------------------+---------------------------------*/


#include <vector>

#include "TAVTbaseParMap.hxx"

//##############################################################################

class TAMSDparMap : public TAVTbaseParMap {
   
public:
   TAMSDparMap();
   virtual ~TAMSDparMap();
   
   ClassDef(TAMSDparMap,1)
  
};

#endif
