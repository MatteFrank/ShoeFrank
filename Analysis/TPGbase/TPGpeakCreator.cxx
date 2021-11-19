/***************************************************************************
 *   Copyright (C) 2004-2006 by Olivier Stezowski & Christian Finck        *
 *   stezow(AT)ipnl.in2p3.fr, cfinck(AT)iphc.cnrs.fr                       *
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
#include "TH1.h"
#include "TList.h"
#include "TMath.h"
#include "TROOT.h"
#include "TContextMenu.h"
#include "TFrame.h"

#include "TPGpadManager.hxx"
#include "TPGpeakCreator.hxx"

TList TPGpeakCreator::fgPeakCreatorList;
TF1*  TPGpeakCreator::fgDefaultFWHM = 0x0;

//
ClassImp(TPGpeakCreator)

//__________________________________________________________
TPGpeakCreator::TPGpeakCreator()
: TObject(),
fDefaultPeak1D(0x0),
fIsCollecting(false),
fIsGate(false),
fDefaultPeakFWHM(0x0),
fCanvas(0x0),
fLastX(0),
fLastY(0)
{
   // default constructor
   fDefaultPeak1D   = new TPGpeak1D();
   // to be sure it exists
   GetGlobalPeakFWHM();
   SetDefaultPeakFWHM("PK_FWHM"); // TPGpeakCreator Global _ FWHM
}

//__________________________________________________________
TPGpeakCreator::TPGpeakCreator(TCanvas *c) : TObject(),
fDefaultPeak1D(0x0),
fIsCollecting(true),
fIsGate(false),
fDefaultPeakFWHM(0x0),
fCanvas(c),
fLastX(0),
fLastY(0)
{
   // default constructor
   fDefaultPeak1D   = new TPGpeak1D();
   GetGlobalPeakFWHM();
   SetDefaultPeakFWHM("PK_FWHM"); // TPGpeakCreator Global _ FWHM
   
   // recursive delete
   SetBit(TObject::kMustCleanup);
   
   fCanvas->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)",
                    "TPGpeakCreator",
                    this,
                    "HandleMovement(Int_t, Int_t, Int_t, TObject*)");
   
   fCanvas->Connect("Closed()",
                    "TPGpeakCreator",
                    this,
                    "Delete()");
   
   fgPeakCreatorList.Add(this);
   //	fgPeakCreatorList.ls();
}

//__________________________________________________________
TPGpeakCreator::~TPGpeakCreator()
{
   //   if ( fLog.IsDebug(5) )
   //      fLog << info << "Destroy TPGpeakCreator " << nline;
   
   // in case this is connected to a canvas
   if ( fCanvas ) {
      
      TQObject::Disconnect(fCanvas,"HandleMovement(Int_t, Int_t, Int_t, TObject*)",this);
      TQObject::Disconnect(fCanvas,"Closed()",this);
      
      fgPeakCreatorList.Remove(this);
      //		fgPeakCreatorList.ls();
   }
   
   // default destructor
   if (fDefaultPeak1D)
   { delete fDefaultPeak1D; fDefaultPeak1D = 0x0; }
   if (fDefaultPeakFWHM)
   { delete fDefaultPeakFWHM; fDefaultPeakFWHM = 0x0; }
   
}

//__________________________________________________________
TVirtualPad *TPGpeakCreator::PadforAction(TVirtualPad *pad)
{ 
   if ( pad )
      return pad ;
   return TVirtualPad::Pad();
}

//__________________________________________________________
TPGpeakCreator *TPGpeakCreator::AddTPGpeakCreator(TVirtualPad *pad)
{
   TPGpeakCreator *pcreator = 0x0;
   
   // to get the pad on which the creator is added
   TVirtualPad *locpad = TPGpeakCreator::PadforAction(pad);
   if ( locpad == 0x0 )
      return pcreator;
   
   TIter next(&fgPeakCreatorList);
   while ( (pcreator = (TPGpeakCreator *)next()) ) {
      if ( pcreator->GetCanvas() == locpad->GetCanvas() )
         return pcreator;
   }
   
   // create a TPGpeakCreator and add it to the current canvas
   pcreator = new TPGpeakCreator(locpad->GetCanvas());
   return pcreator;
}

//__________________________________________________________
TPGpeakCreator *TPGpeakCreator::IsTPGpeakCreator(TVirtualPad *pad)
{
   TPGpeakCreator *pcreator = 0x0;
   
   // to get the pad on which the creator is added
   TVirtualPad *locpad = TPGpeakCreator::PadforAction(pad);
   if ( locpad == 0x0 )
      return pcreator;
   
   TIter next(&fgPeakCreatorList);
   while ( (pcreator = (TPGpeakCreator *)next()) ) {
      if ( pcreator->GetCanvas() == locpad->GetCanvas() ) {
         break;
      }
   }
   return pcreator;
}

//__________________________________________________________
void TPGpeakCreator::Clear(Option_t * opt)
{
   TString o = opt;
   
   // collect peaks in the selected canvas
   TList listofpeaks;
   TPGpadManager::Collect<TPGbasePeak>(listofpeaks,fCanvas->GetSelectedPad());
   listofpeaks.Clear();
   TPGpadManager::Collect<TPolyLine>(listofpeaks,fCanvas->GetSelectedPad());
   listofpeaks.Clear();
}

//__________________________________________________________
void TPGpeakCreator::Collect(Bool_t do_collect)
{
   fIsCollecting = do_collect;
}

//__________________________________________________________
void TPGpeakCreator::SetGateMode(Bool_t ngate)
{
   if ( ngate )
      fDefaultPeak1D->SetDrawAs(TPGpeak1D::kGate);
   else
      fDefaultPeak1D->SetDrawAs(TPGpeak1D::kPeak);
}

//__________________________________________________________
void TPGpeakCreator::HandleMovement(Int_t eventType, Int_t eventX, Int_t eventY, TObject* /*select*/)
{ 	
   // handle cursor mouvement
   TVirtualPad *pad = fCanvas->GetSelectedPad();
   if ( pad == 0x0 )
      return;
   if ( !pad->IsEditable() ) {
      return ;
   }
   if (eventType == kMouseMotion) {
      // to keep track of the last mouse position
      fLastX = eventX;
      fLastY = eventY;
   }
   if( eventType == kButton1Up ) {
      HandleRefresh();
   }
   if( eventType == kKeyPress) {
      
      Int_t keysym = eventY;
      switch ((EKeySym) keysym ) {
         case kKey_0 :
            //		fLog << info << "Current color/style for new peak is now style 0" << nline;
            fDefaultPeak1D->SetLineColorPeak(TPGbasePeak::fgGateColor.k0);
            fDefaultPeak1D->SetFillColorPeak(TPGbasePeak::fgGateColor.k0);
            break;
         case kKey_1:
            //	fLog << info << "Current color/style for new peak is now style 1" << nline;
            fDefaultPeak1D->SetLineColorPeak(TPGbasePeak::fgGateColor.k1);
            fDefaultPeak1D->SetFillColorPeak(TPGbasePeak::fgGateColor.k1);
            break;
         case kKey_2 :
            //		fLog << info << "Current color/style for new peak is now style 2" << nline;
            fDefaultPeak1D->SetLineColorPeak(TPGbasePeak::fgGateColor.k2);
            fDefaultPeak1D->SetFillColorPeak(TPGbasePeak::fgGateColor.k2);
            break;
         case kKey_3:
            //		fLog << info << "Current color/style for new peak is now style 3" << nline;
            fDefaultPeak1D->SetLineColorPeak(TPGbasePeak::fgGateColor.k3);
            fDefaultPeak1D->SetLineColorPeak(TPGbasePeak::fgGateColor.k3);
            break;
         case kKey_a:
            if ( fIsCollecting ) {
               TPGbasePeak *peak = CreatePeak(pad->AbsPixeltoX(fLastX),"",pad);
               if ( peak && pad->GetFrame() )
                  peak->PaintFor(pad->GetFrame()->GetX1(),pad->GetFrame()->GetX2(),pad->GetFrame()->GetY1(),pad->GetFrame()->GetY2());
               pad->Update();
            }
            break;
         case kKey_A:
            if ( fIsCollecting ) {
               TPGbasePeak *peak = CreatePeak(pad->AbsPixeltoX(fLastX),"bg",pad);
               if ( peak && pad->GetFrame() )
                  peak->PaintFor(pad->GetFrame()->GetX1(),pad->GetFrame()->GetX2(),pad->GetFrame()->GetY1(),pad->GetFrame()->GetY2());
               pad->Update();
            }
            break;
         case kKey_p:
            if ( fIsCollecting ) {
               AddPointToPolyline(pad->AbsPixeltoX(fLastX), pad->AbsPixeltoY(fLastY),pad);
               pad->Update();
            }
            break;
         case kKey_c:
            Clear();
            pad->Update();
            break;
         case kKey_Greater:
            fCanvas->GetContextMenu()->Popup(fLastX,fLastY,this,fCanvas,pad);
            break;
         case kKey_M:
            fCanvas->GetContextMenu()->Popup(fLastX,fLastY,this,fCanvas,pad);
            break;
         case kKey_h:
            std::cout << "\ta: Add a peak at mouse position" << std::endl;
            std::cout << "\tA: Add a peak + background point at mouse position" << std::endl;
            std::cout << "\tp: Add a point to the graphical polyline" << std::endl;
            std::cout << "\t0: Switch Color to default #0" << std::endl;
            std::cout << "\t1: Switch Color to default #1" << std::endl;
            std::cout << "\t2: Switch Color to default #2" << std::endl;
            std::cout << "\t3: Switch Color to default #3" << std::endl;
            std::cout << "\tc: Clear all (peaks + graphical polyline)" << std::endl;
            
            std::cout << "\t>: Popup TPGpeakCreator menu" << std::endl;
            std::cout << "\tM: Popup TPGpeakCreator menu" << std::endl;
            
            break;
         default:
            break;
      }
   }
}

//__________________________________________________________
void TPGpeakCreator::HandleRefresh()
{
   TObject *obj; TFrame *frame; TPGbasePeak *apeak;
   
   // if no Frame, go out
   if ( (frame = fCanvas->GetSelectedPad()->GetFrame()) == 0x0 )
      return;
   
   // collect peaks in the selected canvas
   TList listofpeaks;
   TPGpadManager::Collect<TPGbasePeak>(listofpeaks,fCanvas->GetSelectedPad());
   
   //	printf(" %s TEST %d \n",fCanvas->GetSelectedPad()->GetFrame()->GetName(),listofpeaks.GetSize() );
   TIter next(&listofpeaks);
   while ( (obj = next()) ) {
      apeak = dynamic_cast<TPGbasePeak *> (obj);
      if ( apeak )
         apeak->PaintFor(frame->GetX1(),frame->GetX2(),frame->GetY1(),frame->GetY2()) ;
      
      fCanvas->GetSelectedPad()->Modified();
   }
   // clear the list
   listofpeaks.Clear("nodelete");
}

//__________________________________________________________
const TF1 *TPGpeakCreator::GetGlobalPeakFWHM()
{
   Bool_t is_new = false;
   
   if ( fgDefaultFWHM == 0 )
      is_new = true;
   else
      if ( gROOT->GetListOfFunctions()->FindObject(fgDefaultFWHM) == 0x0 ) // has been deleted somewhere else
         is_new = true;
   // creates it.
   if ( is_new ) {
      // to do: a better definition ex - definition of the Ge resolution
      // or ask a central something
      fgDefaultFWHM = new TF1("PK_FWHM","0.0002");  // TPGpeakCreator Global FWHM
   }
   
   return fgDefaultFWHM;
}

//__________________________________________________________
void TPGpeakCreator::SetDefaultPeakFWHM( const char *fname )
{
   TObject *o = gROOT->GetListOfFunctions()->FindObject(fname); TF1 *f;
   if ( o == 0x0 )
      return;
   else
      f = dynamic_cast<TF1 *> (o);
   
   if ( fDefaultPeakFWHM )
      delete fDefaultPeakFWHM;
   
   fDefaultPeakFWHM = (TF1 *)(f->Clone());
   gROOT->GetListOfFunctions()->Remove(fDefaultPeakFWHM);
}

//__________________________________________________________
TPGbasePeak *TPGpeakCreator::CreatePeak(Double_t x, Option_t *opt, TVirtualPad *pad)
{
   // create peak from current position in pad
   TPGbasePeak* peak = 0x0; TH1* h;
   
   // to get the pad on which the creator is added
   TVirtualPad *locpad = TPGpeakCreator::PadforAction(pad);
   if ( locpad == 0x0 )
      return peak;
   
   h = TPGpadManager::GetHisto(locpad);
   if ( h ) {
      // look in the current pad for the list of peak that already exist
      std::vector<TPGpeak1D *> peaks_in_pad;
      // collect
      TPGpadManager::Collect<TPGpeak1D>(peaks_in_pad,locpad);
      // set the name of the peak used as a cloner of the new peak to be created
      TString pname; Int_t i = 0;
      while ( 1 ) {
         pname = Form("Peak_%d",i++); Bool_t is_in = false;
         for (size_t i = 0; i < peaks_in_pad.size(); i++) {
            if ( pname == peaks_in_pad[i]->GetName() ) {
               is_in = true;
               break;
            }
         }
         if ( is_in == false ) {
            fDefaultPeak1D->SetName( pname.Data() );
            break;
         }
      }
      
      // now creates the peak
      peak = CreatePeak(h,x,opt);
      if ( peak ) {
         //
         peak->SetBit(TObject::kCanDelete);
         peak->SetBit(TObject::kMustCleanup);
         
         // a1  dd peak to pad
         locpad->GetListOfPrimitives()->Add(peak,"");
         locpad->Modified();
      }
   }
   return peak;
}

//__________________________________________________________
TPGbasePeak* TPGpeakCreator::CreatePeak(const TH1 *histo, Double_t x, Option_t *opt)
{
   // to check options
   TString o = opt;
   
   // the peak to be created
   TPGbasePeak* peak = 0x0;
   
   if (histo->GetDimension() == 0 || histo->GetDimension() > 1) {
      Error("CreatePeak()", "Dimension not handled %d", histo->GetDimension());
      return peak;
   }
   switch ( histo->GetDimension() ) {
      case 1:
      { // 1D :
        // scan the histogram 6 sigma around the given position
         Double_t width =
         fDefaultPeakFWHM->Eval(x);
         Int_t binmin = histo->GetXaxis()->FindFixBin(x-3*width);
         Int_t binmax = histo->GetXaxis()->FindFixBin(x+3*width);
         
         // to avoid including under/over flow
         binmin = TMath::Max(1,binmin);
         binmax = TMath::Min(histo->GetNbinsX(),binmax);
         
         Double_t yminleft, yminright, ymin, ymax, ymax_abs; Int_t bin_ymax, bin_ymax_abs;
         
         bin_ymax = bin_ymax_abs = histo->GetXaxis()->FindFixBin(x); ymax = ymax_abs = histo->GetBinContent(histo->GetXaxis()->FindFixBin(x));
         ymin = yminleft = yminright = 10E100;
         
         // first compute some characteritic values
         for (Int_t i = binmin ; i < binmax; i++) {
            
            if ( histo->GetBinCenter(i) < x-width && histo->GetBinContent(i) < yminleft ) {
               yminleft = histo->GetBinContent(i);
            }
            if ( histo->GetBinCenter(i) > x+width && histo->GetBinContent(i) < yminright ) {
               yminright = histo->GetBinContent(i);
            }
            // fine maximum
            if ( histo->GetBinContent(i) > ymax && histo->GetBinCenter(i) > (x-0.4*width) && histo->GetBinCenter(i) < (x+0.4*width) )
            { ymax = histo->GetBinContent(i); bin_ymax = i; }
            
            if ( histo->GetBinContent(i) > ymax_abs )
            { ymax_abs = histo->GetBinContent(i); bin_ymax_abs = i; }
         }
         // set maximum ... avoid having the max in a hole
         if ( ymax < yminleft || ymax < yminright )
            ymax = ymax_abs ;
         
         /*				Double_t DeltaY = TMath::Max ( TMath::Abs(ymax-yminleft) , TMath::Abs(ymax-yminright) ) ;
          for (Int_t i = binmin; i < binmax; i++) {
          
          
          }				*/
         // yminleft /= width; yminright /= width;
         ymin = (yminleft+yminright)/2;
         
         // Set peak properties
         TPGpeak1D* rpeak =
         dynamic_cast<TPGpeak1D*>(fDefaultPeak1D->Clone());
         
         rpeak->SetBackground(x-3*width,
                              x-2*width,
                              x+2*width,
                              x+3*width, yminleft, ymin, ymin, yminright );
         /*
          histo->GetBinContent(histo->GetXaxis()->FindFixBin(x-3*width)),
          histo->GetBinContent(histo->GetXaxis()->FindFixBin(x-2*width)),
          histo->GetBinContent(histo->GetXaxis()->FindFixBin(x+2*width)),
          histo->GetBinContent(histo->GetXaxis()->FindFixBin(x+3*width)) ); */
         
         if ( o.Contains("atp") ) // does not change the position
            rpeak->SetPeak(x, ymax-ymin, width);
         else
            rpeak->SetPeak(histo->GetBinCenter(bin_ymax), ymax-ymin, width);
         
         if ( o.Contains("bg") )
            rpeak->WithBkg(true);
         else
            rpeak->WithBkg(false);
         
         peak = rpeak;
      }
         break;
      default:
         break;
   }
   
   return peak;
}


//__________________________________________________________
void TPGpeakCreator::AddPointToPolyline(Double_t x, Double_t y, TVirtualPad *pad)
{	
   // to get the pad on which the creator is added
   TVirtualPad *locpad = TPGpeakCreator::PadforAction(pad);
   if ( locpad == 0x0 )
      return ;
   
   TPolyLine* line = 0x0;
   TList* list = locpad->GetListOfPrimitives();
   for (Int_t i = 0; i < list->GetEntries(); ++i) {
      TString tmp(list->At(i)->ClassName());
      if ( tmp.CompareTo("TPolyLine") == 0 )
         line = static_cast<TPolyLine*>(list->At(i));
   }
   
   if (!line) {
      line = new TPolyLine(0);
      
      line->SetBit(kCanDelete, true); line->SetBit(kMustCleanup, true);
      
      line->SetLineWidth(2);
      line->SetLineColor(kRed-5);
      line->SetFillColor(kRed-5);
      line->SetFillStyle(3010);
      line->SetNextPoint(x,y);
      
      locpad->GetListOfPrimitives()->Add(line,"f");
      
   }
   else line->SetNextPoint(x,y);
   
   locpad->Modified();
}



