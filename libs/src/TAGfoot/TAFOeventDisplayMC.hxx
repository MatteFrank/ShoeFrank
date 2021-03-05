
#ifndef _TAFOeventDisplayMC_HXX_
#define _TAFOeventDisplayMC_HXX_


/** TAFOeventDisplay FOOT class to work on MC event display
 
 */

#include "TAFObaseEventDisplay.hxx"
#include "TAEDpoint.hxx"

class TTree;

class TAFOeventDisplayMC : public TAFObaseEventDisplay
{
   
protected:
   //! default constructor
   TAFOeventDisplayMC(const TString expName, Int_t runNumber, Int_t type);
   
protected:
   static TAFOeventDisplayMC* fgInstance; // static instance of class
   
public:
   //! Instance of class
   static TAFOeventDisplayMC* Instance(const TString name = "", Int_t runNumber = -1, Int_t type = 2);
   
   virtual ~TAFOeventDisplayMC();
   
   //! Tree entry
   Bool_t GetEntry(Int_t entry);
   
   //! Update elements
   void UpdateMcElements();

   //! Add elements
   void AddMcElements();

   //! Connect elements
   void ConnectMcElements();

   //! Set local reconstruction
   void SetLocalReco();

   //! Update MC info
   void UpdateStInfo(Int_t idx);
   void UpdateBmInfo(Int_t idx);
   void UpdateVtInfo(Int_t idx);
   void UpdateItInfo(Int_t idx);
   void UpdateMsInfo(Int_t idx);
   void UpdateTwInfo(Int_t idx);
   void UpdateCaInfo(Int_t idx);
   
private:
   //Display
   TAEDpoint*      fStMcDisplay;
   TAEDpoint*      fBmMcDisplay;
   TAEDpoint*      fVtMcDisplay;
   TAEDpoint*      fItMcDisplay;
   TAEDpoint*      fMsdMcDisplay;
   TAEDpoint*      fTwMcDisplay;
   TAEDpoint*      fCaMcDisplay;

private:
   void  UpdateMcElements(const TString prefix);
   void  UpdateMcInfo(TString prefix, Int_t idx);
   
   void AddRequiredMcItemSt();
   void AddRequiredMcItemBm();
   void AddRequiredMcItemVt();
   void AddRequiredMcItemIt();
   void AddRequiredMcItemMs();
   void AddRequiredMcItemTw();
   void AddRequiredMcItemCa();

   ClassDef(TAFOeventDisplayMC, 2); // Class for MC event display
};


#endif
