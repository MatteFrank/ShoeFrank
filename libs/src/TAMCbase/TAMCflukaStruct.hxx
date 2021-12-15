#ifndef _EVENT_STRUCT_HH
#define _EVENT_STRUCT_HH

#include "Rtypes.h"

const int MAXTR    = 6000;  
const int MAXSTC   = 200;   
const int MAXBMN   = 1000;   
const int MAXVTX   = 300;   
const int MAXITR   = 300;   
const int MAXMSD   = 1000;   
const int MAXSCN   = 5000;  
const int MAXCAL   = 6000;  
const int MAXCROSS = 10000;

/*!
 \struct EVENT_STRUCT
 \brief  Fluka event structure
 */
typedef struct {
  
  Int_t EventNumber;
  
  Int_t TRn;							      
  Int_t TRpaid[MAXTR];					   
  Int_t TRgen[MAXTR];					   
  Int_t TRcha[MAXTR];					   
  Int_t TRreg[MAXTR];					   
  Int_t TRbar[MAXTR];					   
  Int_t TRdead[MAXTR];					   
  Int_t TRfid[MAXTR];					   
  Double_t TRix[MAXTR];					   
  Double_t TRiy[MAXTR];					   
  Double_t TRiz[MAXTR];					   
  Double_t TRfx[MAXTR];					   
  Double_t TRfy[MAXTR];					   
  Double_t TRfz[MAXTR];					   
  Double_t TRipx[MAXTR];					   
  Double_t TRipy[MAXTR];					   
  Double_t TRipz[MAXTR];					   
  Double_t TRfpx[MAXTR];					   
  Double_t TRfpy[MAXTR];					   
  Double_t TRfpz[MAXTR];					   
  Double_t TRmass[MAXTR];					   
  Double_t TRtime[MAXTR];					   
  Double_t TRtof[MAXTR];  
  Double_t TRtrlen[MAXTR];

  Int_t    STCn;               
  Int_t    STCid[MAXSTC];      
  Double_t STCxin[MAXSTC];   
  Double_t STCyin[MAXSTC];   
  Double_t STCzin[MAXSTC];   
  Double_t STCxout[MAXSTC];  
  Double_t STCyout[MAXSTC];  
  Double_t STCzout[MAXSTC];  
  Double_t STCpxin[MAXSTC];  
  Double_t STCpyin[MAXSTC];  
  Double_t STCpzin[MAXSTC];  
  Double_t STCpxout[MAXSTC]; 
  Double_t STCpyout[MAXSTC]; 
  Double_t STCpzout[MAXSTC]; 
  Double_t STCde[MAXSTC];      
  Double_t STCal[MAXSTC];      
  Double_t STCtim[MAXSTC];   

  Int_t    BMNn;               
  Int_t    BMNid[MAXBMN];	     
  Int_t    BMNilay[MAXBMN];    
  Int_t    BMNicell[MAXBMN];   
  Int_t    BMNiview[MAXBMN];   
  Double_t BMNxin[MAXBMN];   
  Double_t BMNyin[MAXBMN];   
  Double_t BMNzin[MAXBMN];   
  Double_t BMNxout[MAXBMN];  
  Double_t BMNyout[MAXBMN];  
  Double_t BMNzout[MAXBMN];  
  Double_t BMNpxin[MAXBMN];  
  Double_t BMNpyin[MAXBMN];  
  Double_t BMNpzin[MAXBMN];  
  Double_t BMNpxout[MAXBMN]; 
  Double_t BMNpyout[MAXBMN]; 
  Double_t BMNpzout[MAXBMN]; 
  Double_t BMNde[MAXBMN];    
  Double_t BMNal[MAXBMN];    
  Double_t BMNtim[MAXBMN];   

  Int_t    VTXn;                	
  Int_t    VTXid[MAXVTX];	     	
  Int_t    VTXilay[MAXVTX];     
  Int_t    VTXirow[MAXVTX];     
  Int_t    VTXicol[MAXVTX];     
  Double_t VTXxin[MAXVTX];   	
  Double_t VTXyin[MAXVTX];   	
  Double_t VTXzin[MAXVTX];   	
  Double_t VTXxout[MAXVTX];  	
  Double_t VTXyout[MAXVTX];  	
  Double_t VTXzout[MAXVTX];  	
  Double_t VTXpxin[MAXVTX];  	
  Double_t VTXpyin[MAXVTX];  	
  Double_t VTXpzin[MAXVTX];  	
  Double_t VTXpxout[MAXVTX]; 	
  Double_t VTXpyout[MAXVTX]; 	
  Double_t VTXpzout[MAXVTX]; 	
  Double_t VTXde[MAXVTX];    	
  Double_t VTXal[MAXVTX];    	
  Double_t VTXtim[MAXVTX];   

  Int_t    ITRn;                
  Int_t    ITRid[MAXITR];	       
  Int_t    ITRisens[MAXITR];   	       
  Int_t    ITRilay[MAXITR];    	       
  Int_t    ITRiplume[MAXITR]; 	       
  Int_t    ITRimimo[MAXITR];    
  Int_t    ITRirow[MAXITR];       
  Int_t    ITRicol[MAXITR];       
  Double_t ITRxin[MAXITR];   
  Double_t ITRyin[MAXITR];   
  Double_t ITRzin[MAXITR];   
  Double_t ITRxout[MAXITR];  
  Double_t ITRyout[MAXITR];  
  Double_t ITRzout[MAXITR];  
  Double_t ITRpxin[MAXITR];  
  Double_t ITRpyin[MAXITR];  
  Double_t ITRpzin[MAXITR];  
  Double_t ITRpxout[MAXITR]; 
  Double_t ITRpyout[MAXITR]; 
  Double_t ITRpzout[MAXITR]; 
  Double_t ITRde[MAXITR];      
  Double_t ITRal[MAXITR];      
  Double_t ITRtim[MAXITR];   
  
  Int_t    MSDn;				
  Int_t    MSDid[MAXMSD];		
  Int_t    MSDilay[MAXMSD];		
  Int_t    MSDistripx[MAXMSD];		
  Int_t    MSDistripy[MAXMSD];		
  Double_t MSDxin[MAXMSD];		
  Double_t MSDyin[MAXMSD];		
  Double_t MSDzin[MAXMSD];		
  Double_t MSDxout[MAXMSD];		
  Double_t MSDyout[MAXMSD];		
  Double_t MSDzout[MAXMSD];		
  Double_t MSDpxin[MAXMSD];		
  Double_t MSDpyin[MAXMSD];		
  Double_t MSDpzin[MAXMSD];		
  Double_t MSDpxout[MAXMSD];		
  Double_t MSDpyout[MAXMSD];		
  Double_t MSDpzout[MAXMSD];		
  Double_t MSDde[MAXMSD];		
  Double_t MSDal[MAXMSD];		
  Double_t MSDtim[MAXMSD];      

  Int_t    SCNn;                   
  Int_t    SCNid[MAXSCN];	      
  Int_t    SCNibar[MAXSCN];      
  Int_t    SCNiview[MAXSCN];      
  Double_t SCNxin[MAXSCN];    
  Double_t SCNyin[MAXSCN];    
  Double_t SCNzin[MAXSCN];    
  Double_t SCNxout[MAXSCN];   
  Double_t SCNyout[MAXSCN];   
  Double_t SCNzout[MAXSCN];   
  Double_t SCNpxin[MAXSCN];   
  Double_t SCNpyin[MAXSCN];   
  Double_t SCNpzin[MAXSCN];   
  Double_t SCNpxout[MAXSCN]; 
  Double_t SCNpyout[MAXSCN]; 
  Double_t SCNpzout[MAXSCN]; 
  Double_t SCNde[MAXSCN];     
  Double_t SCNal[MAXSCN];     
  Double_t SCNtim[MAXSCN];   

  Int_t    CALn;                 
  Int_t    CALid[MAXCAL];	      
  Int_t    CALicry[MAXCAL];      
  Double_t CALxin[MAXCAL];    
  Double_t CALyin[MAXCAL];    
  Double_t CALzin[MAXCAL];    
  Double_t CALxout[MAXCAL];   
  Double_t CALyout[MAXCAL];   
  Double_t CALzout[MAXCAL];   
  Double_t CALpxin[MAXCAL];   
  Double_t CALpyin[MAXCAL];   
  Double_t CALpzin[MAXCAL];   
  Double_t CALpxout[MAXCAL];  
  Double_t CALpyout[MAXCAL];  
  Double_t CALpzout[MAXCAL];  
  Double_t CALde[MAXCAL];     
  Double_t CALal[MAXCAL];     
  Double_t CALtim[MAXCAL];        

  Int_t    CROSSn;
  Int_t    CROSSid[MAXCROSS];
  Int_t    CROSSnreg[MAXCROSS];
  Int_t    CROSSnregold[MAXCROSS];
  Double_t CROSSx[MAXCROSS];
  Double_t CROSSy[MAXCROSS];
  Double_t CROSSz[MAXCROSS];
  Double_t CROSSpx[MAXCROSS];
  Double_t CROSSpy[MAXCROSS];
  Double_t CROSSpz[MAXCROSS];
  Double_t CROSSm[MAXCROSS];
  Double_t CROSSch[MAXCROSS];
  Double_t CROSSt[MAXCROSS];

} EVENT_STRUCT;

#endif
