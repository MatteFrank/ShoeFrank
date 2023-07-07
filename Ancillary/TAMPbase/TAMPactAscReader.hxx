#ifndef _TAMPactAscReader_HXX
#define _TAMPactAscReader_HXX

#include <fstream>

#include "TAGactionFile.hxx"

/*!
 \file TAMPactAscReader.hxx
  \brief   Declaration of TAMPactAscReader.
 */

/*------------------------------------------+---------------------------------*/
class TAGdataDsc;

using namespace std;
class TAMPactAscReader : public TAGactionFile {
public:
   
   explicit TAMPactAscReader(const char* name     = 0,
                             TAGdataDsc* p_datraw = 0,
                             TAGparaDsc* p_geomap = 0,
                             TAGparaDsc* p_config = 0,
                             TAGparaDsc* pParMap  = 0);
   virtual  ~TAMPactAscReader();
   
   // Action
   virtual Bool_t  Action();
   
   // Open file with a given prefix and suffix for the files
   virtual Int_t   Open(const TString& prefix, Option_t* opt = 0, const TString treeName="tree", Bool_t dscBranch = true);
   
   // close files
   virtual void    Close();
   
   // decode event
   Bool_t DecodeSensor();

private:
   ifstream          fRawFileAscii;     ///< file streamm
   TAGdataDsc*       fpNtuRaw;               ///< output data dsc
   TAGparaDsc*       fpGeoMap;               ///< geo para dsc
   TAGparaDsc*       fpConfig;               ///< config para dsc
   TAGparaDsc*       fpParMap;               ///< map para dsc
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
