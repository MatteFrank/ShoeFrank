/*!
  \file
  \version $Id: TATWparCal.cxx,v 1.2 2003/06/22 19:34:21 mueller Exp $
  \brief   Implementation of TATWparCal.
 */

#include <Riostream.h>
#include <limits>

#include "GlobalPar.hxx"
#include "TAGroot.hxx"

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

  // Standard constructor
  cMapCal=new CCalibrationMap();

  m_parGeo = (TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();

  m_geoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

  RetrieveBeamQuantities();

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
void TATWparCal::RetrieveBeamQuantities() {

  Z_beam = m_parGeo->GetBeamPar().AtomicNumber;
  ion_name = m_parGeo->GetBeamPar().Material;
  A_beam = m_parGeo->GetBeamPar().AtomicMass;
  kinE_beam = m_parGeo->GetBeamPar().Energy*TAGgeoTrafo::GevToMev()*A_beam; //MeV
  
  if(fDebugLevel)
    printf("ion::%s Z::%d A::%d E::%d\n",ion_name.Data(),Z_beam,A_beam,(int)(kinE_beam/A_beam));
  
  z_SC = ((TVector3)m_geoTrafo->GetSTCenter()).Z();    // cm
  z_TW = ((TVector3)m_geoTrafo->GetTWCenter()).Z();    // cm
  Lmin = z_TW-z_SC; // roughly particles shorter path length
  
  C_speed = TMath::C()*TAGgeoTrafo::MToCm()/TAGgeoTrafo::SecToNs(); //~30 cm/ns
  Tof_min = Lmin/C_speed; // for Beta==1
  
  mass_p = (Double_t)((TParticlePDG*)db.GetParticle(2212))->Mass();
  mass_n = (Double_t)((TParticlePDG*)db.GetParticle(2112))->Mass();
  
  switch(A_beam) {
  case H:   // p
    binding_energy = 0;
    break;
  case He4:   // 4He
    binding_energy = 7.0739185;
    break;
  case C12:  // 12C
    binding_energy = 7.6801458;
    break;
  case O16:  // 16O
    binding_energy = 7.9762085;
    break;
  default:  // others
    binding_energy = 8;
  }  
  
  Mass_beam = Z_beam*(mass_p+mass_n)*TAGgeoTrafo::GevToMev()-binding_energy*A_beam; //MeV
  Energy_beam = kinE_beam+Mass_beam;  //MeV
  Beta_beam = sqrt(pow(Energy_beam,2)-pow(Mass_beam,2))/Energy_beam;
  Tof_beam = Lmin/(C_speed*Beta_beam);
  Tof_max = 5*Tof_beam;
  
  if(fDebugLevel)
    cout<<"L::"<<Lmin<<"  Tof_min::"<<Tof_min<<"  Tof_max::"<<Tof_max<<"  <Tof>::"<<Tof_beam<<"  Beta::"<<Beta_beam<<"  Mass::"<<Mass_beam<<"  Energy::"<<Energy_beam<<"  B::"<<binding_energy<<endl;

}
//------------------------------------------+-----------------------------------
Bool_t TATWparCal::FromFile(const TString& name) 
{

  Clear();

  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);
  int verbose = 0;
  
  if (FootDebugLevel(1))
    verbose = 1;
  
  cMapCal->LoadCalibrationMap(name_exp.Data(), verbose);
  
  return kFALSE;
  
}  

//
////------------------------------------------+-----------------------------------
Bool_t TATWparCal::FromFile(Int_t Zbeam, const TString& name) {
 
    
  TString nameExp;
  
  if (name.IsNull())
    nameExp = fgkBBparamName;
  else
    nameExp = name;
  
  if (!Open(nameExp)) return false;
    
  Double_t* tmp = new Double_t[4];
  
  (fChargeParameter.Norm_BB).clear();
  (fChargeParameter.Const_BB).clear();
  (fChargeParameter.CutLow).clear();
  (fChargeParameter.CutUp).clear();

  for (Int_t iZ = 0; iZ < Zbeam; iZ++) { // Loop on each charge

    // read parameters
    ReadItem(tmp, 4, ' ');
    
    (fChargeParameter.Norm_BB).push_back(tmp[0]);
    (fChargeParameter.Const_BB).push_back(tmp[1]);
    (fChargeParameter.CutLow).push_back(tmp[2]);
    (fChargeParameter.CutUp).push_back(tmp[3]);
    // fChargeParameter[iZ].Layer    = tmp[4];
    // fChargeParameter[iZ].Charge   = tmp[5];
    
    if(fDebugLevel)
      cout << endl << " TW Parameter: "<< Form("Z=%d %f %f %f %f", iZ+1, fChargeParameter.Norm_BB[iZ], fChargeParameter.Const_BB[iZ], fChargeParameter.CutLow[iZ], fChargeParameter.CutUp[iZ]) << endl;
    
  }
  
  delete[] tmp;
  
  return kFALSE;
}


//
////------------------------------------------+-----------------------------------
Int_t TATWparCal::GetChargeZ(Float_t edep, Float_t tof, Int_t layer)
{
  if(edep<0) {
    Zraw=-1;
    if (fDebugLevel) printf("the energy released is %.f so Zraw is set to %d\n",edep,Zraw);
  }
  else
    ComputeBBDistance(edep,tof,layer);       
  
  return Zraw;
  
  /*
    for (Int_t p = 0; p < Z_beam; p++) { // Loop on each charge
    
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
  
  Zraw = 0;  //nonsense intialization: charge must be > 0
  dist_min_Z = std::numeric_limits<double>::max(); //inf

  const float deltaToFmin = 0.01;  // 10 ps

  dist_Z.clear();

  if(fDebugLevel)
    cout<<"edep::"<<edep<<"  tof::"<<tof<<endl;
  
  for (int iZ=1; iZ<Z_beam+1; iZ++) {

    double fBB_x(-99),fBB_prime_x(-99);
    float dist(-99);
    // float dist  = std::numeric_limits<double>::max(); //inf
    // BB parameters
    double parNorm(fChargeParameter.Norm_BB[iZ-1]),parConst(fChargeParameter.Const_BB[iZ-1]);
    // double Tof_max = iZ<3 ? 5*Tof_beam : 2*Tof_beam; // different for H and He
    double xmin(Tof_min), xmax(Tof_max), xhalf(-99);

    double f_prime_dist_min(10000), f_prime_dist_max(-10000), f_prime_dist_half(-99);


    if(tof>xmin && tof<xmax) {   // smaller than 0<Beta<1
      
      //f_prime_dist_min has to be < 0
      while(f_prime_dist_min>0 && xmin<xmax) {

	//compute distance in x==xmin
	fBB_x = fBBparametrized(xmin,iZ,parNorm,parConst);
	fBB_prime_x = fBB_prime_parametrized(xmin,iZ,parNorm,parConst);
	dist = fDist(tof,edep,xmin,fBB_x);
	f_prime_dist_min = fDistPrime(tof,edep,xmin,fBB_x,fBB_prime_x,dist);
	if(f_prime_dist_min>0)
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
	if(f_prime_dist_max<0)      
	  xmax-=deltaToFmin*10;
      }
      if(fDebugLevel) {
	cout<<"xmax::"<<xmax<</*" "<<fBB_x<<" "<<fBB_prime_x<<*/" dist::"<<dist<<" f_prime::"<<f_prime_dist_max<<endl;
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
	  
	} else if (f_prime_dist_min*f_prime_dist_max>0) {
	  if(fDebugLevel) printf("no bisection algorithm is possible to assign Z = %d to the TW hit with (tof,eloss) = (%f,%f)\n",iZ,tof,edep);
	  dist=std::numeric_limits<double>::max(); //inf
	}
	
	if(fDebugLevel) cout<<"dist::"<<dist<<"  distMIN::"<<dist_min_Z<<"  Zraw::"<<iZ<<endl;
	// assign Z value
	if(dist<dist_min_Z) {
	  dist_min_Z=dist;
	  Zraw = iZ;
	  if(fDebugLevel) cout<<"selected:: dist::"<<dist<<"  distMIN::"<<dist_min_Z<<"  Zraw::"<<iZ<<endl;
	}
	
	if(edep<fBB_x) dist_Z.push_back( -dist );
	else dist_Z.push_back( dist );

	if(fDebugLevel) printf("the selected Z is:: %d\n\n",Zraw);
	
      } // Xmin<xmax
      else {
	dist=std::numeric_limits<double>::max(); //inf
	dist_Z.push_back( dist );
	if (fDebugLevel)
	  printf("xmin(%.5f) > xmax(%.5f) so no possible interval for bisection algorithm\n",xmin,xmax);      
      }
    } else {
      dist=std::numeric_limits<double>::max(); //inf
      dist_Z.push_back( dist );
      if (fDebugLevel)
	printf("tof (%.f) is outside the selected interval [%.f,%.f] so Zraw is set to %d\n",tof,xmin,xmax,Zraw);      
    }

    if(fDebugLevel) printf("for loop over iZ::%d  with dist::%.5f\n\n",iZ,dist_Z.at(iZ-1));
    
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


