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
  fErrOrigin.Set(0.,0.);
  fErrSlope.Set(0.,0.);
  fErrCov.Set(0.,0.);
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
  cout<<"fErrOrigin=("<<fErrOrigin.X()<<", "<<fErrOrigin.Y()<<")"<<endl;
  cout<<"fErrSlope=("<<fErrSlope.X()<<", "<<fErrSlope.Y()<<")"<<endl;
  cout<<"fErrCovX="<<fErrCov.X()<<"  fErrCovY="<<fErrCov.Y()<<endl;
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
  fErrOrigin.Set(0.,0.);
  fErrSlope.Set(0.,0.);
  fErrCov.Set(0.,0.);
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
  fErrOrigin=tr_in.fErrOrigin;
  fErrSlope=tr_in.fErrSlope;
  fErrCov=tr_in.fErrCov;
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
    this->fErrOrigin=in.fErrOrigin;
    this->fErrSlope=in.fErrSlope;
    this->fErrCov=in.fErrCov;
  }
  return *this;
}

TVector3 TABMtrack::Intersection(Float_t zloc) const
{
  return TAGgeoTrafo::Intersection(fSlope, fOrigin, zloc);
}


void TABMtrack::PrintTrackPosDir(){
  cout<<"fOrigin=("<<fOrigin.X()<<", "<<fOrigin.Y()<<" ,"<<fOrigin.Z()<<")"<<endl;
  cout<<"fSlope=("<<fSlope.X()<<", "<<fSlope.Y()<<" ,"<<fSlope.Z()<<")"<<endl;
  return;
}

Int_t TABMtrack::mergeTrack(const TABMtrack &yviewtrack){

    fSlope.SetY(yviewtrack.fSlope.Y());
    fOrigin.SetY(yviewtrack.fOrigin.Y());
    fNHitY=yviewtrack.fNHitY;
    fTrackIdY=yviewtrack.fTrackIdY;

  return 0;
}

Bool_t TABMtrack::EvaluateResChi2(TABMntuHit* p_nturaw, TABMparGeo* p_bmgeo){

  Float_t res, chi2redX=0., chi2redY=0.;
  Int_t nselhitX=0, nselhitY=0;
  for(Int_t i=0;i<p_nturaw->GetHitsN();++i){
    TABMhit* p_hit=p_nturaw->GetHit(i);
    if(p_hit->GetIsSelected()==fTrackIdX){
      ++nselhitX;
      res=p_hit->GetRdrift()-p_bmgeo->PointLineDist(fSlope.X()/fSlope.Z(),fOrigin.X(),p_hit->GetWirePos().X(), p_hit->GetWirePos().Z());
      p_hit->SetResidual(res);
      p_hit->SetChi2(res*res/p_hit->GetSigma()/p_hit->GetSigma());
      chi2redX+=p_hit->GetChi2();
    }else if(p_hit->GetIsSelected()==fTrackIdY){
      ++nselhitY;
      res=p_hit->GetRdrift()-p_bmgeo->PointLineDist(fSlope.Y()/fSlope.Z(),fOrigin.Y(),p_hit->GetWirePos().Y(), p_hit->GetWirePos().Z());
      p_hit->SetResidual(res);
      p_hit->SetChi2(res*res/p_hit->GetSigma()/p_hit->GetSigma());
      chi2redY+=p_hit->GetChi2();
    }
  }

  fChiSquareX=chi2redX/(nselhitX-2.);
  fChiSquareY=chi2redY/(nselhitY-2.);
  fChiSquare=(chi2redX+chi2redY)/(nselhitX+nselhitY-4.);

  if (FootDebugLevel(2))
      cout<<"TABMtrack::EvaluateResChi2 finished: track has been reconstructed,  chi2redX="<<fChiSquareX<<" chi2redY="<<fChiSquareY<<"  chi2red="<<fChiSquare<<endl;

  return kFALSE;
}
