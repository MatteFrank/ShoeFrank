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
  eff_paoloni=-3;  
  eff_paolonixview=-3;  
  eff_paoloniyview=-3;  
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

TABMntuHit* TABMntuRaw::NewHit(Int_t iv, Int_t il, Int_t ic, Double_t r, Double_t t, Double_t s)
{
   TClonesArray &pixelArray = *fListOfHits;
   
   TABMntuHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TABMntuHit(iv, il, ic, r, t,  s);
   
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
    const TABMntuHit*  hit = Hit(i);
    os << Form("%4d", hit->Cell());
    os << "  "; print_value(os, hit->Plane());
    os << endl;
  }
  return;
}

void TABMntuRaw::ClearCellOccupy(){
  memset(cell_occupy, 0, sizeof(cell_occupy));
return;
}

Bool_t TABMntuRaw::AddCellOccupyHit(Int_t pos){
  if(pos<36 && pos>=0){  
    cell_occupy[pos]++;
    return kTRUE;
  }else
    cout<<"ERROR in TABMntuRaw::AddCellOccupyHit: cell id of the hit is wrong: pos="<<pos<<endl;
  return kFALSE;
}


void TABMntuRaw::Efficiency_paoloni(Int_t pivot[], Int_t probe[]){

  //xview
  if(cell_occupy[0]>0 && cell_occupy[12]>0 && cell_occupy[24]>0){
    pivot[0]++;
    if((cell_occupy[7]>0 || cell_occupy[6]>0) && (cell_occupy[19]>0 || cell_occupy[18]>0))
      probe[0]++;
  }
  if(cell_occupy[1]>0 && cell_occupy[13]>0 && cell_occupy[25]>0){
    pivot[1]++;
    if((cell_occupy[8]>0 || cell_occupy[7]>0) && (cell_occupy[19]>0 || cell_occupy[20]>0))
      probe[1]++;
  }
  if(cell_occupy[7]>0 && cell_occupy[19]>0 && cell_occupy[31]>0){
    pivot[2]++;
    if((cell_occupy[12]>0 || cell_occupy[13]>0) && (cell_occupy[24]>0 || cell_occupy[25]>0))
      probe[2]++;
  }
  if(cell_occupy[8]>0 && cell_occupy[20]>0 && cell_occupy[32]>0){
    pivot[3]++;
    if((cell_occupy[13]>0 || cell_occupy[14]>0) && (cell_occupy[25]>0 || cell_occupy[26]>0))
      probe[3]++;
  }
  
  //yview
  if(cell_occupy[4]>0 && cell_occupy[16]>0 && cell_occupy[28]>0){
    pivot[4]++;
    if((cell_occupy[10]>0 || cell_occupy[19]>0) && (cell_occupy[22]>0 || cell_occupy[21]>0))
      probe[4]++;
  }
  if(cell_occupy[5]>0 && cell_occupy[17]>0 && cell_occupy[29]>0){
    pivot[5]++;
    if((cell_occupy[10]>0 || cell_occupy[11]>0) && (cell_occupy[22]>0 || cell_occupy[23]>0))
      probe[5]++;
  }
  if(cell_occupy[9]>0 && cell_occupy[21]>0 && cell_occupy[33]>0){
    pivot[6]++;
    if((cell_occupy[15]>0 || cell_occupy[16]>0) && (cell_occupy[27]>0 || cell_occupy[28]>0))
      probe[6]++;
  }
  if(cell_occupy[10]>0 && cell_occupy[22]>0 && cell_occupy[34]>0){
    pivot[7]++;
    if((cell_occupy[16]>0 || cell_occupy[17]>0) && (cell_occupy[28]>0 || cell_occupy[29]>0))
      probe[7]++;
  }
  
  Int_t total_probesxview=0, total_pivotsxview=0, total_probesyview=0, total_pivotsyview=0;
  for(Int_t i=0;i<4;i++){
    total_probesxview+=probe[i];
    total_pivotsxview+=pivot[i];
    total_probesyview+=probe[i+4];
    total_pivotsyview+=pivot[i+4];
  }
  eff_paoloni= ((total_pivotsxview+total_pivotsyview)==0) ?  -1 : ((Double_t) (total_probesxview+total_probesyview))/(total_pivotsxview+total_pivotsyview);
  eff_paolonixview=(total_pivotsxview==0) ? -1: ((Double_t) total_probesxview)/total_pivotsxview;
  eff_paoloniyview=(total_pivotsyview==0) ? -1: ((Double_t) total_probesyview)/total_pivotsyview;
  
return;
}

void TABMntuRaw::PrintCellOccupy(){
  
  cout<<"TABMactNtuRaw::evaluate_cell_occupy: print cell_occupy"<<endl;
  for(Int_t i=0;i<36;i++)
    cout<<cell_occupy[i]<<" ";
  cout<<endl;

return;  
}



