
#ifndef _TAGeventDisplayMC_HXX_
#define _TAGeventDisplayMC_HXX_

/*!
 \file TAGeventDisplayMC.hxx
 \brief FOOT class to work on MC event display
 \author Ch. Finck
 */
/*------------------------------------------+---------------------------------*/

#include "TAGbaseEventDisplay.hxx"
#include "TAEDpoint.hxx"

class TTree;

class TAGeventDisplayMC : public TAGbaseEventDisplay
{
   
protected:
   // default constructor
   TAGeventDisplayMC(const TString expName, Int_t runNumber, Int_t type);
   
protected:
   static TAGeventDisplayMC* fgInstance; ///< static instance of class
   
public:
   // Instance of class
   static TAGeventDisplayMC* Instance(const TString name = "", Int_t runNumber = -1, Int_t type = 2);
   
   virtual ~TAGeventDisplayMC();
   
   // Tree entry
   Bool_t GetEntry(Int_t entry);
   
   // Update elements
   void UpdateMcElements();

   // Add elements
   void AddMcElements();

   // Connect elements
   void ConnectMcElements();

   // Set local reconstruction
   void SetLocalReco();

   // Update STC MC info
   void UpdateStInfo(Int_t idx);
   // Update BM MC info
   void UpdateBmInfo(Int_t idx);
   // Update VTX MC info
   void UpdateVtInfo(Int_t idx);
   // Update ITR MC info
   void UpdateItInfo(Int_t idx);
   // Update MSD MC info
   void UpdateMsInfo(Int_t idx);
   // Update TW MC info
   void UpdateTwInfo(Int_t idx);
   // Update CAL MC info
   void UpdateCaInfo(Int_t idx);
   
private:
   //Display
   TAEDpoint*      fStMcDisplay;        ///< STC MC hits display
   TAEDpoint*      fBmMcDisplay;        ///< BM MC hits display
   TAEDpoint*      fVtMcDisplay;        ///< VTX MC hits display
   TAEDpoint*      fItMcDisplay;        ///< ITR MC hits display
   TAEDpoint*      fMsdMcDisplay;       ///< MSD MC hits display
   TAEDpoint*      fTwMcDisplay;        ///< TW MC hits display
   TAEDpoint*      fCaMcDisplay;        ///< CAL MC hits display

   TAEDglbTrack*   fPartTrackDisplay;   ///< list of line to display MC particle track


private:
   // Update MC elements
   void  UpdateMcElements(const TString prefix);
   // Update MC info
   void  UpdateMcInfo(TString prefix, Int_t idx);
   
   // Add required MC item for STC
   void AddRequiredMcItemSt();
   // Add required MC item for BM
   void AddRequiredMcItemBm();
   // Add required MC item for VTX
   void AddRequiredMcItemVt();
   // Add required MC item for ITR
   void AddRequiredMcItemIt();
   // Add required MC item for MSD
   void AddRequiredMcItemMs();
   // Add required MC item for TW
   void AddRequiredMcItemTw();
   // Add required MC item for CAL
   void AddRequiredMcItemCa();

   ClassDef(TAGeventDisplayMC, 2); ///< Class for MC event display
};

#endif
