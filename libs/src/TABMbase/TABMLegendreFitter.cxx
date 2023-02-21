/*!
  \file
  \brief   Implementation of TABMLegendreFitter.
*/
#include "TABMLegendreFitter.hxx"
#include "TABMparConf.hxx"
#include "TABMntuHit.hxx"
#include "TABMhit.hxx"
#include "TAGroot.hxx"
#include "TAGaction.hxx"

#include <iostream>
#include <math.h>    
#include <algorithm>    // std::max

using namespace std;
/*!
  \class TABMLegendreFitter TABMLegendreFitter.hxx "TABMLegendreFitter.hxx"
  \brief Track Fitter with the Legendre method for Beam Monitor. **
*/

ClassImp(TABMLegendreFitter);

//------------------------------------------+-----------------------------------
//! Default constructor.
TABMLegendreFitter::TABMLegendreFitter(const char* name, TABMparConf* p_bmcon):
  TAGnamed(name,"Legendre Fitter - find track starting values"),
  fpbmcon(p_bmcon)  {

  //legendre
  fLegPolSum=new TH2F("fLegPolSum","Legendre polynomial space; m; q[cm]",
                      fpbmcon->GetLegMBin(),-fpbmcon->GetLegMRange(),
		      fpbmcon->GetLegMRange(), 
		      fpbmcon->GetLegRBin(),-fpbmcon->GetLegRRange(),
		      fpbmcon->GetLegRRange());
  fNxBins = fpbmcon->GetLegMBin();
  Float_t mcoeff, sqrtm2p1;
  fpM      = new Float_t[fNxBins+2];
  fpSqmqp1 = new Float_t[fNxBins+2];
  for(Int_t k=0; k<fNxBins; ++k){
    mcoeff = fLegPolSum->GetXaxis()->GetBinCenter(k+1);
    sqrtm2p1 = sqrt(mcoeff*mcoeff+1.);
    fpM[k] = mcoeff;
    fpSqmqp1[k] = sqrtm2p1;
  }

  fpbmcon = p_bmcon;
};


TABMLegendreFitter::~TABMLegendreFitter(){
  delete [] fpSqmqp1;
  delete [] fpM;
  delete fLegPolSum;
}


void TABMLegendreFitter::ToStream(ostream& os, Option_t* option) const {

  os<<"\nLegendreFitter for view="<<fNowView<<", track="<<fTrackNum
    <<"\nSlope term selected: value="<<bestMValue<<", bin="<<fBestMBin
    <<", width="<<mBinWidth<<", rebins="<<mRebins
    <<"\nConstant term selected: value="<<bestQValue<<", bin="<<fBestQBin
    <<", width="<<qBinWidth<<", rebins="<<qRebins<<"\n";
 
}


// make all the analysis from here!
Int_t TABMLegendreFitter::doLegendreFit(TABMntuHit* p_nturaw, Int_t trackNum, Int_t nowView){

  fpnturaw = p_nturaw; // save hit list pointer
  fTrackNum = trackNum;
  fNowView = nowView;
  
  //prepare 2D histo
  ChargeLegendrePoly();
  
  //find the highest peak in legendre space
  FindLegendreBestValues();
  
  //check and flag the associated hits
  Int_t numSelectedHits = CheckAssHits(fpbmcon->GetAssHitErr(),  0.1 /* +-0.1 cm acceptance */);

  return numSelectedHits;
}


void TABMLegendreFitter::ChargeLegendrePoly(){

  if(FootDebugLevel(2))
    std::cout<<"TABMLegendreFitter:: start charge legendre pol graphs"<<endl;

  fLegPolSum->Reset("ICESM");
  fLegPolSum->SetBins(fpbmcon->GetLegMBin(),
		      -fpbmcon->GetLegMRange(), fpbmcon->GetLegMRange(), 
                      fpbmcon->GetLegRBin(),
		      -fpbmcon->GetLegRRange(), fpbmcon->GetLegRRange());

  Float_t mcoeff, sqmqp1, z;
  
  for(Int_t i_h = 0; i_h < fpnturaw->GetHitsN(); ++i_h) {

    TABMhit* p_hit = fpnturaw->GetHit(i_h);	
    if(p_hit->GetView()==fNowView && p_hit->GetIsSelected()<=0){
      for(Int_t k=0; k<fNxBins; ++k){
        mcoeff = fpM[k];
	sqmqp1 = fpSqmqp1[k];
	Float_t y = ((fNowView==1) ? p_hit->GetWirePos().X() : p_hit->GetWirePos().Y());
	Float_t zwire = p_hit->GetWirePos().Z();
	Float_t rdrift = p_hit->GetRdrift();
        fLegPolSum->Fill( mcoeff, y -mcoeff*zwire+rdrift*sqmqp1);
        fLegPolSum->Fill( mcoeff, y -mcoeff*zwire-rdrift*sqmqp1);
      }
    }
	
  }

  if(FootDebugLevel(2))
    cout<<"TABMLegendreFitter::ChargeLegendrePoly done"<<endl;

  return;
}


void TABMLegendreFitter::FindLegendreBestValues(){

  if(FootDebugLevel(2))
    cout<<"TABMactNtuTrack::FindLegendreBestValues: start"<<endl;

  static int nwr=20;
  if( nwr>0 ){
    SaveLegpol();
    nwr--;
  }

  // Test with single maximum
  Int_t maxm, maxq, dummy;
  Double_t val;
  fLegPolSum->GetMaximumBin(maxm, maxq, dummy);
  val = fLegPolSum->GetBinContent(maxm, maxq);
  if(FootDebugLevel(2))
    cout<<"\nTABMLegendreFitter::FindLegendreBestValues: max at im="<<maxm
	<<" iq="<<maxq<<" Value="<<val<<"\n";

  Int_t first_maxbin,last_maxbin;

  // Start with the Q value -- it is the most sensitive quantity!

  fBestQBin=-1;
  qRebins=0;
  do{
    first_maxbin = fLegPolSum->FindFirstBinAbove(fLegPolSum->GetMaximum()-0.5,2);
    last_maxbin = fLegPolSum->FindLastBinAbove(fLegPolSum->GetMaximum()-0.5,2);
    if( first_maxbin==last_maxbin ){
      fBestQBin=first_maxbin;
      bestQValue = fLegPolSum->GetYaxis()->GetBinCenter(fBestQBin);
      qBinWidth = fLegPolSum->GetYaxis()->GetBinWidth(fBestQBin);

    } else if( (last_maxbin-first_maxbin)<3 ){//value to optimize
      fBestQBin=(Int_t)((last_maxbin+first_maxbin)/2);
      bestQValue = (  fLegPolSum->GetYaxis()->GetBinCenter(first_maxbin)
		      +fLegPolSum->GetYaxis()->GetBinCenter(last_maxbin)  )*0.5;	
      qBinWidth = fLegPolSum->GetYaxis()->GetBinWidth(fBestQBin);

    } else {
      fLegPolSum->RebinY();
      qRebins++;
    }
  } while( fBestQBin==-1 );

  if(FootDebugLevel(2))
    cout<<"TABMLegendreFitter::FindLegendreBestValues:check on r done  fBestQBin="<<fBestQBin<<"   bincontent="<<fLegPolSum->GetBinContent(fBestMBin,fBestQBin)<<"  best m="<<bestMValue<<"  best q="<<bestQValue<<"   rebinm="<<mRebins<<"  rebinr="<<qRebins<<endl;

  
  //M values
  fBestMBin=-1;
  mRebins=0;

  do{
    first_maxbin = fLegPolSum->FindFirstBinAbove(fLegPolSum->GetMaximum()-0.5);
    last_maxbin = fLegPolSum->FindLastBinAbove(fLegPolSum->GetMaximum()-0.5);
    Int_t qmaxbin = fLegPolSum->FindFirstBinAbove(fLegPolSum->GetMaximum()-1,2);
    if( FootDebugLevel(3) )
      cout<<"First max="<<first_maxbin
	  <<" ("<< fLegPolSum->GetBinContent(first_maxbin,qmaxbin)<<") "
	  <<"Last max="<<last_maxbin
	  <<" ("<< fLegPolSum->GetBinContent(last_maxbin,qmaxbin)<<") "
	  <<" maxim:"<<fLegPolSum->GetMaximum()<<"  qmaxbin="<<qmaxbin<<"\n";
    if( first_maxbin==last_maxbin ){
      fBestMBin = first_maxbin;
      bestMValue = fLegPolSum->GetXaxis()->GetBinCenter(fBestMBin);
      mBinWidth = fLegPolSum->GetXaxis()->GetBinWidth(fBestMBin);
      if( FootDebugLevel(3) )
	cout<<" END: first=last BestMvalue="<<bestMValue<<" width="<<mBinWidth 
	    <<"\n";
      
    } else if( (last_maxbin-first_maxbin)<5    ){//value to optimize
      fBestMBin=(Int_t)((last_maxbin+first_maxbin)/2);
      bestMValue = (  fLegPolSum->GetXaxis()->GetBinCenter(first_maxbin)
		      +fLegPolSum->GetXaxis()->GetBinCenter(last_maxbin)  )*0.5;	
      mBinWidth = fLegPolSum->GetXaxis()->GetBinWidth(fBestMBin);
      if( FootDebugLevel(3) )
	cout<<" END: first=last+1 BestMvalue="<<bestMValue
	    <<" width="<<mBinWidth<<"\n";

    } else {
      if( FootDebugLevel(3) )      cout<<" Continuing... rebinning\n";

      fLegPolSum->RebinX();
      fLegPolSum->RebinY();
      mRebins++;
    }

  } while( fBestMBin==-1 );

  if(FootDebugLevel(3)){
    cout<<"TABMLegendreFitter::FindLegendreBestValues: check on m done, now I'll check on q  fBestMBin="<<fBestMBin<<endl;
  cout<<" Qrebins= "<<qRebins<<"   Mrebins= "<<mRebins<<"\n";
  }

}


//for Legendre polynomy tracking, set the associated hits
Int_t TABMLegendreFitter::CheckAssHits(const Float_t asshiterror, const Float_t minRerr) {
  //for Legendre polynomy tracking, set the associated hits

  if(FootDebugLevel(2)){
    cout<<"TABMLegendreFitter::CheckAssHits with fNowView="<<fNowView<<" start"<<endl;
    cout<<"first estimate of the track: slope="<<bestMValue<<"  Mbinwidth="<<mBinWidth
	<<"  Q="<<bestQValue<<"  qBinwidth="<<qBinWidth<<endl;
  }

  Int_t wireplane[6]={-1,-1,-1,-1,-1,-1};
  Int_t selectedHits=0;

  for(Int_t i=0;i<fpnturaw->GetHitsN();++i) {
    TABMhit* p_hit = fpnturaw->GetHit(i);

    if(p_hit->GetIsSelected()<=0 && p_hit->GetView()==fNowView){
      Double_t maxerror = (p_hit->GetSigma()*asshiterror > minRerr) ? p_hit->GetSigma()*asshiterror : minRerr;
      //~ if(mindist<0)
      //~ mindist=0;
      Float_t a0pos = (fNowView==0) ? p_hit->GetWirePos().Y() : p_hit->GetWirePos().X();
      Float_t z = p_hit->GetWirePos().Z();
      Float_t rDrift = p_hit->GetRdrift();
	  
      Float_t mvalue=bestMValue;
      Float_t sqm2p1=sqrt(mvalue*mvalue+1.);
      Float_t yvalue = a0pos -mvalue*z+rDrift*sqm2p1;
      Float_t yvaluemax = yvalue +maxerror*sqm2p1;
      Float_t yvaluemin = yvalue -maxerror*sqm2p1;
      Float_t resolution = std::max((Float_t)p_hit->GetSigma(), qBinWidth);
      Float_t legendreValue = bestQValue;
      Float_t residual=fabs(yvalue - legendreValue);

      if( yvaluemin<=legendreValue && legendreValue<=yvaluemax ){
        selectedHits += CheckPossibleHits(yvalue,residual,resolution,i, 
					  p_hit, wireplane);

      } else {

        yvalue = a0pos -mvalue*z -rDrift*sqm2p1;
        yvaluemin = yvalue -maxerror*sqm2p1;
        yvaluemax = yvalue +maxerror*sqm2p1;
        residual=fabs(yvalue - legendreValue);
        if( yvaluemin<=legendreValue && legendreValue<=yvaluemax ){
          selectedHits += CheckPossibleHits(yvalue,residual,resolution,i, 
					    p_hit, wireplane);

        }else{
          if(FootDebugLevel(4))
            cout<<"TABMLegendreFitter::CheckAssHits: HIT DISCHARGED: hitnum="<<i<<"  isFake="<<p_hit->GetIsFake()
		<<"  view="<<0<<"  yvalue="<<yvalue<<"  legvalue="<<legendreValue
		<<"  diff="<<residual<<"   resolution="<<resolution<<endl;
          p_hit->SetIsSelected(-1);
        }
      }
    }
  }

  if(FootDebugLevel(2))
    cout<<"TABMLegendreFitter::CheckAssHits: done"<<endl;

  return selectedHits;
}


//used in CheckAssHits
Int_t TABMLegendreFitter::CheckPossibleHits(Float_t yvalue, Float_t residual, Float_t resolution, const Int_t hitnum, 
					    TABMhit* p_hit, Int_t wireplane[]){  // IO params

  bool isSelected = false;
  TABMhit*   p_doublehit;
  Int_t plane = p_hit->GetPlane();
  
  if( wireplane[plane]==-1 ){  // plane without a hit
    if(FootDebugLevel(4))
      cout<<"TABMLegendreFitter::CheckAssHits: hit selected hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()
	  <<"  view="<<p_hit->GetView()<<"  cellid="<<p_hit->GetIdCell()<<"  yvalue="<<yvalue<<"  bestvalue="<<bestQValue
	  <<"  diff="<<residual<<"   pull="<<residual/resolution<<endl;
    wireplane[plane] = hitnum;
    p_hit->SetIsSelected(fTrackNum+1);
    p_hit->SetResidual(residual/resolution);
    isSelected=true;

  } else { // this plane already contains a hit...
	  
    p_doublehit = fpnturaw->GetHit(wireplane[plane]);
    if( p_doublehit->GetResidual() > residual/resolution ){
      // current hit is better!
      if(FootDebugLevel(4))
        cout<<"TABMactNtuTrack::CheckAssHits: hit replaced! selected= hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->GetView()<<"  cellid="<<p_hit->GetIdCell()<<"  yvalue="<<yvalue<<"  bestvalue="<<fLegPolSum->GetYaxis()->GetBinCenter(fBestQBin)<<"  diff="<<residual<<"   residual="<<residual/resolution<<"  replace the hit num="<<wireplane[p_hit->GetPlane()]<<endl;
      wireplane[plane] = hitnum;
      p_hit->SetIsSelected(fTrackNum+1);
      p_hit->SetResidual(residual/resolution);
      p_doublehit->SetIsSelected(-1);  // deselect previous hit

    } else {
      // provided hit is not good...
      p_hit->SetIsSelected(-1);
      if(FootDebugLevel(4))
        cout<<"TABMactNtuTrack::CheckAssHits: HIT DISCHARGED: hitnum="<<hitnum<<"  isFake="<<p_hit->GetIsFake()<<"  view="<<p_hit->GetView()<<"  yvalue="<<yvalue<<"  bestvalue="<<fLegPolSum->GetYaxis()->GetBinCenter(fBestQBin)<<"  diff="<<residual<<"   res="<<resolution<<endl;
    }
  }

  return (isSelected ? 1 : 0);
}



// adopted to save the legpolsum th2d
void TABMLegendreFitter::SaveLegpol(){

  TString tmp_tstring="legpolsum_";
  tmp_tstring+=gTAGroot->CurrentEventNumber();
  tmp_tstring+="_";
  tmp_tstring+=fTrackNum;
  tmp_tstring+=(fNowView==0) ? "_y_" : "_x_";
  tmp_tstring+=".root";
  fLegPolSum->SaveAs(tmp_tstring.Data());

  return;
}

