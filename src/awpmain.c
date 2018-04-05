/*
    awpmain.c
*/
#include "awpipl.h"

int main (int argc, char **argv)
{
   printf("Hello awpipl!\n");
   awpImage* img = NULL; 
   if (awpCreateImage(&img, 128,128,1, AWP_BYTE) != AWP_OK)
   {			
	printf("cannot create image.\n");
	return -1;
   }
   
   awpRect rect;
   rect.left = 32;
   rect.top  = 32;
   rect.right = rect.left + 64;
   rect.bottom = rect.top + 64;

   awpPoint point;
   point.X = 64;
   point.Y = 64; 

   awpDrawRect(img, &rect, 0, 128, 2);
   awpDrawEllipse(img, point, 50, 50, 0,0, 200, 1);

   awpSaveImage("test.jpg", img);  	
   _AWP_SAFE_RELEASE_(img);  
   return 0;
}
