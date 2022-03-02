/***************************************************************************
 *   Copyright (C) 2004-2006 by Olivier Stezowski & Christian Finck        *
 *   stezow(AT)ipnl.in2p3.fr, cfinck(AT)iphc.cnrs.fr                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*!
 \file TPGbasePeak.cxx
 \brief   Implementation of TPGbasePeak.
 */

#include "TPGbasePeak.hxx"

/*!
 \class TPGbasePeak
 \brief A TPGbasePeak is defined by a height, intensity and a dimension of the peak
 */

//! Class Imp
ClassImp(TPGbasePeak)

TPGbasePeak::GateColor_t TPGbasePeak::fgGateColor = { kRed-9, kBlue-7, kGreen-8, kYellow-6 };

//__________________________________________________________
//! Default Constructor
//!
//! \param[in] name name of peak
//! \param[in] title title of peak
TPGbasePeak::TPGbasePeak(const char* name, const char* title)
 : TNamed(name, title),
	fHeight(0.0),
	fIntensity(0.0),
	fDimension(0),
	fIsFitOn(false),
	fIsCombined(false)
{
}

//__________________________________________________________
/// default destructor
TPGbasePeak::~TPGbasePeak()
{
}

//__________________________________________________________
//! Copy Constructor
//!
//! \param[in] p peak to copy
TPGbasePeak::TPGbasePeak(const TPGbasePeak& p)
 : TNamed(p),
	fHeight(p.fHeight),
	fIntensity(p.fIntensity),
	fDimension(p.fDimension),
	fIsFitOn(p.fIsFitOn),
	fIsCombined(p.fIsCombined)
{
  // copy constructor
}

//__________________________________________________________
//! operator=
//!
//! \param[in] p peak to equal
TPGbasePeak& TPGbasePeak::operator=(const TPGbasePeak& p)
{
	if (this == &p) 
			return *this;

	fHeight     = p.fHeight;
	fIntensity  = p.fIntensity;
	fDimension  = p.fDimension;
	fIsFitOn    = p.fIsFitOn;
	fIsCombined = p.fIsCombined;

	return *this;
}

//__________________________________________________________
//! Return valid gate color
//!
//! \param[in] co a given color
Bool_t TPGbasePeak::IsvalideGateColor(Color_t co)
{
	if (co == fgGateColor.k0) return true;
	if (co == fgGateColor.k1) return true;
	if (co == fgGateColor.k2) return true;
	if (co == fgGateColor.k3) return true;
	
	return false;
}

//__________________________________________________________
//! Get color for a given gate
//!
//! \param[in] idx gate index
Color_t TPGbasePeak::GetGateColor(Int_t idx)
{
	switch (idx) {
		case 0:
			return fgGateColor.k0;
		case 2:
			return fgGateColor.k1;	
		case 3:
			return fgGateColor.k2;
		case 4:
			return fgGateColor.k3;
		default:
			return -1;
	}
	
}

//__________________________________________________________
//! Return color for a given axis
//!
//! \param[in] co a given color
Int_t TPGbasePeak::GetGateAxis(Color_t co)
{
	if (co == fgGateColor.k0) 
		return 0;
	else if (co == fgGateColor.k1) 
		return 1;
	else if (co == fgGateColor.k2) 
		return 2;
	else if (co == fgGateColor.k3) 
		return 3;
	else 
		return -1;
}

//__________________________________________________________
//! Set fit flag
//!
//! \param[in] fit_on fit flag
void TPGbasePeak::FitOn(Bool_t fit_on)
{
	fIsFitOn = fit_on;
}

//__________________________________________________________
//! Print
//!
//! \param[in] opt not used
void TPGbasePeak::Print(Option_t* /*opt*/) const
{
  printf("Height: %6.1f\n", fHeight);
  if (fIntensity > 0.)
    printf("Intensity: %7.1f\n", fIntensity);
}





