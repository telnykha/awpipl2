/*	$Id: awpEdge.c,v 1.2 2005/04/02 15:48:52 alt Exp $	*/

#include "_awpipl.h"

AWPRESULT awpLocalMax(awpImage* pImage, awpImage** ppImage, AWPBYTE thr, AWPINT options)
{
	AWPRESULT res = AWP_OK;
	awpImage* pTempImage = NULL;
	AWPBYTE *pSrcPix, *pDstPix;
	AWPWORD i, j;
	AWPBYTE curcolor;
		
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	if ((pImage->bChannels != 1) || (pImage->dwType != AWP_BYTE))
		_ERROR_EXIT_RES_(AWP_NOTSUPPORT)

	_CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE)))
	
	pSrcPix = (AWPBYTE *) pImage->pPixels;
	pDstPix = (AWPBYTE *) pTempImage->pPixels;

	switch (options)
	{
	case _4_NEIGHBOURS:
		for(j = 0; j < pImage->sSizeY; j++)
			for(i = 0; i < pImage->sSizeX; i++)
			{
				curcolor = pSrcPix[(j * pImage->sSizeX) + i];

				if ((curcolor < thr) ||
					((i > 0) && (pSrcPix[(j * pImage->sSizeX) + i - 1] > curcolor)) ||
					((i < pImage->sSizeX - 1) && (pSrcPix[(j * pImage->sSizeX) + i + 1] > curcolor)) ||
					((j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i] > curcolor)) ||
					((j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i] > curcolor)))
					continue;

				pDstPix[(j * pTempImage->sSizeX) + i] = curcolor;
			}
		break;
	case _8_NEIGHBOURS:
		for(j = 0; j < pImage->sSizeY; j++)
			for(i = 0; i < pImage->sSizeX; i++)
			{
				curcolor = pSrcPix[(j * pImage->sSizeX) + i];

				if ((curcolor < thr) ||
					((i > 0) && (j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i - 1] > curcolor)) ||
					((i > 0) && (pSrcPix[(j * pImage->sSizeX) + i - 1] > curcolor)) ||
					((i > 0) && (j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i - 1] > curcolor)) ||
					((j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i] > curcolor)) ||
					((i < pImage->sSizeX - 1) && (j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i + 1] > curcolor)) ||
					((i < pImage->sSizeX - 1) && (pSrcPix[(j * pImage->sSizeX) + i + 1] > curcolor)) ||
					((i < pImage->sSizeX - 1) && (j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i + 1] > curcolor)) ||
					((j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i] > curcolor)))
					continue;

				pDstPix[(j * pTempImage->sSizeX) + i] = curcolor;
			}
		break;
	default:
		res = AWP_BADARG;
		_CHECK_RESULT_(awpReleaseImage(&pTempImage))
		_ERR_EXIT_
	}

	*ppImage = pTempImage;

CLEANUP:
	return res;
}

AWPRESULT awpLocalMin(awpImage* pImage, awpImage** ppImage, AWPBYTE thr, AWPINT options)
{
	AWPRESULT res = AWP_OK;
	awpImage* pTempImage = NULL;
	AWPBYTE *pSrcPix, *pDstPix;
	AWPWORD i, j;
	AWPBYTE curcolor;
	
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	if ((pImage->bChannels != 1) || (pImage->dwType != AWP_BYTE))
		_ERROR_EXIT_RES_(AWP_NOTSUPPORT)

	_CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE)))

	memset((AWPBYTE*)pTempImage->pPixels, 255, pImage->sSizeX * pImage->sSizeY * sizeof(AWPBYTE));

	pSrcPix = (AWPBYTE *) pImage->pPixels;
	pDstPix = (AWPBYTE *) pTempImage->pPixels;

	switch (options)
	{
	case _4_NEIGHBOURS:
		for(j = 0; j < pImage->sSizeY; j++)
			for(i = 0; i < pImage->sSizeX; i++)
			{
				curcolor = pSrcPix[(j * pImage->sSizeX) + i];

				if ((curcolor > thr) ||
					((i > 0) && (pSrcPix[(j * pImage->sSizeX) + i - 1] < curcolor)) ||
					((i < pImage->sSizeX - 1) && (pSrcPix[(j * pImage->sSizeX) + i + 1] < curcolor)) ||
					((j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i] < curcolor)) ||
					((j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i] < curcolor)))
					continue;

				pDstPix[(j * pTempImage->sSizeX) + i] = curcolor;
			}
		break;
	case _8_NEIGHBOURS:
		for(j = 0; j < pImage->sSizeY; j++)
			for(i = 0; i < pImage->sSizeX; i++)
			{
				curcolor = pSrcPix[(j * pImage->sSizeX) + i];

				if ((curcolor > thr) ||
					((i > 0) && (j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i - 1] < curcolor)) ||
					((i > 0) && (pSrcPix[(j * pImage->sSizeX) + i - 1] < curcolor)) ||
					((i > 0) && (j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i - 1] < curcolor)) ||
					((j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i] < curcolor)) ||
					((i < pImage->sSizeX - 1) && (j < pImage->sSizeY - 1) && (pSrcPix[((j + 1) * pImage->sSizeX) + i + 1] < curcolor)) ||
					((i < pImage->sSizeX - 1) && (pSrcPix[(j * pImage->sSizeX) + i + 1] < curcolor)) ||
					((i < pImage->sSizeX - 1) && (j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i + 1] < curcolor)) ||
					((j > 0) && (pSrcPix[((j - 1) * pImage->sSizeX) + i] < curcolor)))
					continue;

				pDstPix[(j * pTempImage->sSizeX) + i] = curcolor;
			}
		break;
	default:
		res = AWP_BADARG;
		_CHECK_RESULT_(awpReleaseImage(&pTempImage))
		_ERR_EXIT_
	}

	*ppImage = pTempImage;

CLEANUP:
	return res;
}


AWPRESULT awpFeatures(awpImage* pImage, awpImage** ppImage, AWPBYTE thr_max, AWPBYTE thr_min, AWPBYTE relative_thr, AWPINT radius, AWPINT MinMax_options, AWPINT options)
{
	AWPRESULT res = AWP_OK;
	awpImage *pImageMax = NULL, *pImageMin = NULL, *pTempImage = NULL;
	AWPBYTE *pSrcMaxPix, *pSrcMinPix, *pDstPix;
	AWPWORD i, j;
	AWPSHORT local_i, local_j;
	AWPINT cur_x, cur_y;
	AWPBYTE cur_max_color, cur_min_color, min_min_color;

	if (radius == 0)
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))

	_CHECK_RESULT_((res = awpLocalMax(pImage, &pImageMax, thr_max, MinMax_options)))
	_CHECK_RESULT_((res = awpLocalMin(pImage, &pImageMin, thr_min, MinMax_options)))

	_CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE)))

	pSrcMaxPix = (AWPBYTE *) pImageMax->pPixels;
	pSrcMinPix = (AWPBYTE *) pImageMin->pPixels;
	pDstPix = (AWPBYTE *) pTempImage->pPixels;

	for(j = 0; j < pImage->sSizeY; j++)
		for(i = 0; i < pImage->sSizeX; i++)
		{
			cur_max_color = pSrcMaxPix[(j * pImage->sSizeX) + i];
			if (cur_max_color == 0)
				continue;

			min_min_color = 255;

			for(local_j = - radius; local_j <= radius; local_j++)
				for(local_i = - radius; local_i <= radius; local_i++)
				{
					if ((local_i == 0) && (local_j == 0)) continue;

					cur_x = i + local_i;
					cur_y = j + local_j;

					if ((cur_x < 0) || (cur_y < 0) || (cur_x >= pImage->sSizeX) ||
						(cur_y >= pImage->sSizeY)) continue;

					cur_min_color = pSrcMinPix[(cur_y * pImage->sSizeX) + cur_x];

					if (cur_min_color < min_min_color)
						min_min_color = cur_min_color;
				}

			if ((cur_max_color - min_min_color >= relative_thr) || ((min_min_color == 255) && (options == LEAVE_UNKNOWN_MAX)))
				pDstPix[(j * pImage->sSizeX) + i] = 255;
		}

	_CHECK_RESULT_((res = awpReleaseImage(&pImageMax)))
	_CHECK_RESULT_((res = awpReleaseImage(&pImageMin)))

	*ppImage = pTempImage;

CLEANUP:
	if (pImageMax != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pImageMax);
		else
			awpReleaseImage(&pImageMax);

	if (pImageMin != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pImageMin);
		else
			awpReleaseImage(&pImageMin);

	return res;
}

AWPRESULT awpFeaturesV2(awpImage* pImage, awpImage** ppImage, AWPBYTE thr_max, AWPBYTE relative_thr, AWPINT radius, AWPINT MinMax_options)
{
	AWPRESULT res = AWP_OK;
	awpImage *pImageMax = NULL, *pTempImage = NULL;
	AWPBYTE *pSrcMaxPix, *pSrcPix, *pDstPix;
	AWPWORD i, j;
	AWPSHORT local_i, local_j;
	AWPINT cur_x, cur_y;
	AWPBYTE cur_max_color, cur_min_color, min_min_color;

	if (radius == 0)
		_ERROR_EXIT_RES_(AWP_BADARG)
	_CHECK_RESULT_((res = awpCheckImage(pImage)))

	_CHECK_RESULT_((res = awpLocalMax(pImage, &pImageMax, thr_max, MinMax_options)))
	
	_CHECK_RESULT_((res = awpCreateImage(&pTempImage, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, AWP_BYTE)))

	pSrcMaxPix = (AWPBYTE *) pImageMax->pPixels;
	pSrcPix = (AWPBYTE *) pImage->pPixels;
	pDstPix = (AWPBYTE *) pTempImage->pPixels;

	for(j = 0; j < pImage->sSizeY; j++)
		for(i = 0; i < pImage->sSizeX; i++)
		{
			cur_max_color = pSrcMaxPix[(j * pImage->sSizeX) + i];
			if (cur_max_color == 0)
				continue;

			min_min_color = 255;

			for(local_j = - radius; local_j <= radius; local_j++)
				for(local_i = - radius; local_i <= radius; local_i++)
				{
					if ((local_i == 0) && (local_j == 0)) continue;

					cur_x = i + local_i;
					cur_y = j + local_j;

					if ((cur_x < 0) || (cur_y < 0) || (cur_x >= pImage->sSizeX) ||
						(cur_y >= pImage->sSizeY)) continue;

					cur_min_color = pSrcPix[(cur_y * pImage->sSizeX) + cur_x];

					if (cur_min_color < min_min_color)
						min_min_color = cur_min_color;
				}

			if (cur_max_color - min_min_color >= relative_thr)
				pDstPix[(j * pImage->sSizeX) + i] = 255;
		}

	_CHECK_RESULT_((res = awpReleaseImage(&pImageMax)))

	*ppImage = pTempImage;

CLEANUP:
	if (pImageMax != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pImageMax);
		else
			awpReleaseImage(&pImageMax);

	return res;
}

#define _AWP_MAKE_GRADIENT_AMPL_(type)																\
	AWPINT i;																							\
	AWPBYTE ch;																						\
	AWPDOUBLE alfa = 1. / AWP_SQTR2;																	\
	awpImage* pChannelDx = NULL;																	\
	awpImage* pChannelDy = NULL;																	\
	awpImage* pDstChannel = NULL;																	\
	type* dxpix = NULL;																				\
	type* dypix = NULL;																				\
	type* dpix = NULL;																				\
	_CHECK_RESULT_(res = awpCreateImage(&pDstChannel, pDx->sSizeX, pDx->sSizeY, 1, pDx->dwType))	\
	dpix = (type*)pDstChannel->pPixels;																\
	for (ch = 0; ch < pDx->bChannels; ch++)															\
	{																								\
		_CHECK_RESULT_(res = awpGetChannel(pDx, &pChannelDx, ch))									\
		_CHECK_RESULT_(res = awpGetChannel(pDy, &pChannelDy, ch))									\
		dxpix = (type*)pChannelDx->pPixels;															\
		dypix = (type*)pChannelDy->pPixels;															\
		for (i = 0; i < pDx->sSizeY*pDx->sSizeX; i++)												\
		{																							\
			dpix[i] = (type)(alfa*sqrt((AWPFLOAT)(dxpix[i]*dxpix[i] + dypix[i]*dypix[i])));						\
		}																							\
		_CHECK_RESULT_(res = awpPutChannel(pDst, pDstChannel, ch));									\
		_SAFE_RELEASE_(pChannelDx);																	\
		_SAFE_RELEASE_(pChannelDy);																	\
	}																								\
	_SAFE_RELEASE_(pDstChannel)																		\
	
const AWPDOUBLE c_dVertSobelGrad[9] = 
{-1,-2,-1,
0,0,0,
1,2,1};
const AWPDOUBLE c_dHorzSobelGrad[9] = 
{-1,0,1,
-2,0,2,
-1,0,1};
AWPRESULT awpCreateSobelKernel(awpImage** ppKernel, AWPINT Direction)
{
    AWPRESULT res = AWP_OK;
	AWPDOUBLE* pix = NULL;
	if (ppKernel == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
	if (Direction != AWP_EDGE_DIR_VERT && Direction != AWP_EDGE_DIR_HORZ)
		_ERROR_EXIT_RES_(AWP_BADARG)

	_CHECK_RESULT_(res = awpCreateImage(ppKernel, 3, 3, 1, AWP_DOUBLE))
	pix = (AWPDOUBLE*)(*ppKernel)->pPixels;
	switch(Direction)
	{
	case AWP_EDGE_DIR_VERT:
		memcpy(pix, c_dVertSobelGrad, sizeof(c_dVertSobelGrad)); 
		break;
	case AWP_EDGE_DIR_HORZ:
		memcpy(pix, c_dHorzSobelGrad, sizeof(c_dHorzSobelGrad));
		break;
	}
CLEANUP:
    return res;
}
const AWPDOUBLE AWP_SQTR2 = 1.4142135623730950488016887242097;

AWPRESULT awpGradientAmplByte(awpImage* pDx, awpImage* pDy, awpImage* pDst)
{
AWPRESULT res = AWP_OK;
	_AWP_MAKE_GRADIENT_AMPL_(AWPBYTE)
CLEANUP:
	return res;
}
AWPRESULT awpGradientAmplShort(awpImage* pDx, awpImage* pDy, awpImage* pDst)
{
AWPRESULT res = AWP_OK;
	_AWP_MAKE_GRADIENT_AMPL_(AWPSHORT)
CLEANUP:
	return res;
}
AWPRESULT awpGradientAmplFloat(awpImage* pDx, awpImage* pDy, awpImage* pDst)
{
AWPRESULT res = AWP_OK;
	_AWP_MAKE_GRADIENT_AMPL_(AWPFLOAT)
CLEANUP:
	return res;
}
AWPRESULT awpGradientAmplAWPAWPDOUBLE(awpImage* pDx, awpImage* pDy, awpImage* pDst)
{
AWPRESULT res = AWP_OK;
	_AWP_MAKE_GRADIENT_AMPL_(AWPDOUBLE)
CLEANUP:
	return res;
}

AWPRESULT awpGradientDirByte(awpImage* pDx, awpImage* pDy, awpImage* pDst)
{
AWPRESULT res = AWP_OK;
_ERROR_EXIT_RES_(res)
CLEANUP:
	return res;
}
AWPRESULT awpGradientDirShort(awpImage* pDx, awpImage* pDy, awpImage* pDst)
{
AWPRESULT res = AWP_OK;
_ERROR_EXIT_RES_(res)
CLEANUP:
	return res;
}
AWPRESULT awpGradientDirFloat(awpImage* pDx, awpImage* pDy, awpImage* pDst)
{
AWPRESULT res = AWP_OK;
_ERROR_EXIT_RES_(res)
CLEANUP:
	return res;
}
AWPRESULT awpGradientDirAWPAWPDOUBLE(awpImage* pDx, awpImage* pDy, awpImage* pDst)
{
AWPRESULT res = AWP_OK;
_ERROR_EXIT_RES_(res)
CLEANUP:
	return res;
}


/*
	awpEdgeSobel1 - \E2\FB\EF\EE\EB\ED\FF\E5\F2 \EF\F0\E5\EE\E1\F0\E0\E7\EE\E2\E0\ED\E8\E5 \D1\EE\E1\E5\EB\FF. 
*/
AWPRESULT awpEdgeSobel(awpImage* pImage, awpImage* pGradAmpl, awpImage* pGradDir, AWPINT Direction)
{
	AWPRESULT res = AWP_OK;
	awpImage* pDx = NULL; 
	awpImage* pDy = NULL; 
	awpImage* pDxKernel = NULL;
	awpImage* pDyKernel = NULL;

	_CHECK_RESULT_(res = awpCheckImage(pImage))
    _CHECK_RESULT_(res = awpCheckImage(pGradAmpl))
	_CHECK_SAME_SIZES(pImage, pGradAmpl)
    _CHECK_SAME_TYPE(pImage,pGradAmpl)
    _CHECK_NUM_CHANNELS(pImage, pGradAmpl)
	
	if (Direction != AWP_EDGE_DIR_HORZ && Direction  != AWP_EDGE_DIR_VERT && Direction != AWP_EDGE_DIR_BOTH)
		_ERROR_EXIT_RES_(AWP_BADARG)

	if (Direction == AWP_EDGE_DIR_BOTH && pGradDir != NULL)
	{
	    _CHECK_RESULT_(res = awpCheckImage(pGradDir))
		_CHECK_SAME_SIZES(pGradDir, pGradAmpl)
		_CHECK_SAME_TYPE(pGradDir,pGradAmpl)
		_CHECK_NUM_CHANNELS(pGradDir, pGradAmpl)
	}

	switch(Direction)
	{
	case AWP_EDGE_DIR_HORZ:
		{
			_CHECK_RESULT_(res = awpCreateSobelKernel(&pDxKernel, AWP_EDGE_DIR_HORZ))
			_CHECK_RESULT_(res = awpCreateImage(&pDx, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, pImage->dwType))
			_CHECK_RESULT_(res = awpMaskConvolution2(pImage, pDx, pDxKernel))
			_CHECK_RESULT_(res = awpFastCopyImage(pDx, pGradAmpl))
		}
		break;
	case AWP_EDGE_DIR_VERT:
		{
			_CHECK_RESULT_(res = awpCreateSobelKernel(&pDyKernel, AWP_EDGE_DIR_VERT))
			_CHECK_RESULT_(res = awpCreateImage(&pDy, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, pImage->dwType))
			_CHECK_RESULT_(res = awpMaskConvolution2(pImage, pDy, pDyKernel))
			_CHECK_RESULT_(res = awpFastCopyImage(pDy, pGradAmpl))
		}
		break;
	case AWP_EDGE_DIR_BOTH:
		{
			_CHECK_RESULT_(res = awpCreateImage(&pDx, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, pImage->dwType))
			_CHECK_RESULT_(res = awpCreateImage(&pDy, pImage->sSizeX, pImage->sSizeY, pImage->bChannels, pImage->dwType))
			_CHECK_RESULT_(res = awpCreateSobelKernel(&pDxKernel, AWP_EDGE_DIR_HORZ))
			_CHECK_RESULT_(res = awpMaskConvolution2(pImage, pDx, pDxKernel))
			_CHECK_RESULT_(res = awpCreateSobelKernel(&pDyKernel, AWP_EDGE_DIR_VERT))
			_CHECK_RESULT_(res = awpMaskConvolution2(pImage, pDy, pDyKernel))
		}
		break;
	}

	if (Direction == AWP_EDGE_DIR_BOTH)
	{
		switch(pImage->dwType)
		{
			case AWP_BYTE:
				_CHECK_RESULT_(res = awpGradientAmplByte(pDx,pDy,pGradAmpl))
					if (pGradDir != NULL)
						_CHECK_RESULT_(res = awpGradientDirByte(pDx,pDy,pGradAmpl))
			break;
			case AWP_SHORT:
				_CHECK_RESULT_(res = awpGradientAmplShort(pDx,pDy,pGradAmpl))
					if (pGradDir != NULL)
						_CHECK_RESULT_(res = awpGradientDirShort(pDx,pDy,pGradAmpl))
			break;
			case AWP_FLOAT:
				_CHECK_RESULT_(res = awpGradientAmplFloat(pDx,pDy,pGradAmpl))
					if (pGradDir != NULL)
						_CHECK_RESULT_(res = awpGradientDirFloat(pDx,pDy,pGradAmpl))
			break;
			case AWP_DOUBLE:
				_CHECK_RESULT_(res = awpGradientAmplAWPAWPDOUBLE(pDx,pDy,pGradAmpl))
					if (pGradDir != NULL)
						_CHECK_RESULT_(res = awpGradientDirAWPAWPDOUBLE(pDx,pDy,pGradAmpl))
			break;
		}
	}
CLEANUP:
	_SAFE_RELEASE_(pDx);
	_SAFE_RELEASE_(pDy);
	_SAFE_RELEASE_(pDxKernel);
	_SAFE_RELEASE_(pDyKernel);

    return res;
}

static AWPRESULT _awpCannyAWP_BYTE(awpImage* pSrc, awpImage* pDst, AWPFLOAT thr, AWPFLOAT sigma)
{
    AWPRESULT res = AWP_OK;
	_ERROR_EXIT_RES_(res)
CLEANUP:
   return res;
}
static AWPRESULT _awpCannyAWP_SHORT(awpImage* pSrc, awpImage* pDst, AWPFLOAT thr, AWPFLOAT sigma)
{
    AWPRESULT res = AWP_OK;
	_ERROR_EXIT_RES_(res)

CLEANUP:
    return res;
}

static AWPRESULT _awpCannyAWP_FLOAT(awpImage* pSrc, awpImage* pDst, AWPFLOAT thr, AWPFLOAT sigma)
{
    AWPRESULT res = AWP_OK;
	_ERROR_EXIT_RES_(res)
CLEANUP:
    return res;
}

static AWPRESULT _awpCannyAWP_AWPAWPDOUBLE(awpImage* pSrc, awpImage* pDst, AWPFLOAT thr, AWPFLOAT sigma)
{
    AWPRESULT res = AWP_OK;
	_ERROR_EXIT_RES_(res)

CLEANUP:
    return res;
}
// awpCanny implementation
// supported data types: AWP_BYTE, AWP_SHORT, AWP_FLOAT, AWP_DOUBLE
// the data types of pSrc and pDst should be same.
// awpCanny works on only single-channel images
// the value of thr should be between 0...1
// sigma is used for smoothig by gauss filter
// size of filter calculates automatically
// if sigma <=0 no noise reduction performed
AWPRESULT awpCanny(awpImage* pSrc, awpImage* pDst, AWPFLOAT thr, AWPFLOAT sigma )
{
    AWPRESULT res = AWP_OK;

    _CHECK_RESULT_(res = awpCheckImage(pSrc))
    _CHECK_RESULT_(res = awpCheckImage(pDst))

    _CHECK_SAME_SIZES(pSrc, pDst)
    _CHECK_SAME_TYPE(pSrc,pDst)
    _CHECK_NUM_CHANNELS(pSrc, pDst)

    if (thr <= 0 || thr >1)
        _ERROR_EXIT_RES_(AWP_BADARG)

    switch(pSrc->dwType)
    {
        case AWP_BYTE:
            res = _awpCannyAWP_BYTE(pSrc,pDst,thr,sigma);
        break;
        case AWP_SHORT:
            res = _awpCannyAWP_SHORT(pSrc,pDst,thr,sigma);
        break;
        case AWP_FLOAT:
            res = _awpCannyAWP_FLOAT(pSrc,pDst,thr,sigma);
        break;
        case AWP_DOUBLE:
            res = _awpCannyAWP_AWPAWPDOUBLE(pSrc,pDst,thr,sigma);
        break;
    }
CLEANUP:
    return res;
}

static const AWPSHORT sobel_v_mask[9] =
	{-1,0,1,
	 -2,0,2,
	 -1,0,1};
static const AWPSHORT sobel_h_mask[9] =
	{1,2,1,
	 0,0,0,
    -1,-2,-1};

AWPRESULT awpEdgeSobel1(awpImage* pImage, awpImage* pVImage, awpImage* pHImage)
{
    AWPRESULT res = AWP_OK;
    AWPBYTE*  b = NULL;
    AWPDOUBLE* v = NULL;
    AWPDOUBLE* h = NULL;
    AWPSHORT    y = 0;
    AWPSHORT    x = 0;
    AWPSHORT    xx = 0;
    AWPSHORT    yy = 0;
    AWPDOUBLE  sumv = 0;
    AWPDOUBLE  sumh = 0;
    AWPWORD    c = 0;
    _CHECK_RESULT_(res = awpCheckImage(pImage))
    _CHECK_RESULT_(res = awpCheckImage(pVImage))
    _CHECK_RESULT_(res = awpCheckImage(pHImage))

    _CHECK_SAME_SIZES(pHImage, pVImage)
    _CHECK_SAME_SIZES(pImage, pVImage)

    if (pHImage->bChannels != 1)
    {
        res = AWP_BADARG;
        _ERROR_EXIT_RES_(res)
    }

    _CHECK_NUM_CHANNELS(pHImage, pVImage)
    _CHECK_SAME_TYPE(pHImage, pVImage)
    //-------------------------

    b = (AWPBYTE*)pImage->pPixels;
    h = (AWPDOUBLE*)pHImage->pPixels;
    v = (AWPDOUBLE*)pVImage->pPixels;

    for ( y = 1; y < pImage->sSizeY - 1; y++)
    {
        for ( x = 1; x < pImage->sSizeX - 1; x++)
        {
            sumv = 0;
            sumh = 0;
            c = 0;
            for (yy = -1; yy <=1; yy++)
            {
               for (xx = -1; xx <= 1; xx++)
               {
                    sumv += sobel_v_mask[c]*b[x+xx +(y+yy)*pImage->sSizeX];
                    sumh += sobel_h_mask[c]*b[x+xx +(y+yy)*pImage->sSizeX];
                    c++;
               }
            }

            v[x+y*pImage->sSizeX] = sumv;
            h[x+y*pImage->sSizeX] = sumh;

        }
    }

CLEANUP:
    return res;
}

  