/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

/*
	Skeleton.h
*/

#pragma once

#ifndef PIXELSORTER_H
#define PIXELSORTER_H

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned short		u_int16;
typedef unsigned long		u_long;
typedef short int			int16;
#define PF_TABLE_BITS	12
#define PF_TABLE_SZ_16	4096

#define PF_DEEP_COLOR_AWARE 1	// make sure we get 16bpc pixels; 
								// AE_Effect.h checks for this.

#include "AEConfig.h"

#ifdef AE_OS_WIN
	typedef unsigned short PixelType;
	#include <Windows.h>
#endif

#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEFX_ChannelDepthTpl.h"
#include "AEGP_SuiteHandler.h"

#include "PixelSorter_Strings.h"

/* Versioning information */

#define	MAJOR_VERSION	1
#define	MINOR_VERSION	0
#define	BUG_VERSION		0
#define	STAGE_VERSION	PF_Stage_DEVELOP
#define	BUILD_VERSION	1


/* Parameter defaults */
#define	PIXELSORTER_THRESH_DFLT				128
#define	PIXELSORTER_ORDER_DFLT				true
#define	PIXELSORTER_INV_DFLT				false
#define	PIXELSORTER_EXTRANSPARENT_DFLT		true
#define	PIXELSORTER_GRAYSCALE_DFLT			false

enum {
	PIXELSORTER_INPUT = 0,
	PIXELSORTER_THRESH,
	PIXELSORTER_ORDER,
	PIXELSORTER_INV,
	PIXELSORTER_EXTRANSPARENT,
	PIXELSORTER_GRAYSCALE,
	PIXELSORTER_NUM_PARAMS
};

enum {
	THRESH_DISK_ID = 1,
	ORDER_DISK_ID,
	INV_DISK_ID,
	EXTRANSPARENT_DISK_ID,
	GRAYSCALE_DISK_ID
};

typedef struct PixSorterInfo{
	A_u_short	thresh;
	PF_Boolean	isAscending;
	PF_Boolean	isInv;
	PF_Boolean	exTransparent;
	PF_Boolean	isGrayscale;
} PixSorterInfo, * PixSorterInfoP, ** PixSorterInfoH;


extern "C" {

	DllExport
	PF_Err
	EffectMain(
		PF_Cmd			cmd,
		PF_InData		*in_data,
		PF_OutData		*out_data,
		PF_ParamDef		*params[],
		PF_LayerDef		*output,
		void			*extra);

}

#endif // SKELETON_H