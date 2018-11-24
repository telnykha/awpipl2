#include "_awpipl.h"

void _awpCountPoints(awpImage* pImage, awpPoint p1, awpPoint p2, AWPINT* count)
{
	AWPWORD t, distance;
	AWPSHORT incx, incy;

	AWPWORD startx = p1.X;
	AWPWORD starty = p1.Y;
	AWPWORD endx = p2.X;
	AWPWORD endy = p2.Y;
	AWPINT xerr = 0, yerr = 0, delta_x, delta_y;

	delta_x = endx - startx;
	delta_y = endy - starty;

	if (delta_x > 0) incx = 1;
	else if (delta_x == 0) incx = 0;
	else incx = -1;

	if (delta_y > 0) incy = 1;
	else if (delta_y == 0) incy = 0;
	else incy = -1;

	delta_x = abs(delta_x);
	delta_y = abs(delta_y);
	if (delta_x > delta_y) 
		distance = delta_x;
	else
		distance = delta_y;
	*count = distance;
}



/*
*/ 
AWPRESULT awpGetLine(awpImage* src, awpPoint p1, awpPoint p2, awpImage** dst)
{
	AWPRESULT result = AWP_OK;
	AWPINT    numPoints = 0;
	
	if (src == NULL || dst == NULL)
		return  AWP_BADARG;
	if (p1.X > src->sSizeX || p1.Y > src->sSizeY || p2.X > src->sSizeX || p2.Y > src->sSizeY)
		return  AWP_BADARG;

	_awpCountPoints(src, p1, p2, &numPoints);

	if (numPoints == 0)
		return AWP_BADARG;

	result = awpCreateImage(dst, numPoints, 1, src->bChannels, src->dwType);
	_CHECK_RESULT_(result)



CLEANUP:
	return result;
}
