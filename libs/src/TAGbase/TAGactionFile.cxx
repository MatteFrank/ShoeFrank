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
//!
//! \param[in] name action name
//! \param[in] title action title name
//! \param[in] openopt open file options
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
//!
//! \param[in] name action name
//! \param[in] option open file options
//! \param[in] treeName name of tree in file
//! \param[in] dscBranch flag for object descriptor
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
//!
//! \param[in] iEvent event index
void TAGactionFile::Reset(Int_t iEvent)
{
   return;
}

//------------------------------------------+-----------------------------------
//! Returns true if a file is open.
Bool_t TAGactionFile::IsOpen() const
{
  return kFALSE;
}

//------------------------------------------+-----------------------------------
//! SetupChannel for DAQ reader only
//!
//! \param[in] p_data data descriptor
//! \param[in] p_filt name
void TAGactionFile::SetupChannel(TAGdataDsc* p_data, TAGnamed* p_filt)
{
   return;  
}

//------------------------------------------+-----------------------------------
//! SetupBranch for tree reader only
//!
//! \param[in] p_data data descriptor
//! \param[in] branch name of branch
void TAGactionFile::SetupBranch(TAGdataDsc* p_data, const char* branch)
{
   return;
}

//------------------------------------------+-----------------------------------
//! Add friend tree
//!
//! \param[in] fileName root file name
//! \param[in] treeName tree name
void TAGactionFile::AddFriendTree(TString /*fileName*/, TString /*treeName*/)
{
   return;
}

//------------------------------------------+-----------------------------------
//! Check trigger count
//!
//! \param[in] trig trigger number to check
Bool_t TAGactionFile::CheckTriggerCnt(UInt_t trig)
{
   if (fgCurrentTriggerCnt != trig)
      return false;
   
   return true;
}

