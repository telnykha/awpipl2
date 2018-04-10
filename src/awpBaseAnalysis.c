/*M
//
//      awpipl2 image processing and image analysis library 
//		File: awpBaseAnalysis.c
//		Purpose: histogramm image analysis implementation 
//
//      CopyRight 2004-2018 (c) ALT-Soft.net
//M*/
#include "_awpipl.h"
#pragma warning(disable: 4996)

/*
we assume that AWP_BYTE images has a min_value = 0 and max_value = 255. 
step between bins defined as 256 / hst->sSizeX
hst->sSizeX cannot be more than 256
*/
static AWPRESULT  _awpGetHstByte(awpImage* pImage, awpImage** ppHst)
{
	AWPRESULT  res = AWP_OK;
	awpImage*  pHst = *ppHst;
	AWPDOUBLE  hst_step = 256. / (AWPDOUBLE)pHst->sSizeX;

	AWPDOUBLE* pix  = _AWP_BPIX_(pHst, AWPDOUBLE);
	AWPBYTE*   spix = _AWP_BPIX_(pImage, AWPBYTE);

	AWPWORD    i = 0;
	AWPBYTE    c = 0;
	AWPWORD    idx = 0;
	AWPWORD    x = 0;
	AWPWORD    y = 0;

	/*setup histogramm value axis*/
	for (i = 0; i < pHst->sSizeX; i++)
	{
		for (c = 0; c < pHst->bChannels; c++)
		{
			pix[i*pHst->bChannels + c] = floor(i*hst_step + 0.5);
		}
	}
	/*calc histogramm*/
	for (y = 0; y < pImage->sSizeY; y++)
	{
		for (x = 0; x < pImage->sSizeX; x++)
		{
			for (c = 0; c < pImage->bChannels; c++)
			{
				idx = (AWPBYTE)floor(spix[y*pImage->bChannels*pImage->sSizeX + x*pImage->bChannels + c] + 0.5);
				pix[pHst->sSizeX*pHst->bChannels + idx*pHst->bChannels + c]++;
			}
		}
	}

CLEANUP:
	return res;
}
#define _CALC_IDX_(v)\
	idx = (AWPBYTE)floor(v[y*pImage->bChannels*pImage->sSizeX + x*pImage->bChannels + c] + 0.5 - image_min[c]);


/*
for other datatypes we use histogramm between min value on the channel data 
and max value. num bins defined as histogramm width
*/
static AWPRESULT _awpGetHst(awpImage* pImage, awpImage** ppHst)
{
	AWPRESULT  res = AWP_OK;
	awpImage*  pHst = *ppHst;
	AWPDOUBLE*       image_min = NULL;
	AWPDOUBLE*       image_max = NULL;
	AWPDOUBLE*		 hst_step = NULL;
	AWPWORD    i = 0;
	AWPBYTE    c = 0;
	AWPWORD    idx = 0;
	AWPWORD    x = 0;
	AWPWORD    y = 0;
	AWPDOUBLE* pix = _AWP_BPIX_(pHst, AWPDOUBLE);
	AWPSHORT*  spix = NULL;
	AWPFLOAT*  fpix = NULL;
	AWPDOUBLE* dpix = NULL;

	/*range of signal variation*/
	if (res = awpMinMax(pImage, &image_min, &image_max))
	{
		if (res != AWP_OK)
			_ERR_EXIT_
	}

	/*setup width step for each channel*/
	hst_step = (AWPDOUBLE*)malloc(pImage->bChannels*sizeof(AWPDOUBLE));
	if (hst_step == NULL)
		_ERROR_EXIT_RES_(AWP_NOTENOUGH_MEM)

		for (i = 0; i < pImage->bChannels; i++)
			hst_step[i] = (image_max[i] - image_min[i]) / pHst->sSizeX;

	/*setup histogramm value axis*/
	for (i = 0; i < pHst->sSizeX; i++)
	{
		for (c = 0; c < pHst->bChannels; c++)
		{
			pix[i*pHst->bChannels + c] = floor(i*hst_step[c] + 0.5);
		}
	}
	/*calc histogramm*/
	switch (pImage->dwType)
	{
	case AWP_SHORT:
		spix = _AWP_BPIX_(pImage, AWPSHORT)
			break;
	case AWP_FLOAT:
		fpix = _AWP_BPIX_(pImage, AWPFLOAT)
			break;
	case AWP_DOUBLE:
		dpix = _AWP_BPIX_(pImage, AWPDOUBLE)
			break;
	}
	for (y = 0; y < pImage->sSizeY; y++)
	{
		for (x = 0; x < pImage->sSizeX; x++)
		{
			for (c = 0; c < pImage->bChannels; c++)
			{
				switch (pImage->dwType)
				{
				case AWP_SHORT:
					_CALC_IDX_(spix)
						break;
				case AWP_FLOAT:
					_CALC_IDX_(fpix)
						break;
				case AWP_DOUBLE:
					_CALC_IDX_(dpix)
						break;
				}
				pix[pHst->sSizeX*pHst->bChannels + idx*pHst->bChannels + c]++;
			}
		}
	}

CLEANUP:
	if (image_min != NULL)
		free(image_min);
	if (image_max != NULL)
		free(image_max);
	if (hst_step != NULL)
		free(hst_step);
	return res;
}
/*
obtain image histogramm function
image histogramm is stored in the awpImage with height = 2
and has num channels as in the source image pImage
first row of the histogramm image represents value of brightess
second row - number of pixels of this brightess
for AWP_BYTE images num bins in the histogramm should be less than 
256 and more then 1. 
for other image types num bins should be more 1. 
*/
AWPRESULT awpGetHst(awpImage* pImage, awpImage** ppHst, AWPINT options)
{
	AWPRESULT res  = AWP_OK;
	awpImage* pHst = NULL;
	if (awpCheckImage(pImage) != AWP_OK || ppHst == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	if (*ppHst == NULL)
	{
		/*create image for histogramm. default image has 256 pin histogramm*/
		if (awpCreateImage(ppHst, 256, 2, pImage->bChannels, AWP_DOUBLE) != AWP_OK)
			_ERROR_EXIT_RES_(AWP_BADMEMORY)
	}
	else
	{
		/*check histogramm image*/
		pHst = *ppHst; 
		if (awpCheckImage(pHst) != AWP_OK)
			_ERROR_EXIT_RES_(AWP_BADARG)

		if (pHst->bChannels != pImage->bChannels || pHst->sSizeY != 2 || 
			pHst->dwType != AWP_DOUBLE || pHst->sSizeX < 2)
			_ERROR_EXIT_RES_(AWP_BADARG)
	}

	if (pImage->dwType == AWP_BYTE)
	{
		/*make histogramm for the 8-bit image*/
		res = _awpGetHstByte(pImage, ppHst);
	}
	else
	{
		/*make histogramm for the  images of other datatypes*/
		res = _awpGetHst(pImage, ppHst);
	}

CLEANUP:
	return res;
}

#define _AWP_BASE_ANALYSIS_CHECK_HST_(v)\
if (res = awpCheckImage(v) != AWP_OK)\
	_ERR_EXIT_;\
if (v->sSizeY != 2 || v->dwType != AWP_DOUBLE)\
	_ERROR_EXIT_RES_(AWP_BADARG)\

#define _AWP_BASE_ANALYSIS_CHECK_RESULT_(v)\
if (res = awpCheckImage(v) != AWP_OK)\
	_ERR_EXIT_;\
if (v->sSizeX != 1 || v->sSizeY != 1)\
	_ERROR_EXIT_RES_(AWP_BADARG)\
if (v->dwType != AWP_DOUBLE)\
	_ERROR_EXIT_RES_(AWP_BADARG)\

/*
find the k-th moment of the distribution pHst ans store them to the pRes 
*/
static AWPRESULT __awpHstMomemt(awpImage* pHst, awpImage* pMoment, AWPDOUBLE k)
{
	AWPRESULT res = AWP_OK;
	AWPWORD	x = 0;
	AWPBYTE c = 0;
	AWPDOUBLE* hst		= NULL;
	AWPDOUBLE* Moment	= NULL;

	if (pHst == NULL || pMoment == NULL || k < 1 || k>3)
		_ERROR_EXIT_RES_(AWP_BADARG)

	_AWP_BASE_ANALYSIS_CHECK_HST_(pHst);
	_AWP_BASE_ANALYSIS_CHECK_RESULT_(pMoment);

	for (x = 0; x < pHst->sSizeX; x++)
	{
		for (c = 0; c < pHst->bChannels; c++)
		{
			Moment[c] += (pow(x, k)*hst[pHst->bChannels*pHst->sSizeX + x*pHst->bChannels + c]);
		}
	}

CLEANUP:
	return res;
}

/*
find the mean over histogramm 
*/
AWPRESULT awpGetHstMean(awpImage* pHst, awpImage* pMean)
{
	AWPRESULT res = AWP_OK;

	if (pHst == NULL || pMean == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)

	_AWP_BASE_ANALYSIS_CHECK_HST_(pHst);
	_AWP_BASE_ANALYSIS_CHECK_RESULT_(pMean);

	if (res = __awpHstMomemt(pHst, pMean, 1) != AWP_OK)
		_ERR_EXIT_

CLEANUP:
	return res;
}
// the function of calculating the initial moments of the distribution
static void _awpMoment(awpHistogramm* hst, awpStat* r, AWPBYTE nOrder)
{

	AWPINT n	= 0;
	AWPINT nr	= 0;
	AWPINT ng  = 0;
	AWPINT nb	= 0;
	AWPINT i = 0;
        AWPINT j = 0;

	AWPDOUBLE f = 1.0;

	r->dRed	= 0;
	r->dGreen	= 0;
	r->dBlue	= 0;
	r->dBright	= 0;

	for (i = 0; i < 256; i++)
	{
		n	+= (AWPINT)hst->Intensity[i];
		nr	+= (AWPINT)hst->ColorData.RedChannel[i];
		ng	+= (AWPINT)hst->ColorData.GreenChannel[i];
		nb	+= (AWPINT)hst->ColorData.BlueChannel[i];
		f = 1.0;
		for(j = 0; j < nOrder; j++)
			f *= i;

		r->dRed		+= hst->ColorData.RedChannel[i]*f;
		r->dGreen	+= hst->ColorData.GreenChannel[i]*f;
		r->dBlue	+= hst->ColorData.BlueChannel[i]*f;
		r->dBright	+= hst->Intensity[i]*f;

	}

	if (n != 0)	r->dBright /= n;
	if (nr!= 0)	r->dRed /= nr;
	if (ng!= 0)	r->dGreen /= ng;
	if (nb!= 0)	r->dBlue /= nb;
}
// copy the histogram into four arrays (by the number of channels)
static void _awpGetHstChannels(awpHistogramm* hst, AWPDOUBLE* rc, 
							   AWPDOUBLE* gc, AWPDOUBLE* bc, AWPDOUBLE* ic)
{
	AWPINT i = 0;
	for (i = 0; i < 256; i++)
	{
		rc[i] = hst->ColorData.RedChannel[i];
		gc[i] = hst->ColorData.GreenChannel[i];
		bc[i] = hst->ColorData.BlueChannel[i];
		ic[i] = hst->Intensity[i];
	}
}

AWPRESULT awpGet2DHistogramm(awpImage* pImage, awpImage* p2DHist, AWPBYTE low, AWPBYTE up, AWPINT needToConvert)
{
    AWPINT i, j;
    AWPDOUBLE*     dblPix;
    awpColor*   clPix;
    AWPBYTE        h,s, v;
    AWPRESULT res = AWP_OK;
    awpImage* pDblHst = NULL;
	// checking arguments
    if (pImage == NULL || p2DHist == NULL)
        return AWP_BADARG;
    if (pImage->dwType != AWP_BYTE)
        return AWP_BADARG;
    if (pImage->bChannels != 3)
        return AWP_BADARG;

    if (p2DHist->dwType != AWP_BYTE && p2DHist->dwType != AWP_DOUBLE )
        return AWP_BADARG;
    if (p2DHist->bChannels != 1)
        return AWP_BADARG;

	// to build a two-dimensional histogram we use the 1st and 2nd channels of the image
	// pImage and write the histogram into a temporary image like AWPDOUBLE
    res = awpCreateImage(&pDblHst, 256, 256, 1, AWP_DOUBLE);
    if (res != AWP_OK)
        return res;
    dblPix = (AWPDOUBLE*)pDblHst->pPixels;
    clPix  = (awpColor*)pImage->pPixels;
    for (i = 0; i < pImage->sSizeY; i++)
    {
        for (j = 0; j < pImage->sSizeX; j++)
        {
            v = clPix[i*pImage->sSizeX + j].bBlue;
            if (v>= low && v <= up)
            {
              h = clPix[i*pImage->sSizeX + j].bRed;
              s = clPix[i*pImage->sSizeX + j].bGreen;
              dblPix[s*256 + h]++;
            }
        }
    }
	// convert a temporary image of type AWPDOUBLE to AWPBYTE
    if ( needToConvert == TRUE )
    {
        if ((res = awpConvert(pDblHst, AWP_CONVERT_TO_BYTE_WITH_NORM)) != AWP_OK)
            return res;
    }
    if((res = awpFastCopyImage(pDblHst, p2DHist)) != AWP_OK)
        return res;
    if ((res = awpReleaseImage(&pDblHst)) != AWP_OK)
        return res;
    if ((res = awpFilter(p2DHist, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)) != AWP_OK)
        return res;
    return AWP_OK;
}


AWPRESULT awpGetAverage(const awpHistogramm* Histogramm, awpStat* Result)
{

	if (Histogramm == NULL)
        return AWP_BADMEMORY;
	if (Result == NULL)
        return AWP_BADMEMORY;
	memset(Result, 0, sizeof(awpStat));

	_awpMoment((awpHistogramm*)Histogramm, Result, 1);
	
	return AWP_OK;
}

AWPRESULT awpGetDispersion(const awpHistogramm* Histogramm, awpStat* Result)
{
	awpStat m1;
	awpStat m2;

	if (Histogramm == NULL)
        return AWP_BADMEMORY;
	if (Result == NULL)
        return AWP_BADMEMORY;

	memset(Result, 0, sizeof(awpStat));
	memset(&m1, 0, sizeof(awpStat));
	memset(&m2, 0, sizeof(awpStat));

	_awpMoment((awpHistogramm*)Histogramm, &m1, 1);
	_awpMoment((awpHistogramm*)Histogramm, &m2, 2);

	Result->dRed = sqrt(m2.dRed - m1.dRed*m1.dRed);
	Result->dGreen = sqrt(m2.dGreen - m1.dGreen*m1.dGreen);
	Result->dBlue = sqrt(m2.dBlue - m1.dBlue*m1.dBlue);
	Result->dBright = sqrt(m2.dBright - m1.dBright*m1.dBright);

	return AWP_OK;
}


// compare two numbers with double precision
static AWPINT AWP_CDECL _awpDCompare(const void* a1, const void* a2)
{
	AWPDOUBLE* e1 = (AWPDOUBLE*)a1;
	AWPDOUBLE* e2 = (AWPDOUBLE*)a2;

	if (*e1 > *e2) 
		return 1;
	else if (*e1 < *e2)
		return -1;
	else
		return 0;
}

AWPRESULT awpGetMedian(const awpHistogramm* Histogramm, awpStat* Result)
{
	AWPDOUBLE r[256];
	AWPDOUBLE g[256];
	AWPDOUBLE b[256];
	AWPDOUBLE i[256];

	if (Histogramm == NULL)
        return AWP_BADMEMORY;
	if (Result == NULL)
        return AWP_BADMEMORY;

	memset(Result, 0, sizeof(awpStat));
	memset(r, 0, 256*sizeof(AWPDOUBLE));
	memset(g, 0, 256*sizeof(AWPDOUBLE));
	memset(b, 0, 256*sizeof(AWPDOUBLE));
	memset(i, 0, 256*sizeof(AWPDOUBLE));

	_awpGetHstChannels((awpHistogramm*)Histogramm, r,g,b,i);

	qsort((void*)r, 256, sizeof(AWPDOUBLE), _awpDCompare);
	qsort((void*)g, 256, sizeof(AWPDOUBLE), _awpDCompare);
	qsort((void*)b, 256, sizeof(AWPDOUBLE), _awpDCompare);
	qsort((void*)i, 256, sizeof(AWPDOUBLE), _awpDCompare);

	Result->dRed	= r[128];
	Result->dGreen	= g[128];
	Result->dBlue	= b[128];
	Result->dBright = i[128];

	return AWP_OK;
}

AWPRESULT awpGetEntropy(const awpHistogramm* Histogramm, awpStat* Result)
{
	awpStat en; 
	AWPFLOAT n	= 0;
	AWPFLOAT nr	= 0;
	AWPFLOAT ng  = 0;
	AWPFLOAT nb	= 0;
	AWPINT i = 0;
	memset(&en,0,sizeof(awpStat));

	if (Histogramm == NULL)
        return AWP_BADMEMORY;
	if (Result == NULL)
        return AWP_BADMEMORY;
	memset(Result, 0, sizeof(awpStat));
	
	for (i = 0; i < 256; i++)
	{
		n	+= (AWPINT)Histogramm->Intensity[i];
		nr	+= (AWPINT)Histogramm->ColorData.RedChannel[i];
		ng	+= (AWPINT)Histogramm->ColorData.GreenChannel[i];
		nb	+= (AWPINT)Histogramm->ColorData.BlueChannel[i];

		if (Histogramm->Intensity[i] != 0)
			en.dBright += Histogramm->Intensity[i]*log(Histogramm->Intensity[i]);

		if (Histogramm->ColorData.RedChannel[i] != 0)
			en.dRed += Histogramm->ColorData.RedChannel[i]*log(Histogramm->ColorData.RedChannel[i]);

		if (Histogramm->ColorData.GreenChannel[i] != 0)
			en.dGreen += Histogramm->ColorData.GreenChannel[i]*log(Histogramm->ColorData.GreenChannel[i]);

		if (Histogramm->ColorData.BlueChannel[i] != 0)
			en.dBlue += Histogramm->ColorData.BlueChannel[i]*log(Histogramm->ColorData.BlueChannel[i]);
	}

	if (n != 0)
		Result->dBright = (log(n) - en.dBright / n) / log(2.f);
	if (nr != 0)
		Result->dRed	= (log(nr) - en.dRed / nr) / log(2.f);
	if (ng != 0)
		Result->dGreen	= (log(ng) - en.dGreen / ng) / log(2.f);
	if (nb != 0)
		Result->dBlue	= (log(nb) - en.dBlue / nb) / log(2.f);

	return AWP_OK;
}
// finding the slope
AWPRESULT awpGetSkewness(const awpHistogramm* Histogramm, awpStat* Result)
{
	awpStat m1;
	awpStat m2;
	awpStat	m3;
	awpStat dsp;

	if (Histogramm == NULL)
        return AWP_BADMEMORY;
	if (Result == NULL)
        return AWP_BADMEMORY;

	memset(Result, 0, sizeof(awpStat));
	memset(&m1, 0, sizeof(awpStat));
	memset(&m2, 0, sizeof(awpStat));
	memset(&m3, 0, sizeof(awpStat));
	memset(&dsp, 0, sizeof(awpStat));

	_awpMoment((awpHistogramm*)Histogramm, &m1, 1);
	_awpMoment((awpHistogramm*)Histogramm, &m2, 2);
	_awpMoment((awpHistogramm*)Histogramm, &m3, 3);
	awpGetDispersion(Histogramm, &dsp);

	if (dsp.dRed != 0)
		Result->dRed = (m3.dRed - 2*m1.dRed*m2.dRed +2*m1.dRed*m1.dRed) 
						/ (dsp.dRed*dsp.dRed*dsp.dRed);

	if (dsp.dGreen != 0)
		Result->dGreen = (m3.dGreen - 2*m1.dGreen*m2.dGreen +2*m1.dGreen*m1.dGreen) 
						/ (dsp.dGreen*dsp.dGreen*dsp.dGreen);

	if (dsp.dBlue != 0)
		Result->dBlue = (m3.dBlue - 2*m1.dBlue*m2.dBlue +2*m1.dBlue*m1.dBlue) 
						/ (dsp.dBlue*dsp.dBlue*dsp.dBlue);

	if (dsp.dBright != 0)
		Result->dBright = (m3.dBright - 2*m1.dBright*m2.dBright +2*m1.dBright*m1.dBright) 
						/ (dsp.dBright*dsp.dBright*dsp.dBright);

	return AWP_OK;
}

AWPRESULT awpGetExcess(const awpHistogramm* Histogramm, awpStat* Result)
{
	awpStat m1;
	awpStat m2;
	awpStat	m3;
	awpStat	m4;
	awpStat dsp;

	if (Histogramm == NULL)
        return AWP_BADMEMORY;
	if (Result == NULL)
        return AWP_BADMEMORY;

	memset(Result, 0, sizeof(awpStat));
	memset(&m1, 0, sizeof(awpStat));
	memset(&m2, 0, sizeof(awpStat));
	memset(&m3, 0, sizeof(awpStat));
	memset(&m4, 0, sizeof(awpStat));
	memset(&dsp, 0, sizeof(awpStat));

	_awpMoment((awpHistogramm*)Histogramm, &m1, 1);
	_awpMoment((awpHistogramm*)Histogramm, &m2, 2);
	_awpMoment((awpHistogramm*)Histogramm, &m3, 3);
	_awpMoment((awpHistogramm*)Histogramm, &m4, 4);
	awpGetDispersion((awpHistogramm*)Histogramm, &dsp);


	if (dsp.dRed != 0)
		Result->dRed = (m4.dRed - 4*m1.dRed*m3.dRed +6*m1.dRed*m1.dRed*m2.dRed - 
						3*m1.dRed*m1.dRed*m1.dRed*m1.dRed) 
						/ (dsp.dRed*dsp.dRed*dsp.dRed*dsp.dRed) - 3;

	if (dsp.dGreen != 0)
		Result->dGreen = (m4.dGreen - 4*m1.dGreen*m3.dGreen +6*m1.dGreen*m1.dGreen* m2.dGreen -
						  3*m1.dGreen*m1.dGreen*m1.dGreen*m1.dGreen) 
						/ (dsp.dGreen*dsp.dGreen*dsp.dGreen*dsp.dGreen) - 3;

	if (dsp.dBlue != 0)
		Result->dBlue = (m4.dBlue - 4*m1.dBlue*m3.dBlue + 6*m1.dBlue*m1.dBlue*m2.dBlue -
						3*m1.dBlue*m1.dBlue*m1.dBlue*m1.dBlue) 
						/ (dsp.dBlue*dsp.dBlue*dsp.dBlue*dsp.dBlue) - 3;

	if (dsp.dBright != 0)
		Result->dBright = (m4.dBright - 4*m1.dBright*m3.dBright + 6*m1.dBright*m1.dBright*m2.dBright -
							3*m1.dBright*m1.dBright*m1.dBright*m1.dBright) 
						/ (dsp.dBright*dsp.dBright*dsp.dBright*dsp.dBright) - 3;

	return AWP_OK;
}

/*Histogramm equalize function*/
/*The function of the histogram equilibration.Brings an image histogram
pImage, to the form as close as possible to the uniform. */
AWPRESULT awpHistogrammEqualize(awpImage* pImage)
{

	    /*local variables */
	    AWPRESULT               res; /*the result of the operation */
	    awpHistogramm           Hst; /*image histogram */
        awpHistogramm           PFunc;
        AWPINT                     count,i,j;
	AWPBYTE* p			= (AWPBYTE*)pImage->pPixels;
	AWPDOUBLE SumOfEntries = 0.0;
        res = AWP_OK;
        memset((void*)&PFunc, 0, sizeof(awpHistogramm));
        /**/
        awpGetHistogramm(pImage, &Hst);
        awpGetPixCount(&Hst, &count);
	for (i = 0; i < 256; i++)
	{
		SumOfEntries = 0;
		for (j = 0; j <= i; j++)
			SumOfEntries += Hst.Intensity[j];

		PFunc.Intensity[i] = SumOfEntries / count;
	}
	for (i = 0; i < count; i++)
        	p[i] = (AWPBYTE)(255*PFunc.Intensity[p[i]]);


    return res;
}

AWPRESULT awpHistogrammNormalize(awpImage* pSource, AWPDOUBLE k1, AWPDOUBLE k2)
{
  AWPBYTE* ImIn;
  AWPBYTE* ImOut;
  awpImage* Im2;
  AWPINT Hd0[256],M[256];// the original histogram and the function of recalculating old intensities into new ones
  AWPINT Lx=pSource->sSizeX, Ly=pSource->sSizeY,n,m=0,s=0;
  long i; // "long" counter in case of big picture

  AWPDOUBLE K0=k1, K1=k2, Ks=Lx*Ly/255.0, sa=0.1;
  AWPDOUBLE GoalIntegralHistogramm[256]; // integrated histogram
  if(pSource->bChannels!=1) awpConvert(pSource, AWP_CONVERT_3TO1_BYTE);
  awpCopyImage(pSource, &Im2);

  ImIn = (AWPBYTE*)pSource->pPixels;
  ImOut = (AWPBYTE*)Im2->pPixels;
  for(n=0; n<256; n++) {Hd0[n]=0; M[n]=0;}// zeroing arrays
  for(i=0;i<Lx*Ly;i++)   Hd0[ImIn[i]]++; // calculation of the source histogram from the input image
  for(m=0;m<256;m++) GoalIntegralHistogramm[m]=Ks*(m+K0/255.0*(m*m-255*m)-K1/255.0/255.0*(2*m*m*m-3*255*m*m+255*255*m));
  n=0;m=0;s=0;
  a:if(s<Lx*Ly&&n<255&&m<255){
  if(s<sa) {do {s+=Hd0[n]; M[n]=m; n++; } while(s<sa);}
  if(s>=sa) {do {m++;sa=GoalIntegralHistogramm[m]+0.1;} while(s>=sa);}
  goto a; }
  if(n<255) do{M[n]=255;n++;} while(n<255); // Done to the end, if necessary
  if(m<255) do{M[n]=m; m++;} while(m<255);   
  for(i=0;i<Lx*Ly;i++) ImOut[i]=M[ImIn[i]];
  for(i=0;i<Lx*Ly;i++) ImIn[i]=ImOut[i];
  awpReleaseImage(&Im2);
  return AWP_OK;
}





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

	switch(pSrc->dwType)
	{
	case AWP_BYTE:
		bp = (AWPBYTE*)pSrc->pPixels;
		for (i=0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
		{
			for (j = 0; j < pSrc->bChannels; j++)
				rp[j] += bp[pSrc->bChannels*i + j];
		}
		break;
	case AWP_SHORT:
		sp = (short*)pSrc->pPixels;
		for (i=0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
		{
			for (j = 0; j < pSrc->bChannels; j++)
				rp[j] += sp[pSrc->bChannels*i + j];
		}
		break;
	case AWP_FLOAT:
		fp = (AWPFLOAT*)pSrc->pPixels;
		for (i=0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
		{
			for (j = 0; j < pSrc->bChannels; j++)
				rp[j] += fp[pSrc->bChannels*i + j];
		}
		break;
	case AWP_DOUBLE:
		dp = (AWPDOUBLE*)pSrc->pPixels;
		for (i=0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
		{
			for (j = 0; j < pSrc->bChannels; j++)
				rp[j] += dp[pSrc->bChannels*i + j];
		}
		break;
	}

CLEANUP:
    return res;	
}