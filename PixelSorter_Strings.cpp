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

#include "PixelSorter.h"

typedef struct {
	A_u_long	index;
	A_char		str[256];
} TableString;



TableString		g_strs[StrID_NUMTYPES] = {
	StrID_NONE,						"",
	StrID_Name,						"Pixcel Sorter",
	StrID_Description,				"This plugin effects like a pixcel sorter",
	StrID_Thresh_Param_Name,		"Threshold",
	StrID_Order_Param_Name,			"Ascending sort",
	StrID_Inv_Param_Name,			"sort values above threshold",
	StrID_Extransparent_Param_Name,	"exclude transparent",
	StrID_Grayscale_Param_Name,		"make sorted pixels grayscale",
};


char	*GetStringPtr(int strNum)
{
	return g_strs[strNum].str;
}
	