#ifndef _TAVTmi26Type_HXX_
#define _TAVTmi26Type_HXX_
/*!
 \file TAVTmi26Type.hxx
 \version $Id: TAVTmi26Type
 \brief   Declaration of TAVTmi26Type
 */
/*------------------------------------------+---------------------------------*/
/*
 *  TAVTmi26Type.h
 *  First
 *
 *  Created by christian finck on 21/9/11.
 *  Copyright 2011 IPHC. All rights reserved.
 *
 */
// structure for PXI format

#define MI26__FFRAME_RAW_MAX_W16  1140

// structure for MBS and ascii file
typedef struct {
   
   UInt_t Header;
   UInt_t TriggerCnt;
   UInt_t TimeStamp;
   UInt_t FrameCnt;
   UInt_t DataLength;
   
   UShort_t ADataW16[MI26__FFRAME_RAW_MAX_W16];
   UInt_t Trailer;
   
} MI26_FrameRaw; 

// structure of state and line
typedef union {
   
   UShort_t W16;
   
   struct {
	  
	  UShort_t StateNb  :  4;
     UShort_t LineAddr : 10;
     UShort_t NotUsed  :  1;
	  UShort_t Ovf      :  1;
	  
   } F;
   
} MI26__TStatesLine;

typedef union {
   
   UShort_t W16;
   
   struct {
	  
	  UShort_t HitNb   :  2;
	  UShort_t ColAddr : 10;
	  UShort_t NotUsed :  4;
	  
   } F;
   
} MI26__TState;

#endif
