#ifndef _EVENT_STRUCT_HH
#define _EVENT_STRUCT_HH

/*!
 \file TAMCflukaStruct.hxx
 \brief   Declaration of TEVENT_STRUCT
 */

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
  
  Int_t EventNumber;           ///< event number
  
  Int_t TRn;						 ///< number of particles
  Int_t TRpaid[MAXTR];			 ///< parent id
  Int_t TRgen[MAXTR];			 ///< type
  Int_t TRcha[MAXTR];			 ///< atomic number
  Int_t TRreg[MAXTR];			 ///< region id
  Int_t TRbar[MAXTR];			 ///< baryon number
  Int_t TRdead[MAXTR];			 ///< dead flag
  Int_t TRfid[MAXTR];			 ///< Fluka id
  Double_t TRix[MAXTR];			 ///< initial x position
  Double_t TRiy[MAXTR];			 ///< initial y position
  Double_t TRiz[MAXTR];			 ///< initial z position
  Double_t TRfx[MAXTR];			 ///< final x position
  Double_t TRfy[MAXTR];			 ///< final y position
  Double_t TRfz[MAXTR];			 ///< final z position
  Double_t TRipx[MAXTR];		 ///< initial x momentum
  Double_t TRipy[MAXTR];		 ///< initial y momentum
  Double_t TRipz[MAXTR];		 ///< initial z momentum
  Double_t TRfpx[MAXTR];		 ///< final x momentum
  Double_t TRfpy[MAXTR];		 ///< final y momentum
  Double_t TRfpz[MAXTR];		 ///< final z momentum
  Double_t TRmass[MAXTR];		 ///< mass
  Double_t TRtime[MAXTR];		 ///< time
  Double_t TRtof[MAXTR];       ///< Time of flight
  Double_t TRtrlen[MAXTR];     ///< track length

  Int_t    STCn;               ///< number of STC hits
  Int_t    STCid[MAXSTC];      ///< hit index
  Double_t STCxin[MAXSTC];     ///< input x position
  Double_t STCyin[MAXSTC];     ///< input y position
  Double_t STCzin[MAXSTC];     ///< input z position
  Double_t STCxout[MAXSTC];    ///< output x position
  Double_t STCyout[MAXSTC];    ///< output y position
  Double_t STCzout[MAXSTC];    ///< output z position
  Double_t STCpxin[MAXSTC];    ///< input x momentum
  Double_t STCpyin[MAXSTC];    ///< input y momentum
  Double_t STCpzin[MAXSTC];    ///< input z momentum
  Double_t STCpxout[MAXSTC];   ///< output x momentum
  Double_t STCpyout[MAXSTC];   ///< output y momentum
  Double_t STCpzout[MAXSTC];   ///< output z momentum
  Double_t STCde[MAXSTC];      ///< Energy loss
  Double_t STCal[MAXSTC];      ///< not used
  Double_t STCtim[MAXSTC];     ///< time

  Int_t    BMNn;              ///< number of BM hits
  Int_t    BMNid[MAXBMN];	   ///< hit index
  Int_t    BMNilay[MAXBMN];   ///< layer id
  Int_t    BMNicell[MAXBMN];  ///< cell id
  Int_t    BMNiview[MAXBMN];  ///< view id
  Double_t BMNxin[MAXBMN];    ///< input x position
  Double_t BMNyin[MAXBMN];    ///< input y position
  Double_t BMNzin[MAXBMN];    ///< input z position
  Double_t BMNxout[MAXBMN];   ///< output x position
  Double_t BMNyout[MAXBMN];   ///< output y position
  Double_t BMNzout[MAXBMN];   ///< output z position
  Double_t BMNpxin[MAXBMN];   ///< input x momentum
  Double_t BMNpyin[MAXBMN];   ///< input y momentum
  Double_t BMNpzin[MAXBMN];   ///< input z momentum
  Double_t BMNpxout[MAXBMN];  ///< output x momentum
  Double_t BMNpyout[MAXBMN];  ///< output y momentum
  Double_t BMNpzout[MAXBMN];  ///< output z momentum
  Double_t BMNde[MAXBMN];     ///< Energy loss
  Double_t BMNal[MAXBMN];     ///< not used
  Double_t BMNtim[MAXBMN];    ///< time

  Int_t    VTXn;              ///< number of VTX hits
  Int_t    VTXid[MAXVTX];	   ///< hit index
  Int_t    VTXilay[MAXVTX];   ///< layer id
  Int_t    VTXirow[MAXVTX];   ///< row id (not used)
  Int_t    VTXicol[MAXVTX];   ///< column id (not used)
  Double_t VTXxin[MAXVTX];   	///< input x position
  Double_t VTXyin[MAXVTX];   	///< input y position
  Double_t VTXzin[MAXVTX];   	///< input z position
  Double_t VTXxout[MAXVTX];  	///< output x position
  Double_t VTXyout[MAXVTX];  	///< output y position
  Double_t VTXzout[MAXVTX];  	///< output z position
  Double_t VTXpxin[MAXVTX];  	///< input x momentum
  Double_t VTXpyin[MAXVTX];  	///< input y momentum
  Double_t VTXpzin[MAXVTX];  	///< input z momentum
  Double_t VTXpxout[MAXVTX]; 	///< output x momentum
  Double_t VTXpyout[MAXVTX]; 	///< output y momentum
  Double_t VTXpzout[MAXVTX]; 	///< output z momentum
  Double_t VTXde[MAXVTX];    	///< Energy loss
  Double_t VTXal[MAXVTX];    	///< not used
  Double_t VTXtim[MAXVTX];    ///< time

  Int_t    ITRn;              ///< number of ITR hits
  Int_t    ITRid[MAXITR];	   ///< hit index
  Int_t    ITRisens[MAXITR];  ///< sensor id
  Int_t    ITRilay[MAXITR];   ///< layer id
  Int_t    ITRiplume[MAXITR]; ///< plume id (not used)
  Int_t    ITRimimo[MAXITR];  ///< mimosa id (not used)
  Int_t    ITRirow[MAXITR];   ///< row id (not used)
  Int_t    ITRicol[MAXITR];   ///< column id (not used)
  Double_t ITRxin[MAXITR];    ///< input x position
  Double_t ITRyin[MAXITR];    ///< input y position
  Double_t ITRzin[MAXITR];    ///< input z position
  Double_t ITRxout[MAXITR];   ///< output x position
  Double_t ITRyout[MAXITR];   ///< output y position
  Double_t ITRzout[MAXITR];   ///< output z position
  Double_t ITRpxin[MAXITR];   ///< input x momentum
  Double_t ITRpyin[MAXITR];   ///< input y momentum
  Double_t ITRpzin[MAXITR];   ///< input z momentum
  Double_t ITRpxout[MAXITR];  ///< output x momentum
  Double_t ITRpyout[MAXITR];  ///< output y momentum
  Double_t ITRpzout[MAXITR];  ///< output z momentum
  Double_t ITRde[MAXITR];     ///< Energy loss
  Double_t ITRal[MAXITR];     ///< not used
  Double_t ITRtim[MAXITR];    ///< time
  
  Int_t    MSDn;				   ///< number of MSD hits
  Int_t    MSDid[MAXMSD];		///< hit index
  Int_t    MSDilay[MAXMSD];	///< layer id
  Int_t    MSDistripx[MAXMSD];///< stripx id (not used)
  Int_t    MSDistripy[MAXMSD];///< stripy id (not used)
  Double_t MSDxin[MAXMSD];		///< input x position
  Double_t MSDyin[MAXMSD];		///< input y position
  Double_t MSDzin[MAXMSD];		///< input z position
  Double_t MSDxout[MAXMSD];	///< output x position
  Double_t MSDyout[MAXMSD];	///< output y position
  Double_t MSDzout[MAXMSD];	///< output z position
  Double_t MSDpxin[MAXMSD];	///< input x momentum
  Double_t MSDpyin[MAXMSD];	///< input y momentum
  Double_t MSDpzin[MAXMSD];	///< input z momentum
  Double_t MSDpxout[MAXMSD];  ///< output x momentum
  Double_t MSDpyout[MAXMSD];  ///< output y momentum
  Double_t MSDpzout[MAXMSD];  ///< output z momentum
  Double_t MSDde[MAXMSD];		///< Energy loss
  Double_t MSDal[MAXMSD];		///< not used
  Double_t MSDtim[MAXMSD];    ///< time

  Int_t    SCNn;              ///< number of TW hits
  Int_t    SCNid[MAXSCN];	   ///< hit index
  Int_t    SCNibar[MAXSCN];   ///< bar id
  Int_t    SCNiview[MAXSCN];  ///< view id
  Double_t SCNxin[MAXSCN];    ///< input x position
  Double_t SCNyin[MAXSCN];    ///< input y position
  Double_t SCNzin[MAXSCN];    ///< input z position
  Double_t SCNxout[MAXSCN];   ///< output x position
  Double_t SCNyout[MAXSCN];   ///< output y position
  Double_t SCNzout[MAXSCN];   ///< output z position
  Double_t SCNpxin[MAXSCN];   ///< input x momentum
  Double_t SCNpyin[MAXSCN];   ///< input y momentum
  Double_t SCNpzin[MAXSCN];   ///< input z momentum
  Double_t SCNpxout[MAXSCN];  ///< output x momentum
  Double_t SCNpyout[MAXSCN];  ///< output y momentum
  Double_t SCNpzout[MAXSCN];  ///< output z momentum
  Double_t SCNde[MAXSCN];     ///< Energy loss
  Double_t SCNal[MAXSCN];     ///< not used
  Double_t SCNtim[MAXSCN];    ///< time

  Int_t    CALn;               ///< number of CAL hits
  Int_t    CALid[MAXCAL];	    ///< hit index
  Int_t    CALicry[MAXCAL];    ///< crystal id
  Double_t CALxin[MAXCAL];     ///< input x position
  Double_t CALyin[MAXCAL];     ///< input y position
  Double_t CALzin[MAXCAL];     ///< input z position
  Double_t CALxout[MAXCAL];    ///< output x position
  Double_t CALyout[MAXCAL];    ///< output y position
  Double_t CALzout[MAXCAL];    ///< output z position
  Double_t CALpxin[MAXCAL];    ///< input x momentum
  Double_t CALpyin[MAXCAL];    ///< input y momentum
  Double_t CALpzin[MAXCAL];    ///< input z momentum
  Double_t CALpxout[MAXCAL];   ///< output x momentum
  Double_t CALpyout[MAXCAL];   ///< output y momentum
  Double_t CALpzout[MAXCAL];   ///< output z momentum
  Double_t CALde[MAXCAL];      ///< Energy loss
  Double_t CALal[MAXCAL];      ///< not used
  Double_t CALtim[MAXCAL];     ///< time

  Int_t    CROSSn;                ///< Number of regions
  Int_t    CROSSid[MAXCROSS];     ///< region index
  Int_t    CROSSnreg[MAXCROSS];   ///< region number
  Int_t    CROSSnregold[MAXCROSS];///< old region number
  Double_t CROSSx[MAXCROSS];      ///< crossing x position
  Double_t CROSSy[MAXCROSS];      ///< crossing y position
  Double_t CROSSz[MAXCROSS];      ///< crossing z position
  Double_t CROSSpx[MAXCROSS];     ///< crossing x momentum
  Double_t CROSSpy[MAXCROSS];     ///< crossing y momentum
  Double_t CROSSpz[MAXCROSS];     ///< crossing z momentum
  Double_t CROSSm[MAXCROSS];      ///< mass
  Double_t CROSSch[MAXCROSS];     ///< atomic charge
  Double_t CROSSt[MAXCROSS];      ///< time

} EVENT_STRUCT;

#endif
