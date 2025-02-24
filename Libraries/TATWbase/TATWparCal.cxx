/*!
  \file TATWparCal.cxx
  \brief   Implementation of TATWparCal.
 */

#include <Riostream.h>
#include <limits>

#include "TAGrecoManager.hxx"
#include "TAGnameManager.hxx"

#include "TAGroot.hxx"
#include <TCanvas.h>
#include "TATWparCal.hxx"


//##############################################################################

/*!
  \class TATWparCal
  \brief Charge (raw) calibration for vertex. **
 */

//! Class Imp
ClassImp(TATWparCal);

TString TATWparCal::fgkDeltaTime = "./config/TATWdeltaTimeMap.map";
TString TATWparCal::fgkBarStatus = "./config/TATWbarsMapStatus.map";
TString TATWparCal::fgkBBparamName = "./config/TATW_BBparameters.cfg";

//------------------------------------------+-----------------------------------
TATWparCal::TATWparCal()
  : TAGparTools(),
    f_isPosCalibration(false),
    f_isBarCalibration(false),
    f_isElossTuningON(false),
    fZraw(-1),
    fHisRate(NULL),
    f_dist_min_Z(std::numeric_limits<float>::max()) //inf
{

  // Standard constructor
  fMapCal=new TATWcalibrationMap();

  fParGeo = (TAGparGeo*)gTAGroot->FindParaDsc(FootParaDscName("TAGparGeo"), "TAGparGeo")->Object();

  fGeoTrafo = (TAGgeoTrafo*)gTAGroot->FindAction(TAGgeoTrafo::GetDefaultActName().Data());

  fParGeoTw = (TATWparGeo*)gTAGroot->FindParaDsc(FootParaDscName("TATWparGeo"), "TATWparGeo")->Object();
  
  RetrieveBeamQuantities();

  f_dist_Z.clear();

  // // TW
  // TAGparaDsc* parGeoTW = new TAGparaDsc( new TATWparGeo() );
  // // TATWparGeo*
  // twparGeo = (TATWparGeo*)parGeoTW->Object();

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
  fIonBeamName = fParGeo->GetBeamPar().Material;

  TString  ion_name = GetIonBeamName();
  Int_t    A_beam = fParGeo->GetBeamPar().AtomicMass;
  Float_t  kinE_beam = fParGeo->GetBeamPar().Energy*TAGgeoTrafo::GevToMev()*A_beam; //MeV
  
  if(FootDebugLevel(4))
    Info("RetrieveBeamQuantities()","ion::%s Z::%d A::%d E::%d\n",fIonBeamName.Data(),fZbeam,A_beam,(int)(kinE_beam/A_beam));
  
  Float_t  z_SC = ((TVector3)fGeoTrafo->GetSTCenter()).Z();    // cm
  Float_t  z_TW = ((TVector3)fGeoTrafo->GetTWCenter()).Z();    // cm
  Float_t  Lmin = z_TW-z_SC; // roughly particles shorter path length
  
  const Double_t C_speed = TMath::C()*TAGgeoTrafo::MToCm()/TAGgeoTrafo::SecToNs(); //~30 cm/ns
  fTof_min = Lmin/C_speed; // for Beta==1
  
  TDatabasePDG db;  
  const Double_t mass_p = (Double_t)((TParticlePDG*)db.GetParticle(2212))->Mass(); //GeV
  const Double_t mass_n = (Double_t)((TParticlePDG*)db.GetParticle(2112))->Mass(); //GeV
  
  enum {H=1,He4=4,C12=12,O16=16}; // beam isotopes
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
  
  Double_t Mass_beam = (fZbeam*mass_p+(A_beam-fZbeam)*mass_n)*TAGgeoTrafo::GevToMev()-binding_energy*A_beam; //MeV
  Double_t Energy_beam = kinE_beam+Mass_beam;  //MeV
  Double_t Beta_beam = sqrt(pow(Energy_beam,2)-pow(Mass_beam,2))/Energy_beam;

  fTof_beam = Lmin/(C_speed*Beta_beam);
  fTof_max = 2*fTof_beam;

  ComputeAvgBeamEnergyLossInTwBar(fZbeam,Beta_beam);
  
  if(FootDebugLevel(4))
    cout<<setprecision(3)<<"L::"<<Lmin<<" cm  Tof_min::"<<fTof_min<<" ns  Tof_max::"<<fTof_max<<" ns  <Tof>::"<<fTof_beam<<" ns  Beta::"<<Beta_beam<<"  Ekin/u::"<<kinE_beam/A_beam<<" MeV/u  Ekin::"<<kinE_beam*TAGgeoTrafo::MevToGev()<<" GeV  Mass::"<<Mass_beam*TAGgeoTrafo::MevToGev()<<" GeV  Energy::"<<Energy_beam*TAGgeoTrafo::MevToGev()<<" GeV  B::"<<binding_energy<<" AMeV  ElossInBar::"<<fAvgBeamEnergyLossInBar<<endl<<endl;

}
//------------------------------------------+-----------------------------------
void  TATWparCal::ComputeAvgBeamEnergyLossInTwBar(Int_t Z, Double_t Beta ) {

  Float_t barDensity = fParGeoTw->GetBarDensity(); // g/cm^3
  Float_t barThick = fParGeoTw->GetBarThick();     // cm

  Double_t ElossMinimumParticle = 2; //dE/dx*1/pho [MeV/g*cm^2]

  fAvgBeamEnergyLossInBar = ElossMinimumParticle * barDensity * barThick * pow(Z/Beta,2);
  fAvgBeamEnergyLossInBar *= 1.3;  // increase of 30%

  return;

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
  if(isTof==0)
     Info("FromCalibFile()", "Open file %s for Energy calibration", name_exp.Data());
  if(isTof==1)
     Info("FromCalibFile()", "Open file %s for TOF calibration", name_exp.Data());


  return kFALSE;
    
}  




//------------------------------------------+-----------------------------------
Bool_t TATWparCal::FromRateFile(const TString& name, Int_t initRun, Int_t endRun)
{

  TString name_exp = name;
  gSystem->ExpandPathName(name_exp);
  
  TFile *f = new TFile(name_exp.Data());
  if(f->IsOpen()){
    Info("FromRateFile()", "Open file %s to import rate distribution", name_exp.Data());
    TH2D *hisRate = (TH2D*)f->Get("Rate_vs_runnumber_C_C_200_MeV");
    if(hisRate!=NULL)fHisRate = (TH1D*)(hisRate->ProjectionY("hisRate",initRun, endRun));
    fHisRate->SetDirectory(0);
    delete f;
  }else{
    Info("FromRateFile()", "Cannot open file %s!!!!!, rate distribution not set", name_exp.Data());
  }
  

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
  
  Double_t* tmp = new Double_t[parNumberZIDcalibFile];
  
  (fChargeParameter.Norm_BB).clear();
  (fChargeParameter.Const_BB).clear();
  (fChargeParameter.CutLow).clear();
  (fChargeParameter.CutUp).clear();
  (fChargeParameter.distMean).clear();
  (fChargeParameter.distSigma).clear();
  
  for (Int_t ilay = 0; ilay < nLayers; ilay++) { // Loop on both TW layers
    for (Int_t iZ = 0; iZ < Zbeam; iZ++) { // Loop on each charge

      // read parameters
      ReadItem(tmp, parNumberZIDcalibFile, ' ');
      
      (fChargeParameter.Norm_BB)[ilay].push_back(tmp[0]);
      (fChargeParameter.Const_BB)[ilay].push_back(tmp[1]);
      (fChargeParameter.CutLow)[ilay].push_back(tmp[2]);
      (fChargeParameter.CutUp)[ilay].push_back(tmp[3]);
      (fChargeParameter.distMean)[ilay].push_back(tmp[4]);
      (fChargeParameter.distSigma)[ilay].push_back(tmp[5]);
      
    if(FootDebugLevel(4))
      Info("FromFileZID()","Z::%d Norm::%f Const::%f Tof_min::%f Tof_max::%f\n", iZ+1, fChargeParameter.Norm_BB[ilay][iZ], fChargeParameter.Const_BB[ilay][iZ], fChargeParameter.CutLow[ilay][iZ], fChargeParameter.CutUp[ilay][iZ]);
      
    }
  }
  
  delete[] tmp;
  
  Close();
  
  return kFALSE;
}


//
////------------------------------------------+-----------------------------------

Bool_t TATWparCal::FromDeltaTimeFile(const TString& name) {
 
    
  TString nameExp;
  
  if (name.IsNull())
    nameExp = fgkDeltaTime;
  else
    nameExp = name;

  if (!Open(nameExp)) return false;

  Info("FromDeltaTimeFile()", "Open file %s for calibration of the position along the TW bars with the delta Time\n", name.Data());
   
  Double_t* tmp = new Double_t[4];
  for (Int_t ibar = 0; ibar < (Int_t)nBars; ibar++) { // Loop over the bars

    // read parameters
    ReadItem(tmp, parNumberDeltaTfile, ' ');
    
    fPairId = TPairId((Int_t)tmp[0],(Int_t)tmp[1]); //barId(shoe format), layerId
    // fPairId = TPairId((Int_t)tmp[0]%nBarsPerLayer,(Int_t)tmp[3]); //barId(shoe format), layerId

    if(FootDebugLevel(4))
      cout<< endl <<fPairId.first<<" "<<fPairId.second<<endl;   

    fMapDeltaTime[fPairId]=TTupleDeltaT(tmp[2],tmp[3]); // velocity of light in the bars in [cm/ns], offset delta cable in [cm]
    
    if(FootDebugLevel(4)) {
      Info("FromDeltaTimeFile()","deltaTime/Offset bar_%d (%d,%d) = %2.3f %.3f", ibar, fPairId.first, fPairId.second, get<0>(fMapDeltaTime[fPairId]), get<1>(fMapDeltaTime[fPairId]));
      cout<<GetBarLightSpeed(fPairId.first,fPairId.second)<<"  "<<GetDeltaTimePosOffset(fPairId.first,fPairId.second)<<endl;

    }    
    
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

  Info("FromBarStatusFile()", "Open file %s for bar status map\n", name.Data());
   
  Double_t* tmp = new Double_t[4];
  for (Int_t ibar = 0; ibar < (Int_t)nBars; ibar++) { // Loop over the bars

    // read parameters
    ReadItem(tmp, parNumberBarStatusFile, ' ');
    
    fPairId = TPairId((Int_t)tmp[0],(Int_t)tmp[1]);

    if(FootDebugLevel(4))
      cout<< endl <<fPairId.first<<" "<<fPairId.second<<endl;   

    fMapInfoBar[fPairId]=TBarsTuple(tmp[2],(Bool_t)tmp[3]);
    
    if(FootDebugLevel(4)) {
      Info("FromBarStatusFile()","status/Thr barID::%d (%d,%d) = %.3f %d", ibar, fPairId.first, fPairId.second, get<0>(fMapInfoBar[fPairId]), get<1>(fMapInfoBar[fPairId]));
      cout<<GetElossThreshold(fPairId.first,fPairId.second)<<"  "<<IsTWbarActive(fPairId.first,fPairId.second)<<endl;
    }    
    
  }
  
  delete[] tmp;

  Close();
  
  return kFALSE;
}


//
////------------------------------------------+-----------------------------------
Double_t TATWparCal::GetBarLightSpeed(Int_t layer, Int_t bar)
{

  TPairId barId = std::make_pair(layer,bar);
  
  if ( fMapDeltaTime.find(barId) != fMapDeltaTime.end() )

    return get<0>(fMapDeltaTime[barId]);

  else {

    Error("GetBarLightSpeed","No pair layer (%d), bar (%d) found!! Set Bar Light Speed to -1",layer,bar);
    return -1;
  }
}

////------------------------------------------+-----------------------------------
Double_t TATWparCal::GetDeltaTimePosOffset(Int_t layer, Int_t bar)
{

  TPairId barId = std::make_pair(layer,bar);
  
  if ( fMapDeltaTime.find(barId) != fMapDeltaTime.end() )

    return get<1>(fMapDeltaTime[barId]);

  else {

    Error("GetDeltaTimePosOffset","No pair layer (%d), bar (%d) found!! Set Delta Time Pos Offset to -1",layer,bar);
    return -1;
  }
}

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

    if (FootDebugLevel(4))
      Info("GetChargeZ()","The energy released is %.f so Zraw is set to %d and dist to inf\n",edep,fZraw);

  }
  else
    ComputeBBDistance(edep,tof,layer);       
  
  return fZraw;
  
}
//
////------------------------------------------+-----------------------------------
Int_t TATWparCal::SelectProtonsFromNeutrons(float distZ1, int twlayer) {

 // for the moment distMean and distSigma based on distances from Z=1 BB curve of the front TW
  float mean  = fChargeParameter.distMean[twlayer][kH_Z-1];
  float sigma = fChargeParameter.distSigma[twlayer][kH_Z-1];
  Int_t Z = abs(distZ1-mean)<5*sigma ? 1 : 0;
  
  if(FootDebugLevel(4))
    Info("SelectProtonsFromNeutrons()","check::Z==1 assignment...nSigma is < 5 ?  nSigma::%.3f...so...  Zraw::%d",abs(distZ1-mean)/sigma,Z);
  
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

  if(FootDebugLevel(4))
    cout<<"edep::"<<edep<<"  tof::"<<tof<<endl;
  
  for (int iZ=1; iZ<fZbeam+1; iZ++) {

    double fBB_x(-99),fBB_prime_x(-99);
    float dist  = std::numeric_limits<float>::max(); //inf
    // BB parameters
    float parNorm(fChargeParameter.Norm_BB[tw_layer][iZ-1]),parConst(fChargeParameter.Const_BB[tw_layer][iZ-1]);
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
      if(FootDebugLevel(4)) {
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
      if(FootDebugLevel(4)) {
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
	  
	  if(FootDebugLevel(4)) cout<<"dist::"<<dist<<"  distMIN::"<<f_dist_min_Z<<"  Zraw::"<<iZ<<endl;
	  // assign Z value
	  if(dist<f_dist_min_Z) {
	    f_dist_min_Z=dist;
	    fZraw = iZ;
	    if(FootDebugLevel(4)) cout<<"selected:: dist::"<<dist<<"  distMIN::"<<f_dist_min_Z<<"  Zraw::"<<iZ<<endl;
	  }
	  
	  if(edep<fBB_x) {
	    f_dist_Z.push_back( -dist );
	    if(iZ==1&&fZraw==1) // remove neutrons from proton selection
	      fZraw = SelectProtonsFromNeutrons(dist,tw_layer);
	  }
	  else f_dist_Z.push_back( dist );

	  
	} else {  // if (f_prime_dist_min*f_prime_dist_max>0)
	  if(FootDebugLevel(4)) Info("ComputeBBdistance()","No bisection algorithm is possible to assign Z = %d to the TW hit with (tof,eloss) = (%f,%f)\n",iZ,tof,edep);
	  dist=std::numeric_limits<float>::max(); //inf
	  f_dist_Z.push_back( dist );

	} // close bisec algo condition
	
      } // Xmin<xmax
      else {
	dist=std::numeric_limits<float>::max(); //inf
	f_dist_Z.push_back( dist );
	if (FootDebugLevel(4))
	  Info("ComputeBBdistance()","xmin(%.5f) > xmax(%.5f) so no possible interval for bisection algorithm\n",xmin,xmax);      
      }
    } else {
      dist=std::numeric_limits<float>::max(); //inf
      f_dist_Z.push_back( dist );
      if (FootDebugLevel(4))
        Info("ComputeBBdistance()","tof (%.f) is outside the selected interval [%.f,%.f] so Zraw is set to %d\n",tof,xmin,xmax,fZraw);
    }

    if(FootDebugLevel(4)) Info("ComputeBBdistance()","for loop over iZ::%d  with dist::%.5f\n\n",iZ,f_dist_Z.at(iZ-1));
    if(FootDebugLevel(4)) Info("ComputeBBdistance()","the selected Z is:: %d\n\n",fZraw);

  } // close for loop over Z
    
  if(FootDebugLevel(4) && fZraw==0)
    Info("ComputeBBdistance()","Z::%d  edep::%f  tof::%f \n",fZraw,edep,tof);
    
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


