/*!
  \class TABMntuTrackTr TABMntuTrack.hxx "TABMntuTrack.hxx"
  \brief Beam Monitor track description - track object **
*/

#include "TABMntuTrackTr.hxx"

ClassImp(TABMntuTrackTr);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMntuTrackTr::TABMntuTrackTr():
    nhit(0), chi2Red(999), isConverged(0),prefit_status(-10)
    
{
  Pvers.SetXYZ(-100.,-100.,-100.);
  R0.SetXYZ(-100.,-100.,-100.);
  nite=0;
  eff_fittedplane=-2.;
 }

//------------------------------------------+-----------------------------------
//! Destructor.

TABMntuTrackTr::~TABMntuTrackTr(){}

/*-----------------------------------------------------------------*/
//**************************OLD AND NEW TRACKING***************************
/*-----------------------------------------------------------------*/

void TABMntuTrackTr::Dump() const
{ 
  cout<<endl<<"------------ Dump Track Class ---------"<<endl;
  cout<<"new tracking: nhit="<<nhit<<"  chi2Red="<<chi2Red<<"  isConverged="<<isConverged<<"  prefit_status="<<prefit_status<<"  nite="<<nite<<endl;
  cout<<"Pvers=("<<Pvers.X()<<", "<<Pvers.Y()<<", "<<Pvers.Z()<<")"<<endl;
  cout<<"R0=("<<R0.X()<<", "<<R0.Y()<<", "<<R0.Z()<<")"<<endl;
}

/*-----------------------------------------------------------------*/

void TABMntuTrackTr::Clean()
{
  /*  
      reset the Track values to default 
  */

  //new tracking
  nhit=0;
  chi2Red=999;
  isConverged=0;
  Pvers.SetXYZ(-100.,-100.,-100.);
  R0.SetXYZ(-100.,-100.,-100.);
  prefit_status=-10;
  nite=0;
}

TABMntuTrackTr::TABMntuTrackTr(const TABMntuTrackTr &tr_in){
  nhit=tr_in.nhit;
  chi2Red=tr_in.chi2Red;
  isConverged=tr_in.isConverged;
  prefit_status=tr_in.prefit_status;
  R0=tr_in.R0;
  Pvers=tr_in.Pvers;
  nite=tr_in.nite;
}

TVector3 TABMntuTrackTr::PointAtLocalZ(double zloc)
{
   TVector3 projected(Pvers.X()/Pvers.Z()*zloc+R0.X() ,Pvers.Y()/Pvers.Z()*zloc+R0.Y(), zloc);   
   return projected;
}


void TABMntuTrackTr::NewSet(TVectorD ftrackpar){
  Double_t transv=ftrackpar[2]*ftrackpar[2]+ftrackpar[3]*ftrackpar[3];
  if(transv<1.1){
    Pvers.SetXYZ(ftrackpar[2], ftrackpar[3], sqrt(1.-transv));
    R0.SetXYZ(ftrackpar[0], ftrackpar[1], 0.);
  }
  else{
    Error("Action()","TABMntuTrackTr::Set(vec)-> ERROR: pvers>1  px=%f py=%f px*px+py*py=%lf ",ftrackpar[2],ftrackpar[3],transv);
    return ;
  }  
  return;
}


void TABMntuTrackTr::PrintR0Pvers(){
  cout<<"R0=("<<R0.X()<<", "<<R0.Y()<<" ,"<<R0.Z()<<")"<<endl;
  cout<<"Pvers=("<<Pvers.X()<<", "<<Pvers.Y()<<" ,"<<Pvers.Z()<<")"<<endl;
  return;
}

