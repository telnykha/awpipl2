/*
//
//  RCS:
//      $Source: /home/cvs/awp/imageproc/awpipl-v2/src/awpBaseAnalysis.c,v $
//      $Revision: 1.6 $
//      $Date: 1999
//
//    Purpose: AWPIPL source file
//    Contents: source code for histogram analysis
//    Authors : Alexander Telnykh
*/
#include "_awpipl.h"
#pragma warning(disable: 4996)

// функция вычисления начальных моментов распределения
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
// копирование гистограмы в четыре массива (по числу каналов)
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
//накопление гистограммы изображения


AWPRESULT awpGetHistogramm(const awpImage* Image, awpHistogramm* Histogramm)
{
	AWPINT i		= 0;
	AWPINT count	= 0;
	AWPBYTE* p		= NULL;
	awpColor* c = NULL;
	/*check image*/
	AWPRESULT res = AWP_OK;
	res = awpCheckImage(Image);
	if (res != AWP_OK)
        return res;

	memset((void*)Histogramm, 0, sizeof(awpHistogramm));
	count = Image->sSizeX*Image->sSizeY;
	if (Image->dwType == AWP_BYTE && Image->bChannels == 1)
	{
		p = (AWPBYTE*)Image->pPixels;
		for (i = 0; i < count; i++)
			Histogramm->Intensity[p[i]]++;
	}
	else if (Image->dwType == AWP_BYTE && Image->bChannels == 3)
	{
		c = (awpColor*)Image->pPixels;
		for (i = 0; i < count; i++)
		{
			Histogramm->ColorData.RedChannel[c[i].bRed]++;
			Histogramm->ColorData.GreenChannel[c[i].bGreen]++;
			Histogramm->ColorData.BlueChannel[c[i].bBlue]++;

			Histogramm->Intensity[(c[i].bRed + c[i].bGreen + c[i].bBlue) / 3]++;
		}

	}
	else
		return AWP_NOTSUPPORT;

	return AWP_OK;
}

AWPRESULT awpGet2DHistogramm(awpImage* pImage, awpImage* p2DHist, AWPBYTE low, AWPBYTE up, AWPINT needToConvert)
{
    AWPINT i, j;
    AWPDOUBLE*     dblPix;
    awpColor*   clPix;
    AWPBYTE        h,s, v;
    AWPRESULT res = AWP_OK;
    awpImage* pDblHst = NULL;
    // проверка аргументов
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

    //для построения двумерной гистограммы используем 1й и 2й каналы изображения
    //pImage и записываем гистограмму во временное изображение типа AWPDOUBLE
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
    // конвертируем временное изображение типа AWPDOUBLE в AWPBYTE
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

AWPRESULT awpGetMax(const awpHistogramm* Histogramm, awpStat* Result)
{
	AWPINT i = 0;
	if (Histogramm == NULL)
        return AWP_BADMEMORY;
	if (Result == NULL)
        return AWP_BADMEMORY;
	// уставновим значение в 0
	memset(Result, 0, sizeof(awpStat));

	// инициализация 
	Result->dRed	= Histogramm->ColorData.RedChannel[0];
	Result->dGreen	= Histogramm->ColorData.GreenChannel[0];
	Result->dBlue	= Histogramm->ColorData.BlueChannel[0];
	Result->dBright	= Histogramm->Intensity[0];
	//вычисление максимума
	for (i = 1; i < 256; i++)
	{
		if (Result->dRed < Histogramm->ColorData.RedChannel[i])
			Result->dRed = Histogramm->ColorData.RedChannel[i];

		if (Result->dGreen < Histogramm->ColorData.GreenChannel[i])
			Result->dGreen = Histogramm->ColorData.GreenChannel[i];

		if (Result->dBlue < Histogramm->ColorData.BlueChannel[i])
			Result->dBlue = Histogramm->ColorData.BlueChannel[i];

		if (Result->dBright < Histogramm->Intensity[i])
			Result->dBright = Histogramm->Intensity[i];
	}
	return AWP_OK;
}

AWPRESULT awpGetMin(const awpHistogramm* Histogramm, awpStat* Result)
{
	AWPINT i = 0;
	if (Histogramm == NULL)
        return AWP_BADMEMORY;
	if (Result == NULL)
        return AWP_BADMEMORY;

	// уставновим значение в 0
	memset(Result, 0, sizeof(awpStat));

	// инициализация
	Result->dRed	= Histogramm->ColorData.RedChannel[0];
	Result->dGreen	= Histogramm->ColorData.GreenChannel[0];
	Result->dBlue	= Histogramm->ColorData.BlueChannel[0];
	Result->dBright	= Histogramm->Intensity[0];
	//вычисление максимума
	for (i = 1; i < 256; i++)
	{
		if (Result->dRed > Histogramm->ColorData.RedChannel[i])
			Result->dRed = Histogramm->ColorData.RedChannel[i];

		if (Result->dGreen > Histogramm->ColorData.GreenChannel[i])
			Result->dGreen = Histogramm->ColorData.GreenChannel[i];

		if (Result->dBlue > Histogramm->ColorData.BlueChannel[i])
			Result->dBlue = Histogramm->ColorData.BlueChannel[i];

		if (Result->dBright > Histogramm->Intensity[i])
			Result->dBright = Histogramm->Intensity[i];
	}
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


// сравнение двух чисел с удвоенной точностью
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
	awpStat en; //энтропия
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
// нахождение скошенности
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
	//обнулим переменные
	memset(Result, 0, sizeof(awpStat));
	memset(&m1, 0, sizeof(awpStat));
	memset(&m2, 0, sizeof(awpStat));
	memset(&m3, 0, sizeof(awpStat));
	memset(&dsp, 0, sizeof(awpStat));
	// вычислим начальные моменты и дисперсию	
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
// нахождение эксцесса
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
	//обнулим переменные
	memset(Result, 0, sizeof(awpStat));
	memset(&m1, 0, sizeof(awpStat));
	memset(&m2, 0, sizeof(awpStat));
	memset(&m3, 0, sizeof(awpStat));
	memset(&m4, 0, sizeof(awpStat));
	memset(&dsp, 0, sizeof(awpStat));
	// вычислим начальные моменты и дисперсию	
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

AWPRESULT awpGetPixCount(const awpHistogramm* Histogramm, AWPINT* count)
{
	AWPINT i;
	if (Histogramm == NULL)
        return AWP_BADMEMORY;

	*count = 0;
	for (i= 0; i < 256; i++)
		*count += (AWPINT)Histogramm->Intensity[i];

	return AWP_OK;
}


// Added by Radzhabova Julia
// Normalization of histogramm
AWPRESULT awpHistNormalization(awpHistogramm* Histogramm)
{
	AWPINT i = 0;
	awpStat minH,maxH;

	if (Histogramm == NULL)
        return AWP_BADMEMORY;

	awpGetMax(Histogramm, &maxH);
	awpGetMin(Histogramm, &minH);


	for (i=0; i< 256; i++)
	{
		if ((maxH.dBright - minH.dBright) > 0)
		{
			Histogramm->Intensity[i] /= (maxH.dBright - minH.dBright);
		}
		if ((maxH.dRed - minH.dRed) > 0)
		{
			Histogramm->ColorData.RedChannel[i] /= (maxH.dRed - minH.dRed);
		}
		if ((maxH.dGreen - minH.dGreen) > 0)
		{
			Histogramm->ColorData.GreenChannel[i] /= (maxH.dGreen - minH.dGreen);
		}
		if ((maxH.dBlue - minH.dBlue) > 0)
		{
			Histogramm->ColorData.BlueChannel[i] /= (maxH.dBlue - minH.dBlue);
		}
	}

	return AWP_OK;
}

/*Histogramm equalize function*/
/*Функция эквилизации гистограммы. Приводит гистограмму изображения
pImage, к виду максимально приближенному к равномерному.*/
AWPRESULT awpHistogrammEqualize(awpImage* pImage)
{

        /*локальные пременные*/
        AWPRESULT               res;         /*результат выполнения операции*/
        awpHistogramm           Hst;         /*гистограмма изображения*/
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
  AWPINT Hd0[256],M[256];//исходная гистограмма и функция пересчета старых интенсивностей в новые
  AWPINT Lx=pSource->sSizeX, Ly=pSource->sSizeY,n,m=0,s=0;
  long i; //"длинный" счетчик на случай большой картинки
//  AWPDOUBLE K0=1.0, K1=1.0, Ks=Lx*Ly/255.0, sa=0.1;
  AWPDOUBLE K0=k1, K1=k2, Ks=Lx*Ly/255.0, sa=0.1;
  AWPDOUBLE GoalIntegralHistogramm[256]; // интегральная гистограмма
  if(pSource->bChannels!=1) awpConvert(pSource, AWP_CONVERT_3TO1_BYTE);
  awpCopyImage(pSource, &Im2);

  ImIn = (AWPBYTE*)pSource->pPixels;
  ImOut = (AWPBYTE*)Im2->pPixels;
  for(n=0; n<256; n++) {Hd0[n]=0; M[n]=0;}// обнуление массивов
  for(i=0;i<Lx*Ly;i++)   Hd0[ImIn[i]]++; // вычисление исходной гистограммы по входной картинке
  for(m=0;m<256;m++) GoalIntegralHistogramm[m]=Ks*(m+K0/255.0*(m*m-255*m)-K1/255.0/255.0*(2*m*m*m-3*255*m*m+255*255*m));
  n=0;m=0;s=0;
  a:if(s<Lx*Ly&&n<255&&m<255){
  if(s<sa) {do {s+=Hd0[n]; M[n]=m; n++; } while(s<sa);}
  if(s>=sa) {do {m++;sa=GoalIntegralHistogramm[m]+0.1;} while(s>=sa);}
  goto a; }
  if(n<255) do{M[n]=255;n++;} while(n<255); // дорисовка до конца при необходимости
  if(m<255) do{M[n]=m; m++;} while(m<255);   // дорисовка
  for(i=0;i<Lx*Ly;i++) ImOut[i]=M[ImIn[i]];
  for(i=0;i<Lx*Ly;i++) ImIn[i]=ImOut[i];
  awpReleaseImage(&Im2);
  return AWP_OK;
}


AWPRESULT awpSaveHistogramm(const char* lpszFileName, awpHistogramm* hist, AWPBOOL fullHist)
{
        AWPRESULT res = AWP_OK;
        AWPINT i;
        FILE* f = fopen(lpszFileName, "w+b");
        AWPINT count = 0;
        awpGetPixCount(hist, &count);
        for(i = 0; i < 256; i++)
           hist->Intensity[i] /= count;
        fwrite(hist->Intensity, 256*sizeof(AWPDOUBLE),1,f);
        fclose(f);
        return res;
}

AWPRESULT awpLoadHistogramm(const char* lpszFileName, awpHistogramm* hist,AWPBOOL fullHist)
{
        AWPRESULT res = AWP_OK;
        FILE* f = fopen(lpszFileName, "w+b");
        fread(hist->Intensity, 256*sizeof(AWPDOUBLE),1,f);
        fclose(f);
        return res;
}


/*
	Вычисляет сумму значений пикселей изображения pSrc 
	и сохраняет ее в pDst. 
	pDst должно быть изображением типа AWP_DOUBLE
	иметь один канал данных, высоту 1 пикс и ширину равную числу каналов pScr
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