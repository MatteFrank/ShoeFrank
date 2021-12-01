/***************************************************************************
 *   Copyright (C) 2004-2006 by Olivier Stezowski & Christian Finck        *
 *   stezow(AT)ipnl.in2p3.fr, cfinck(AT)ires.in2p3.fr                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <Riostream.h>

#include "KeySymbols.h"
#include "TCanvas.h"
#include "TClass.h"
#include "TContextMenu.h"
#include "TH1.h"
#include "TH2.h"
#include "TFormula.h"
#include "TList.h"
#include "TMath.h"
#include "TPad.h"
#include "TROOT.h"

#include "TPGpeakCreator.hxx"
#include "TPGspectrumPlayer.hxx"

//
ClassImp(TPGspectrumPlayer)

//__________________________________________________________
TPGspectrumPlayer::TPGspectrumPlayer()
: TNamed("SpectrumPlayer","Player for spectra"),
fCreator(0x0),
fPeakList(0x0),
fInnerPeakList(0x0),
fLastX(0),
fLastY(0),
fContextMenu(0x0)
{
   fCreator = new TPGpeakCreator();
   
   // default constructor
   fInnerPeakList   = new TList(); fInnerPeakList->SetOwner(true);
   fPeakList        = fInnerPeakList;
}

//__________________________________________________________
TPGspectrumPlayer::~TPGspectrumPlayer()
{
   // default destructor
   fPeakList = 0x0;
   
   if (fCreator)
   { delete fCreator; fCreator = 0x0; }
   
   fInnerPeakList->Delete();
   if (fInnerPeakList)
   { delete fInnerPeakList; fInnerPeakList = 0x0; }
   
   Disconnect();
}


//__________________________________________________________
Bool_t TPGspectrumPlayer::IsInRange(TH1* h, TPGbasePeak* peak)
{
   if ( h->GetDimension() != 1) {
      Error("IsInRange()", "2D peak method not implemented yet");
      return false;
   }
   
   Int_t firstBin = h->GetXaxis()->GetFirst();
   Int_t lastBin  = h->GetXaxis()->GetLast();
   
   Float_t low  = h->GetBinCenter(firstBin);
   Float_t high = h->GetBinCenter(lastBin);
   
   if (peak->GetPosition() - 2*peak->GetFWHM() > low && peak->GetPosition() + 2*peak->GetFWHM() < high)
      return true;
   else {
      Info("IsInRange()", "Peak %s not fitted cos out of zoom", peak->GetName());
      return false;
   }
   
   return false;
}

//__________________________________________________________
Bool_t TPGspectrumPlayer::Connect(TCanvas *canvas)
{
   TCanvas *localCanvas = canvas;
   
   // in this case connect the current canvas
   if ( canvas == 0x0 ) {
      if ( TVirtualPad::Pad() )
         localCanvas = TVirtualPad::Pad()->GetCanvas();
      else {
         Error("Connect()", "gPad not existing");
      }
   }
   if ( localCanvas) {
      
      // to fit graphically
      localCanvas->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)",
                           "TPGspectrumPlayer",
                           this,
                           "HandleMovement(Int_t, Int_t, Int_t, TObject*)");
      
      fContextMenu = localCanvas->GetContextMenu();
      
      // add a peak creator to the canvas if it does not exists
      if ( ! TPGpeakCreator::IsTPGpeakCreator(localCanvas) ) {
         // change the characteristics ?
         // TPGpeakCreator *p = TPGpeakCreator::AddTPGpeakCreator(localCanvas);
         TPGpeakCreator::AddTPGpeakCreator(localCanvas);
      }
      else TPGpeakCreator::IsTPGpeakCreator(localCanvas)->Collect();
   }
   return localCanvas != 0x0;
}

//__________________________________________________________
Bool_t TPGspectrumPlayer::Disconnect(TCanvas *canvas)
{
   TCanvas *localCanvas = canvas;
   
   // in this case connect the current canvas
   if ( canvas == 0x0 ) {
      if ( TVirtualPad::Pad() )
         localCanvas = TVirtualPad::Pad()->GetCanvas();
      else {
         Error("Disconnect()", "gPad not existing");
      }
   }
   if ( localCanvas ) {
      if (localCanvas->Disconnect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)",
                                  this,
                                  "HandleMovement(Int_t, Int_t, Int_t, TObject*)"))
         Info("Disconnect()", "Disconnected from Canvas");
      else
         Error("Disconnect", "Cannot disconnected from Canvas");
      
   }
   fContextMenu = 0x0;
   
   return localCanvas != 0x0;
} 

//__________________________________________________________
void TPGspectrumPlayer::HandleMovement(Int_t eventType, Int_t eventX, Int_t eventY, TObject* select)
{ 
   // handle cursor mouvement
   TVirtualPad *pad = gROOT->GetSelectedPad(), *cpad = TVirtualPad::Pad();
   
   if ( pad != cpad )
      pad->cd();
   
   TString name(select->ClassName());
   if (eventType == kMouseMotion) {
      // to keep track of the last mouse position
      fLastX = eventX; fLastY = eventY;
   }
   if( eventType == kKeyPress) {
      
      Int_t keysym = eventY;
      
      if ( (EKeySym)keysym == kKey_f)
         PopupFitMenu();
      
      if ( (EKeySym)keysym == kKey_h) {
         printf("\tf: pop up fit panel menu\n");
      }
   }
   cpad->cd();
}

//__________________________________________________________
void TPGspectrumPlayer::PopupFitMenu()
{
   TMethod* m = Class()->GetMethod("FitAll", "");
   
   if (m != 0x0 && fContextMenu != 0x0)
      fContextMenu->Action(this, m);
   else {
      Error("PopupFitMenu()", "Cannot Popup FitAll dialog box");
   }
}


//__________________________________________________________
void TPGspectrumPlayer::FitAll(const char* nameFunc, Option_t* optFit, Option_t* optBkg)
{
   // fit all peak in list with a common function and background
   if ( CollectPeaks() == 0 )
      return;
   
   TPGbasePeak* peak  = 0x0;
   TPGbasePeak* peak2 = 0x0;
   
   TList peaks; peaks.SetOwner(false);
   
   // sort peak in list
   SortPeakList();
   
   TString opt(optFit);
   // rename if name of peaks are the same
   if ( RenamePeak(GetName()) ) {
      Warning("FitAll()", "Rename peaks cos they have the same name");
   }
   
   TH1 *h = TPGpadManager::GetHisto();
   
   if ( h->GetDimension() >= 2) {
      Error("FitAll()", "2D peak fitting not implemented yet");
      return;
   }
   
   Bool_t IsUsed[GetPeakList()->GetEntries()];
   for (Int_t i = 0; i < GetPeakList()->GetEntries(); ++i)
      IsUsed[i] = false;
   
   for (Int_t i = 0; i < GetPeakList()->GetEntries(); ++i) {
      if (!IsUsed[i]) IsUsed[i] = true;
      else  continue;
      
      peak = static_cast<TPGbasePeak*> (GetPeakList()->At(i));
      if (opt.Contains("R") && !IsInRange(h, peak)) continue;
      
      for (Int_t j = i+1; j < GetPeakList()->GetEntries(); ++j) {
         peak2 = static_cast<TPGbasePeak*> (GetPeakList()->At(j));
         if (opt.Contains("R") && !IsInRange(h, peak2)) continue;
         
         if (peak->IsPointInBkg(peak2->GetPosition()) < 1) {
            peak2->SetSignalFunction(nameFunc);
            if (!IsUsed[j]) {
               peaks.Add(peak2);
               IsUsed[j] = true;
            }
         }
      }
      
      if (peaks.GetEntries() != 0) {
         
         peak->SetSignalFunction(nameFunc);
         peak->FitCombined(h, &peaks, optFit, optBkg);
         peaks.Clear("nodelete");
      } else {
         
         peak->SetSignalFunction(nameFunc);
         peak->Fit(h,optFit, optBkg);
      }
   }
   
   TIter next(&peaks); TObject *obj;
   while ( (obj = next()) ) {
      peaks.Remove(obj);
   }
}


//__________________________________________________________
TH1 *TPGspectrumPlayer::Background(Option_t* opt)
{ 
   // set bkg from current histo in pad
   TH1 *h = TPGpadManager::GetHisto(), *result = 0x0;
   if ( h )
      result = Background(h,opt);
   if ( result ) {
      result->Draw("same");
   } // TODO Wheel Color/Style
   
   return result;
}

//__________________________________________________________
void TPGspectrumPlayer::DoBackground(TH1 *histo, Option_t* opt)
{ 
   // set bkg from histo
   TH1 *bg = Background(histo,opt);
   if ( bg ) {
      histo->Add(bg,-1.0);
      delete bg;
   }
}

//__________________________________________________________
void TPGspectrumPlayer::SetPeakList(TSeqCollection *col)
{ 
   // set peak list from outside
   col ? fPeakList = col : fPeakList = dynamic_cast<TSeqCollection*>(fInnerPeakList);
}

//__________________________________________________________
Bool_t TPGspectrumPlayer::SetParameter(const char* name, const TObject *value)
{
   // set parameters from list
   Bool_t ok = false;
   
   if ( value == 0x0 )
      return ok;
   
   // set search parameter
   if (strncmp(name, "DefaultPeak1D", strlen(name)) == 0 && value->InheritsFrom("Gw::TPGbasePeak") ) {
      /*		if ( fDefaultPeak1D )
       { delete fDefaultPeak1D; fDefaultPeak1D = 0x0; }
       fDefaultPeak1D =
       dynamic_cast<TPGbasePeak *>(value->Clone());
       ok = true; */
   }
   else {
      Error("SetParameter()","Unkown parameter %s", name);
   }
   
   return ok;
}

//__________________________________________________________
void TPGspectrumPlayer::SetDefaultPeakFWHM(const char* name)
{
   TPGpeakCreator* peakCreator = TPGpeakCreator::IsTPGpeakCreator(TVirtualPad::Pad());
   if (peakCreator) {
      peakCreator->SetDefaultPeakFWHM(name);
   } else {
      Warning("SetDefaultPeakFWHM()", "Peak Creator not existing in the current Pad");
   }
   
}

//__________________________________________________________
Int_t TPGspectrumPlayer::FindPeaks(Option_t* opt)
{
   // find peak method
   TH1* h = TPGpadManager::GetHisto();
   if ( h == 0x0 ) {
      Error("FindPeaks()", "No spectrum displayed in current pad");
      return 0;
   }
   else return FindPeaks(h,opt);
}

//__________________________________________________________
Bool_t TPGspectrumPlayer::RenamePeak(const Char_t* baseName, Bool_t force)
{
   // rename and re-ordered peak with respect to position
   TPGbasePeak* peak = 0x0;
   TPGbasePeak* peak2 = 0x0;
   Int_t k = 0;
   
   Bool_t rename = false;
   for (Int_t i = 0; i < GetPeakList()->GetEntries(); ++i) {
      peak = static_cast<TPGbasePeak*> (GetPeakList()->At(i));
      TString name(peak->GetName());
      for (Int_t j = i+1; j < GetPeakList()->GetEntries(); ++j) {
         peak2 = static_cast<TPGbasePeak*> (GetPeakList()->At(j));
         
         if (name.CompareTo(peak2->GetName()) == 0)
            rename = true;
      }
   }
   
   if (!force && !rename) return false;
   
   TIter next(fPeakList);
   while ( (peak = (TPGbasePeak*) next()) )  { // iterator skips empty slots
      peak->SetName( Form("%s%d", baseName, k++) );
   }
   return true;
}

//__________________________________________________________
void TPGspectrumPlayer::AddPeak(TPGbasePeak* peak)
{
   // add peak to list
   if ( !fPeakList->FindObject(peak) )
      fPeakList->Add(peak);
}

//__________________________________________________________
void TPGspectrumPlayer::SortPeakList(const Char_t* parName, Bool_t sortDes)
{
   // sort peak respect to given parameter
   
   TPGbasePeak* peak = 0x0;
   Int_t i = 0;
   Int_t n = fPeakList->GetEntries();
   
   Int_t*    index = new Int_t[n];
   Double_t* par   = new Double_t[n];
   
   TList* tmp = new TList();
   tmp->SetOwner(false);
   
   // save parameters
   TIter next(fPeakList);
   while ( (peak = (TPGbasePeak*)next()) ) {
      if (strncmp(parName, "Position", n) == 0)
         par[i++] = peak->GetPosition();
      if (strncmp(parName, "FWHM", n) == 0)
         par[i++] = peak->GetFWHM();
      if (strncmp(parName, "Height", n) == 0)
         par[i++] = peak->GetHeight();
      tmp->AddLast(peak);
   }
   
   // sort in index
   TMath::Sort(n, par, index, sortDes);
   
   // just reset counter, no delete
   fPeakList->Clear("nodelete");
   
   // add again in fInnerPeakList
   for (i = 0; i < n; ++i )  {
      peak = static_cast<TPGbasePeak*>(tmp->At(index[i]));
      fPeakList->AddLast(peak);
   }
   
   tmp->Clear("nodelete");
   delete tmp;
   delete[] index;
   delete[] par;
}

//__________________________________________________________
void TPGspectrumPlayer::ShowPeakList() const
{ 
   // re-draw peaks onto current pad
   fPeakList->AppendPad();
}

//__________________________________________________________
Int_t TPGspectrumPlayer::CollectPeaks(Option_t* o)
{
   // create peak list from pad
   if ( !TVirtualPad::Pad() ) {
      Error("CollectPeaks()", "Pad not existing");
      return 0;
   }
   
   TString opt = o;
   if ( !opt.Contains("+") )
      fPeakList->Clear("nodelete");
   
   TObject *obj; Int_t i = 0;
   
   TIter next(TVirtualPad::Pad()->GetListOfPrimitives());
   while ( (obj = next()) ) { // iterator skips empty slots
      if ( obj->InheritsFrom("TPGbasePeak") ) {
         if (!fPeakList->FindObject(obj)) {
            obj->SetBit(TObject::kCanDelete, false);
            obj->SetBit(TObject::kMustCleanup, false);
            fPeakList->Add(obj); i++;
         }
      }
   }
   
   return i;
}

//__________________________________________________________
void TPGspectrumPlayer::Print(Option_t* opt) const
{
   // print peak info
   fPeakList->Print(opt);
}
