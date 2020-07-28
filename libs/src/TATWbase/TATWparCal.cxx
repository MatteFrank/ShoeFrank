
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

TString TATWparCal::fgkBarStatus = "./config/TATWbarsMapStatus.map";
TString TATWparCal::fgkBBparamName = "./config/TATW_BBparameters.cfg";

//------------------------------------------+-----------------------------------
TATWparCal::TATWparCal()
  : TAGparTools(),
    f_isPosCalibration(false),
    f_isBarCalibration(false),
    f_isElossTuningON(false),
    fZraw(-1),
    f_dist_min_Z(std::numeric_limits<float>::max()) //inf
{

  // Standard constructor
  fMapCal=new TATWcalibrationMap();

  fParGeo = (TAGparGeo*)gTAGroot->FindParaDsc(TAGparGeo::GetDefParaName(), "TAGparGeo")->Object();

  fGeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

  RetrieveBeamQuantities();

  f_dist_Z.clear();

}
//------------------------------------------+-----------------------------------
TATWparCal::~TATWparCal()
{
	if (fMapCal!=nullptr)
	{
		free (fMapCal);
	}
}
//------------------------------------------+-----------------------------------
void TATWparCal::RetrieveBeamQuantities() {

  fZbeam = fParGeo->GetBeamPar().AtomicNumber;

  TString  ion_name = fParGeo->GetBeamPar().Material;
  Int_t    A_beam = fParGeo->GetBeamPar().AtomicMass;
  Float_t  kinE_beam = fParGeo->GetBeamPar().Energy*TAGgeoTrafo::GevToMev()*A_beam; //MeV
  
  if(FootDebugLevel(1))
    printf("ion::%s Z::%d A::%d E::%d\n",ion_name.Data(),fZbeam,A_beam,(int)(kinE_beam/A_beam));
  
  Float_t  z_SC = ((TVector3)fGeoTrafo->GetSTCenter()).Z();    // cm
  Float_t  z_TW = ((TVector3)fGeoTrafo->GetTWCenter()).Z();    // cm
  Float_t  Lmin = z_TW-z_SC; // roughly particles shorter path length
  
  const Double_t C_speed = TMath::C()*TAGgeoTrafo::MToCm()/TAGgeoTrafo::SecToNs(); //~30 cm/ns
  fTof_min = Lmin/C_speed; // for Beta==1
  
  TDatabasePDG db;  
  const Double_t mass_p = (Double_t)((TParticlePDG*)db.GetParticle(2212))->Mass();
  const Double_t mass_n = (Double_t)((TParticlePDG*)db.GetParticle(2112))->Mass();
  
  enum {H=1,He4=4,C12=12,O16=16};
  Double_t binding_energy; //MeV/u

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
  
  Double_t Mass_beam = fZbeam*(mass_p+mass_n)*TAGgeoTrafo::GevToMev()-binding_energy*A_beam; //MeV
  Double_t Energy_beam = kinE_beam+Mass_beam;  //MeV
  Double_t Beta_beam = sqrt(pow(Energy_beam,2)-pow(Mass_beam,2))/Energy_beam;
  fTof_beam = Lmin/(C_speed*Beta_beam);
  fTof_max = 5*fTof_beam;
  
  if(FootDebugLevel(1))
    cout<<"L::"<<Lmin<<"  Tof_min::"<<fTof_min<<"  Tof_max::"<<fTof_max<<"  <Tof>::"<<fTof_beam<<"  Beta::"<<Beta_beam<<"  Mass::"<<Mass_beam<<"  Energy::"<<Energy_beam<<"  B::"<<binding_energy<<endl;

}
//------------------------------------------+-----------------------------------
Bool_t TATWparCal::FromCalibFile(const TString& name, Bool_t isTof, Bool_t barCalib)
{

  f_isBarCalibration = barCalib;
  if(f_isBarCalibration)
    f_isPosCalibration = false;
  else  
    f_isPosCalibration = true;

  Clear();

  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);
  int verbose = 0;
  // fMapCal->LoadCalibrationMap(name_exp.Data());

  if(f_isBarCalibration) {  // calibration per TW bar
    if(isTof==0)
      fMapCal->LoadBarEnergyCalibrationMap(name_exp.Data());
    else if (isTof==1)
      fMapCal->LoadBarTofCalibrationMap(name_exp.Data());
  }  
  else if(f_isPosCalibration){  // calibration per bar crossing ( per position )
    if(isTof==0)
      fMapCal->LoadEnergyCalibrationMap(name_exp.Data());
    else if (isTof==1)
      fMapCal->LoadTofCalibrationMap(name_exp.Data());
  }
   Info("FromCalibFile()", "Open file %s for calibration\n", name_exp.Data());


  return kFALSE;
    
}  

//------------------------------------------+-----------------------------------
Bool_t TATWparCal::FromElossTuningFile(const TString& name)
{

  f_isElossTuningON = true;

  Clear();

  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);
  int verbose = 0;

  fMapCal->LoadBarElossTuning(name_exp.Data());

  return kFALSE;
    
}  

//
////------------------------------------------+-----------------------------------
Bool_t TATWparCal::FromFileZID(const TString& name, Int_t Zbeam) {
 
    
  TString nameExp;
  
  if (name.IsNull())
    nameExp = fgkBBparamName;
  else
    nameExp = name;
  
  if (!Open(nameExp)) return false;

  Info("FromFileZID()", "Open file %s for Bethe-Bloch parametrization\n", name.Data());
  
  Double_t* tmp = new Double_t[6];
  
  (fChargeParameter.Norm_BB).clear();
  (fChargeParameter.Const_BB).clear();
  (fChargeParameter.CutLow).clear();
  (fChargeParameter.CutUp).clear();
  (fChargeParameter.distMean).clear();
  (fChargeParameter.distSigma).clear();

  for (Int_t iZ = 0; iZ < Zbeam; iZ++) { // Loop on each charge

    // read parameters
    ReadItem(tmp, 6, ' ');
    
    (fChargeParameter.Norm_BB).push_back(tmp[0]);
    (fChargeParameter.Const_BB).push_back(tmp[1]);
    (fChargeParameter.CutLow).push_back(tmp[2]);
    (fChargeParameter.CutUp).push_back(tmp[3]);
    (fChargeParameter.distMean).push_back(tmp[4]);
    (fChargeParameter.distSigma).push_back(tmp[5]);
    // fChargeParameter[iZ].Layer    = tmp[4];
    // fChargeParameter[iZ].Charge   = tmp[5];
    
    if(FootDebugLevel(1))
      cout << endl << " TW Parameter: "<< Form("Z=%d %f %f %f %f", iZ+1, fChargeParameter.Norm_BB[iZ], fChargeParameter.Const_BB[iZ], fChargeParameter.CutLow[iZ], fChargeParameter.CutUp[iZ]) << endl;
    
  }
  
  delete[] tmp;

  Close();
  
  return kFALSE;
}


//
////------------------------------------------+-----------------------------------

Bool_t TATWparCal::FromBarStatusFile(const TString& name) {
 
    
  TString nameExp;
  
  if (name.IsNull())
    nameExp = fgkBarStatus;
  else
    nameExp = name;

  if (!Open(nameExp)) return false;

 // if(FootDebugLevel(1))
     Info("FromBarStatusFile()", "Open file %s", name.Data());

  Double_t* tmp = new Double_t[4];
  
  // (fBarsParameter.LayerId).clear();
  // (fBarsParameter.BarId).clear();
  // (fBarsParameter.ActiveBar).clear();
  // (fBarsParameter.ElossThr).clear();

  for (Int_t ibar = 0; ibar < (Int_t)nSlats; ibar++) { // Loop over the bars

    // read parameters
    ReadItem(tmp, 4, ' ');
    
    // (fBarsParameter.LayerId).push_back((Int_t)tmp[0]);
    // (fBarsParameter.BarId).push_back((Int_t)tmp[1]);
    // (fBarsParameter.ElossThr).push_back(tmp[2]);
    // (fBarsParameter.ActiveBar).push_back((Bool_t)tmp[3]);

    fPairId = TPairId((Int_t)tmp[0],(Int_t)tmp[1]);
    // fPairId = std::make_pair(tmp[0],tmp[1]);

    if(FootDebugLevel(1))
      cout<< endl <<fPairId.first<<" "<<fPairId.second<<endl;   

    fMapInfoBar[fPairId]=TBarsTuple(tmp[2],(Bool_t)tmp[3]);
    
    if(FootDebugLevel(1)) {
      cout << " TW bar status: "<< Form("status/Thr bar_%d (%d,%d) = %.3f %d", ibar, fPairId.first, fPairId.second, get<0>(fMapInfoBar[fPairId]), get<1>(fMapInfoBar[fPairId])) << endl;
      cout<<GetElossThreshold(fPairId.first,fPairId.second)<<"  "<<IsTWbarActive(fPairId.first,fPairId.second)<<endl;
    }    
    // if(FootDebugLevel(1))
      // cout << endl << " TW bar status: "<< Form("status/Thr bar_%d = %.3f %d", ibar, fBarsParameter.ElossThr[ibar], fBarsParameter.ActiveBar[ibar]) << endl;
    
  }
  
  delete[] tmp;

  Close();
  
  return kFALSE;
}


//
////------------------------------------------+-----------------------------------
Double_t TATWparCal::GetElossThreshold(Int_t layer, Int_t bar)
{

  TPairId barId = std::make_pair(layer,bar);
  
  if ( fMapInfoBar.find(barId) != fMapInfoBar.end() )

    return get<0>(fMapInfoBar[barId]);

  else {

    Error("GetElossThreshold","No pair layer (%d), bar (%d) found!! Set Eloss Thresholt to -1",layer,bar);
    return -1;
  }
}


//
////------------------------------------------+-----------------------------------
Bool_t TATWparCal::IsTWbarActive(Int_t layer, Int_t bar)
{

  TPairId barId = std::make_pair(layer,bar);
  
  if ( fMapInfoBar.find(barId) != fMapInfoBar.end() )

    return get<1>(fMapInfoBar[barId]);

  else{

    Error("IsTWbarActive", "No pair layer (%d), bar (%d) found!!",layer,bar);
    return 0;

  }
}

//
////------------------------------------------+-----------------------------------
Int_t TATWparCal::GetChargeZ(Float_t edep, Float_t tof, Int_t layer)
{
  if(edep<0) {
    fZraw=-1;
    f_dist_Z.clear();
    for(int iZ=0; iZ<fZbeam; iZ++)
      f_dist_Z.push_back( std::numeric_limits<float>::max() ); //inf

    if (FootDebugLevel(1))
      printf("the energy released is %.f so Zraw is set to %d and dist to inf\n",edep,fZraw);

  }
  else
    ComputeBBDistance(edep,tof,layer);       
  
  return fZraw;
  
  /*
    for (Int_t p = 0; p < fZbeam; p++) { // Loop on each charge
    
    if (edep >= fChargeParameter[p].CutLow && edep < fChargeParameter[p].CutUp )
    return fChargeParameter[p].Charge;
    }
    
    return -1;
  */
}
//
////------------------------------------------+-----------------------------------
Int_t TATWparCal::SelectProtonsFromNeutrons(float distZ1) {

  float mean  = fChargeParameter.distMean[0];
  float sigma = fChargeParameter.distSigma[0];
  Int_t Z = abs(distZ1-mean)<5*sigma ? 1 : 0;
  
  if(FootDebugLevel(1)) cout<<"check::Z==1 assignment...nSigma is < 5 ?  nSigma::"<<abs(distZ1-mean)/sigma<<" ...so...  Zraw::"<<Z<<endl;
  
  return Z;  
}
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

  fZraw = -2;  //nonsense intialization: charge must be > 0
  f_dist_min_Z = std::numeric_limits<float>::max(); //inf

  const float deltaToFmin = 1.e-04;  // 0.1 ps
  // const float deltaToFmin = 0.001;  // 1 ps

  f_dist_Z.clear();

  if(FootDebugLevel(1))
    cout<<"edep::"<<edep<<"  tof::"<<tof<<endl;
  
  for (int iZ=1; iZ<fZbeam+1; iZ++) {

    double fBB_x(-99),fBB_prime_x(-99);
    float dist  = std::numeric_limits<float>::max(); //inf
    // BB parameters
    float parNorm(fChargeParameter.Norm_BB[iZ-1]),parConst(fChargeParameter.Const_BB[iZ-1]);
    // float mean(fChargeParameter.distMean[iZ-1]),sigma(fChargeParameter.distSigma[iZ-1]);
    // fTof_max = iZ<3 ? 5*fTof_beam : 2*fTof_beam; // different for H and He
    double xmin(fTof_min), xmax(fTof_max), xhalf(-99);

    double f_prime_dist_min(10000), f_prime_dist_max(-10000), f_prime_dist_half(-99);


    if(tof>xmin && tof<xmax) { // Beta<=1
      
      //f_prime_dist_min has to be < 0
      while(f_prime_dist_min>0 && xmin<xmax) {

	//compute distance in x==xmin
	fBB_x = fBBparametrized(xmin,iZ,parNorm,parConst);
	fBB_prime_x = fBB_prime_parametrized(xmin,iZ,parNorm,parConst);
	dist = fDist(tof,edep,xmin,fBB_x);
	f_prime_dist_min = fDistPrime(tof,edep,xmin,fBB_x,fBB_prime_x,dist);
	if(f_prime_dist_min>0)
	  xmin+=deltaToFmin*100;
      }
      if(FootDebugLevel(1)) {
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
	  xmax-=deltaToFmin*100;
      }
      if(FootDebugLevel(1)) {
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
	    
	    if(f_prime_dist_min*f_prime_dist_half<0)
	      xmax = xhalf;
	    else if(f_prime_dist_half*f_prime_dist_max<0) 
	      xmin = xhalf;
	  }
	  
	  if(FootDebugLevel(1)) cout<<"dist::"<<dist<<"  distMIN::"<<f_dist_min_Z<<"  Zraw::"<<iZ<<endl;
	  // assign Z value
	  if(dist<f_dist_min_Z) {
	    f_dist_min_Z=dist;
	    fZraw = iZ;
	    if(FootDebugLevel(1)) cout<<"selected:: dist::"<<dist<<"  distMIN::"<<f_dist_min_Z<<"  Zraw::"<<iZ<<endl;
	  }
	  
	  if(edep<fBB_x) {
	    f_dist_Z.push_back( -dist );
	    if(iZ==1&&fZraw==1) // remove neutrons from proton selection
	      fZraw = SelectProtonsFromNeutrons(dist);
	  }
	  else f_dist_Z.push_back( dist );

	  
	} else {  // if (f_prime_dist_min*f_prime_dist_max>0)
	  if(FootDebugLevel(1)) printf("no bisection algorithm is possible to assign Z = %d to the TW hit with (tof,eloss) = (%f,%f)\n",iZ,tof,edep);
	  dist=std::numeric_limits<float>::max(); //inf
	  f_dist_Z.push_back( dist );

	} // close bisec algo condition
	
      } // Xmin<xmax
      else {
	dist=std::numeric_limits<float>::max(); //inf
	f_dist_Z.push_back( dist );
	if (FootDebugLevel(1))
	  printf("xmin(%.5f) > xmax(%.5f) so no possible interval for bisection algorithm\n",xmin,xmax);      
      }
    } else {
      dist=std::numeric_limits<float>::max(); //inf
      f_dist_Z.push_back( dist );
      if (FootDebugLevel(1))
	printf("tof (%.f) is outside the selected interval [%.f,%.f] so Zraw is set to %d\n",tof,xmin,xmax,fZraw);      
    }

    if(FootDebugLevel(1)) printf("for loop over iZ::%d  with dist::%.5f\n\n",iZ,f_dist_Z.at(iZ-1));
    if(FootDebugLevel(1)) printf("the selected Z is:: %d\n\n",fZraw);

  } // close for loop over Z
    
  if(FootDebugLevel(1) && fZraw==0)
    printf("Z::%d  edep::%f  tof::%f \n",fZraw,edep,tof);
    
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


