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


#include <vector>
#include <algorithm>
#include <numeric>
#include "PixelSorter.h"

//エフェクトコントロールの「情報」を選択したときに表示される内容
static PF_Err 
About (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	
	suites.ANSICallbacksSuite1()->sprintf(	out_data->return_msg,
											"%s v%d.%d\r%s",
											STR(StrID_Name), 
											MAJOR_VERSION, 
											MINOR_VERSION, 
											STR(StrID_Description));
	return PF_Err_NONE;
}

//プラグインを読み込んだ時に呼び出されるっぽい
//バージョン情報とか
static PF_Err 
GlobalSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	out_data->my_version = PF_VERSION(	MAJOR_VERSION, 
										MINOR_VERSION,
										BUG_VERSION, 
										STAGE_VERSION, 
										BUILD_VERSION);

	out_data->out_flags = PF_OutFlag_DEEP_COLOR_AWARE;	// just 16bpc, not 32bpc
	
	return PF_Err_NONE;
}

//多分GlobalSetupと同じタイミングで呼び出されてる
//パラメーターの定義
static PF_Err 
ParamsSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err		err		= PF_Err_NONE;
	PF_ParamDef	def;	

	AEFX_CLR_STRUCT(def);
	PF_ADD_255_SLIDER(STR(StrID_Thresh_Param_Name),
		PIXELSORTER_THRESH_DFLT,
		THRESH_DISK_ID);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOXX(STR(StrID_Order_Param_Name),
		PIXELSORTER_ORDER_DFLT,
		PF_ParamFlag_CANNOT_TIME_VARY,
		ORDER_DISK_ID);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOXX(STR(StrID_Inv_Param_Name),
		PIXELSORTER_INV_DFLT,
		PF_ParamFlag_CANNOT_TIME_VARY,
		INV_DISK_ID);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOXX(STR(StrID_Extransparent_Param_Name),
		PIXELSORTER_EXTRANSPARENT_DFLT,
		PF_ParamFlag_CANNOT_TIME_VARY,
		EXTRANSPARENT_DISK_ID);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOXX(STR(StrID_Grayscale_Param_Name),
		PIXELSORTER_GRAYSCALE_DFLT,
		PF_ParamFlag_CANNOT_TIME_VARY,
		GRAYSCALE_DISK_ID);

	
	out_data->num_params = PIXELSORTER_NUM_PARAMS;

	return err;
}

static void
getSortedPixels16(
	PF_Pixel16* inPtr,
	PF_Pixel16* outPtr,
	PixSorterInfo* piP,
	std::vector<A_u_short>& lums,
	A_u_long				sx,
	A_u_long				ex)
{
	if (piP->isGrayscale) {
		(piP->isAscending) ? std::sort(lums.begin(), lums.end())
			: std::sort(lums.begin(), lums.end(), std::greater<int>());
		for (A_u_long i = sx; i < ex; ++i) {
			A_u_short lum = lums[i - sx];
			outPtr[i] = { PF_MAX_CHAN16,lum,lum,lum };
		}
	}
	else {
		std::vector<A_u_long> idxs(lums.size());
		std::iota(idxs.begin(), idxs.end(), sx);
		std::vector<std::pair<A_u_short, A_u_long>> pv(lums.size());
		int g_itr = 0;
		std::generate(pv.begin(), pv.end(), [&]() {
			auto ret = std::make_pair(lums[g_itr], idxs[g_itr]);
			++g_itr;
			return ret;
			}
		);
		(piP->isAscending) ? std::sort(pv.begin(), pv.end())
			: std::sort(pv.begin(), pv.end(), std::greater<std::pair<A_u_short, A_u_long>>());

		for (A_u_long i = sx; i < ex; ++i) {
			A_u_long sortedIdx = pv[i - sx].second;
			outPtr[i] = inPtr[sortedIdx];
		}
	}
}

static void
getSortedPixels8(
	PF_Pixel8*				inPtr,
	PF_Pixel8*				outPtr,
	PixSorterInfo*			piP,
	std::vector<A_u_char>&	lums,
	A_u_long				sx,
	A_u_long				ex)
{
	if (piP->isGrayscale) {
		(piP->isAscending) ? std::sort(lums.begin(), lums.end())
			: std::sort(lums.begin(), lums.end(), std::greater<int>());
		for (A_u_long i = sx; i < ex; ++i) {
			A_u_char lum = lums[i - sx];
			outPtr[i] = { PF_MAX_CHAN8,lum,lum,lum };
		}
	}
	else {
		std::vector<A_u_long> idxs(lums.size());
		std::iota(idxs.begin(), idxs.end(), sx);
		std::vector<std::pair<A_u_char, A_u_long>> pv(lums.size());
		int g_itr = 0;
		std::generate(pv.begin(), pv.end(), [&]() {
				auto ret = std::make_pair(lums[g_itr], idxs[g_itr]);
				++g_itr;
				return ret;
			}
		);
		(piP->isAscending) ? std::sort(pv.begin(), pv.end())
			: std::sort(pv.begin(), pv.end(), std::greater<std::pair<A_u_char, A_u_long>>());

		for (A_u_long i = sx; i < ex; ++i) {
			A_u_long sortedIdx = pv[i - sx].second;
			outPtr[i] = inPtr[sortedIdx];
		}
	}
}

static PF_Err
PixelSorter16(
	PixSorterInfo* piP,
	PF_InData* in_data,
	PF_LayerDef* input,
	PF_LayerDef* output)
{
	PF_Err		err = PF_Err_NONE;
	PF_Pixel16* inPP;
	PF_Pixel16* outPP;
	const A_long width = output->width;
	const A_long height = output->height;
	err = PF_GET_PIXEL_DATA16(input, NULL, &inPP);
	err = PF_GET_PIXEL_DATA16(output, NULL, &outPP);
	if (piP) {
		piP->thresh = PF_MAX_CHAN16 * ((double)piP->thresh / PF_MAX_CHAN8);
		for (A_long y = 0; y < height; ++y) {
			PF_Pixel16* inPtr = (PF_Pixel16*)((A_u_char*)inPP + (y * input->rowbytes));
			PF_Pixel16* outPtr = (PF_Pixel16*)((A_u_char*)outPP + (y * output->rowbytes));
			std::vector<A_u_short> lums;
			A_u_long sx = 0;
			for (A_long x = 0; x < width; ++x) {
				PF_Pixel16* inPix = inPtr + x;
				PF_Pixel16* outPix = outPtr + x;

				if (piP->exTransparent && inPix->alpha == 0) {
					if (lums.empty()) {
						*outPix = { 0,0,0,0 };
					}
					else {
						A_u_long ex = x;
						getSortedPixels16(inPtr, outPtr, piP, lums, sx, ex);
						lums.clear();
					}
					continue;
				}

				A_u_short lum = 0.299 * inPix->red + 0.587 * inPix->green + 0.114 * inPix->blue;

				PF_Boolean isIn = (piP->isInv) ? piP->thresh <= lum : piP->thresh >= lum;
				if (isIn) {
					if (lums.empty())sx = x;
					lums.push_back(lum);
				}
				else if (!lums.empty()) {
					A_u_long ex = x;
					getSortedPixels16(inPtr, outPtr, piP, lums, sx, ex);
					lums.clear();
				}
				else {
					*outPix = *inPix;
				}
			}

			if (!lums.empty()) {
				A_u_long ex = width;
				getSortedPixels16(inPtr, outPtr, piP, lums, sx, ex);
				lums.clear();
			}
		}
	}
	return err;
}

static PF_Err
PixelSorter8(
	PixSorterInfo*	piP,
	PF_InData* in_data,
	PF_LayerDef* input,
	PF_LayerDef* output)
{
	PF_Err		err = PF_Err_NONE;
	PF_Pixel8* inPP;
	PF_Pixel8* outPP;
	const A_long width = output->width;
	const A_long height = output->height;
	err = PF_GET_PIXEL_DATA8(input, NULL, &inPP);
	err = PF_GET_PIXEL_DATA8(output, NULL, &outPP);
	if (piP) {
		for (A_long y = 0; y < height; ++y) {
			PF_Pixel8* inPtr = (PF_Pixel8*)((A_u_char*)inPP + (y * input->rowbytes));
			PF_Pixel8* outPtr = (PF_Pixel8*)((A_u_char*)outPP + (y * output->rowbytes));
			std::vector<A_u_char> lums;
			A_u_long sx = 0;
			for (A_long x = 0; x < width; ++x) {
				PF_Pixel8* inPix = inPtr + x;
				PF_Pixel8* outPix = outPtr + x;

				if (piP->exTransparent && inPix->alpha == 0) {
					if (lums.empty()) {
						*outPix = { 0,0,0,0 };
					}
					else {
						A_u_long ex = x;
						getSortedPixels8(inPtr, outPtr, piP, lums, sx, ex);
						lums.clear();
					}
					continue;
				}

				A_u_char lum = 0.299 * inPix->red + 0.587 * inPix->green + 0.114 * inPix->blue;

				PF_Boolean isIn = (piP->isInv) ? piP->thresh <= lum : piP->thresh >= lum;
				if (isIn) {
					if (lums.empty())sx = x;
					lums.push_back(lum);
				}
				else if (!lums.empty()) {
					A_u_long ex = x;
					getSortedPixels8(inPtr, outPtr, piP, lums, sx, ex);
					lums.clear();
				}
				else {
					*outPix = *inPix;
				}
			}

			if (!lums.empty()) {
				A_u_long ex = width;
				getSortedPixels8(inPtr, outPtr, piP, lums, sx, ex);
				lums.clear();
			}
		}
	}
	return err;
}


static PF_Err
Render(
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[],
	PF_LayerDef* output)
{
	PF_Err				err = PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	PixSorterInfo		pi;
	AEFX_CLR_STRUCT(pi);
	A_long				linesL = 0;

	linesL = output->extent_hint.bottom - output->extent_hint.top;
	pi.thresh = params[PIXELSORTER_THRESH]->u.sd.value;
	pi.isAscending = params[PIXELSORTER_ORDER]->u.bd.value;
	pi.isInv = params[PIXELSORTER_INV]->u.bd.value;
	pi.exTransparent = params[PIXELSORTER_EXTRANSPARENT]->u.bd.value;
	pi.isGrayscale = params[PIXELSORTER_GRAYSCALE]->u.bd.value;

	PF_LayerDef* input = &params[PIXELSORTER_INPUT]->u.ld;

	if (PF_WORLD_IS_DEEP(output)) {
		err = PixelSorter16(&pi, in_data, input, output);
	}
	else {
		err = PixelSorter8(&pi, in_data, input, output);
	}

	return err;
}

////登録情報(Match Nameって何？)
extern "C" DllExport
PF_Err PluginDataEntryFunction(
	PF_PluginDataPtr inPtr,
	PF_PluginDataCB inPluginDataCallBackPtr,
	SPBasicSuite* inSPBasicSuitePtr,
	const char* inHostName,
	const char* inHostVersion)
{

	PF_Err result = PF_Err_INVALID_CALLBACK;

	result = PF_REGISTER_EFFECT(
		inPtr,
		inPluginDataCallBackPtr,
		"tmcm_PixelSorter", // Name
		"TMCM Pixel Sorter", // Match Name
		"tmcmPlugins", // Category
		AE_RESERVED_INFO); // Reserved Info

	return result;
}


PF_Err
EffectMain(
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extra)
{
	PF_Err		err = PF_Err_NONE;
	
	try {
		switch (cmd) {
			case PF_Cmd_ABOUT:

				err = About(in_data,
							out_data,
							params,
							output);
				break;
				
			case PF_Cmd_GLOBAL_SETUP:

				err = GlobalSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_PARAMS_SETUP:

				err = ParamsSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_RENDER:
				err = Render(		in_data,
									out_data,
									params,
									output);
				break;
		}
	}
	catch(PF_Err &thrown_err){
		err = thrown_err;
	}
	return err;
}

