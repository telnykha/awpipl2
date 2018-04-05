#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#include "float.h"
#include "_awpipl.h"

AWPRESULT awpAutoLevels(awpImage* pImage)
{
    AWPRESULT res = AWP_OK;
    AWPINT total = pImage->sSizeX*pImage->sSizeY;
  	awpHistogramm			Histogramm;
	//awpStat					stat;
    AWPDOUBLE sum = 0;
    AWPINT i;
    AWPINT white_level = 255;
    AWPINT black_level = 0;
    AWPBYTE* b;


    // autolevels
	_CHECK_RESULT_(res = awpGetHistogramm(pImage, &Histogramm))
    // fing white level
    for (i = 255; i > 0; i--)
    {
       sum += Histogramm.Intensity[i];
       if (sum > total / 1000)
       {
            white_level = i;
            break;
       }

    }

    // find black_level
    sum = 0;
    for (i = 0; i < 255; i++)
    {
       sum += Histogramm.Intensity[i];
       if (sum > total / 1000)
       {
            black_level = i;
            break;
       }
    }

    // transform pixels
    b = (AWPBYTE*)pImage->pPixels;
    for (i = 0; i < pImage->sSizeX*pImage->sSizeY; i++)
    {
        if (b[i] > white_level)
            {
                b[i] = 255;
                continue;
            }
        if (b[i] < black_level)
        {
            b[i] = 0;
            continue;
        }
        b[i] = (AWPBYTE)(255.0*((AWPDOUBLE)(b[i] - black_level)/(AWPDOUBLE)(white_level - black_level)));
    }

CLEANUP:
    return res;
}

AWPRESULT awpBilinearBlur( awpImage* pImage, AWPDOUBLE dfStrength )
{
    AWPRESULT res = AWP_OK;

    AWPDOUBLE	dfStrengthA = 1.0 / ( dfStrength + 1.0 );
    AWPDOUBLE  dfStrengthA1 = dfStrength * dfStrengthA;
    AWPINT	iX , iY;
    //AWPINT	iOffset00 , iOffset01 , iOffset10 , iOffset11;

    AWPDOUBLE* pdfPixels = (AWPDOUBLE*)pImage->pPixels;

    AWPDOUBLE* pdfPix00 = NULL;
    AWPDOUBLE* pdfPix01 = NULL;
    AWPDOUBLE* pdfPix10 = NULL;
    AWPDOUBLE* pdfPix11 = NULL;

    
    _CHECK_RESULT_( res = awpCheckImage(pImage) );
    _CHECK_RESULT_( res = (pImage->dwType == AWP_DOUBLE) ? AWP_OK : AWP_BADARG );
      
    pdfPix10 = pdfPixels;
    pdfPix11 = pdfPixels + 1;
    for ( iX = 1 ; iX<pImage->sSizeX ; ++iX, ++pdfPix11, ++pdfPix10 )//, --pdfPix00, --pdfPix01)
    {
        pdfPix11[0] =  ( dfStrengthA * pdfPix11[0] + dfStrengthA1 * pdfPix10[0] );
    }
    
    --pdfPix11; --pdfPix10;
    for ( iX = 1 ; iX<pImage->sSizeX ; ++iX, --pdfPix11, --pdfPix10 )//, --pdfPix00, --pdfPix01)
    {
        pdfPix10[0] = dfStrengthA * pdfPix10[0] + dfStrengthA1 * pdfPix11[0];
    }

    pdfPix01 = pdfPixels;
    pdfPix11 =  pdfPixels + pImage->sSizeX;
    for ( iY = 1 ; iY<pImage->sSizeY ; ++iY,  pdfPix01 += pImage->sSizeX, pdfPix11 += pImage->sSizeX )
    {
        pdfPix11[0] = ( dfStrengthA * pdfPix11[0] + dfStrengthA1 * pdfPix01[0] );
    }

    pdfPix01 -= pImage->sSizeX; pdfPix11 -= pImage->sSizeX;

    for ( iY = 1 ; iY<pImage->sSizeY ; ++iY,  pdfPix01 -= pImage->sSizeX, pdfPix11 -= pImage->sSizeX )
    {
        pdfPix01[0] = ( dfStrengthA * pdfPix01[0] + dfStrengthA1 * pdfPix11[0] );
    }



    pdfPix00	= pdfPixels;
    pdfPix01	= pdfPixels + 1;
    pdfPix10	= pdfPixels + pImage->sSizeX;
    pdfPix11	= pdfPixels + pImage->sSizeX + 1;
   
    for ( iY = 1 ; iY<pImage->sSizeY ; ++iY, ++pdfPix00, ++pdfPix01, ++pdfPix10, ++pdfPix11 )
        for ( iX = 1 ; iX<pImage->sSizeX ; ++iX, ++pdfPix00, ++pdfPix01, ++pdfPix10, ++pdfPix11 )
        {
            pdfPix11[0] = dfStrengthA * ( dfStrengthA * pdfPix11[0] + dfStrength * ( pdfPix01[0] + pdfPix10[0] - dfStrengthA1 * pdfPix00[0] ) );
        }


 

    pdfPix00	= pdfPixels + pImage->sSizeX * pImage->sSizeY - 1;
    pdfPix01	= pdfPix00 - 1;
    pdfPix10	= pdfPix00 - pImage->sSizeX;
    pdfPix11	= pdfPix10 - 1;
   
    for ( iY = pImage->sSizeY-1; iY > 0; --iY, --pdfPix00, --pdfPix01, --pdfPix10, --pdfPix11 )
        for ( iX = pImage->sSizeX-1 ; iX > 0; --iX, --pdfPix00, --pdfPix01, --pdfPix10, --pdfPix11 )
        {
            pdfPix11[0] = dfStrengthA * ( dfStrengthA * pdfPix11[0] + dfStrength * ( pdfPix01[0] + pdfPix10[0] - dfStrengthA1 * pdfPix00[0] ) );
        }


CLEANUP:
    return res;
}

typedef union 
{
    AWPDOUBLE value;
    struct 
    {
        AWPDWORD     :32;
        AWPDWORD     :31;
        AWPDWORD sign : 1;
    } data;
} __ieee_AWPAWPDOUBLE_shape_type;


AWPRESULT awpNormalizeAdaptive( awpImage* pImage,  awpImage const* pAverage, awpImage const* pDispercy )
{
    AWPRESULT                   res = AWP_OK;
      
    AWPINT					        iOffset = pImage->sSizeX* pImage->sSizeY;

    AWPDOUBLE                      delta, scale;
    __ieee_AWPAWPDOUBLE_shape_type*   pDelta = (__ieee_AWPAWPDOUBLE_shape_type*)&delta;
    __ieee_AWPAWPDOUBLE_shape_type*   pScale = (__ieee_AWPAWPDOUBLE_shape_type*)&scale;

    AWPDOUBLE*                     pdfImage = (AWPDOUBLE*)pImage->pPixels;
    AWPDOUBLE const*               pdfAverage = (AWPDOUBLE const*)pAverage->pPixels;
    AWPDOUBLE const*               pdfDispercy = (AWPDOUBLE const*)pDispercy->pPixels;

    _CHECK_RESULT_( res = awpCheckImage(pImage) );
    _CHECK_RESULT_( res = awpCheckImage(pAverage) );
    _CHECK_RESULT_( res = awpCheckImage(pDispercy) );

    //TODO: check sizes;

    for (; iOffset > 0; --iOffset, ++pdfImage, ++pdfAverage, ++pdfDispercy )
    {
      /*  dfValue	= pdfImage[0] - pdfAverage[0];
        dfValue	*= dfValue;

        if ( dfValue > pdfDispercy[0] || 0.0 == pdfDispercy[0] )
        {
            if ( pdfImage[0] < pdfAverage[0] )
            {
                pdfImage[0]	= 0.0;
            }
            else
            {
                pdfImage[0]	= range;
            }
        }
        else
        {
            if ( pdfImage[0] < pdfAverage[0] )
            {
                pdfImage[0] = mean - dispLo * dfValue / pdfDispercy[0];
            }
            else
            {
                pdfImage[0] = mean + dispHi * dfValue / pdfDispercy[0];
            }
        }*/

      
        delta	= pdfImage[0] - pdfAverage[0];
        //Calc scale of disp;        
        scale = delta * delta/( pdfDispercy[0] + DBL_MIN );
        //Clamp scale to [0, 1];
        scale = AWP_MIN( scale , 1.0 );
        //Copy sign;
        pScale->data.sign = pDelta->data.sign;
        //scale = _copysign( scale, delta );
        pdfImage[0] = 0.5 + 0.5*scale;
    }
CLEANUP:
    return res;
}

AWPRESULT awpDispercy( awpImage const* pImage, awpImage const* pAverage, awpImage* pDispercy )
{
    AWPRESULT res = AWP_OK;
       
    AWPINT					iOffset = pImage->sSizeX*pImage->sSizeY;

    AWPDOUBLE const* pdfImage = (AWPDOUBLE const*)pImage->pPixels;
    AWPDOUBLE const* pdfAverage = (AWPDOUBLE const*)pAverage->pPixels;
    AWPDOUBLE* pdfDispercy = (AWPDOUBLE*)pDispercy->pPixels;

    _CHECK_RESULT_( res = awpCheckImage(pImage) );
    _CHECK_RESULT_( res = awpCheckImage(pAverage) );
    _CHECK_RESULT_( res = awpCheckImage(pDispercy) );

    for ( ; iOffset > 0 ; --iOffset, ++pdfImage, ++pdfAverage, ++pdfDispercy )
    {
        pdfDispercy[0]	= pdfImage[0] - pdfAverage[0];
        pdfDispercy[0] *= pdfDispercy[0];
    }


CLEANUP:
    return res;
}
 
