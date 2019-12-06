#include "_awpipl.h"
/*
  awpCreateGaussKernel - creates Gauss kernel as with sigma. sigma  < 50
  high_precision = true - creates AWPDOUBLE kernel
  else creates AWPFLOAT kernel.
*/
AWPRESULT awpCreateGaussKernel(awpImage** ppKernel, AWPDOUBLE sigma, AWPBOOL high_precision)
{
    AWPRESULT res = AWP_OK;
    AWPDWORD type =  high_precision?AWP_DOUBLE:AWP_FLOAT;
    AWPSHORT x,y,hw,xx,yy;
    AWPWORD w = 0;
    void* p = NULL;
    AWPFLOAT* fp = NULL;
    AWPDOUBLE* dp = NULL;
    AWPDOUBLE  falfa = 1;
    AWPDOUBLE  fbeta = 1;
    if (sigma <= 0)
        _ERROR_EXIT_RES_(AWP_BADARG)
    w = (AWPWORD)(4*sigma);
    if (w < 3) w = 3;
	w = w % 2 == 0?w+1:w;
    hw = w / 2;
	
    res = awpCreateImage(ppKernel, w,w,1,type);
   // _CHECK_RESULT_(res)
    p = (*ppKernel)->pPixels;

    switch (type)
    {
        case AWP_FLOAT:
        {		
            fp = (AWPFLOAT*)p;
            falfa =  1. /(2.f*AWP_PI*sigma*sigma);
            fbeta = -1./(2.f*sigma*sigma);
            for (y=0; y < w; y++)
            {
                yy = y - hw;
                for (x = 0; x < w; x++)
                {
                    xx = x - hw;
                    fp[y*w + x] = (AWPFLOAT)(falfa*exp(fbeta*((AWPFLOAT)(xx*xx) +(AWPFLOAT)(yy*yy))));
                }
            }
        }
        break;
        case AWP_DOUBLE:
        {
            dp = (AWPDOUBLE*)p;
            falfa = 1. /(2.*3.14159*sigma*sigma);
            fbeta = -1./(2.*sigma*sigma);
            for (y=0; y < w; y++)
            {
                yy = y - hw;
                for (x = 0; x < w; x++)
                {
                    xx = x - hw;
                    dp[y*w + x] = falfa*exp(fbeta*(xx*xx +yy*yy));
                }
            }
        }
        break;
    }

CLEANUP:
    return res;
}

AWPRESULT awpGaussianBlurAWP_AWPAWPDOUBLE(awpImage* pSrc, awpImage* pDst, AWPFLOAT sigma)
{
    AWPRESULT res = AWP_OK;
	awpImage* pKernel = NULL;

	_CHECK_RESULT_(res = awpCreateGaussKernel(&pKernel, sigma, TRUE))
	_CHECK_RESULT_(res = awpMaskConvolution(pSrc, pDst, pKernel))


CLEANUP:
	_SAFE_RELEASE_(pKernel);
    return res;
}

/*
    awpGaussianBlur - performs Gaussian Blur operator over pSrc
    result of operator stores into pDst image. The size of Gaussian
    kernel is defined by sigma.
*/
AWPRESULT awpGaussianBlur(awpImage* pSrc, awpImage* pDst, AWPFLOAT sigma)
{
    AWPRESULT res = AWP_OK;
	awpImage* pKernel = NULL;
	if (sigma <= 0)
		_ERROR_EXIT_RES_(AWP_BADARG);

	_CHECK_RESULT_(res = awpCheckImage(pSrc))
    _CHECK_RESULT_(res = awpCheckImage(pDst))

    _CHECK_SAME_SIZES(pSrc, pDst)
    _CHECK_SAME_TYPE(pSrc,pDst)
    _CHECK_NUM_CHANNELS(pSrc, pDst)	

	// process AWPDOUBLE calculation
	if (pSrc->dwType == AWP_DOUBLE)
		res = awpGaussianBlurAWP_AWPAWPDOUBLE(pSrc,pDst,sigma);
	else
	{
		_CHECK_RESULT_(res = awpCreateGaussKernel(&pKernel, sigma, TRUE))
		_CHECK_RESULT_(res = awpMaskConvolution(pSrc, pDst, pKernel))
	}

CLEANUP:
	_SAFE_RELEASE_(pKernel);
    return res;
}
