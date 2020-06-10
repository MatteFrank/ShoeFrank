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
    fEffPaoloniXview=-3;
    fEffPaoloniYview=-3;
    fNselhitX=0;
    fNselhitY=0;
    fNtothitX=0;
    fNtothitY=0;
  }

//------------------------------------------+-----------------------------------
//! Destructor.

TABMntuRaw::~TABMntuRaw() {
  delete fListOfHits;
}


//______________________________________________________________________________
//! new hit

TABMntuHit* TABMntuRaw::NewHit(Int_t iv, Int_t il, Int_t ic, Int_t id, Double_t r, Double_t t, Double_t s)
{
  TClonesArray &pixelArray = *fListOfHits;
  TABMntuHit* hit = new(pixelArray[pixelArray.GetEntriesFast()]) TABMntuHit(iv, il, ic, id, r, t,  s);
  if(iv==0)
    ++fNtothitY;
  else
    ++fNtothitX;
  fCellOccMap[id]++;
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

    fCellOccMap.clear();
    fEffPaoloni=-3;
    fEffPaoloniXview=-3;
    fEffPaoloniYview=-3;
    fNselhitX=0;
    fNselhitY=0;
    fNtothitX=0;
    fNtothitY=0;
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
    os << Form("%4d", hit->GetCell());
    os << "  "; print_value(os, hit->GetPlane());
    os << endl;
  }
  return;
}

void TABMntuRaw::ClearCellOccupy(){
  fCellOccMap.clear();
  return;
}

Int_t TABMntuRaw::GetCellOccupy(Int_t pos){
   std::map<Int_t,Int_t>::iterator it=fCellOccMap.find(pos);
  if(it==fCellOccMap.end())
    return 0;
  return it->second;
}

void TABMntuRaw::PrintCellOccupy(){

  cout<<"TABMactNtuRaw::fCellOccMap: print fCellOccMap"<<endl;
  map<Int_t, Int_t>::iterator it;
  for (auto const& x : fCellOccMap)
    cout<<"cellid="<<x.first<<"    number of hits="<<x.second<<endl;

return;
}

void TABMntuRaw::Efficiency_paoloni(Int_t pivot[], Int_t probe[]){

  //yzview
  if(fCellOccMap.find(0)!=fCellOccMap.end() && fCellOccMap.find(12)!=fCellOccMap.end() && fCellOccMap.find(24)!=fCellOccMap.end()){
    pivot[0]+=2;
    if(fCellOccMap.find(7)!=fCellOccMap.end() || fCellOccMap.find(6)!=fCellOccMap.end())
      probe[0]++;
    if(fCellOccMap.find(19)!=fCellOccMap.end() || fCellOccMap.find(18)!=fCellOccMap.end())
      probe[0]++;
  }
  if(fCellOccMap.find(1)!=fCellOccMap.end() && fCellOccMap.find(13)!=fCellOccMap.end() && fCellOccMap.find(25)!=fCellOccMap.end()){
    pivot[1]+=2;
    if(fCellOccMap.find(8)!=fCellOccMap.end() || fCellOccMap.find(7)!=fCellOccMap.end())
      probe[1]++;
    if(fCellOccMap.find(19)!=fCellOccMap.end() || fCellOccMap.find(20)!=fCellOccMap.end())
      probe[1]++;
  }
  if(fCellOccMap.find(7)!=fCellOccMap.end() && fCellOccMap.find(19)!=fCellOccMap.end() && fCellOccMap.find(31)!=fCellOccMap.end()){
    pivot[2]+=2;
    if(fCellOccMap.find(12)!=fCellOccMap.end() || fCellOccMap.find(13)!=fCellOccMap.end())
      probe[2]++;
    if(fCellOccMap.find(24)!=fCellOccMap.end() || fCellOccMap.find(25)!=fCellOccMap.end())
      probe[2]++;
  }
  if(fCellOccMap.find(8)!=fCellOccMap.end() && fCellOccMap.find(20)!=fCellOccMap.end() && fCellOccMap.find(32)!=fCellOccMap.end()){
    pivot[3]+=2;
    if(fCellOccMap.find(13)!=fCellOccMap.end() || fCellOccMap.find(14)!=fCellOccMap.end())
      probe[3]++;
    if(fCellOccMap.find(25)!=fCellOccMap.end() || fCellOccMap.find(26)!=fCellOccMap.end())
      probe[3]++;
  }

  //xzview
  if(fCellOccMap.find(4)!=fCellOccMap.end() && fCellOccMap.find(16)!=fCellOccMap.end() && fCellOccMap.find(28)!=fCellOccMap.end()){
    pivot[4]+=2;
    if(fCellOccMap.find(10)!=fCellOccMap.end() || fCellOccMap.find(9)!=fCellOccMap.end())
      probe[4]++;
    if(fCellOccMap.find(22)!=fCellOccMap.end() || fCellOccMap.find(21)!=fCellOccMap.end())
      probe[4]++;
  }
  if(fCellOccMap.find(5)!=fCellOccMap.end() && fCellOccMap.find(17)!=fCellOccMap.end() && fCellOccMap.find(29)!=fCellOccMap.end()){
    pivot[5]+=2;
    if(fCellOccMap.find(10)!=fCellOccMap.end() || fCellOccMap.find(11)!=fCellOccMap.end())
      probe[5]++;
    if(fCellOccMap.find(22)!=fCellOccMap.end() || fCellOccMap.find(23)!=fCellOccMap.end())
      probe[5]++;
  }
  if(fCellOccMap.find(9)!=fCellOccMap.end() && fCellOccMap.find(21)!=fCellOccMap.end() && fCellOccMap.find(33)!=fCellOccMap.end()){
    pivot[6]+=2;
    if(fCellOccMap.find(15)!=fCellOccMap.end() || fCellOccMap.find(16)!=fCellOccMap.end())
      probe[6]++;
    if(fCellOccMap.find(27)!=fCellOccMap.end() || fCellOccMap.find(28)!=fCellOccMap.end())
      probe[6]++;
  }
  if(fCellOccMap.find(10)!=fCellOccMap.end() && fCellOccMap.find(22)!=fCellOccMap.end() && fCellOccMap.find(34)!=fCellOccMap.end()){
    pivot[7]+=2;
    if(fCellOccMap.find(16)!=fCellOccMap.end() || fCellOccMap.find(17)!=fCellOccMap.end())
      probe[7]++;
    if(fCellOccMap.find(28)!=fCellOccMap.end() || fCellOccMap.find(29)!=fCellOccMap.end())
      probe[7]++;
  }

  Int_t total_probesxview=0, total_pivotsxview=0, total_probesyview=0, total_pivotsyview=0;
  for(Int_t i=0;i<4;i++){
    total_probesyview+=probe[i];
    total_pivotsyview+=pivot[i];
    total_probesxview+=probe[i+4];
    total_pivotsxview+=pivot[i+4];
  }
  fEffPaoloni= ((total_pivotsxview+total_pivotsyview)==0) ?  -1 : ((Double_t) (total_probesxview+total_probesyview))/(total_pivotsxview+total_pivotsyview);
  fEffPaoloniXview=(total_pivotsxview==0) ? -1: ((Double_t) total_probesxview)/total_pivotsxview;
  fEffPaoloniYview=(total_pivotsyview==0) ? -1: ((Double_t) total_probesyview)/total_pivotsyview;

return;
}
