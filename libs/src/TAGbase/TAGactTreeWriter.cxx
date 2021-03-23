/*!
  \file
  \version $Id: TAGactTreeWriter.cxx,v 1.17 2003/07/08 18:54:43 mueller Exp $
  \brief   Implementation of TAGactTreeWriter.
*/

#include <math.h>
#include <string.h>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TBranchElement.h"
#include "TLeafObject.h"
#include "TDirectory.h"
#include "TString.h"

#include "TAGroot.hxx"
#include "TAGactTreeWriter.hxx"

/*!
  \class TAGactTreeWriter TAGactTreeWriter.hxx "TAGactTreeWriter.hxx"
  \brief Write tree's. **
*/

ClassImp(TAGactTreeWriterBranch);
ClassImp(TAGactTreeWriter);

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGactTreeWriterBranch::TAGactTreeWriterBranch(TAGdataDsc* p_data, TString name, Int_t i_size,
                                                      Int_t i_compress, Bool_t b_object)
: fpDataDsc(p_data),
  fName(name),
  fiBasketSize(i_size),
  fiCompress(i_compress),
  fpBranch(0),
  fbObject(b_object)
{
}

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGactTreeWriter::TAGactTreeWriter(const char* name)
  : TAGactionFile(name, "TAGactTreeWriter - Tree writer", "NEW"),
    fpBranchList(0),
    fpFile(0),
    fpTree(0),
    fiNEntry(0),
    fiCompress(1)
{
  fpBranchList = new TList();
  fpBranchList->SetOwner(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGactTreeWriter::~TAGactTreeWriter()
{
  Close();
  delete fpBranchList;
}

//------------------------------------------+-----------------------------------
//! Add input data descriptor.

void TAGactTreeWriter::SetupObjectBranch(TAGdataDsc* p_data, const char* branch,
					 Int_t i_size, Int_t i_compress)
{
  TAGactTreeWriterBranch* p_chan = new TAGactTreeWriterBranch(p_data, branch,
					   i_size, i_compress, kTRUE);
  AddDataIn(p_data, "TAGdata");		    // ??? be more specific ???
  fpBranchList->Add(p_chan);
  return;
}

//------------------------------------------+-----------------------------------
//! Add input data descriptor.

void TAGactTreeWriter::SetupElementBranch(TAGdataDsc* p_data,
					  const char* branch,
					  Int_t i_size, Int_t i_compress)
{
  TAGactTreeWriterBranch* p_chan = new TAGactTreeWriterBranch(p_data, branch,
                                           i_size, i_compress, kFALSE);
  AddDataIn(p_data, "TAGdata");		    // ??? be more specific ???
  fpBranchList->Add(p_chan);
  return;
}

//------------------------------------------+-----------------------------------
//! Set compression level for file and all branches.

void TAGactTreeWriter::SetCompressionLevel(Int_t i_compress)
{
  fiCompress = i_compress;
  if (fpFile && fpTree) {
    fpFile->SetCompressionLevel(fiCompress);
    for (TObjLink* lnk = fpBranchList->FirstLink(); lnk; lnk=lnk->Next()) {
      TAGactTreeWriterBranch* p_chan =(TAGactTreeWriterBranch*)lnk->GetObject();
      TBranch* p_branch = p_chan->fpBranch;
      if (p_branch) p_branch->SetCompressionLevel(fiCompress);
    }
  }
 
  return;
}

//------------------------------------------+-----------------------------------
//! Open root file.

Int_t TAGactTreeWriter::Open(const TString& name, Option_t* option, const TString treeName, Bool_t dscBranch)
{
  TDirectory* p_cwd = gDirectory;

  Close();
  
  if (option == 0) option = fOpenOpt.Data();
  fpFile = TFile::Open(name, option);
  if (!fpFile->IsOpen()) {
    Error("Open()", "Failed to open file '%s'", name.Data());
    delete fpFile;
    fpFile = 0;
    gDirectory = p_cwd;
    return 1;
  }

  fpFile->SetCompressionLevel(fiCompress);

  fpTree = new TTree(treeName.Data(), GetTitle());

  for (TObjLink* lnk = fpBranchList->FirstLink(); lnk; lnk=lnk->Next()) {
    TAGactTreeWriterBranch* p_chan =(TAGactTreeWriterBranch*)lnk->GetObject();
    TClass*  p_dataclass  = p_chan->fpDataDsc->ObjectClass();
    Int_t    i_compress   = p_chan->fiCompress;
    Bool_t   b_object     = p_chan->fbObject;

    p_dataclass->SetCanSplit(1);

    if (b_object) {
      TBranch* p_branch   = fpTree->Branch(p_chan->fName, 
					   p_dataclass->GetName(), 
					   p_chan->fpDataDsc->ObjectPointer(), 
					   p_chan->fiBasketSize, 0);
      TLeafObject* p_leaf = (TLeafObject*) p_branch->GetLeaf(p_chan->fName);
      p_leaf->SetVirtual(kFALSE);
      p_branch->SetAutoDelete(p_chan->fpDataDsc->Object()->NeedAutoDelete());
      if (i_compress >= 0) p_branch->SetCompressionLevel(i_compress);
      p_chan->fpBranch = p_branch;

    } else {
      p_chan->fpDataDsc->Object()->SetupClones();
      TBranch* p_branch   = fpTree->Branch(p_chan->fName,
					   p_dataclass->GetName(),
					   p_chan->fpDataDsc->ObjectPointer(),
					   p_chan->fiBasketSize, 2);
      if (i_compress >= 0) p_branch->SetCompressionLevel(i_compress);
      p_chan->fpBranch = p_branch;
    }
  }

  fiNEntry = 0;

  gDirectory = p_cwd;
  return 0;
}

//------------------------------------------+-----------------------------------
//! Close file.

void TAGactTreeWriter::Close()
{
  TDirectory* p_cwd = gDirectory;

  if (fpFile && fpTree) {
    fpFile->cd();
    fpTree->Write();
    TAGrunInfo runinfo(gTAGroot->CurrentRunInfo());
    runinfo.Write(TAGrunInfo::GetObjectName());
  }

  delete fpFile;
  fpFile   = 0;
  fpTree   = 0;
  fiNEntry = 0;

  gDirectory = p_cwd;

  return;
}

//------------------------------------------+-----------------------------------
//! Returns \a true if a file is currently open.

Bool_t TAGactTreeWriter::IsOpen() const
{
  return fpFile != 0;
}

//------------------------------------------+-----------------------------------
//! Process TreeWriter.

Bool_t TAGactTreeWriter::Process()
{
  if (Valid()) return kTRUE;
  if (IsZombie()) return kFALSE;

  Bool_t b_valid = CheckDependencies();

  if (!b_valid) return kFALSE;

  if (fpTree) fpTree->Fill();
  
  SetBit(kValid);
  return Valid();
}

//------------------------------------------+-----------------------------------
Int_t TAGactTreeWriter::get_be_type(TBranch* p_branch)
{
  Int_t i_btyp = -1;
  if (p_branch->InheritsFrom("TBranchElement")) {
    i_btyp = ((TBranchElement*)p_branch)->GetType();
  }
  return i_btyp;
}

//------------------------------------------+-----------------------------------
Int_t TAGactTreeWriter::get_be_id(TBranch* p_branch)
{
  Int_t i_bid = 0;
  if (p_branch->InheritsFrom("TBranchElement")) {
    i_bid = ((TBranchElement*)p_branch)->GetID();
  }
  return i_bid;
}

//------------------------------------------+-----------------------------------
void TAGactTreeWriter::sum_branch(Int_t& i_nsub, Double_t& d_usize, Double_t& d_csize,
		       TBranch* p_branch)
{
  Int_t i_btyp = get_be_type(p_branch);
  Int_t i_bid  = get_be_id(p_branch);
  if (i_btyp != 1 && i_btyp != 3 && i_bid != -2) {
    Double_t d_ubytes = p_branch->GetTotalSize();
    Double_t d_cbytes = p_branch->GetZipBytes();
    Double_t d_comp   = 1.;
    
    if (d_cbytes > 0.) d_comp = p_branch->GetTotBytes() / d_cbytes;
    
    d_usize += d_ubytes;
    d_csize += floor(d_ubytes / d_comp);
  }
  
  TIter it_next(p_branch->GetListOfBranches());
  TBranch* p_next;
  while ((p_next= (TBranch*)it_next())) {
    i_nsub += 1;
    sum_branch(i_nsub, d_usize, d_csize, p_next);
  }
}

//------------------------------------------+-----------------------------------
void TAGactTreeWriter::print_branch(ostream& os, Double_t d_bsize, 
			 TBranch* p_branch, Bool_t b_ptop)
{
  Int_t i_btyp = get_be_type(p_branch);
  Int_t i_bid  = get_be_id(p_branch);
  
  if (b_ptop) {
    os << "  " << Form("%-24s", p_branch->GetName());
    const char* c_par = strchr(p_branch->GetTitle(), '[');
    if (strcmp(p_branch->GetName(), p_branch->GetTitle()) != 0 && c_par) {
      os << Form("%-12s", c_par);
    } else {
      os << "            ";
    }

    os << Form(" %2d %3d", i_btyp, i_bid);

    if (i_btyp != 1 && i_btyp != 3) {	    // if not split container
      Double_t d_ubytes = p_branch->GetTotalSize();
      Double_t d_cbytes = p_branch->GetZipBytes();
      Double_t d_comp   = 1.;
      Double_t d_frac   = 0.;
      
      if (d_cbytes > 0.) d_comp = p_branch->GetTotBytes() / d_cbytes;
      if (d_bsize  > 0.) d_frac = d_cbytes/d_bsize;

      os << Form(" %8.1fk", d_ubytes/1000);
      if (d_cbytes > 0.) {
	os << Form(" %8.1fk %6.2f", d_cbytes/1000, d_comp);
      } else {
	os << "   <1 bask    (1)";
      }
      os << Form(" %5.1f%%", 100*d_frac);    
    }
    os << endl;
  }
  
  TIter it_next(p_branch->GetListOfBranches());
  TBranch* p_next;
  while ((p_next= (TBranch*)it_next())) {
    print_branch(os, d_bsize, p_next, kTRUE);
  }
}

//------------------------------------------+-----------------------------------
//! ostream insertion.

void TAGactTreeWriter::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGactTreeWriter:   " << IsA()->GetName() << " '" << GetName() 
     << "'" << endl;

  if (!fpTree) return;

  Double_t d_tree_usize = fpTree->GetTotBytes();
  Double_t d_tree_csize = fpTree->GetZipBytes();
  Double_t d_comp = 1.;
  
  if (d_tree_csize > 0.) d_comp = d_tree_usize / d_tree_csize;

  os << "branch name               index/type  "
     <<" bt bid  tot size file size   comp   frac"
     << endl;

  os << "tree:" << Form("%-19s", fpTree->GetName()) << "  "
     << "-- tree ----"
     << "       "
     << Form(" %8.1fk", d_tree_usize/1000)
     << Form(" %8.1fk", d_tree_csize/1000)
     << Form(" %6.2f",  d_comp)
     << endl;

  TIter it_top(fpTree->GetListOfBranches());
  TBranch* p_top;
  while ((p_top= (TBranch*)it_top())) {
    Int_t    i_btyp  = get_be_type(p_top);
    Int_t    i_bid   = get_be_id(p_top);
    Int_t    i_nsub  = 0;
    Double_t d_usize = 0;
    Double_t d_csize = 0;
    Double_t d_comp  = 1.;
    Double_t d_frac  = 0.;
    
    if (i_bid == -2) {
      sum_branch(i_nsub, d_usize, d_csize, p_top);
      if (d_csize > 0.) d_comp = d_usize/d_csize;
    }
    else {
      d_usize = p_top->GetTotalSize();
      d_csize = p_top->GetZipBytes();
      if (d_csize > 0.) d_comp = p_top->GetTotBytes() / d_csize;
    }

    if (d_tree_csize > 0.) d_frac = d_csize / d_tree_csize;

    os << Form("%-24s", p_top->GetName()) << "  ";
    os << "-- branch --";
    os << Form(" %2d %3d", i_btyp, i_bid);
    os << Form(" %8.1fk %8.1fk", d_usize/1000, d_csize/1000)
       << Form(" %6.2f %5.1f%%", d_comp, 100*d_frac) << endl;

    if (strstr(option, "all") != 0) print_branch(os, d_csize, p_top, kFALSE);
  }

  return;
}
