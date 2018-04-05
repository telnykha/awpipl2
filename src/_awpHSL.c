/*
//
//  RCS:
//      $Source: /home/cvs/awp/imageproc/awpipl-v2/src/_awpHSL.c,v $
//      $Revision: 1.2 $
//      $Date:  
//
//    Purpose: AWPIPL source file
//    Contents: internal source code for HSL manipulation functions
//    Authors : Alexander Telnykh
*/
#include "_awpipl.h"
void _awpRGBtoHSL(AWPBYTE rr,AWPBYTE gg,AWPBYTE bb,AWPDOUBLE *h, AWPDOUBLE *s, AWPDOUBLE *l )
{
    AWPDOUBLE v;
    AWPDOUBLE m;
    AWPDOUBLE vm;
    AWPDOUBLE r2, g2, b2;
	AWPDOUBLE r,g,b;

	r = rr / 255.0;
	g = gg / 255.0;
	b = bb / 255.0;

    v = AWP_MAX(r,g);
    v = AWP_MAX(v,b);
    m = AWP_MIN(r,g);
    m = AWP_MIN(m,b);

    if ((*l = (m + v) / 2.0) <= 0.0) return;
    if ((*s = vm = v - m) > 0.0) {
		*s /= (*l <= 0.5) ? (v + m ) :
			(2.0 - v - m) ;
    } else
	return;


    r2 = (v - r) / vm;
    g2 = (v - g) / vm;
    b2 = (v - b) / vm;

    if (r == v)
		*h = (g == m ? 5.0 + b2 : 1.0 - g2);
    else if (g == v)
		*h = (b == m ? 1.0 + r2 : 3.0 - b2);
    else
		*h = (r == m ? 3.0 + g2 : 5.0 - r2);

    	*h /= 6;

}
/*convert HSL to RGB */
void _awpHLStoRGB(AWPDOUBLE h, AWPDOUBLE s, AWPDOUBLE l, AWPBYTE *rr,AWPBYTE *gg,AWPBYTE *bb)
{
    AWPDOUBLE v;
    AWPDOUBLE r,g,b;
    
    r = g = b = 0.0;
    
    v = (l <= 0.5) ? (l * (1.0 + s)) : (l + s - l * s);
    
    if (v <= 0) {
		r = g = b = 0.0;
    } else {
		AWPDOUBLE m;
		AWPDOUBLE sv;
		AWPINT sextant;
		AWPDOUBLE fract, vsf, mid1, mid2;

		m = l + l - v;
		sv = (v - m ) / v;
		h *= 6.0;
		sextant = (AWPINT)h;	
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant) {
			case 0: r = v; g = mid1; b = m; break;
			case 1: r = mid2; g = v; b = m; break;
			case 2: r = m; g = v; b = mid1; break;
			case 3: r = m; g = mid2; b = v; break;
			case 4: r = mid1; g = m; b = v; break;
			case 5: r = v; g = m; b = mid2; break;
		}
    }

	*rr = (AWPBYTE)(255*r);
	*gg = (AWPBYTE)(255*g);
	*bb = (AWPBYTE)(255*b); 

}


