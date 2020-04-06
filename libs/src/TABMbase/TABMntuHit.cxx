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
: fiView(0),    fiLayer(0),    fiCell(0), fiCellid(-1),
  frDrift(0.),    ftDrift(0.), fSigma(0)
{
  fChi2=999;
  fA0.SetXYZ(0,0,0);
  fWvers.SetXYZ(0,0,0);
  fRealrDrift=100.;
  fResidual=100.;
  fIsSelected=false;
  fIsFake=-1;  
  fiCellid=-1;
}

TABMntuHit::TABMntuHit(Int_t iv, Int_t il, Int_t ic, Int_t id, Double_t r, Double_t t, Double_t s) {

  fiView = iv;
  fiLayer = il;
  fiCell = ic;
  fiCellid=id;
  frDrift = r;
  ftDrift = t;
  fChi2 = 999;
  fA0.SetXYZ(0,0,0);
  fWvers.SetXYZ(0,0,0);
  fRealrDrift=100.;
  fResidual=100.;
  fIsSelected=false;
  fIsFake=-1;
  fSigma=s;
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

   fA0.SetXYZ(f_bmgeo->GetWireX(idfilo,Plane(),fiView), f_bmgeo->GetWireY(idfilo,Plane(),fiView), f_bmgeo->GetWireZ(idfilo,Plane(),fiView));
   fWvers.SetXYZ(f_bmgeo->GetWireCX(idfilo,Plane(),fiView), f_bmgeo->GetWireCY(idfilo,Plane(),fiView), f_bmgeo->GetWireCZ(idfilo,Plane(),fiView));
  
   if(fWvers.Mag()!=0.)
      fWvers.SetMag(1.);
   else{
      cout<<"Error in TABMntuHit constructor::AddWire-> ERROR fWvers.Mag()==0!!!"<<endl;
      return;
   }
  return;
}




