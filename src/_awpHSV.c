//    Purpose: AWPIPL source file
//    Contents: internal source code for HSV manipulation functions
//    Authors : Julia Radzhabova
#include "_awpipl.h"

/**
  * given rgb in [0..1], produce h in [0..360), s and v in [0..1], except
  * if s == 0, then h = undefined.
**/
static
void rgbtohsv_pixel(AWPFLOAT* H,AWPFLOAT *S,AWPFLOAT* V, AWPFLOAT* r, AWPFLOAT* g,AWPFLOAT* b)
{
    AWPFLOAT max_rgb,min_rgb;
    AWPFLOAT s,h,rc,gc,bc;

    h=0;

    max_rgb = *r;
    max_rgb = (max_rgb > *g) ? max_rgb : *g;
    max_rgb = (max_rgb > *b) ? max_rgb : *b;

    min_rgb = *r;
    min_rgb = (min_rgb < *g) ? min_rgb : *g;
    min_rgb = (min_rgb < *b) ? min_rgb : *b;

   // v = max_rgb;

    if (max_rgb != 0.)
	s = (max_rgb - min_rgb)/max_rgb;
    else
	s = 0.f;

    if (s == 0.f)
    {
	/* h should be undefined. */
	h = 0.;
    }
    else
    {
	rc = (max_rgb - *r)/(max_rgb - min_rgb);
	gc = (max_rgb - *g)/(max_rgb - min_rgb);
	bc = (max_rgb - *b)/(max_rgb - min_rgb);
	if (*r == max_rgb)
	    h = bc - gc;
	else if (*g == max_rgb)
	    h = 2.f + rc - bc;
	else if (*b == max_rgb)
	    h = 4.f + gc - rc;
	h = h * 60.f;
	if (h < 0.)
	    h = h + 360.f;
    }
    *H = h / 360.f; /* scale to range 0..1 */
    *S = s;
    *V = (*r+*g+*b)/3;
}

void _awpRGBtoHSV(AWPBYTE rr,AWPBYTE gg,AWPBYTE bb,AWPFLOAT *h, AWPFLOAT *s, AWPFLOAT *v )
{
	AWPFLOAT r,g,b;

	r = (AWPFLOAT)rr / 255.0f;
	g = (AWPFLOAT)gg / 255.0f;
	b = (AWPFLOAT)bb / 255.0f;
    *h = 0; *s = 0; *v = 0;
    rgbtohsv_pixel(h,s,v, &r,&g,&b);
}

void _awpHSVtoRGB(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE v, AWPBYTE *rr,AWPBYTE *gg,AWPBYTE *bb)
{
	AWPINT i;
	AWPDOUBLE f, p, q, t;
	AWPDOUBLE r,g,b;

	if( s == 0 )
	{
		// achromatic (grey)
		*rr = *gg = *bb = (AWPBYTE)(255*v);
		return;
	}
	h *= 6;			// sector 0 to 5
	i = (AWPINT)floor( h );
	f = h - i;			
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}

	*rr = (AWPBYTE)(255*r);
	*gg = (AWPBYTE)(255*g);
	*bb = (AWPBYTE)(255*b); 

}

