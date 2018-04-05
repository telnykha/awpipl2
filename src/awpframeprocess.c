
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "_awpipl.h"

static AWPRESULT _awpMltImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const AWPLONG lWhere)
{
    void *pData1;
    void *pData2;
    void *pData3;

    awpImage *pImageTemp = NULL;

    AWPDOUBLE dSrc1;
    AWPDOUBLE dSrc2;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;

    if( lWhere != AWP_CALC_OUTPLACE && lWhere != AWP_CALC_INPLACE )
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_DOUBLE)))

    _CHECK_RESULT_((res = awpGetImagePixelType( pImageSrc1, &iImageType)))
 
    switch(iImageType)
            {
                case AWP_BYTE:   pData1 = (AWPBYTE*)pImageSrc1->pPixels;
                                 pData2 = (AWPBYTE*)pImageSrc2->pPixels;    break; 
                case AWP_SHORT:  pData1 = (AWPSHORT*)pImageSrc1->pPixels;
                                 pData2 = (AWPSHORT*)pImageSrc2->pPixels;   break;  
                case AWP_FLOAT:  pData1 = (AWPFLOAT*)pImageSrc1->pPixels; 
                                 pData2 = (AWPFLOAT*)pImageSrc2->pPixels;   break; 
                case AWP_DOUBLE: pData1 = (AWPDOUBLE*)pImageSrc1->pPixels; 
                                 pData2 = (AWPDOUBLE*)pImageSrc2->pPixels; break;
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }

    pData3 = (AWPDOUBLE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels ; ++dwIndex)
    {
            switch(iImageType)
            {
                case AWP_BYTE:   dSrc1 = ((AWPBYTE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPBYTE*)pData2)[dwIndex]; break;
                case AWP_SHORT:  dSrc1 = ((AWPSHORT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPSHORT*)pData2)[dwIndex]; break;
                case AWP_FLOAT:  dSrc1 = ((AWPFLOAT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPFLOAT*)pData2)[dwIndex]; break;
                case AWP_DOUBLE: dSrc1 = ((AWPDOUBLE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPDOUBLE*)pData2)[dwIndex]; break;
                
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }

            ((AWPDOUBLE*)pData3)[dwIndex] = dSrc1*dSrc2;
    }

    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))  
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

static AWPRESULT _awpDivImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const long lWhere)
{
    void *pData1;
    void *pData2;
    void *pData3;

    awpImage *pImageTemp = NULL;

    AWPDOUBLE dSrc1;
    AWPDOUBLE dSrc2;
    AWPDOUBLE dEps;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;

    if( lWhere != AWP_CALC_OUTPLACE && lWhere != AWP_CALC_INPLACE )
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_DOUBLE)))

    _CHECK_RESULT_((res = awpGetImagePixelType( pImageSrc1, &iImageType)))
 
    switch(iImageType)
            {
                case AWP_BYTE:   pData1 = (AWPBYTE*)pImageSrc1->pPixels;
                                 pData2 = (AWPBYTE*)pImageSrc2->pPixels;    break; 
                case AWP_SHORT:  pData1 = (AWPSHORT*)pImageSrc1->pPixels;
                                 pData2 = (AWPSHORT*)pImageSrc2->pPixels;   break;  
                case AWP_FLOAT:  pData1 = (AWPFLOAT*)pImageSrc1->pPixels; 
                                 pData2 = (AWPFLOAT*)pImageSrc2->pPixels;   break; 
                case AWP_DOUBLE: pData1 = (AWPDOUBLE*)pImageSrc1->pPixels; 
                                 pData2 = (AWPDOUBLE*)pImageSrc2->pPixels; break;
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }
    
    dEps = 1.0e-5;
    pData3 = (AWPDOUBLE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels ; ++dwIndex)
    {
            switch(iImageType)
            {
                case AWP_BYTE:   dSrc1 = ((AWPBYTE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPBYTE*)pData2)[dwIndex]; break;
                case AWP_SHORT:  dSrc1 = ((AWPSHORT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPSHORT*)pData2)[dwIndex]; break;
                case AWP_FLOAT:  dSrc1 = ((AWPFLOAT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPFLOAT*)pData2)[dwIndex]; break;
                case AWP_DOUBLE: dSrc1 = ((AWPDOUBLE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPDOUBLE*)pData2)[dwIndex]; break;
                
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }

            ((AWPDOUBLE*)pData3)[dwIndex] = (dSrc2 < dEps)? 0:(dSrc1/dSrc2);
    }

    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))  
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

static AWPRESULT _awpAddImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const long lWhere)
{
    void *pData1;
    void *pData2;
    void *pData3;

    awpImage *pImageTemp = NULL;

    AWPDOUBLE dSrc1;
    AWPDOUBLE dSrc2;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;

    if( lWhere != AWP_CALC_OUTPLACE && lWhere != AWP_CALC_INPLACE )
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_DOUBLE)))

    _CHECK_RESULT_((res = awpGetImagePixelType( pImageSrc1, &iImageType)))
 
    switch(iImageType)
            {
                case AWP_BYTE:   pData1 = (AWPBYTE*)pImageSrc1->pPixels;
                                 pData2 = (AWPBYTE*)pImageSrc2->pPixels;    break; 
                case AWP_SHORT:  pData1 = (AWPSHORT*)pImageSrc1->pPixels;
                                 pData2 = (AWPSHORT*)pImageSrc2->pPixels;   break;  
                case AWP_FLOAT:  pData1 = (AWPFLOAT*)pImageSrc1->pPixels; 
                                 pData2 = (AWPFLOAT*)pImageSrc2->pPixels;   break; 
                case AWP_DOUBLE: pData1 = (AWPDOUBLE*)pImageSrc1->pPixels; 
                                 pData2 = (AWPDOUBLE*)pImageSrc2->pPixels; break;
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }
    
    pData3 = (AWPDOUBLE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels ; ++dwIndex)
    {
            switch(iImageType)
            {
                case AWP_BYTE:   dSrc1 = ((AWPBYTE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPBYTE*)pData2)[dwIndex]; break;
                case AWP_SHORT:  dSrc1 = ((AWPSHORT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPSHORT*)pData2)[dwIndex]; break;
                case AWP_FLOAT:  dSrc1 = ((AWPFLOAT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPFLOAT*)pData2)[dwIndex]; break;
                case AWP_DOUBLE: dSrc1 = ((AWPDOUBLE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPDOUBLE*)pData2)[dwIndex]; break;
                
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }

            ((AWPDOUBLE*)pData3)[dwIndex] = dSrc1+dSrc2;
    }

    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

static AWPRESULT _awpSubImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const long lWhere)
{
    void *pData1;
    void *pData2;
    void *pData3;

    awpImage *pImageTemp = NULL;

    AWPDOUBLE dSrc1;
    AWPDOUBLE dSrc2;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;

    if( lWhere != AWP_CALC_OUTPLACE && lWhere != AWP_CALC_INPLACE )
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_DOUBLE)))

    _CHECK_RESULT_((res = awpGetImagePixelType( pImageSrc1, &iImageType)))
 
    switch(iImageType)
            {
                case AWP_BYTE:   pData1 = (AWPBYTE*)pImageSrc1->pPixels;
                                 pData2 = (AWPBYTE*)pImageSrc2->pPixels;    break; 
                case AWP_SHORT:  pData1 = (AWPSHORT*)pImageSrc1->pPixels;
                                 pData2 = (AWPSHORT*)pImageSrc2->pPixels;   break;  
                case AWP_FLOAT:  pData1 = (AWPFLOAT*)pImageSrc1->pPixels; 
                                 pData2 = (AWPFLOAT*)pImageSrc2->pPixels;   break; 
                case AWP_DOUBLE: pData1 = (AWPDOUBLE*)pImageSrc1->pPixels; 
                                 pData2 = (AWPDOUBLE*)pImageSrc2->pPixels; break;
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }
    
    pData3 = (AWPDOUBLE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels; ++dwIndex)
    {
            switch(iImageType)
            {
                case AWP_BYTE:   dSrc1 = ((AWPBYTE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPBYTE*)pData2)[dwIndex]; break;
                case AWP_SHORT:  dSrc1 = ((AWPSHORT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPSHORT*)pData2)[dwIndex]; break;
                case AWP_FLOAT:  dSrc1 = ((AWPFLOAT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPFLOAT*)pData2)[dwIndex]; break;
                case AWP_DOUBLE: dSrc1 = ((AWPDOUBLE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPDOUBLE*)pData2)[dwIndex]; break;
                
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }

            ((AWPDOUBLE*)pData3)[dwIndex] = ((dSrc1-dSrc2)<0)? 0:(dSrc1-dSrc2);
    }

    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))  
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

static AWPRESULT _awpAndImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const long lWhere)
{
    AWPBYTE *pData1;
    AWPBYTE *pData2;
    AWPBYTE *pData3;

    awpImage *pImageTemp = NULL;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;
            
    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc1, &iImageType)))

    if(iImageType != AWP_BYTE && 
       lWhere != AWP_CALC_OUTPLACE &&
       lWhere != AWP_CALC_INPLACE)
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_BYTE)))
 
    pData1 = (AWPBYTE*)pImageSrc1->pPixels;
    pData2 = (AWPBYTE*)pImageSrc2->pPixels;
    pData3 = (AWPBYTE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels; ++dwIndex)
        pData3[dwIndex] = pData1[dwIndex]&pData2[dwIndex];
    
    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))  
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

static AWPRESULT _awpOrImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const long lWhere)
{
    AWPBYTE *pData1;
    AWPBYTE *pData2;
    AWPBYTE *pData3;

    awpImage *pImageTemp = NULL;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;
            
    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc1, &iImageType)))

    if(iImageType != AWP_BYTE && 
       lWhere != AWP_CALC_OUTPLACE &&
       lWhere != AWP_CALC_INPLACE)
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_BYTE)))
 
    pData1 = (AWPBYTE*)pImageSrc1->pPixels;
    pData2 = (AWPBYTE*)pImageSrc2->pPixels;
    pData3 = (AWPBYTE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels; ++dwIndex)
        pData3[dwIndex] = pData1[dwIndex]|pData2[dwIndex];
    
    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))  
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

static AWPRESULT _awpXorImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const long lWhere)
{
    AWPBYTE *pData1;
    AWPBYTE *pData2;
    AWPBYTE *pData3;

    awpImage *pImageTemp = NULL;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;
            
    _CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc1, &iImageType)))

    if(iImageType != AWP_BYTE && 
       lWhere != AWP_CALC_OUTPLACE &&
       lWhere != AWP_CALC_INPLACE)
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_BYTE)))
 
    pData1 = (AWPBYTE*)pImageSrc1->pPixels;
    pData2 = (AWPBYTE*)pImageSrc2->pPixels;
    pData3 = (AWPBYTE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels; ++dwIndex)
        pData3[dwIndex] = pData1[dwIndex]^pData2[dwIndex];
    
    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))  
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

static AWPRESULT _awpMinImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const long lWhere)
{
    void *pData1;
    void *pData2;
    void *pData3;

    awpImage *pImageTemp = NULL;

    AWPDOUBLE dSrc1;
    AWPDOUBLE dSrc2;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;

    if( lWhere != AWP_CALC_OUTPLACE && lWhere != AWP_CALC_INPLACE )
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_DOUBLE)))

    _CHECK_RESULT_((res = awpGetImagePixelType( pImageSrc1, &iImageType)))
 
    switch(iImageType)
            {
                case AWP_BYTE:   pData1 = (AWPBYTE*)pImageSrc1->pPixels;
                                 pData2 = (AWPBYTE*)pImageSrc2->pPixels;    break; 
                case AWP_SHORT:  pData1 = (AWPSHORT*)pImageSrc1->pPixels;
                                 pData2 = (AWPSHORT*)pImageSrc2->pPixels;   break;  
                case AWP_FLOAT:  pData1 = (AWPFLOAT*)pImageSrc1->pPixels; 
                                 pData2 = (AWPFLOAT*)pImageSrc2->pPixels;   break; 
                case AWP_DOUBLE: pData1 = (AWPDOUBLE*)pImageSrc1->pPixels; 
                                 pData2 = (AWPDOUBLE*)pImageSrc2->pPixels; break;
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }
    
    pData3 = (AWPDOUBLE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels; ++dwIndex)
    {
            switch(iImageType)
            {
                case AWP_BYTE:   dSrc1 = ((AWPBYTE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPBYTE*)pData2)[dwIndex]; break;
                case AWP_SHORT:  dSrc1 = ((AWPSHORT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPSHORT*)pData2)[dwIndex]; break;
                case AWP_FLOAT:  dSrc1 = ((AWPFLOAT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPFLOAT*)pData2)[dwIndex]; break;
                case AWP_DOUBLE: dSrc1 = ((AWPDOUBLE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPDOUBLE*)pData2)[dwIndex]; break;
                
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }

            ((AWPDOUBLE*)pData3)[dwIndex] = AWP_MIN(dSrc1, dSrc2);
    }

    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))  
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

static AWPRESULT _awpMaxImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const long lWhere)
{
    void *pData1;
    void *pData2;
    void *pData3;

    awpImage *pImageTemp = NULL;

    AWPDOUBLE dSrc1;
    AWPDOUBLE dSrc2;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;

    if( lWhere != AWP_CALC_OUTPLACE && lWhere != AWP_CALC_INPLACE )
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_DOUBLE)))

    _CHECK_RESULT_((res = awpGetImagePixelType( pImageSrc1, &iImageType)))
 
    switch(iImageType)
            {
                case AWP_BYTE:   pData1 = (AWPBYTE*)pImageSrc1->pPixels;
                                 pData2 = (AWPBYTE*)pImageSrc2->pPixels;    break; 
                case AWP_SHORT:  pData1 = (AWPSHORT*)pImageSrc1->pPixels;
                                 pData2 = (AWPSHORT*)pImageSrc2->pPixels;   break;  
                case AWP_FLOAT:  pData1 = (AWPFLOAT*)pImageSrc1->pPixels; 
                                 pData2 = (AWPFLOAT*)pImageSrc2->pPixels;   break; 
                case AWP_DOUBLE: pData1 = (AWPDOUBLE*)pImageSrc1->pPixels; 
                                 pData2 = (AWPDOUBLE*)pImageSrc2->pPixels; break;
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }
    
    pData3 = (AWPDOUBLE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels; ++dwIndex)
    {
            switch(iImageType)
            {
                case AWP_BYTE:   dSrc1 = ((AWPBYTE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPBYTE*)pData2)[dwIndex]; break;
                case AWP_SHORT:  dSrc1 = ((AWPSHORT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPSHORT*)pData2)[dwIndex]; break;
                case AWP_FLOAT:  dSrc1 = ((AWPFLOAT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPFLOAT*)pData2)[dwIndex]; break;
                case AWP_DOUBLE: dSrc1 = ((AWPDOUBLE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPDOUBLE*)pData2)[dwIndex]; break;
                
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }

            ((AWPDOUBLE*)pData3)[dwIndex] = AWP_MAX(dSrc1, dSrc2);
    }

    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))  
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

static AWPRESULT _awpAvgImages(const awpImage *pImageSrc1, awpImage *pImageSrc2, awpImage **ppImageDst, const long lWhere)
{
    void *pData1;
    void *pData2;
    void *pData3;

    awpImage *pImageTemp = NULL;

    AWPDOUBLE dSrc1;
    AWPDOUBLE dSrc2;

    AWPDWORD dwIndex;
    AWPINT  iImageType;

    AWPRESULT res;
    res = AWP_OK;

    if( lWhere != AWP_CALC_OUTPLACE && lWhere != AWP_CALC_INPLACE )
    {
         _SAFE_RELEASE_(pImageTemp)
         res = AWP_BADARG;
         _ERR_EXIT_
    }

    _CHECK_RESULT_((res = awpCreateImage(&pImageTemp, pImageSrc1->sSizeX, pImageSrc1->sSizeY,\
                    pImageSrc1->bChannels, AWP_DOUBLE)))

    _CHECK_RESULT_((res = awpGetImagePixelType( pImageSrc1, &iImageType)))
 
    switch(iImageType)
            {
                case AWP_BYTE:   pData1 = (AWPBYTE*)pImageSrc1->pPixels;
                                 pData2 = (AWPBYTE*)pImageSrc2->pPixels;    break; 
                case AWP_SHORT:  pData1 = (AWPSHORT*)pImageSrc1->pPixels;
                                 pData2 = (AWPSHORT*)pImageSrc2->pPixels;   break;  
                case AWP_FLOAT:  pData1 = (AWPFLOAT*)pImageSrc1->pPixels; 
                                 pData2 = (AWPFLOAT*)pImageSrc2->pPixels;   break; 
                case AWP_DOUBLE: pData1 = (AWPDOUBLE*)pImageSrc1->pPixels; 
                                 pData2 = (AWPDOUBLE*)pImageSrc2->pPixels; break;
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }
    
    pData3 = (AWPDOUBLE*)pImageTemp->pPixels;
    
    for(dwIndex = 0; dwIndex < (AWPDWORD)pImageSrc1->sSizeX*pImageSrc1->sSizeY*pImageSrc1->bChannels ; ++dwIndex)
    {
            switch(iImageType)
            {
                case AWP_BYTE:   dSrc1 = ((AWPBYTE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPBYTE*)pData2)[dwIndex]; break;
                case AWP_SHORT:  dSrc1 = ((AWPSHORT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPSHORT*)pData2)[dwIndex]; break;
                case AWP_FLOAT:  dSrc1 = ((AWPFLOAT*)pData1)[dwIndex];
                                 dSrc2 = ((AWPFLOAT*)pData2)[dwIndex]; break;
                case AWP_DOUBLE: dSrc1 = ((AWPDOUBLE*)pData1)[dwIndex];
                                 dSrc2 = ((AWPDOUBLE*)pData2)[dwIndex]; break;
                
                default:
                       {
                           _SAFE_RELEASE_(pImageTemp)
                           res = AWP_BADARG;
                           _ERR_EXIT_
                       }
            }

            ((AWPDOUBLE*)pData3)[dwIndex] = (dSrc1+dSrc2)/2;
    }

    if(lWhere == AWP_CALC_OUTPLACE)
    {
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, ppImageDst)))  
    }	
    else if(lWhere == AWP_CALC_INPLACE)
    {
        _SAFE_RELEASE_(pImageSrc2)
        _CHECK_RESULT_((res = awpCopyImage(pImageTemp, &pImageSrc2)))            
    }

CLEANUP:
    _SAFE_RELEASE_(pImageTemp)
    return res;
}

AWPRESULT awpCalcImage(const awpImage *pImageSrc1,  awpImage *pImageSrc2, awpImage** ppImageDst, const long lOptions, const long lWhere)
{
    AWPINT iImageType1;
	AWPINT iImageType2;

	AWPRESULT res;
	res = AWP_OK;

	_CHECK_RESULT_((res = awpCheckImage(pImageSrc1)))
        _CHECK_RESULT_((res = awpCheckImage(pImageSrc2)))
	_CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc1, &iImageType1))) 
	_CHECK_RESULT_((res = awpGetImagePixelType(pImageSrc2, &iImageType2))) 

        if(pImageSrc1->sSizeX != pImageSrc2->sSizeX ||
           pImageSrc1->sSizeY != pImageSrc2->sSizeY ||
           pImageSrc1->bChannels != pImageSrc2->bChannels ||
		   iImageType1 != iImageType2)
		{
            res =  AWP_BADARG;
            _ERR_EXIT_
        }
        
        switch(lOptions)
        {
            case AWP_CALC_ANDIMAGES: _CHECK_RESULT_((res = _awpAndImages(pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
            case AWP_CALC_ORIMAGES:  _CHECK_RESULT_((res = _awpOrImages (pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
            case AWP_CALC_XORIMAGES: _CHECK_RESULT_((res = _awpXorImages(pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
            case AWP_CALC_ADDIMAGES: _CHECK_RESULT_((res = _awpAddImages(pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
            case AWP_CALC_SUBIMAGES: _CHECK_RESULT_((res = _awpSubImages(pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
            case AWP_CALC_MLTIMAGES: _CHECK_RESULT_((res = _awpMltImages(pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
            case AWP_CALC_DIVIMAGES: _CHECK_RESULT_((res = _awpDivImages(pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
            case AWP_CALC_MINIMAGES: _CHECK_RESULT_((res = _awpMinImages(pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
            case AWP_CALC_MAXIMAGES: _CHECK_RESULT_((res = _awpMaxImages(pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
            case AWP_CALC_AVGIMAGES: _CHECK_RESULT_((res = _awpAvgImages(pImageSrc1, pImageSrc2, ppImageDst, lWhere))); break;
        
            default:
                   {
		       res = AWP_BADARG;
                       _ERR_EXIT_
                   }
        }

CLEANUP:
    return res;
}

