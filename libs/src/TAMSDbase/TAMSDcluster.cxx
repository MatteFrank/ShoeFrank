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
Float_t TAMSDcluster::ComputeEtaChargeCorrection(Float_t eta) {

   //TODO: template for charge loss correction wrt eta, will need to use parameters loaded from "TAMSD_Energy_Calibration.cal"
   Float_t correction = 1.0;

   if(eta == 0 || eta == 1)
      return correction;


   TF1 *fa = new TF1("fa","[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)+[6]*pow(x,6)",0,1);
   fa->SetParameter(0,2458.8);
   fa->SetParameter(1,606.9);
   fa->SetParameter(2,-59574.6);
   fa->SetParameter(3,276349);
   fa->SetParameter(4,-533689);
   fa->SetParameter(5,474078);
   fa->SetParameter(6,-157766);

   correction = fa->Eval(eta)/fa->Eval(1);

   return correction;
}

//______________________________________________________________________________
//
Float_t TAMSDcluster::ComputeEtaPosCorrection(Float_t eta) {

   //TODO: template for hit position correction wrt eta, will need to use parameters loaded from "TAMSD_Energy_Calibration.cal"
   Float_t correction = 0.0;

   TF1 *f1 = new TF1("f1","[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)+[6]*pow(x,6)+[7]*pow(x,7)+[8]*pow(x,8)",0,0.33);
   f1->SetParameter(0,-1.64509e-05);    
   f1->SetParameter(1,0.00764971);     
   f1->SetParameter(2,-0.820312);     
   f1->SetParameter(3,30.5879);     
   f1->SetParameter(4,-416.444);     
   f1->SetParameter(5,2846.94);     
   f1->SetParameter(6,-10502.1);     
   f1->SetParameter(7,19985.3); 
   f1->SetParameter(8,-15388.5); 

   TF1 *f2 = new TF1("f2","[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)",0.33,0.66);
   f2->SetParameter(0,-0.102357);     
   f2->SetParameter(1,1.04854);     
   f2->SetParameter(2,-4.12363);     
   f2->SetParameter(3,8.22154);     
   f2->SetParameter(4,-8.23574);     
   f2->SetParameter(5,3.31505);  
   
   TF1 *f3 = new TF1("f3","[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)+[4]*pow(x,4)+[5]*pow(x,5)",0.66,1);
   f3->SetParameter(0,2.60998);
   f3->SetParameter(1,-17.2915);
   f3->SetParameter(2,45.2007);
   f3->SetParameter(3,-58.2217);
   f3->SetParameter(4,37.0426);
   f3->SetParameter(5,-9.32504);


   if(eta < 0.33)
   {
      correction = f1->Eval(eta);
   } else if (eta >= 0.33 && eta <= 0.66)
   {
      correction = f2->Eval(eta);
   } else 
   {
      correction = f3->Eval(eta);
   }

   return correction;
}


Float_t TAMSDcluster::ComputeAddress(TClonesArray* fListOfStrips) {

   TAMSDhit* strip = (TAMSDhit*)fListOfStrips->At(0);
   return strip->GetPosition();
}


