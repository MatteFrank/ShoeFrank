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
class TH2F;
class TH1F;


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
   
   // Base creation of histogram
   virtual void CreateHistogram();

   // decode event
   Bool_t DecodeSensor();

private:
   ifstream          fRawFileAscii;     ///< file streamm
   TAGdataDsc*       fpNtuRaw;               ///< output data dsc
   TAGparaDsc*       fpGeoMap;               ///< geo para dsc
   TAGparaDsc*       fpConfig;               ///< config para dsc
   TAGparaDsc*       fpParMap;               ///< map para dsc
   Int_t             fRunNumber;        ///< run number
   Int_t             fEventSize;        /// event data size
   Int_t             fIndex;        /// event data size

   TH2F*             fpHisPixelMap[10];      ///< pixel map per sensor histogram
   TH1F*             fpHisEvtNumber[10];     ///< event number for each sensor (all 3 frames) histogram

private:
   static const TString  fgkKeyEvent;    ///< event key
   static const TString  fgkKeyDetector; ///< detector key

private:
   // Get Event
   Bool_t GetEvent();
   
   // Set run number
   void   SetRunNumber(const TString& name);

   // Add pixel to list
   virtual void  AddPixel( Int_t input, Int_t value, Int_t aLine, Int_t aColumn);
   
   // Fill histogram pixel
   virtual void FillHistoPixel(Int_t planeId, Int_t aLine, Int_t aColumn);
   
   ClassDef(TAMPactAscReader,0)
};

#endif
