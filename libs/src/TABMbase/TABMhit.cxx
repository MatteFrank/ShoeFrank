#include "TAGroot.hxx"
#include "TAGparaDsc.hxx"
#include "TABMparGeo.hxx"
#include "TABMhit.hxx"

using namespace std;

ClassImp(TABMhit);

//------------------------------------------+-----------------------------------
//! Destructor.

TABMhit::~TABMhit()
{

}

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMhit::TABMhit()
: fIdCell(-1), fPlane(0), fView(0), fCell(0),
  fRdrift(0.),    fTdrift(0.), fSigma(0)
{
  fChi2=999;
  fWirePos.SetXYZ(0,0,0);
  fWireDir.SetXYZ(0,0,0);
  fResidual=100.;
  fIsSelected=0;
  fIsFake=-1;
  fIdCell=-1;
}

TABMhit::TABMhit( Int_t id, Int_t il, Int_t iv, Int_t ic, Double_t r, Double_t t, Double_t s) {

  fIdCell=id;
  fPlane = il;
  fView = iv;
  fCell = ic;
  fRdrift = r;
  fTdrift = t;
  fChi2 = 999;
  fResidual=100.;
  fIsSelected=0;
  fIsFake=-1;
  fSigma=s;

  TABMparGeo* f_bmgeo = (TABMparGeo*) gTAGroot->FindParaDsc(TABMparGeo::GetDefParaName(), "TABMparGeo")->Object();
  Int_t idfilo = f_bmgeo->GetSenseId(fCell);
  fWirePos.SetXYZ(f_bmgeo->GetWireX(idfilo,fPlane,fView), f_bmgeo->GetWireY(idfilo,fPlane,fView), f_bmgeo->GetWireZ(idfilo,fPlane,fView));
  fWireDir.SetXYZ(f_bmgeo->GetWireCX(idfilo,fPlane,fView), f_bmgeo->GetWireCY(idfilo,fPlane,fView), f_bmgeo->GetWireCZ(idfilo,fPlane,fView));
  if(fWireDir.Mag()!=0.)
     fWireDir.SetMag(1.);
  else
     cout<<"Error in TABMhit constructor::AddWire-> ERROR fWireDir.Mag()==0!!!"<<endl;
}

void TABMhit::Clear(Option_t* /*option*/)
{
   fMCindex.Set(0);
   fMcTrackIdx.Set(0);
}

void TABMhit:: AddMcTrackIdx(Int_t trackId, Int_t mcId)
{
   fMCindex.Set(fMCindex.GetSize()+1);
   fMCindex[fMCindex.GetSize()-1]   = mcId;

   fMcTrackIdx.Set(fMcTrackIdx.GetSize()+1);
   fMcTrackIdx[fMcTrackIdx.GetSize()-1] = trackId;
}
/*
void TABMhit::SetAW() {

   TABMparGeo* f_bmgeo = (TABMparGeo*) gTAGroot->FindParaDsc(TABMparGeo::GetDefParaName(), "TABMparGeo")->Object();

  Int_t idfilo = f_bmgeo->GetSenseId(Cell());

   fA0.SetXYZ(f_bmgeo->GetWireX(idfilo,Plane(),fiView), f_bmgeo->GetWireY(idfilo,Plane(),fiView), f_bmgeo->GetWireZ(idfilo,Plane(),fiView));
   fWvers.SetXYZ(f_bmgeo->GetWireCX(idfilo,Plane(),fiView), f_bmgeo->GetWireCY(idfilo,Plane(),fiView), f_bmgeo->GetWireCZ(idfilo,Plane(),fiView));

   if(fWvers.Mag()!=0.)
      fWvers.SetMag(1.);
   else{
      cout<<"Error in TABMhit constructor::AddWire-> ERROR fWvers.Mag()==0!!!"<<endl;
      return;
   }
  return;
}

*/
