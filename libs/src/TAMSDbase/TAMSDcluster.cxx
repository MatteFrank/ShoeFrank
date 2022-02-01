////////////////////////////////////////////////////////////
//                                                        //
// Class Description of TAMSDcluster                       //
//                                                        //
////////////////////////////////////////////////////////////

#include "TAMSDcluster.hxx"
#include "TAMSDhit.hxx"
#include "TAMSDparGeo.hxx"
#include "TAGgeoTrafo.hxx"
#include "TF1.h"

#include <math.h>

ClassImp(TAMSDcluster) // Description of a cluster

//______________________________________________________________________________
//  
TAMSDcluster::TAMSDcluster()
:  TAGcluster(),
   fPositionF(0.),
   fPosErrorF(0),
   fCurPosition(0,0,0),
   fPlaneView(-1),
   fEnergyLoss(0)
{
   // TAMSDcluster constructor
   SetupClones();
}

//______________________________________________________________________________
//
void TAMSDcluster::SetupClones()
{
   fListOfStrips = new TClonesArray("TAMSDhit");
   fListOfStrips->SetOwner(true);
}

//______________________________________________________________________________
//  
TAMSDcluster::TAMSDcluster(const TAMSDcluster& cluster)
:  TAGcluster(cluster),
   fPositionF(cluster.fPositionF),
   fPosErrorF(cluster.fPosErrorF),
   fCurPosition(cluster.fCurPosition),
   fPlaneView(cluster.fPlaneView),
   fEnergyLoss(cluster.fEnergyLoss)
{
   fListOfStrips = (TClonesArray*)cluster.fListOfStrips->Clone();
}

//______________________________________________________________________________
//  
TAMSDcluster::~TAMSDcluster()
{ 
   // TAMSDcluster default destructor
   delete fListOfStrips;
}


//______________________________________________________________________________
//  
void TAMSDcluster::AddStrip(TAMSDhit* strip)
{
   for (Int_t k = 0; k < strip->GetMcTracksN(); ++k) {
      Int_t idx = strip->GetMcTrackIdx(k);
      AddMcTrackIdx(idx);
   }
   
   TClonesArray &StripArray = *fListOfStrips;
   new(StripArray[StripArray.GetEntriesFast()]) TAMSDhit(*strip);
   
   fElementsN = fListOfStrips->GetEntries();
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPositionG(TVector3& posGlo)
{
   fPosition2.SetXYZ(posGlo.X(), posGlo.Y(), posGlo.Z());
   
   if (GetPlaneView() == 0)
      fPosError2.SetXYZ(fPosErrorF, 0, 0.01);
   else
      fPosError2.SetXYZ(0, fPosErrorF, 0.01);
}

//______________________________________________________________________________
//
TAMSDhit* TAMSDcluster::GetStrip(Int_t idx)
{
   if (idx >=0 && idx < fListOfStrips->GetEntries())
      return (TAMSDhit*)fListOfStrips->At(idx);
   else
      return 0x0;
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPositionF(Float_t pos)
{
   fPositionF = pos;
   if (GetPlaneView() == 0)
      fCurPosition.SetXYZ(fPositionF, 0, 0);
   else
      fCurPosition.SetXYZ(0, fPositionF, 0);
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPosErrorF(Float_t pos)
{
   fPosErrorF = pos;
}

//______________________________________________________________________________
//
void TAMSDcluster::SetCog(Float_t pos)
{
   fCog = pos;
}

//______________________________________________________________________________
//
void TAMSDcluster::SetPlaneView(Int_t v)
{
   fPlaneView = v;
   fDeviceType = TAGgeoTrafo::GetDeviceType(TAMSDparGeo::GetBaseName()) + fPlaneView;
}

//______________________________________________________________________________
//
Float_t TAMSDcluster::Distance(TAMSDcluster *aClus) {
   // Return the distance between this clusters and the pointed cluster
   // regardless of the plane
   
   TVector3 clusPosition( aClus->GetPositionG() );
   
   // Now compute the distance beetween the two hits
   clusPosition -= (GetPositionG());
   
   // Insure that z position is 0 for 2D length computation
   clusPosition.SetXYZ( clusPosition[0], clusPosition[1], 0.);
   
   return clusPosition.Mag();
}

//______________________________________________________________________________
//
Float_t TAMSDcluster::ComputeEtaFast(Float_t cog) {

   Double_t fractpart, intpart;
   fractpart = modf (cog , &intpart);

   return fractpart;
}

Float_t TAMSDcluster::ComputeEta(TClonesArray* fListOfStrips) {

   Int_t nstrips = fListOfStrips->GetEntries();

   if (nstrips == 1)
      return 1.0;

   Float_t eta;
   TAMSDhit* strip;
   Float_t max_adc = -1;
   Int_t max_pos = 0;

   //cout << "Cluster has " << fListOfStrips->GetEntries() << " strips" << endl;
   for (int i = 0; i < nstrips; i++)
   {  
      strip = (TAMSDhit*)fListOfStrips->At(i);
      // cout << "\tStrip " << i << " has value " << strip->GetEnergyLoss() << endl;

      if(strip->GetEnergyLoss() > max_adc)
      {
         max_adc = strip->GetEnergyLoss();
         max_pos = i;
      }
   }

   if (max_pos == 0)
   {
      eta = ((TAMSDhit*)fListOfStrips->At(0))->GetEnergyLoss() / ( ((TAMSDhit*)fListOfStrips->At(0))->GetEnergyLoss() + ((TAMSDhit*)fListOfStrips->At(1))->GetEnergyLoss() );
   } 
   else if(max_pos == nstrips - 1)
   {
      eta = ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() / ( ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() + ((TAMSDhit*)fListOfStrips->At(max_pos))->GetEnergyLoss() );
   } 
   else
   {
      if( ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() >  ((TAMSDhit*)fListOfStrips->At(max_pos + 1))->GetEnergyLoss() )
      {
         eta = ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() / ( ((TAMSDhit*)fListOfStrips->At(max_pos - 1))->GetEnergyLoss() + ((TAMSDhit*)fListOfStrips->At(max_pos))->GetEnergyLoss() );
      }
      else
      {
         eta = ((TAMSDhit*)fListOfStrips->At(max_pos))->GetEnergyLoss() / ( ((TAMSDhit*)fListOfStrips->At(max_pos))->GetEnergyLoss() + ((TAMSDhit*)fListOfStrips->At(max_pos + 1))->GetEnergyLoss() );
      }
   }

   return eta;
}

//______________________________________________________________________________
//
Float_t TAMSDcluster::ComputeEtaCorrection(Float_t eta) {

   Float_t correction = 1.0;

   if(eta == 0 || eta == 1)
      return correction;


   TF1 *fa = new TF1("fa","[0]+[1]*x+[2]*x*x+[3]*x*x*x",0.2,0.5);
   fa->SetParameter(0,1961.03);
   fa->SetParameter(1,-7429.58);
   fa->SetParameter(2,27733.4);
   fa->SetParameter(3,-19994.8);

   if (eta >= 0.5)
   {  
      if(eta>0.66)
         correction = fa->Eval(eta-0.5)/fa->Eval(0.5)/0.9;
      
      correction  = 1410.04/fa->Eval(0.5)/0.9;
   }
   else
   {
      if(eta >0.33)
         correction = fa->Eval(-eta+0.5)/fa->Eval(0.5)/0.9;

      correction  = 1410.04/fa->Eval(0.5)/0.9;  
   }

   return correction;
}


Float_t TAMSDcluster::ComputeAddress(TClonesArray* fListOfStrips) {

   TAMSDhit* strip = (TAMSDhit*)fListOfStrips->At(0);
   return strip->GetPosition();
}


