/*!
  \file
  \version $Id: TATWparCal.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TATWparCal.
 */

#include <Riostream.h>
#include <limits>
#include "GlobalPar.hxx"
#include "TATWparCal.hxx"

//##############################################################################

/*!
  \class TATWparCal TATWparCal.hxx "TATWparCal.hxx"
  \brief Charge (raw) calibration for vertex. **
 */

ClassImp(TATWparCal);

TString TATWparCal::fgkDefaultCalName = "./config/TATWCalibrationMap.xml";
TString TATWparCal::fgkBBparamName = "./config/TATW_BBparameters.cfg";

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
Bool_t TATWparCal::FromFile(const TString& name, Bool_t isBBparam) 
{

  Clear();

  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);
  int verbose = 0;
  
  if (FootDebugLevel(1))
    verbose = 1;
  
  if(!isBBparam)      // Reading calibration file
    
    cMapCal->LoadCalibrationMap(name_exp.Data(), verbose);
  
  else {  // BB parameters for Z identification
    
    TString nameExp;
    
    if (name.IsNull())
      nameExp = fgkBBparamName;
    else
      nameExp = name;
    
    if (!Open(nameExp)) return false;
    
    Double_t* tmp = new Double_t[4];
    
    for (Int_t iZ = 0; iZ < kCharges; iZ++) { // Loop on each charge
      
      // read parameters
       ReadItem(tmp, 4, ' ');
      
      fChargeParameter[iZ].Norm_BB  = tmp[0];
      fChargeParameter[iZ].Const_BB = tmp[1];
      fChargeParameter[iZ].CutLow   = tmp[2];
      fChargeParameter[iZ].CutUp    = tmp[3];
      // fChargeParameter[iZ].Layer    = tmp[4];
      // fChargeParameter[iZ].Charge   = tmp[5];
      
      if(fDebugLevel)
	cout << endl << " TW Parameter: "<< Form("Z=%d %f %f %f %f", iZ+1, fChargeParameter[iZ].Norm_BB, fChargeParameter[iZ].Const_BB, fChargeParameter[iZ].CutLow, fChargeParameter[iZ].CutUp) << endl;
      
    }
    
    delete[] tmp;
    
  }
  
  return kFALSE;
}
//
////------------------------------------------+-----------------------------------
Int_t TATWparCal::GetChargeZ(Float_t edep, Float_t tof, Int_t layer) //const
{
  if(edep<0) {
    Zraw=-1;
    if (fDebugLevel) printf("the energy released is %.f so Zraw is set to %d\n",edep,Zraw);
  }
  else
    ComputeBBDistance(edep,tof,layer);       
  
  return Zraw;
  
  /*
    for (Int_t p = 0; p < kCharges; p++) { // Loop on each charge
    
    if (edep >= fChargeParameter[p].CutLow && edep < fChargeParameter[p].CutUp )
    return fChargeParameter[p].Charge;
    }
    
    return -1;
  */
}
//
////------------------------------------------+-----------------------------------
Double_t TATWparCal::fBBparametrized(double tof, int Z, double A, double B)
{
  return A*pow(Z,2)*(B*pow(tof,2)-1); 
}
////------------------------------------------+-----------------------------------
Double_t TATWparCal::fBB_prime_parametrized(double tof, int Z, double A, double B)
{
  return A*pow(Z,2)*2*B*tof; 
}
//
////------------------------------------------+-----------------------------------
Float_t TATWparCal::fDist(double tof, double dE, double x, double fBB)
{ 
  return sqrt(pow((tof-x),2)+pow((dE-fBB),2)); 
}
//
////------------------------------------------+-----------------------------------
Double_t TATWparCal::fDistPrime(double tof, double dE, double x, double fBB, double fBB_prime, double dist)
{ 
  return (-1/dist)*((tof-x)+fBB_prime*(dE-fBB)); 
}
//
//------------------------------------------+-----------------------------------
//! Compute distance from BB with bisection method
void TATWparCal::ComputeBBDistance(double edep, double tof, int tw_layer)
{
  
  Zraw = -2;  //nonsense intialization
  dist_min_Z = std::numeric_limits<double>::max(); //inf

  const float deltaToFmin = 0.01;  // 10 ps

  // cout<<"edep::"<<edep<<"  tof::"<<tof/*<<"  layer::"<<tw_layer*/<<endl;

  for (int iZ=1; iZ<kCharges+1; iZ++) {

    dist_Z[iZ-1] = std::numeric_limits<double>::max(); //inf

    double fBB_x(-99),fBB_prime_x(-99);
    float dist(-99);    
    // BB parameters
    double parNorm(fChargeParameter[iZ-1].Norm_BB),parConst(fChargeParameter[iZ-1].Const_BB);
    double xmin(fChargeParameter[iZ-1].CutLow), xmax(fChargeParameter[iZ-1].CutUp), xhalf(-99);
    double f_prime_dist_min(10000), f_prime_dist_max(-10000), f_prime_dist_half(-99);


    if(tof>xmin && tof<xmax) {   // smaller than 0<Beta<1
      
      //f_prime_dist_min has to be < 0
      while(f_prime_dist_min>0 && xmin<xmax) {
	
	//compute distance in x==xmin
	fBB_x = fBBparametrized(xmin,iZ,parNorm,parConst);
	fBB_prime_x = fBB_prime_parametrized(xmin,iZ,parNorm,parConst);
	dist = fDist(tof,edep,xmin,fBB_x);
	f_prime_dist_min = fDistPrime(tof,edep,xmin,fBB_x,fBB_prime_x,dist);

	if(f_prime_dist_min==0) { // root found in x = xmin
	  dist_min_Z = dist;      
	  if(edep<fBB_x) dist_Z[iZ-1] = -dist;
	  else dist_Z[iZ-1] = dist;
	}	
	else if(f_prime_dist_min>0)
	  xmin+=deltaToFmin*10;
      }
      if(fDebugLevel) {
	cout<<"xmin::"<<xmin<</*" "<<fBB_x<<" "<<fBB_prime_x<<*/" dist::"<<dist<<" f_prime::"<<f_prime_dist_min<<endl;
      }
      
      
      //f_prime_dist_max has to be > 0
      while(f_prime_dist_max<0 && xmax>xmin) {
	
	//compute distance in x==xmax
	fBB_x = fBBparametrized(xmax,iZ,parNorm,parConst);
	fBB_prime_x = fBB_prime_parametrized(xmax,iZ,parNorm,parConst);
	dist = fDist(tof,edep,xmax,fBB_x);
	f_prime_dist_max = fDistPrime(tof,edep,xmax,fBB_x,fBB_prime_x,dist);
	if(f_prime_dist_max==0) { // root found in x = xmax
	  dist_min_Z = dist;
	  if(edep<fBB_x) dist_Z[iZ-1] = -dist;
	  else dist_Z[iZ-1] = dist;      
	}	
	else if(f_prime_dist_max<0)      
	  xmax-=deltaToFmin*10;
      }
      if(fDebugLevel) {
	cout<<"xmax::"<<xmax<<" "<<fBB_x<<" "<<fBB_prime_x<<" dist::"<<dist<<" f_prime::"<<f_prime_dist_max<<endl;
      }
      
      
      if(xmin<xmax) {
	
	if(f_prime_dist_min*f_prime_dist_max<0) {  // then bisection algorithm
	  
	  while(abs(xmax-xmin)>deltaToFmin) { 
	    
	    xhalf = (xmin + xmax)/2.;
	    
	    fBB_x = fBBparametrized(xhalf,iZ,parNorm,parConst);
	    fBB_prime_x = fBB_prime_parametrized(xhalf,iZ,parNorm,parConst);
	    dist = fDist(tof,edep,xhalf,fBB_x);
	    f_prime_dist_half = fDistPrime(tof,edep,xhalf,fBB_x,fBB_prime_x,dist);
	    
	    if(f_prime_dist_half==0)  // root found in x = xhalf
	      break;

	    if(f_prime_dist_min*f_prime_dist_half<0)
	      xmax = xhalf;
	    else if(f_prime_dist_half*f_prime_dist_max<0) 
	      xmin = xhalf;
	  }
	  
	} else if (f_prime_dist_min*f_prime_dist_max>0)
	  if(fDebugLevel) printf("no bisection algorithm is possible to assign Z = %d to the TW hit with (tof,eloss) = (%f,%f)\n",iZ,tof,edep);
	
	if(fDebugLevel) cout<<"dist::"<<dist<<"  distMIN::"<<dist_min_Z<<"  Zraw::"<<iZ<<endl;
	// assign Z value
	if(dist<dist_min_Z) {
	  dist_min_Z=dist;
	  Zraw = iZ;
	  if(fDebugLevel) cout<<"selected:: dist::"<<dist<<"  distMIN::"<<dist_min_Z<<"  Zraw::"<<iZ<<endl;
	}
	
	if(edep<fBB_x) dist_Z[iZ-1] = -dist;
	else dist_Z[iZ-1] = dist;
	
      } // Xmin<xmax

      if(fDebugLevel) printf("the selected Z is:: %d\n\n",Zraw);

    } else {
      Zraw = 0;
      if (fDebugLevel)
	printf("tof (%.f) is outside the selected interval [%.f,%.f] so Zraw is set to %d\n",tof,xmin,xmax,Zraw);      
    }
      
  } // close for loop over Z
    
  return;
  
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


