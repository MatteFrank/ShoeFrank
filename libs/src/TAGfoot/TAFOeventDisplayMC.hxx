
#ifndef _TAFOeventDisplayMC_HXX_
#define _TAFOeventDisplayMC_HXX_


/** TAFOeventDisplay FOOT class to work on MC event display
 
 */

#include "TAFOeventDisplay.hxx"
#include "TAEDpoint.hxx"

class TTree;

class TAFOeventDisplayMC : public TAFOeventDisplay
{
   
protected:
   //! default constructor
   TAFOeventDisplayMC(Int_t type, const TString expName);
   
public:
   //! Instance of class
   static TAFOeventDisplay* Instance(Int_t type = 0, const TString name = "");
   
   virtual ~TAFOeventDisplayMC();
   
   //! Tree entry
   Bool_t GetEntry(Int_t entry);
   
   //! Update elements
   void UpdateElements();

   //! Add elements
   void AddElements();

   //! Connect elements
   void ConnectElements();

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

   ClassDef(TAFOeventDisplayMC, 1); // Base class for event display
};


#endif
