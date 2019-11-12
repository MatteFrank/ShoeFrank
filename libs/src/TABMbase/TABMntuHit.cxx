#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
#include "TABMparGeo.hxx"
#include "TABMntuHit.hxx"

using namespace std;

ClassImp(TABMntuHit);

//------------------------------------------+-----------------------------------
//! Destructor.

TABMntuHit::~TABMntuHit()
{

}

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMntuHit::TABMntuHit()
: iview(0),    ilayer(0),    icell(0), ichi2(999),
  rdrift(0.),    tdrift(0.)
{
  sigma = 0;
  A0.SetXYZ(0,0,0);
  Wvers.SetXYZ(0,0,0);
  realRdrift=100.;
  residual=100.;
  isSelected=false;
  isFake=false;  
}

TABMntuHit::TABMntuHit(Int_t iv, Int_t il, Int_t ic, Double_t r, Double_t t, Double_t s) {

  iview = iv;  ilayer = il;   icell = ic;  
  rdrift = r;   tdrift = t; 
  ichi2 = 999;
  A0.SetXYZ(0,0,0);
  Wvers.SetXYZ(0,0,0);
  realRdrift=100.;
  residual=100.;
  isSelected=false;
  isFake=false;
  sigma=s;

   SetAW();
}

void TABMntuHit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackId.Set(0);
}

void TABMntuHit:: AddMcTrackId(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackId.Set(fMcTrackId.GetSize()+1);
   fMcTrackId[fMcTrackId.GetSize()-1] = trackId;
}


void TABMntuHit::SetAW() {

   TABMparGeo* f_bmgeo = (TABMparGeo*) gTAGroot->FindParaDsc(TABMparGeo::GetDefParaName(), "TABMparGeo")->Object();

  Int_t idfilo = f_bmgeo->GetSenseId(Cell());

   A0.SetXYZ(f_bmgeo->GetWireX(idfilo,Plane(),iview), f_bmgeo->GetWireY(idfilo,Plane(),iview), f_bmgeo->GetWireZ(idfilo,Plane(),iview));
   Wvers.SetXYZ(f_bmgeo->GetWireCX(idfilo,Plane(),iview), f_bmgeo->GetWireCY(idfilo,Plane(),iview), f_bmgeo->GetWireCZ(idfilo,Plane(),iview));
  
   if(Wvers.Mag()!=0.)
      Wvers.SetMag(1.);
   else{
      cout<<"Error in TABMntuHit constructor::AddWire-> ERROR Wvers.Mag()==0!!!"<<endl;
      return;
   }
  return;
}

void TABMntuHit::SmearRdrift(Int_t smear_type){
   Double_t smeared;
   
   if(smear_type==0)
      return;
   
   if(smear_type==1){ //gaussian truncated to 1 sigma
      do{smeared=gRandom->Gaus(rdrift,sigma);}while(fabs(smeared-rdrift)>sigma);
   }
   
   if(smear_type==2){ //gaussian truncated to 2 sigma
      do{smeared=gRandom->Gaus(rdrift,sigma);}while(fabs(smeared-rdrift)>2.*sigma);
   }
   
   if(smear_type==3){ //gaussian truncated to 3 sigma
      do{smeared=gRandom->Gaus(rdrift,sigma);}while(fabs(smeared-rdrift)>3.*sigma);
   }
   
   if(smear_type==4) //gaussian not truncated
      smeared=gRandom->Gaus(rdrift,sigma);
   
   
   if(smear_type==5) //flat smearing
      smeared=rdrift+gRandom->Uniform(-sigma*sqrt(12.)/2.,sigma*sqrt(12.)/2.);
   
   
   if (smeared<0)
      smeared=0.;
   rdrift=smeared;
   return;  
} 


