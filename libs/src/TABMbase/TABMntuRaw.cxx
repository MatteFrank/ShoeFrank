/*!
  \file
  \version $Id: TABMntuRaw.cxx,v 1.12 2003/06/09 18:41:17 mueller Exp $
  \brief   Implementation of TABMntuRaw.
*/

#include "TABMntuRaw.hxx"

using namespace std;

/*!
  \class TABMntuRaw TABMntuRaw.hxx "TABMntuRaw.hxx"
  \brief Mapping and Geometry parameters for Tof wall. **
*/


//##############################################################################

ClassImp(TABMntuRaw);

TString TABMntuRaw::fgkBranchName   = "bmrh.";

//------------------------------------------+-----------------------------------
//! Default constructor.

TABMntuRaw::TABMntuRaw() :
  fListOfHits(0x0) {
    fEffPaoloni=-3;
    fEffPaolonixview=-3;
    fEffPaoloniyview=-3;
    fNselhitx=0;
    fNselhity=0;  
  }

//------------------------------------------+-----------------------------------
//! Destructor.

TABMntuRaw::~TABMntuRaw() {
  delete fListOfHits;
}

//------------------------------------------+-----------------------------------
// ! Get number of hits
Int_t TABMntuRaw::GetHitsN() const
{
   return fListOfHits->GetEntries();
}


//______________________________________________________________________________
//! new hit

TABMntuHit* TABMntuRaw::NewHit(Int_t iv, Int_t il, Int_t ic, Int_t id, Double_t r, Double_t t, Double_t s)
{
   TClonesArray &pixelArray = *fListOfHits;
   
   TABMntuHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TABMntuHit(iv, il, ic, id, r, t,  s);
   
   return hit;
   
}

//------------------------------------------+-----------------------------------
//! Setup clones.

void TABMntuRaw::SetupClones()
{
  if (!fListOfHits) fListOfHits = new TClonesArray("TABMntuHit");
  return;
}

//------------------------------------------+-----------------------------------
//! Clear event.

void TABMntuRaw::Clear(Option_t*)
{
  if (fListOfHits)
    fListOfHits->Clear("C");
    
    fEffPaoloni=-3;
    fEffPaolonixview=-3;
    fEffPaoloniyview=-3;
    fNselhitx=0;
    fNselhity=0;
  return;
}

/*------------------------------------------+---------------------------------*/
//! helper to format one converter value

static void print_value(ostream& os, Int_t i_val)
{
  char c_ran = (i_val & 0x1000) ? 'h' : 'l';
  os << Form("%4d %s %d", i_val & 0x0fff, "d", 4) << " " << c_ran;
  return;
}

/*------------------------------------------+---------------------------------*/
//! ostream insertion.

void TABMntuRaw::ToStream(ostream& os, Option_t* option) const
{
  os << "TABMntuRaw " << GetName()
     << Form("  nhit=%3d", fListOfHits->GetEntries())
     << endl;
  
  os << "slat stat    adct    adcb    tdct    tdcb" << endl;
  for (Int_t i = 0; i < fListOfHits->GetEntries(); i++) {
    const TABMntuHit*  hit = GetHit(i);
    os << Form("%4d", hit->Cell());
    os << "  "; print_value(os, hit->Plane());
    os << endl;
  }
  return;
}

void TABMntuRaw::ClearCellOccupy(){
  memset(fCellOccupy, 0, sizeof(fCellOccupy));
return;
}

Bool_t TABMntuRaw::AddCellOccupyHit(Int_t pos){
  if(pos<36 && pos>=0){  
    fCellOccupy[pos]++;
    return kTRUE;
  }else
    cout<<"ERROR in TABMntuRaw::AddCellOccupyHit: cellid of the hit is wrong: pos="<<pos<<endl;
  return kFALSE;
}

Bool_t TABMntuRaw::RemoveCellOccupyHit(Int_t pos){
  if(pos<36 && pos>=0){
    if(fCellOccupy[pos]>0){
      fCellOccupy[pos]--;
      return kTRUE;
    }else
      cout<<"ERROR in TABMntuRaw::RemoveCellOccupyHit: remove an empty hit! pos="<<pos<<endl;
  }else
    cout<<"ERROR in TABMntuRaw::RemoveCellOccupyHit: cellid of the hit is wrong: pos="<<pos<<endl;

int provv;
cin>>provv;

return kFALSE;
}

void TABMntuRaw::Efficiency_paoloni(Int_t pivot[], Int_t probe[], Double_t &efftot, Double_t &effxview, Double_t &effyview){

  //xview
  if(fCellOccupy[0]>0 && fCellOccupy[12]>0 && fCellOccupy[24]>0){
    pivot[0]++;
    if((fCellOccupy[7]>0 || fCellOccupy[6]>0) && (fCellOccupy[19]>0 || fCellOccupy[18]>0))
      probe[0]++;
  }
  if(fCellOccupy[1]>0 && fCellOccupy[13]>0 && fCellOccupy[25]>0){
    pivot[1]++;
    if((fCellOccupy[8]>0 || fCellOccupy[7]>0) && (fCellOccupy[19]>0 || fCellOccupy[20]>0))
      probe[1]++;
  }
  if(fCellOccupy[7]>0 && fCellOccupy[19]>0 && fCellOccupy[31]>0){
    pivot[2]++;
    if((fCellOccupy[12]>0 || fCellOccupy[13]>0) && (fCellOccupy[24]>0 || fCellOccupy[25]>0))
      probe[2]++;
  }
  if(fCellOccupy[8]>0 && fCellOccupy[20]>0 && fCellOccupy[32]>0){
    pivot[3]++;
    if((fCellOccupy[13]>0 || fCellOccupy[14]>0) && (fCellOccupy[25]>0 || fCellOccupy[26]>0))
      probe[3]++;
  }
  
  //yview
  if(fCellOccupy[4]>0 && fCellOccupy[16]>0 && fCellOccupy[28]>0){
    pivot[4]++;
    if((fCellOccupy[10]>0 || fCellOccupy[19]>0) && (fCellOccupy[22]>0 || fCellOccupy[21]>0))
      probe[4]++;
  }
  if(fCellOccupy[5]>0 && fCellOccupy[17]>0 && fCellOccupy[29]>0){
    pivot[5]++;
    if((fCellOccupy[10]>0 || fCellOccupy[11]>0) && (fCellOccupy[22]>0 || fCellOccupy[23]>0))
      probe[5]++;
  }
  if(fCellOccupy[9]>0 && fCellOccupy[21]>0 && fCellOccupy[33]>0){
    pivot[6]++;
    if((fCellOccupy[15]>0 || fCellOccupy[16]>0) && (fCellOccupy[27]>0 || fCellOccupy[28]>0))
      probe[6]++;
  }
  if(fCellOccupy[10]>0 && fCellOccupy[22]>0 && fCellOccupy[34]>0){
    pivot[7]++;
    if((fCellOccupy[16]>0 || fCellOccupy[17]>0) && (fCellOccupy[28]>0 || fCellOccupy[29]>0))
      probe[7]++;
  }
  
  Int_t total_probesxview=0, total_pivotsxview=0, total_probesyview=0, total_pivotsyview=0;
  for(Int_t i=0;i<4;i++){
    total_probesxview+=probe[i];
    total_pivotsxview+=pivot[i];
    total_probesyview+=probe[i+4];
    total_pivotsyview+=pivot[i+4];
  }
  efftot= ((total_pivotsxview+total_pivotsyview)==0) ?  -1 : ((Double_t) (total_probesxview+total_probesyview))/(total_pivotsxview+total_pivotsyview);
  effxview=(total_pivotsxview==0) ? -1: ((Double_t) total_probesxview)/total_pivotsxview;
  effyview=(total_pivotsyview==0) ? -1: ((Double_t) total_probesyview)/total_pivotsyview;
  
return;
}

void TABMntuRaw::SetEfficiency(Double_t efftot, Double_t effxview, Double_t effyview){
  fEffPaoloni=efftot;
  fEffPaolonixview=effxview;
  fEffPaoloniyview=effyview;
return;
}

void TABMntuRaw::PrintCellOccupy(){
  
  cout<<"TABMactNtuRaw::evaluate_fCellOccupy: print fCellOccupy"<<endl;
  for(Int_t i=0;i<36;i++)
    cout<<fCellOccupy[i]<<" ";
  cout<<endl;

return;  
}



