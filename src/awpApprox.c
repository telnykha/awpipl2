/*M
//
//      awpipl2 image processing and image analysis library
//		File: awpApprox.c
//		Purpose: approximation implementation 
//		Autors: Alex A.Telnykh (tlnykha@yahoo.com)
//      CopyRight 2004-2018 (c) ALT-Soft.net
//M*/
#include "_awpipl.h"

static AWPDOUBLE Power(AWPDOUBLE v, int p)
{
	if (p == 0)
		return 1;
	if (p>1)
		v *= Power(v, p - 1);
	return v;
}

void _awpPolynomCalculate(AWPINT K, AWPINT N, AWPDOUBLE*x, AWPDOUBLE*y, AWPDOUBLE*a)
{
	AWPINT i, j, k;
	AWPDOUBLE s, M;
	AWPDOUBLE *b = NULL;
	AWPDOUBLE** sums = NULL;
	memset(a, 0, N*sizeof(AWPDOUBLE));
	b = (double*)malloc(N * sizeof(AWPDOUBLE));
	sums = (AWPDOUBLE**)malloc(N* sizeof(AWPDOUBLE*));//�������� ���������� �������
	for (i = 0; i < N; i++)
	{
		sums[i] = (AWPDOUBLE*)malloc(N* sizeof(AWPDOUBLE));
	}
	//��������� ������������ ������� ���������
	for (i = 0; i<K + 1; i++)
	{
		for (j = 0; j<K + 1; j++)
		{
			sums[i][j] = 0;
			for (k = 0; k<N; k++)
				sums[i][j] += Power(x[k], i + j);
		}
	}
	//��������� ������� ��������� ������
	for (i = 0; i<K + 1; i++)
	{
		b[i] = 0;
		for (k = 0; k<N; k++)
			b[i] += Power(x[k], i)*y[k];
	}
	//��������� ����� ������ ��� ���������� ������� ������� � ������������ ����
	for (k = 0; k<K + 1; k++)
	{
		for (i = k + 1; i<K + 1; i++)
		{
			M = sums[i][k] / sums[k][k];
			for (j = k; j<K + 1; j++)
				sums[i][j] -= M * sums[k][j];
			b[i] -= M*b[k];
		}
	}
	//��������� ������������ ����������������� ��������
	for (i = K; i >= 0; i--)
	{
		s = 0;
		for (j = i; j<K + 1; j++)
			s += sums[i][j] * a[j];
		a[i] = (b[i] - s) / sums[i][i];
	}
	free(b);
	for (i = 0; i < N; i++)
	{
		free(sums[i]);
	}
	free(sums);
}


AWPRESULT awpApproxPoly(awpImage* src, awpImage* dst)
{
	AWPRESULT  res = AWP_OK;
	AWPDOUBLE *s, *d;

	if (src == NULL || dst == NULL)
		_ERROR_EXIT_RES_(AWP_BADARG)
	if (src->dwType != AWP_DOUBLE || dst->dwType != AWP_DOUBLE)
		_ERROR_EXIT_RES_(AWP_BADARG)
	if (src->sSizeY != 2 || dst->sSizeY != 1)
		_ERROR_EXIT_RES_(AWP_BADARG)
	if (dst->sSizeX > 6)
		_ERROR_EXIT_RES_(AWP_BADARG)

	s = _AWP_BPIX_(src, AWPDOUBLE)
	d = _AWP_BPIX_(dst, AWPDOUBLE)

	_awpPolynomCalculate(src->sSizeX, dst->sSizeX, s, s + src->sSizeX, d);

CLEANUP:
	return res;

}
