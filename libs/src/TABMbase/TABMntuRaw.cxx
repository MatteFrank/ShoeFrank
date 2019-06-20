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
  cell_occupy.clear();
  for(Int_t i=0;i<36;i++)
    cell_occupy.push_back(0);
return;
}

Bool_t TABMntuRaw::AddCellOccupyHit(Int_t pos){
  if(pos<36 && pos>=0){  
    cell_occupy.at(pos)++;
    return kTRUE;
  }else
    cout<<"ERROR in TABMntuRaw::AddCellOccupyHit: cell id of the hit is wrong: pos="<<pos<<endl;
  return kFALSE;
}


void TABMntuRaw::Efficiency_paoloni(vector<Int_t> &pivot, vector<Int_t> &probe){

  if(cell_occupy.size()!=36){
    eff_paoloni=-2;
    eff_paolonixview=-2;
    eff_paoloniyview=-2;
    return;
  }

  //xview
  if(cell_occupy.at(0)>0 && cell_occupy.at(12)>0 && cell_occupy.at(24)>0){
    pivot.at(0)++;
    if((cell_occupy.at(7)>0 || cell_occupy.at(6)>0) && (cell_occupy.at(19)>0 || cell_occupy.at(18)>0))
      probe.at(0)++;
  }
  if(cell_occupy.at(1)>0 && cell_occupy.at(13)>0 && cell_occupy.at(25)>0){
    pivot.at(1)++;
    if((cell_occupy.at(8)>0 || cell_occupy.at(7)>0) && (cell_occupy.at(19)>0 || cell_occupy.at(20)>0))
      probe.at(1)++;
  }
  if(cell_occupy.at(7)>0 && cell_occupy.at(19)>0 && cell_occupy.at(31)>0){
    pivot.at(2)++;
    if((cell_occupy.at(12)>0 || cell_occupy.at(13)>0) && (cell_occupy.at(24)>0 || cell_occupy.at(25)>0))
      probe.at(2)++;
  }
  if(cell_occupy.at(8)>0 && cell_occupy.at(20)>0 && cell_occupy.at(32)>0){
    pivot.at(3)++;
    if((cell_occupy.at(13)>0 || cell_occupy.at(14)>0) && (cell_occupy.at(25)>0 || cell_occupy.at(26)>0))
      probe.at(3)++;
  }
  
  //yview
  if(cell_occupy.at(4)>0 && cell_occupy.at(16)>0 && cell_occupy.at(28)>0){
    pivot.at(4)++;
    if((cell_occupy.at(10)>0 || cell_occupy.at(19)>0) && (cell_occupy.at(22)>0 || cell_occupy.at(21)>0))
      probe.at(4)++;
  }
  if(cell_occupy.at(5)>0 && cell_occupy.at(17)>0 && cell_occupy.at(29)>0){
    pivot.at(5)++;
    if((cell_occupy.at(10)>0 || cell_occupy.at(11)>0) && (cell_occupy.at(22)>0 || cell_occupy.at(23)>0))
      probe.at(5)++;
  }
  if(cell_occupy.at(9)>0 && cell_occupy.at(21)>0 && cell_occupy.at(33)>0){
    pivot.at(6)++;
    if((cell_occupy.at(15)>0 || cell_occupy.at(16)>0) && (cell_occupy.at(27)>0 || cell_occupy.at(28)>0))
      probe.at(6)++;
  }
  if(cell_occupy.at(10)>0 && cell_occupy.at(22)>0 && cell_occupy.at(34)>0){
    pivot.at(7)++;
    if((cell_occupy.at(16)>0 || cell_occupy.at(17)>0) && (cell_occupy.at(28)>0 || cell_occupy.at(29)>0))
      probe.at(7)++;
  }
  
  Int_t total_probesxview=0, total_pivotsxview=0, total_probesyview=0, total_pivotsyview=0;
  for(Int_t i=0;i<4;i++){
    total_probesxview+=probe.at(i);
    total_pivotsxview+=pivot.at(i);
    total_probesyview+=probe.at(i+4);
    total_pivotsyview+=pivot.at(i+4);
  }
  eff_paoloni= ((total_pivotsxview+total_pivotsyview)==0) ?  -1 : ((Double_t) (total_probesxview+total_probesyview))/(total_pivotsxview+total_pivotsyview);
  eff_paolonixview=(total_pivotsxview==0) ? -1: ((Double_t) total_probesxview)/total_pivotsxview;
  eff_paoloniyview=(total_pivotsyview==0) ? -1: ((Double_t) total_probesyview)/total_pivotsyview;
  
return;
}

void TABMntuRaw::PrintCellOccupy(){
  
  if(cell_occupy.size()==0)
    return;
  else if(cell_occupy.size()==36){
    cout<<"TABMactNtuRaw::evaluate_cell_occupy: print cell_occupy"<<endl;
    for(Int_t i=0;i<cell_occupy.size();i++)
      cout<<cell_occupy.at(i)<<" ";
    cout<<endl;
  }
  else
    cout<<"ERROR in TABMactNtuRaw::cell_occupy.size()!=0 || cell_occupy.size()!=36"<<endl;
return;  
}



