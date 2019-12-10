/*!
 \file
 \version $Id: TAMSDparMap.cxx,v 1.5 2003/06/09 18:41:04 mueller Exp $
 \brief   Implementation of TAMSDparMap.
 */

#include "TAMSDparMap.hxx"

//##############################################################################

/*!
 \class TAMSDparMap TAMSDparMap.hxx "TAMSDparMap.hxx"
 \brief Map and Geometry parameters for Tof wall. **
 */

ClassImp(TAMSDparMap);

const Int_t TAMSDparMap::fgkSensorsN = 6;
//------------------------------------------+-----------------------------------
//! Default constructor.

TAMSDparMap::TAMSDparMap() 
: TAGpara(),
  fPixelsNu(1152),
  fPixelsNv(576),
  fPitchU(18.4),
  fPitchV(18.4),
  fPixelSize(18.4)
{  
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAMSDparMap::~TAMSDparMap()
{}

/*------------------------------------------+---------------------------------*/
//! Returns real id sensor respect to MC
Int_t TAMSDparMap::GetRealId(Int_t mcSensorId)
{
   switch (mcSensorId) {
	  case 0:
		 return 4;
		 break;  
	  case 1:
		 return 0;
		 break;
	  case 2:
		 return 5;
		 break;
	  case 3:
		 return 1;
		 break;
	  case 4:
		 return 6;
		 break;
	  case 5:
		 return 2;
		 break;
	  case 6:
		 return 7;
		 break;
	  case 7:
		 return 3;
		 break;
	  default:
		 return -1;
   }
}

/*------------------------------------------+---------------------------------*/
//! Returns position from pixel line/column in U direction
Int_t TAMSDparMap::GetIndex(Int_t line, Int_t column) const
{
   return line*fPixelsNu + column;
}

/*------------------------------------------+---------------------------------*/
//! Returns position from pixel line/column in U direction
Float_t TAMSDparMap::GetPositionU(Int_t column) const
{
   return ((2*column - fPixelsNu + 1 ) * fPitchU)/2 ;
}

/*------------------------------------------+---------------------------------*/
//! Returns position from pixel line/column in V direction
Float_t TAMSDparMap::GetPositionV(Int_t line) const
{
   return -((2*line - fPixelsNv + 1 ) * fPitchV)/2 ;

}
