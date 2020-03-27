/*!
  \file
  \version $Id: TATWparCal.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TATWparCal.
 */

#include <Riostream.h>
#include "GlobalPar.hxx"
#include "TATWparCal.hxx"

//##############################################################################

/*!
  \class TATWparCal TATWparCal.hxx "TATWparCal.hxx"
  \brief Charge (raw) calibration for vertex. **
 */

ClassImp(TATWparCal);

Int_t TATWparCal::fgkChargesN       = 8;
TString TATWparCal::fgkDefaultCalName = "./config/TATWCalibrationMap.xml";

//------------------------------------------+-----------------------------------
TATWparCal::TATWparCal()
: TAGparTools()
{
	cMapCal=new CCalibrationMap();
	// Standard constructor
}

//------------------------------------------+-----------------------------------
TATWparCal::~TATWparCal()
{
	if (cMapCal!=nullptr)
	{
		free (cMapCal);
	}
}

//------------------------------------------+-----------------------------------
Bool_t TATWparCal::FromFile(const TString& name) 
{
	Clear();

	TString name_exp = name;
	gSystem->ExpandPathName(name_exp);
   
	cMapCal->LoadCalibrationMap(name_exp.Data());
	//   // Reading calibration file
	//   TString nameExp;
	//
	//   if (name.IsNull())
	//     nameExp = fgkDefaultCalName;
	//   else
	//     nameExp = name;
	//
	//   if (!Open(nameExp)) return false;
	//
	//   Float_t* tmp = new Float_t[2];
	//   for (Int_t p = 0; p < fgkChargesN; p++) { // Loop on each charge
	//
	//	  // read parameters
	//	  ReadItem(tmp, 2, ' ');
	//	  fChargeParameter[p].Charge = p+1;
	//	  fChargeParameter[p].CutLow = tmp[0];
	//	  fChargeParameter[p].CutUp  = tmp[1];
	//
	//	  if(fDebugLevel)
	//		 cout << endl << " Charge Parameter: "
	//		      << Form("%d %f %f", fChargeParameter[p].Charge, fChargeParameter[p].CutLow,
	//				 fChargeParameter[p].CutUp) << endl;
	//   }
	//   delete[] tmp;
	//
	return kFALSE;
}
//
////------------------------------------------+-----------------------------------
Int_t TATWparCal::GetChargeZ(Float_t edep) const
{
	for (Int_t p = 0; p < fgkChargesN; p++) { // Loop on each charge

		if (edep >= fChargeParameter[p].CutLow && edep < fChargeParameter[p].CutUp )
			return fChargeParameter[p].Charge;
	}

	return -1;
}


//------------------------------------------+-----------------------------------
//! Clear geometry info.
void TATWparCal::Clear(Option_t*)
{
	return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.
void TATWparCal::ToStream(ostream& os, Option_t*) const
{
	//  os << "TATWparCal " << GetName() << endl;
	//  os << "p 8p   ref_x   ref_y   ref_z   hor_x   hor_y   hor_z"
	//     << "   ver_x   ver_y   ver_z  width" << endl;

	return;
}


