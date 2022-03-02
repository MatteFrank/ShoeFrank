#ifndef _TAVTmi26Type_HXX_
#define _TAVTmi26Type_HXX_
/*!
 \file TAVTmi26Type.hxx
 \brief   Declaration of TAVTmi26Type
 */
/*------------------------------------------+---------------------------------*/
/*
 *
 *  Created by Ch. Finck on 21/9/11.
 *  Copyright 2011 IPHC. All rights reserved.
 *
 */
// structure for PXI format

#define MI26__FFRAME_RAW_MAX_W16  1140 ///< maximum word in frame structure

/*!
 \struct MI26_FrameRaw
 \brief  Data frame structure
*/
typedef struct {
   
   UInt_t Header;       ///< Key header
   UInt_t TriggerCnt;   ///< Trigger counter
   UInt_t TimeStamp;    ///< Time stamp
   UInt_t FrameCnt;     ///< Frame counter
   UInt_t DataLength;   ///< Data length
   
   UShort_t ADataW16[MI26__FFRAME_RAW_MAX_W16]; ///< Data array
   UInt_t Trailer;      ///< Key trailler
   
} MI26_FrameRaw; 

// structure of state and line

/*!
 \struct MI26__TStatesLine
 \brief  State line status
*/
typedef union {
   
   UShort_t W16;             ///< Word
   
   //! Frame state line structure
   struct {
	  
	  UShort_t StateNb  :  4; ///< Number of states
     UShort_t LineAddr : 10; ///< Line adress
     UShort_t NotUsed  :  1; ///< Not used
	  UShort_t Ovf      :  1; ///< Overflow bit
	  
   } F;
   
} MI26__TStatesLine;

/*!
 \struct MI26__TState
 \brief  State status
 */
typedef union {
   
   UShort_t W16;             ///< Word
   //! Frame state structure
   struct {
	  
	  UShort_t HitNb   :  2;  ///< Number of hits
	  UShort_t ColAddr : 10;  ///< Column adress
	  UShort_t NotUsed :  4;  ///< Not used
	  
   } F;
   
} MI26__TState;

#endif
