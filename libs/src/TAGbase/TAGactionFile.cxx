/*!
  \file
  \version $Id: TAGactionFile.cxx,v 1.1 2003/07/08 18:51:50 mueller Exp $
  \brief   Implementation of TAGactionFile.
*/

#include "TAGactionFile.hxx"

/*!
  \class TAGactionFile TAGactionFile.hxx "TAGactionFile.hxx"
  \brief Abstract base class for event I/O handling actions **
*/

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

Int_t TAGactionFile::Open(const TString& name, Option_t* option)
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
   
