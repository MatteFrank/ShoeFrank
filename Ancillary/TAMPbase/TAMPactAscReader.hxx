#ifndef _TAMPactAscReader_HXX
#define _TAMPactAscReader_HXX

#include <fstream>

#include "TAGaction.hxx"

/*!
 \file TAMPactAscReader.hxx
  \brief   Declaration of TAMPactAscReader.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;

using namespace std;
class TAMPactAscReader : public TAGaction {
public:
   
   explicit TAMPactAscReader(const char* name=0);
   virtual  ~TAMPactAscReader();
   
   // Action
   virtual Bool_t  Action();
   
   // Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = 0, const TString treeName="tree", Bool_t dscBranch = true);
   
   // close files
   virtual void    Close();
   
   // decode event
   TString DecodeSensor();

private:
   ifstream          fRawFileAscii;     ///< file streamm
   Int_t             fRunNumber;        ///< run number
   vector<UInt_t>    fData;             ///< vector
   Int_t             fEventSize;        /// event data size
   Int_t             fIndex;        /// event data size

private:
   // Get Event
   Bool_t GetEvent();
   
   // Set run number
   void   SetRunNumber(const TString& name);

   ClassDef(TAMPactAscReader,0)
};

#endif
