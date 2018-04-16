#include "_awpipl.h"
static AWPSHORT blur_mask[9] =
	{1,1,1,
	 1,1,1,
	 1,1,1};

static AWPSHORT blur_more_mask[25] =
	{1,1,1,1,1,
	 1,1,1,1,1,
	 1,1,1,1,1,
	 1,1,1,1,1,
	 1,1,1,1,1};

static AWPSHORT sharpen_mask[9] =
	{0,-1,0,
	 -1,5,-1,
	 0,-1,0};

static AWPSHORT sharpen_more_mask[9] =
	{-1,-1,-1,
	 -1,9,-1,
	 -1,-1,-1};

static AWPSHORT sharpen_edges_mask[9] =
	{1,-2,1,
	 -2,5,-2,
	 1,-2,1};

static AWPSHORT find_edges_mask[9] =
	{-1,-1,-1,
	 -1,8,-1,
	 -1,-1,-1};

static AWPSHORT find_edges1_mask[9] =
	{1,-2,1,
	 -2,4,-2,
	 1,-2,1};

static AWPSHORT emboss_north_mask[9] =
	{1,1,1,
	 1,-2,1,
	 -1,-1,-1};

static AWPSHORT emboss_north_ost_mask[9] =
	{1,1,1,
	 -1,-2,1,
	 -1,-1,1};

static AWPSHORT emboss_ost_mask[9] =
	{-1,1,1,
	 -1,-2,1,
	 -1,1,1};

static AWPSHORT emboss_south_ost_mask[9] =
	{-1,-1,1,
	 -1,-2,1,
	 1,1,1};

static AWPSHORT emboss_south_mask[9] =
	{-1,-1,-1,
	  1,-2,1,
	  1,1,1};

static AWPSHORT emboss_south_west_mask[9] =
	{1,-1,-1,
	 1,-2,-1,
	 1,1,1};

static AWPSHORT emboss_west_mask[9] =
	{1,1,-1,
	 1,-2,-1,
	 1,1,-1};

static AWPSHORT emboss_north_west_mask[9] =
	{1,1,1,
	 1,-2,-1,
	 1,-1,-1};

static AWPSHORT prewitt_h_mask[9] =
	{1,1,1,
	 0,0,0,
	 -1,-1,-1};

static AWPSHORT prewitt_v_mask[9] =
	{-1,0,1,
	 -1,0,1,
	 -1,0,1};
static AWPSHORT sobel_v_mask[9] =
	{-1,0,1,
	 -2,0,2,
	 -1,0,1};
static AWPSHORT sobel_h_mask[9] =
	{1,2,1,
	 0,0,0,
	 -1,-2,-1};

/*
Calculates the sum of the pixel values of the pSrc image
and stores it in pDst.
pDst must be an image of type AWP_DOUBLE
have one data channel, a height of 1 pixel and a width equal to the number of channels pScr
*/
AWPRESULT awpGetSumPix(awpImage* pSrc, awpImage* pDst)
{
	AWPRESULT res = AWP_OK;
	AWPINT i, j;
	AWPBYTE* bp = NULL;
	short* sp = NULL;
	AWPFLOAT* fp = NULL;
	AWPDOUBLE* dp = NULL;
	AWPDOUBLE* rp = (AWPDOUBLE*)pDst->pPixels;
	memset(rp, 0, pDst->sSizeX);

	_CHECK_RESULT_(res = awpCheckImage(pSrc))
		_CHECK_RESULT_(res = awpCheckImage(pDst))

		if (pDst->dwType != AWP_DOUBLE)
			_ERROR_EXIT_RES_(AWP_BADARG)
			if (pDst->bChannels != 1)
				_ERROR_EXIT_RES_(AWP_BADARG)
				if (pDst->sSizeY > 1)
					_ERROR_EXIT_RES_(AWP_BADARG)
					if (pDst->sSizeX != pSrc->bChannels)
						_ERROR_EXIT_RES_(AWP_BADARG)

						switch (pSrc->dwType)
					{
						case AWP_BYTE:
							bp = (AWPBYTE*)pSrc->pPixels;
							for (i = 0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
							{
								for (j = 0; j < pSrc->bChannels; j++)
									rp[j] += bp[pSrc->bChannels*i + j];
							}
							break;
						case AWP_SHORT:
							sp = (short*)pSrc->pPixels;
							for (i = 0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
							{
								for (j = 0; j < pSrc->bChannels; j++)
									rp[j] += sp[pSrc->bChannels*i + j];
							}
							break;
						case AWP_FLOAT:
							fp = (AWPFLOAT*)pSrc->pPixels;
							for (i = 0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
							{
								for (j = 0; j < pSrc->bChannels; j++)
									rp[j] += fp[pSrc->bChannels*i + j];
							}
							break;
						case AWP_DOUBLE:
							dp = (AWPDOUBLE*)pSrc->pPixels;
							for (i = 0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
							{
								for (j = 0; j < pSrc->bChannels; j++)
									rp[j] += dp[pSrc->bChannels*i + j];
							}
							break;
					}

CLEANUP:
	return res;
}

/*
 apply mask to all channles
*/


static AWPBYTE _awpSat(AWPINT a)
{
	if (a > 255)
		return 255;
	else if (a < 0)
		return 0;
	else return a;
}
AWPRESULT __awpMaskConvolution(awpImage* pImage, awpImage* pMask)
{
    AWPRESULT res;
	AWPINT io, jo, iw, jw, i, r, x, y; /*идексы*/
	AWPINT sx,sy,wx,wy;		  /*пределы изменения индексов*/
	AWPBYTE* pix;
	AWPBYTE* o_pix;
	AWPSHORT* m_pix;
	void* t_pix;
	AWPFLOAT sum,/* sum_r, sum_g, sum_b,*/ winsum;
	awpImage* tmp_img = NULL;
	res = AWP_OK;
	/*проверка иходных переменных*/
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	_CHECK_RESULT_((res = awpCheckImage(pMask)))
	if (pImage->bChannels != 1 || pMask->bChannels != 1 ||
		pImage->dwType != AWP_BYTE || pMask->dwType != AWP_SHORT)
		_ERROR_EXIT_RES_(AWP_NOTSUPPORT)
	_CHECK_RESULT_((res = awpCreateImage(&tmp_img, pImage->sSizeX, pImage->sSizeY, 1, pImage->dwType)))
	/*инициализация переменных*/
	io = 0; 
	jo = 0;
	iw = 0;
	jw = 0;

	i = 0;
	r = 0;

	winsum = 0;

	sx = pImage->sSizeX;
	sy = pImage->sSizeY;

	wx = pMask->sSizeX;
	wy = pMask->sSizeY;
	pix = (AWPBYTE*)pImage->pPixels;
	m_pix = (AWPSHORT*)pMask->pPixels;
	o_pix = (AWPBYTE*)tmp_img->pPixels;

	r = pMask->sSizeX*pMask->sSizeY; while (r>0) winsum += m_pix[--r];
	if (winsum == 0)winsum = 1;

	/*организация цикла по всем пикселям*/
	for (io = 0; io < sy; io++)
	{
		for (jo = 0; jo < sx; jo++)
		{
			/*сбросим сумму*/
			sum = 0;// sum_r = 0; sum_g = 0; sum_b = 0;
			for (iw = 0; iw < wy; iw++)
			{
				for (jw = 0; jw < wx; jw++)
				{
					r = iw*wx + jw;
					x = jo + jw - wx / 2; 
					y = io + iw - wy / 2;

					if (x < 0 || x >= sx || y < 0 || y >= sy)
						i = -1;
					else
						i = y*sx + x;

					if (i >= 0)
							sum += pix[i]*m_pix[r];
				}
			}
			/*заполним пиксели результирующего изображения*/
			o_pix[io*sx + jo] = _awpSat((AWPINT)(sum / winsum + 0.5));
		}
	}
	/*обмен указателями на пиксели между исходным и временным изображениями*/
	t_pix = pImage->pPixels;
	pImage->pPixels = tmp_img->pPixels;
	tmp_img->pPixels = t_pix;


CLEANUP:
_SAFE_RELEASE_(tmp_img)
return res;
}

AWPRESULT awpMaskConvolutionDbl(awpImage* pImage,awpImage* pMask, awpImage** pDstImage)
{
    AWPRESULT res;
	AWPINT io, jo, iw, jw, i, r, x, y; /*идексы*/
	AWPINT sx,sy,wx,wy;		  /*пределы изменения индексов*/
	AWPDOUBLE* pix;
	AWPDOUBLE* o_pix;
	AWPDOUBLE* m_pix;
	//void* t_pix;
	AWPDOUBLE sum,/* sum_r, sum_g, sum_b,*/ winsum;
	awpImage* tmp_img = NULL;
	res = AWP_OK;
	/*проверка иходных переменных*/
	_CHECK_RESULT_((res = awpCheckImage(pImage)))
	_CHECK_RESULT_((res = awpCheckImage(pMask)))
	if (pImage->bChannels != 1 || pMask->bChannels != 1 ||
		pImage->dwType != AWP_DOUBLE || pMask->dwType != AWP_DOUBLE)
		_ERROR_EXIT_RES_(AWP_NOTSUPPORT)
	_CHECK_RESULT_((res = awpCreateImage(&tmp_img, pImage->sSizeX, pImage->sSizeY, 1, pImage->dwType)))
	/*инициализация переменных*/
	io = 0;
	jo = 0;
	iw = 0;
	jw = 0;

	i = 0;
	r = 0;

	winsum = 0;

	sx = pImage->sSizeX;
	sy = pImage->sSizeY;

	wx = pMask->sSizeX;
	wy = pMask->sSizeY;
	pix = (AWPDOUBLE*)pImage->pPixels;
	m_pix = (AWPDOUBLE*)pMask->pPixels;
	o_pix = (AWPDOUBLE*)tmp_img->pPixels;

	r = pMask->sSizeX*pMask->sSizeY; while (r>0) winsum += m_pix[--r];
	if (winsum == 0)winsum = 1;

	/*организация цикла по всем пикселям*/
	for (io = 0; io < sy; io++)
	{
		for (jo = 0; jo < sx; jo++)
		{
			/*сбросим сумму*/
			sum = 0; //sum_r = 0; sum_g = 0; sum_b = 0;
			for (iw = 0; iw < wy; iw++)
			{
				for (jw = 0; jw < wx; jw++)
				{
					r = iw*wx + jw;
					x = jo + jw - wx / 2;
					y = io + iw - wy / 2;

					if (x < 0 || x >= sx || y < 0 || y >= sy)
						i = -1;
					else
						i = y*sx + x;

					if (i >= 0)
							sum += pix[i]*m_pix[r];
				}
			}
			/*заполним пиксели результирующего изображения*/
			o_pix[io*sx + jo] = sum / (winsum + 0.5);
            /*
            p3=(o_pix[io*sx + jo]-Kt*o_pix[io*sx + jo]+Thr)/255.0;
            p3=(exp(Ko*p3)-exp(-Ko*p3))/(exp(Ko*p3)+exp(-Ko*p3));
            o_pix[io*sx + jo] = (127.5+127.5*p3) ;
            */
		}
	}
    *pDstImage = tmp_img;

CLEANUP:
return res;
}

AWPRESULT awpFilter(awpImage* pImage, AWPINT options, AWPINT method)
{
	AWPRESULT res = AWP_OK;
	awpImage *pMaskImage = NULL, *pTempImage = NULL;
	awpImage *pDImage = NULL, *pDMaskImage = NULL;
	awpImage* pResultImage = NULL;
	AWPWORD wMSizeX, wMSizeY;
	void* pMaskArray = NULL;

	switch (options)
	{
	case AWP_BLUR:
		wMSizeX = wMSizeY = 3;
		pMaskArray = blur_mask;
		break;
	case AWP_BLUR_MORE:
		wMSizeX = wMSizeY = 5;
		pMaskArray = blur_more_mask;
		break;
	case AWP_SHARPEN:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sharpen_mask;
		break;
	case AWP_SHARPEN_MORE:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sharpen_more_mask;
		break;
	case AWP_SHARPEN_EDGES:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sharpen_edges_mask;
		break;
	case AWP_FIND_EDGES:
		wMSizeX = wMSizeY = 3;
		pMaskArray = find_edges_mask;
		break;
	case AWP_FIND_EDGES1:
		wMSizeX = wMSizeY = 3;
		pMaskArray = find_edges1_mask;
		break;
	case AWP_EMBOSS_NORTH:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_north_mask;
		break;
	case AWP_EMBOSS_NORTH_OST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_north_ost_mask;
		break;
	case AWP_EMBOSS_OST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_ost_mask;
		break;
	case AWP_EMBOSS_SOUTH_OST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_south_ost_mask;
		break;
	case AWP_EMBOSS_SOUTH:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_south_mask;
		break;
	case AWP_EMBOSS_SOUTH_WEST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_south_west_mask;
		break;
	case AWP_EMBOSS_WEST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_west_mask;
		break;
	case AWP_EMBOSS_NORTH_WEST:
		wMSizeX = wMSizeY = 3;
		pMaskArray = emboss_north_west_mask;
		break;
	case AWP_PREWITT_V:
		wMSizeX = wMSizeY = 3;
		pMaskArray = prewitt_v_mask;
		break;
	case AWP_PREWITT_H:
		wMSizeX = wMSizeY = 3;
		pMaskArray = prewitt_h_mask;
		break;
	case AWP_SOBEL_V:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sobel_v_mask;
		break;
	case AWP_SOBEL_H:
		wMSizeX = wMSizeY = 3;
		pMaskArray = sobel_h_mask;
		break;
	default:
		res = AWP_BADARG;
		_ERR_EXIT_
	}

//	_CHECK_RESULT_((res = awpCreateImage(&pMaskImage, wMSizeX, wMSizeY, 1, AWP_SHORT)))
//	memcpy((void*)pMaskImage->pPixels, pMaskArray, wMSizeY*wMSizeX*sizeof(SHORT));

	switch (method)
	{
	case AWP_FILTER_METHOD_ORDINARY:
		_CHECK_RESULT_((res = awpCreateImage(&pMaskImage, wMSizeX, wMSizeY, 1, AWP_SHORT)))
		memcpy((void*)pMaskImage->pPixels, pMaskArray, wMSizeY*wMSizeX*sizeof(AWPSHORT));
		if (pImage->dwType != AWP_BYTE)
		{
			_CHECK_RESULT_((res = awpCopyImage(pImage, &pTempImage)))
			_CHECK_RESULT_((res = awpConvert(pTempImage, AWP_CONVERT_TO_BYTE)))
			_CHECK_RESULT_((res = __awpMaskConvolution(pTempImage, pMaskImage)))

			switch (pImage->dwType)
			{
			case AWP_SHORT:
				_CHECK_RESULT_((res = awpConvertV2(pTempImage, AWP_CONVERT_TO_SHORT)))
				memcpy((void*) pImage->pPixels, (void*) pTempImage->pPixels, pImage->sSizeX * pImage->sSizeY * sizeof(AWPSHORT));
				break;
			case AWP_DOUBLE:
				_CHECK_RESULT_((res = awpConvert(pTempImage, AWP_CONVERT_TO_DOUBLE)))
				memcpy((void*) pImage->pPixels, (void*) pTempImage->pPixels, pImage->sSizeX * pImage->sSizeY * sizeof(AWPDOUBLE));
				break;
			default:
				res = AWP_NOTSUPPORT;
				_ERR_EXIT_
			}
		}
		else
			_CHECK_RESULT_((res = __awpMaskConvolution(pImage, pMaskImage)))

		break;
	case AWP_FILTER_METHOD_DOUBLE:
		_CHECK_RESULT_((res = awpCreateImage(&pMaskImage, wMSizeX, wMSizeY, 1, AWP_DOUBLE)))
		memcpy((void*)pMaskImage->pPixels, pMaskArray, wMSizeY*wMSizeX*sizeof(AWP_DOUBLE));
		if (pImage->dwType == AWP_DOUBLE)
		{
			//_CHECK_RESULT_((res = awpCopyImage(pImage, &pTempImage)))
			//_CHECK_RESULT_((res = awpConvert(pTempImage, AWP_CONVERT_TO_BYTE)))
			_CHECK_RESULT_((res = awpMaskConvolutionDbl(pImage, pMaskImage, &pResultImage)))
			memcpy((void*) pImage->pPixels, (void*) pResultImage->pPixels, pImage->sSizeX * pImage->sSizeY * sizeof(AWPDOUBLE));
		}
		break;
	default:
		res = AWP_BADARG;
		_ERR_EXIT_
	}

CLEANUP:
	if (pDImage != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pDImage);
		else
			awpReleaseImage(&pDImage);

	if (pDMaskImage != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pDMaskImage);
		else
			awpReleaseImage(&pDMaskImage);

	if (pMaskImage != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pMaskImage);
		else
			awpReleaseImage(&pMaskImage);

	if (pTempImage != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pTempImage);
		else
			awpReleaseImage(&pTempImage);
	
	if (pResultImage != NULL)
		if (res == AWP_OK)
			res = awpReleaseImage(&pResultImage);
		else
			awpReleaseImage(&pResultImage);
	
	return res;
}


#define _AWP_CONVOLUTION2_(u,v,z) AWPINT x,y,xx,yy,c,xq,yq;											\
	AWPBYTE ch = 0;																					\
	AWPDOUBLE ks = 0;																					\
	AWPDOUBLE dsum = 0;																				\
	awpImage* pChannel = NULL;																		\
	awpImage* pDstChannel = NULL;																	\
	awpImage* pSum = NULL;																			\
	AWPINT dw = (AWPINT)floor((AWPFLOAT)pKernel->sSizeX /2);														\
	u* kf = (u*)pKernel->pPixels;																	\
	v* spix = NULL;																					\
	v* dpix = NULL;																					\
	if (dw >= pSrc->sSizeX || dw >= pSrc->sSizeY)													\
		 _ERROR_EXIT_RES_(AWP_BADARG);																\
	_CHECK_RESULT_(res = awpCreateImage(&pSum, pKernel->bChannels, 1, 1, AWP_DOUBLE));				\
	_CHECK_RESULT_(res = awpGetSumPix(pKernel, pSum));												\
	ks = ((AWPDOUBLE*)pSum->pPixels)[0]; if (ks == 0) ks = 1;											\
	_CHECK_RESULT_(res = awpCreateImage(&pDstChannel, pSrc->sSizeX, pSrc->sSizeY, 1, pSrc->dwType))	\
	_CHECK_RESULT_(res = awpCreateImage(&pChannel, pSrc->sSizeX, pSrc->sSizeY, 1, pSrc->dwType))	\
	dpix = (v*)pDstChannel->pPixels;																\
	for (ch = 0; ch < pSrc->bChannels; ch++)														\
	{_CHECK_RESULT_(res = awpGetChannel2(pSrc, pChannel, ch));										\
		spix = (v*)pChannel->pPixels;																\
		for (y = 0; y < pSrc->sSizeY; y++)															\
		{for (x = 0; x < pSrc->sSizeX; x++)															\
			{dsum = 0;																				\
				c = 0;																				\
				for (yy = y-dw; yy <= y + dw; yy++)													\
				{yq = yy;																			\
					if (yy < 0)																		\
						yq = abs(yy);																\
					if (yy >= pSrc->sSizeY)															\
						yq = 2*pSrc->sSizeY - yy;													\
					for(xx = x - dw; xx <= x + dw; xx++)											\
					{																				\
						xq = xx;																	\
						if (xx < 0)																	\
							xq = abs(xx);															\
						if (xx >= pSrc->sSizeX)														\
							xq = 2*pSrc->sSizeX - xx;												\
						dsum += spix[yq*pSrc->sSizeX + xq]*kf[c++];									\
					}																				\
				}																					\
				dpix[y*pSrc->sSizeX + x] = (v)(dsum / ks);											\
			}																						\
		}																							\
	_CHECK_RESULT_(res = awpPutChannel(pDst, pDstChannel, ch));										\
	}																								\


AWPRESULT awpMaskConvolution2AWPAWPDOUBLEByte(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
//	_AWP_CONVOLUTION2_(AWPDOUBLE,AWPBYTE);
	AWPINT x,y,xx,yy,c,xq,yq;												
	AWPBYTE ch = 0;																					
	AWPDOUBLE ks = 0;																					
	AWPDOUBLE dsum = 0;																				
	awpImage* pChannel = NULL;																		
	awpImage* pDstChannel = NULL;																	
	awpImage* pSum = NULL;
	AWPDOUBLE v;
	AWPINT dw = (AWPINT)floor((AWPFLOAT)pKernel->sSizeX /2);														
	AWPDOUBLE* kf = (AWPDOUBLE*)pKernel->pPixels;																	
	AWPBYTE* spix = NULL;																					
	AWPBYTE* dpix = NULL;																					
	if (dw >= pSrc->sSizeX || dw >= pSrc->sSizeY)													
		 _ERROR_EXIT_RES_(AWP_BADARG);																
	_CHECK_RESULT_(res = awpCreateImage(&pSum, pKernel->bChannels, 1, 1, AWP_DOUBLE));				
	_CHECK_RESULT_(res = awpGetSumPix(pKernel, pSum));												
	ks = ((AWPDOUBLE*)pSum->pPixels)[0]; if (ks == 0) ks = 1;											
	_CHECK_RESULT_(res = awpCreateImage(&pDstChannel, pSrc->sSizeX, pSrc->sSizeY, 1, pSrc->dwType))	
	dpix = (AWPBYTE*)pDstChannel->pPixels;																
	for (ch = 0; ch < pSrc->bChannels; ch++)														
	{_CHECK_RESULT_(res = awpGetChannel(pSrc, &pChannel, ch));										
		spix = (AWPBYTE*)pChannel->pPixels;																
		for (y = 0; y < pSrc->sSizeY; y++)															
		{for (x = 0; x < pSrc->sSizeX; x++)															
			{dsum = 0;																				
				c = 0;																				
				for (yy = y-dw; yy <= y + dw; yy++)													
				{yq = yy;																			
					if (yy < 0)																		
						yq = abs(yy);																
					if (yy >= pSrc->sSizeY)															
						yq = 2*pSrc->sSizeY - yy;													
					for(xx = x - dw; xx <= x + dw; xx++)											
					{																				
						xq = xx;																	
						if (xx < 0)																	
							xq = abs(xx);															
						if (xx >= pSrc->sSizeX)														
							xq = 2*pSrc->sSizeX - xx;												
						dsum += spix[yq*pSrc->sSizeX + xq]*kf[c++];									
					}																				
				}																					
				v = 128 + dsum / ks; if (v < 0) v = 0; if (v > 255) v = 255;
				dpix[y*pSrc->sSizeX + x] = (AWPBYTE)(v);											
			}																						
		}																							
	_CHECK_RESULT_(res = awpPutChannel(pDst, pDstChannel, ch));										
	_SAFE_RELEASE_(pChannel);																		
	}							
CLEANUP:
   _SAFE_RELEASE_(pChannel)
   _SAFE_RELEASE_(pDstChannel)
   _SAFE_RELEASE_(pSum)
 return res;
}
AWPRESULT awpMaskConvolution2AWPAWPDOUBLEShort(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
	_AWP_CONVOLUTION2_(AWPDOUBLE,AWPSHORT,FALSE);
CLEANUP:
   _SAFE_RELEASE_(pChannel)
   _SAFE_RELEASE_(pDstChannel)
   _SAFE_RELEASE_(pSum)
 return res;
}
AWPRESULT awpMaskConvolution2AWPAWPDOUBLEFloat(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
	_AWP_CONVOLUTION2_(AWPDOUBLE,AWPFLOAT,FALSE);
CLEANUP:
   _SAFE_RELEASE_(pChannel)
   _SAFE_RELEASE_(pDstChannel)
   _SAFE_RELEASE_(pSum)
 return res;
}
AWPRESULT awpMaskConvolution2AWPAWPDOUBLEAWPAWPDOUBLE(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
	_AWP_CONVOLUTION2_(AWPDOUBLE,AWPDOUBLE,FALSE);
CLEANUP:
   _SAFE_RELEASE_(pChannel)
   _SAFE_RELEASE_(pDstChannel)
   _SAFE_RELEASE_(pSum)
 return res;
}


AWPRESULT awpMaskConvolution2AWPAWPDOUBLEKernel(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
	switch(pSrc->dwType)
	{
	case AWP_BYTE:
		_CHECK_RESULT_(res = awpMaskConvolution2AWPAWPDOUBLEByte(pSrc,pDst,pKernel))
		break;
	case AWP_SHORT:
		_CHECK_RESULT_(res = awpMaskConvolution2AWPAWPDOUBLEShort(pSrc,pDst,pKernel))
		break;
	case AWP_FLOAT:
		_CHECK_RESULT_(res = awpMaskConvolution2AWPAWPDOUBLEFloat(pSrc,pDst,pKernel))
		break;
	case AWP_DOUBLE:
		_CHECK_RESULT_(res = awpMaskConvolution2AWPAWPDOUBLEAWPAWPDOUBLE(pSrc,pDst,pKernel))
		break;
	}
CLEANUP:
    return res;
}


AWPRESULT awpMaskConvolution2FloatByte(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
	_AWP_CONVOLUTION2_(AWPFLOAT,AWPBYTE,TRUE);
CLEANUP:
   _SAFE_RELEASE_(pChannel)
   _SAFE_RELEASE_(pDstChannel)
   _SAFE_RELEASE_(pSum)
 return res;
}
AWPRESULT awpMaskConvolution2FloatShort(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
	_AWP_CONVOLUTION2_(AWPFLOAT,AWPSHORT,FALSE);
CLEANUP:
   _SAFE_RELEASE_(pChannel)
   _SAFE_RELEASE_(pDstChannel)
   _SAFE_RELEASE_(pSum)
 return res;
}
AWPRESULT awpMaskConvolution2FloatFloat(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
	_AWP_CONVOLUTION2_(AWPFLOAT,AWPFLOAT,FALSE);
CLEANUP:
   _SAFE_RELEASE_(pChannel)
   _SAFE_RELEASE_(pDstChannel)
   _SAFE_RELEASE_(pSum)
 return res;
}
/*

*/
AWPRESULT awpMaskConvolution2FloatAWPAWPDOUBLE(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
	_AWP_CONVOLUTION2_(AWPFLOAT,AWPDOUBLE,FALSE);
CLEANUP:
   _SAFE_RELEASE_(pChannel)
   _SAFE_RELEASE_(pDstChannel)
   _SAFE_RELEASE_(pSum)
 return res;
}

AWPRESULT awpMaskConvolution2FloatKernel(awpImage* pSrc, awpImage* pDst, awpImage* pKernel)
{
    AWPRESULT res = AWP_OK;
	switch(pSrc->dwType)
	{
	case AWP_BYTE:
		_CHECK_RESULT_(res = awpMaskConvolution2FloatByte(pSrc,pDst,pKernel))
		break;
	case AWP_SHORT:
		_CHECK_RESULT_(res = awpMaskConvolution2FloatShort(pSrc,pDst,pKernel))
		break;
	case AWP_FLOAT:
		_CHECK_RESULT_(res = awpMaskConvolution2FloatFloat(pSrc,pDst,pKernel))
		break;
	case AWP_DOUBLE:
		_CHECK_RESULT_(res = awpMaskConvolution2FloatAWPAWPDOUBLE(pSrc,pDst,pKernel))
		break;
	}

CLEANUP:
    return res;
}


/*
	awpMaskConvolution2 - performs mask convolution operator with AWPDOUBLE and AWPFLOAT kernels
*/
AWPRESULT awpMaskConvolution2(awpImage* pSrcImage, awpImage* pDst, awpImage* pMask)
{
    AWPRESULT res = AWP_OK;

	_CHECK_RESULT_(res = awpCheckImage(pSrcImage))
    _CHECK_RESULT_(res = awpCheckImage(pDst))
    _CHECK_RESULT_(res = awpCheckImage(pMask))

    _CHECK_SAME_SIZES(pSrcImage, pDst)
    _CHECK_SAME_TYPE(pSrcImage,pDst)
    _CHECK_NUM_CHANNELS(pSrcImage, pDst)	
	
	if (pMask->dwType == AWP_DOUBLE)
	{
		_CHECK_RESULT_(res = awpMaskConvolution2AWPAWPDOUBLEKernel(pSrcImage, pDst, pMask))
	}
	else if (pMask->dwType == AWP_FLOAT)
	{
		_CHECK_RESULT_(res = awpMaskConvolution2FloatKernel(pSrcImage, pDst, pMask));
	}
	else
		_ERROR_EXIT_RES_(AWP_BADARG)
	
CLEANUP:
    return res;
}
