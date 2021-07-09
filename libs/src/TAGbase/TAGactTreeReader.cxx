/*!
  \file
  \version $Id: TAGactTreeReader.cxx,v 1.17 2003/07/16 19:32:54 mueller Exp $
  \brief   Implementation of TAGactTreeReader.
*/

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TDirectory.h"
#include "TString.h"

#include "TAGroot.hxx"
#include "TAGrunInfo.hxx"

#include "TAGactTreeReader.hxx"
#include "TAGrecoManager.hxx"

/*!
  \class TAGactTreeReader TAGactTreeReader.hxx "TAGactTreeReader.hxx"
  \brief Read object tree's. **
*/

ClassImp(TAGactTreeReaderBranch);
ClassImp(TAGactTreeReader);


//------------------------------------------+-----------------------------------
//! Default constructor.
TAGactTreeReaderBranch::TAGactTreeReaderBranch(TAGdataDsc* p_data, TString name)
: fpDataDsc(p_data),
  fName(name),
  fpBranch(0),
  fiNByte(0)
{
}

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGactTreeReader::TAGactTreeReader(const char* name)
  : TAGactionFile(name, "TAGactTreeReader - Tree reader", "READ"),
    fpBranchList(0),
    fpFile(0),
    fpTree(0),
    fiNEntry(0),
    fiCurrentEntry( TAGrecoManager::GetPar()->SkipN() ),
    fbDscBranch(true)
{
  fpBranchList = new TList();
  fpBranchList->SetOwner(kTRUE);
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGactTreeReader::~TAGactTreeReader()
{
  delete fpBranchList;
  delete fpFile;
}

//------------------------------------------+-----------------------------------
//! Add input data descriptor.

void TAGactTreeReader::SetupBranch(TAGdataDsc* p_data, const char* branch)
{
  TAGactTreeReaderBranch* p_chan = new TAGactTreeReaderBranch(p_data, branch);
  AddDataOut(p_data, "TAGdata");	    // ??? be more specific ???
  fpBranchList->Add(p_chan);
  return;
}

//------------------------------------------+-----------------------------------
//! Open root file.

Int_t TAGactTreeReader::NEvents()
{
  return fiNEntry;
}

//------------------------------------------+-----------------------------------
//! Check branch

Bool_t TAGactTreeReader::CheckBranch(const char* branch, Bool_t verbose)
{
  TBranch* p_branch = fpTree->GetBranch(branch);
  if (p_branch) {
    return true;
  } else {
    if (verbose)
      Warning("CheckBranch()", "Failed to find branch '%s'", branch);
    return false;
  }
}

//------------------------------------------+-----------------------------------
//! Open root file.

Int_t TAGactTreeReader::Open(const TString& name, Option_t* option, const TString treeName, Bool_t dscBranch)
{
  fbDscBranch = dscBranch;
  TDirectory* p_cwd = gDirectory;
  
  gTAGroot->SetEventNumber(-1);

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

  fpTree = (TTree*) fpFile->Get(treeName.Data());
  if (!fpTree) {
    Error("Open()", "No object named 'tree' found");
    Close();
    gDirectory = p_cwd;
    return 2;
  }

  if (!fpTree->InheritsFrom("TTree")) {
    Error("Open()", "Object named 'tree' is not a TTree");
    Close();
    gDirectory = p_cwd;
    return 3;
  }

	if (fbDscBranch) {
	for (TObjLink* lnk = fpBranchList->FirstLink(); lnk; lnk=lnk->Next()) {
			TAGactTreeReaderBranch* p_chan =(TAGactTreeReaderBranch*)lnk->GetObject();
			TBranch* p_branch = fpTree->GetBranch(p_chan->fName);
			if (p_branch) {
				p_branch->SetAddress(p_chan->fpDataDsc->ObjectPointer());
			} else {
				Warning("Open()", "Failed to find branch '%s'", p_chan->fName.Data());
			}
			p_chan->fpBranch = p_branch;
			p_chan->fiNByte  = 0;
		}
	}
  
  fiNEntry = (Int_t) fpTree->GetEntries();
  fiCurrentEntry = TAGrecoManager::GetPar()->SkipN();

  TAGrunInfo* p_ri = (TAGrunInfo*) fpFile->Get(TAGrunInfo::GetObjectName());

  if (p_ri  && p_ri->InheritsFrom("TAGrunInfo")) {
    gTAGroot->SetRunInfo(*p_ri);
  } else {
    Warning("Open()", "No object named 'runinfo' found");
  } 

  delete p_ri;

  gDirectory = p_cwd;
  return 0;
}

//------------------------------------------+-----------------------------------
//! Close file.

void TAGactTreeReader::Close()
{
  delete fpFile;
  fpFile   = 0;
  fpTree   = 0;
  fiNEntry = 0;
  fiCurrentEntry = -1;
  return;
}

//------------------------------------------+-----------------------------------
//! Returns \a true if a file is currently open.

Bool_t TAGactTreeReader::IsOpen() const
{
  return fpFile != 0;
}

//------------------------------------------+-----------------------------------
//! Reset

void TAGactTreeReader::Reset(Int_t iEvent)
{
   fiCurrentEntry = iEvent-1;
   return;
}

//------------------------------------------+-----------------------------------
//! Process TreeReader.

Bool_t TAGactTreeReader::Process()	{
	
  if (Valid()) return kTRUE;
  if (IsZombie()) return kFALSE;

  if (fpTree) {
    if (fiCurrentEntry + 1 < fiNEntry) {
      fiCurrentEntry += 1;
      SetBit(kValid);
    } else {
      SetBit(kEof);
      SetBitAllDataOut(kEof);
    }

  } else {
    Error("Process()","No input file opened");
    SetBit(kFail);
    SetBitAllDataOut(kFail);
   return kFALSE;
  }
  
	if (Valid()) {
		if (fbDscBranch) {
			for (TObjLink* lnk = fpBranchList->FirstLink(); lnk; lnk=lnk->Next()) {
					TAGactTreeReaderBranch* p_chan =(TAGactTreeReaderBranch*)lnk->GetObject();
					if (p_chan->fpBranch) {
						Int_t i_nbyte = p_chan->fpBranch->GetEntry(fiCurrentEntry);
						p_chan->fiNByte += i_nbyte;
					}
					p_chan->fpDataDsc->SetBit(kValid);
				}
			// }
		} 
		else {
			fpTree->GetEntry(fiCurrentEntry);
		}
	}

  return Valid();
}

//------------------------------------------+-----------------------------------
//! ostream insertion.

void TAGactTreeReader::ToStream(ostream& os, Option_t* option) const
{
  os << "TAGactTreeReader:   " << IsA()->GetName() << " '" << GetName() 
     << "'" << endl;

  if (fpBranchList) {
    const char* pref = "  branches:      ";
    for (TObjLink* lnk = fpBranchList->FirstLink(); lnk; lnk=lnk->Next()) {
      TAGactTreeReaderBranch* p = (TAGactTreeReaderBranch*) lnk->GetObject();
      os << pref << "'" << p->fName << "'  " 
	 << Form("%10d", p->fiNByte) << endl;
      pref = "                 ";
    }
  }

  return;
}

