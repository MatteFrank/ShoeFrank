/*!
  \file TAGactionFile.cxx
  \brief   Implementation of TAGactionFile.
*/

#include "TAGactionFile.hxx"

/*!
  \class TAGactionFile TAGactionFile.hxx "TAGactionFile.hxx"
  \brief Abstract base class for event I/O handling actions **
*/

UInt_t TAGactionFile::fgCurrentTriggerCnt = 0;

//------------------------------------------+-----------------------------------
//! Default constructor.

TAGactionFile::TAGactionFile(const char* name, const char* title,
			     const char* openopt)
  : TAGaction(name, title),
    fOpenOpt("READ")
{
  if (openopt) fOpenOpt = openopt;
}

//------------------------------------------+-----------------------------------
//! Destructor.

TAGactionFile::~TAGactionFile()
{
  Close();
}

//------------------------------------------+-----------------------------------
//! Open file.

Int_t TAGactionFile::Open(const TString& name, Option_t* option, const TString treeName, Bool_t dscBranch)
{
  MayNotUse("Open");
  return -1;
}

//------------------------------------------+-----------------------------------
//! Close file.

void TAGactionFile::Close()
{
  return;
}

//------------------------------------------+-----------------------------------
//! Reset

void TAGactionFile::Reset(Int_t iEvent)
{
   return;
}

//------------------------------------------+-----------------------------------
//! Returns \a true if a file is open.

Bool_t TAGactionFile::IsOpen() const
{
  return kFALSE;
}

//------------------------------------------+-----------------------------------
//! SetupChannel for mbs reader only
void TAGactionFile::SetupChannel(TAGdataDsc* p_data, TAGnamed* p_filt)
{
   return;  
}

//------------------------------------------+-----------------------------------
//! SetupBranch for tree reader only
void TAGactionFile::SetupBranch(TAGdataDsc* p_data, const char* branch)
{
   return;
}

//------------------------------------------+-----------------------------------
//! Add friend tree
void TAGactionFile::AddFriendTree(TString /*fileName*/, TString /*treeName*/)
{
   return;
}

// --------------------------------------------------------------------------------------
Bool_t TAGactionFile::CheckTriggerCnt(UInt_t trig)
{
   if (fgCurrentTriggerCnt != trig)
      return false;
   
   return true;
}

