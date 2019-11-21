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
: iview(0),    ilayer(0),    icell(0), cellid(-1),
  rdrift(0.),    tdrift(0.), sigma(0)
{
  ichi2=999;
  A0.SetXYZ(0,0,0);
  Wvers.SetXYZ(0,0,0);
  realRdrift=100.;
  residual=100.;
  isSelected=false;
  isFake=-1;  
  cellid=-1;
}

TABMntuHit::TABMntuHit(Int_t iv, Int_t il, Int_t ic, Int_t id, Double_t r, Double_t t, Double_t s) {

  iview = iv;  
  ilayer = il;   
  icell = ic;
  cellid=id;  
  rdrift = r;   
  tdrift = t; 
  ichi2 = 999;
  A0.SetXYZ(0,0,0);
  Wvers.SetXYZ(0,0,0);
  realRdrift=100.;
  residual=100.;
  isSelected=false;
  isFake=-1;
  sigma=s;
  SetAW();
}

void TABMntuHit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

void TABMntuHit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;
   
   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
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




