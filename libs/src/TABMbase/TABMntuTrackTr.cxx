/*!
  \class TABMntuTrackTr TABMntuTrack.hxx "TABMntuTrack.hxx"
  \brief Beam Monitor track description - track object **
*/

#include "TABMntuTrackTr.hxx"

ClassImp(TABMntuTrackTr);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMntuTrackTr::TABMntuTrackTr():
    fHitsN(0), fChi2Red(999), fIsConverged(0),fEffFittedPlane(-1)
{
  fPvers.SetXYZ(-100.,-100.,-100.);
  fR0.SetXYZ(-100.,-100.,-100.);
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
  cout<<"new tracking: nhits="<<fHitsN<<"  chi2Red="<<fChi2Red<<"  isConverged="<<fIsConverged<<"  eff_fittedplane="<<fEffFittedPlane<<endl;
  cout<<"Pvers=("<<fPvers.X()<<", "<<fPvers.Y()<<", "<<fPvers.Z()<<")"<<endl;
  cout<<"R0=("<<fR0.X()<<", "<<fR0.Y()<<", "<<fR0.Z()<<")"<<endl;
}

/*-----------------------------------------------------------------*/

//reset the Track values to default 
void TABMntuTrackTr::Clean()
{
  //new tracking
  fHitsN=0;
  fChi2Red=999;
  fIsConverged=0;
  fEffFittedPlane=-1;
  fR0.SetXYZ(-100.,-100.,-100.);
  fPvers.SetXYZ(-100.,-100.,-100.);
  
}

TABMntuTrackTr::TABMntuTrackTr(const TABMntuTrackTr &tr_in)
{
  fHitsN=tr_in.fHitsN;
  fChi2Red=tr_in.fChi2Red;
  fIsConverged=tr_in.fIsConverged;
  fEffFittedPlane=tr_in.fEffFittedPlane;
  fR0=tr_in.fR0;
  fPvers=tr_in.fPvers;
}

TABMntuTrackTr& TABMntuTrackTr::operator=(TABMntuTrackTr const& in)
{
   if(this!=&in){
      fHitsN=in.fHitsN;
      fChi2Red=in.fChi2Red;
      fIsConverged=in.fIsConverged;
      fPvers=in.fPvers;
      fR0=in.fR0;
      fEffFittedPlane=in.fEffFittedPlane;
   }
   return *this;
}

TVector3 TABMntuTrackTr::PointAtLocalZ(double zloc)
{
   TVector3 projected(fPvers.X()/fPvers.Z()*zloc+fR0.X() ,fPvers.Y()/fPvers.Z()*zloc+fR0.Y(), zloc);
   return projected;
}


void TABMntuTrackTr::NewSet(TVectorD ftrackpar){
  Double_t transv=ftrackpar[2]*ftrackpar[2]+ftrackpar[3]*ftrackpar[3];
  if(transv<1.1){
    fPvers.SetXYZ(ftrackpar[2], ftrackpar[3], sqrt(1.-transv));
    fR0.SetXYZ(ftrackpar[0], ftrackpar[1], 0.);
  }
  else{
    Error("Action()","TABMntuTrackTr::Set(vec)-> ERROR: pvers>1  px=%f py=%f px*px+py*py=%lf ",ftrackpar[2],ftrackpar[3],transv);
    return ;
  }  
  return;
}


void TABMntuTrackTr::PrintR0Pvers(){
  cout<<"R0=("<<fR0.X()<<", "<<fR0.Y()<<" ,"<<fR0.Z()<<")"<<endl;
  cout<<"Pvers=("<<fPvers.X()<<", "<<fPvers.Y()<<" ,"<<fPvers.Z()<<")"<<endl;
  return;
}


Int_t TABMntuTrackTr::mergeTrack(const TABMntuTrackTr &otherview){
  if((fChiSquareX>=0 && otherview.fChiSquareX>=0) || (fChiSquareY>=0 && otherview.fChiSquareY>=0)){
    cout<<"TABMtrack::mergeTrack: ERROR!!!: current track fChiSquareX="<<fChiSquareX<<"  fChiSquareY="<<fChiSquareY<<endl<<"input track fChiSquareX="<<otherview.fChiSquareX<<"  fChiSquareY="<<otherview.fChiSquareY<<endl;
    return -1;
  }
  if(otherview.fIsConverged!=1)
    return 1;
  if(fIsConverged!=1)
    return 2;

  if(fChiSquareX<0){
    fChiSquareX=otherview.fChiSquareX;
    fChiSquare=(fChiSquareY*(fNHitY-2.)+otherview.fChiSquareX*(otherview.fNHitX-2.))/(fNHitY+otherview.fNHitX-4.);
    fSlope.SetX(otherview.fSlope.X());
    fOrigin.SetX(otherview.fOrigin.X());
    fNHitX=otherview.fNHitX;
  }else{
    fChiSquareY=otherview.fChiSquareY;
    fChiSquare=(fChiSquareX*(fNHitX-2.)+otherview.fChiSquareY*(otherview.fNHitY-2.))/(fNHitX+otherview.fNHitY-4.);
    fSlope.SetY(otherview.fSlope.Y());
    fOrigin.SetY(otherview.fOrigin.Y());
    fNHitY=otherview.fNHitY;
  }

  return 0;
}
