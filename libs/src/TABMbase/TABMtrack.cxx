/*!
  \class TABMtrack TABMntuTrack.hxx "TABMntuTrack.hxx"
  \brief Beam Monitor track description - track object **
*/

#include "TABMtrack.hxx"

ClassImp(TABMtrack);

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMtrack::TABMtrack():
    fNHitX(0),fNHitY(0), fChiSquare(-999), fChiSquareX(-999),fChiSquareY(-999), fIsConv(0),fGhost(-1), fTrackIdX(-1), fTrackIdY(-1)
{
  fSlope.SetXYZ(1.,1.,1.);
  fOrigin.SetXYZ(0.,0.,0.);
 }

//------------------------------------------+-----------------------------------
//! Destructor.

TABMtrack::~TABMtrack(){}


void TABMtrack::Dump() const
{
  cout<<endl<<"------------ Dump Track Class ---------"<<endl;
  cout<<"new tracking: fNHitX="<<fNHitX<<"  fNHitY="<<fNHitY<<"  fChiSquare="<<fChiSquare<<"  fChiSquareX="<<fChiSquareX<<"  fChiSquareY="<<fChiSquareY<<"  fIsConv="<<fIsConv<<"  fGhost="<<fGhost<<"  fTrackIdX="<<fTrackIdX<<"  fTrackIdY="<<fTrackIdY<<endl;
  cout<<"fSlope=("<<fSlope.X()<<", "<<fSlope.Y()<<", "<<fSlope.Z()<<")"<<endl;
  cout<<"fOrigin=("<<fOrigin.X()<<", "<<fOrigin.Y()<<", "<<fOrigin.Z()<<")"<<endl;
}

/*-----------------------------------------------------------------*/

//reset the Track values to default
void TABMtrack::Clean()
{
  //new tracking
  fNHitX=0;
  fNHitY=0;
  fChiSquare=-999;
  fChiSquareX=-999;
  fChiSquareY=-999;
  fIsConv=0;
  fGhost=-1;
  fTrackIdX=-1;
  fTrackIdY=-1;
  fOrigin.SetXYZ(0.,0.,0.);
  fSlope.SetXYZ(1.,1.,1.);

}

TABMtrack::TABMtrack(const TABMtrack &tr_in){
  fNHitX=tr_in.fNHitX;
  fNHitY=tr_in.fNHitY;
  fChiSquare=tr_in.fChiSquare;
  fChiSquareX=tr_in.fChiSquareX;
  fChiSquareY=tr_in.fChiSquareY;
  fIsConv=tr_in.fIsConv;
  fGhost=tr_in.fGhost;
  fTrackIdX=tr_in.fTrackIdX;
  fTrackIdY=tr_in.fTrackIdY;
  fOrigin=tr_in.fOrigin;
  fSlope=tr_in.fSlope;
}

TABMtrack& TABMtrack::operator=(TABMtrack const& in){
  if(this!=&in){
    this->fNHitY=in.fNHitY;
    this->fNHitX=in.fNHitX;
    this->fChiSquare=in.fChiSquare;
    this->fChiSquareX=in.fChiSquareX;
    this->fChiSquareY=in.fChiSquareY;
    this->fIsConv=in.fIsConv;
    this->fGhost=in.fGhost;
    this->fTrackIdX=in.fTrackIdX;
    this->fTrackIdY=in.fTrackIdY;
    this->fSlope=in.fSlope;
    this->fOrigin=in.fOrigin;
  }
  return *this;
}


TVector3 TABMtrack::PointAtLocalZ(double zloc) const
{
   TVector3 projected(fSlope.X()/fSlope.Z()*zloc+fOrigin.X() ,fSlope.Y()/fSlope.Z()*zloc+fOrigin.Y(), zloc);
   return projected;
}


TVector3 TABMtrack::Intersection(Float_t zloc) const
{
  return PointAtLocalZ(zloc);
}


void TABMtrack::PrintTrackPosDir(){
  cout<<"fOrigin=("<<fOrigin.X()<<", "<<fOrigin.Y()<<" ,"<<fOrigin.Z()<<")"<<endl;
  cout<<"fSlope=("<<fSlope.X()<<", "<<fSlope.Y()<<" ,"<<fSlope.Z()<<")"<<endl;
  return;
}

Int_t TABMtrack::mergeTrack(const TABMtrack &otherview){
  if((fChiSquareX>=0 && otherview.fChiSquareX>=0) || (fChiSquareY>=0 && otherview.fChiSquareY>=0)){
    cout<<"TABMtrack::mergeTrack: ERROR!!!: current track fChiSquareX="<<fChiSquareX<<"  fChiSquareY="<<fChiSquareY<<endl<<"input track fChiSquareX="<<otherview.fChiSquareX<<"  fChiSquareY="<<otherview.fChiSquareY<<endl;
    return -1;
  }
  if(otherview.fIsConv!=1)
    return 1;
  if(fIsConv!=1)
    return 2;

  if(fChiSquareX<0){
    fChiSquareX=otherview.fChiSquareX;
    fChiSquare=(fChiSquareY*(fNHitY-2.)+otherview.fChiSquareX*(otherview.fNHitX-2.))/(fNHitY+otherview.fNHitX-4.);
    fSlope.SetX(otherview.fSlope.X());
    fOrigin.SetX(otherview.fOrigin.X());
    fNHitX=otherview.fNHitX;
    fTrackIdX=otherview.fTrackIdX;
  }else{
    fChiSquareY=otherview.fChiSquareY;
    fChiSquare=(fChiSquareX*(fNHitX-2.)+otherview.fChiSquareY*(otherview.fNHitY-2.))/(fNHitX+otherview.fNHitY-4.);
    fSlope.SetY(otherview.fSlope.Y());
    fOrigin.SetY(otherview.fOrigin.Y());
    fNHitY=otherview.fNHitY;
    fTrackIdY=otherview.fTrackIdY;
  }

  return 0;
}
