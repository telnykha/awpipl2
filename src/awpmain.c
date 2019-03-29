/*
    awpmain.c
*/


#define _CRT_SECURE_NO_WARNINGS
#include "../include/awpipl.h"
#include "stdlib.h"
#define CHECK_RESULT  \
	if(res!=AWP_OK){ \
		printf("ERROR CODE:%i", res);\
		exit(-1); \
	}\

#define __GET_IDX__ \
int idx0 = InputKey(argc, argv, "-i");\
int idx1 = InputKey(argc, argv, "-o");\
int idx2 = InputKey(argc, argv, "-p");\
if (idx0 < 2)\
	exit(-1);\

int InputKey(int argc, char **argv, const char* key)
{
	int i = 0;
	int result = -1;
	for (i = 2; i < argc; i++)
	{
		if (strcmp(argv[i], key) == 0)
		{
			result = i + 1;
			break;
		}
	}
	if (result < 0)
	{
		printf("cannot find key %s.\n", key);
		exit(-1);
	}
		
	return result;
}


awpImage* __LoadImage(const char* lpFileName)
{
	awpImage* img = NULL;
	if (awpLoadImage(lpFileName, &img) != AWP_OK)
	{
		printf("cannot load image %s\n", lpFileName);
		exit(-1);
	}
	return img;
}

void __SaveImage(const char* lpFileName, awpImage* img)
{
	if (awpSaveImage(lpFileName, img) != AWP_OK)
	{
		printf("cannot save image %s\n", lpFileName);
		exit(-1);
	}
}


void Help(int argc, char **argv)
{
	printf("--help \n");
	printf("--info -i filename\n");
	printf("--flip -i filename -o outfile -p mode [mode = r - right, l - left, v - vertical, h - horizontal]\n");
	printf("--resize -i filename -o outfile -p width:height:mode [mode = r - fast, b - bilinear, n - not in place]\n");
	printf("--rescale -i filename -o outfile -p width:height\n");
	printf("--rotate -i filename -o outfile -p angle:mode [mode = r - fast, b - bilinear]\n");
	printf("--rotatecenter -i filename -o outfile -p angle:x:y\n");
	printf("--calc -i filename -i1 - filename2 -p mode [mode = and,or,xor,add,sub,mlt,div,min,max,avg] -o outfile \n");
	printf("--crop -i filename -o outfile -p left:top:right:bottom\n");
	printf("--filter options -i filename -o outfile\n");
	printf("--contrast -i filename -o outfile -p mode [mode = a - autolevels, h - histogramm equalize]\n");
	printf("--stat \n");
	printf("--convert -i filename -o outfile\n");
	printf("--draw \n");
	printf("--blobs \n");
	printf("--detect \n");
	printf("--bacrproject \n");
	printf("--camera \n");
}
void Convert(int argc, char** argv) {
	awpImage* img = NULL;
	__GET_IDX__
	
		
		img = __LoadImage(argv[idx0]);
	if (strcmp(argv[idx2], "c") == 0) {
		awpConvert(img, AWP_CONVERT_TO_BYTE_WITH_NORM);

	}

	__SaveImage(argv[idx1], img);
	_AWP_SAFE_RELEASE_(img);
}
void Calc(int argc,char** argv) {
	awpImage* img = NULL;
	awpImage* img2 = NULL;
	awpImage* img3 = NULL;
	int res = AWP_OK;
	__GET_IDX__
		int idx3 = InputKey(argc, argv, "-i1");
		img = __LoadImage(argv[idx0]);
		img2 = __LoadImage(argv[idx3]);
		if (strcmp(argv[idx2], "and") == 0) {
			res=awpCalcImage(img, img2, &img3, AWP_CALC_ANDIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else if (strcmp(argv[idx2], "or") == 0){
			res=awpCalcImage(img, img2, &img3, AWP_CALC_ORIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else if (strcmp(argv[idx2], "xor") == 0)	{
			res = awpCalcImage(img, img2, &img3, AWP_CALC_XORIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else if (strcmp(argv[idx2], "add") == 0) {
			res = awpCalcImage(img, img2, &img3, AWP_CALC_ADDIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else if (strcmp(argv[idx2], "sub") == 0) {
			res = awpCalcImage(img, img2, &img3, AWP_CALC_SUBIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else if (strcmp(argv[idx2], "mlt") == 0) {
			res = awpCalcImage(img, img2, &img3, AWP_CALC_MLTIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else if (strcmp(argv[idx2], "div") == 0) {
			res = awpCalcImage(img, img2, &img3, AWP_CALC_DIVIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else if (strcmp(argv[idx2], "min") == 0) {
			res = awpCalcImage(img, img2, &img3, AWP_CALC_MINIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else if (strcmp(argv[idx2], "max") == 0) {
			res = awpCalcImage(img, img2, &img3, AWP_CALC_MAXIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else if (strcmp(argv[idx2], "avg") == 0) {
			res = awpCalcImage(img, img2, &img3, AWP_CALC_AVGIMAGES, AWP_CALC_OUTPLACE);
			CHECK_RESULT
		}
		else {
			printf("unknown calc option %s\n", argv[idx2]);
			exit(-1);
		}
		awpConvert(img3, AWP_CONVERT_TO_BYTE_WITH_NORM);
		__SaveImage(argv[idx1], img3);
		_AWP_SAFE_RELEASE_(img3);
}
void Info(int argc, char **argv)
{
	awpImage* img = NULL;
	int idx = InputKey(argc, argv, "-i");
	if (idx < 2)
		exit(-1);
	img = __LoadImage(argv[idx]);
	printf("width = %d\n", img->sSizeX);
	printf("height = %d\n", img->sSizeY);
	printf("channels = %d\n", img->bChannels);
	printf("type = %d\n", img->dwType);
	FILE* f = fopen(argv[idx], "rb");
	if (f != NULL)
	{
		fseek(f, 0, SEEK_END); // seek to end of file 
		long size = ftell(f); // get current file pointer 
		fseek(f, 0, SEEK_SET);
		printf("size = %d\n", size);
		fclose(f);
	}
	else
	{
		printf("cannot open file.\n");
	}


	_AWP_SAFE_RELEASE_(img);
}
void Flip(int argc, char **argv)
{
	awpImage* img = NULL;
	int res = AWP_OK;
	//int idx0 = InputKey(argc, argv, "-i");
	//int idx1 = InputKey(argc, argv, "-o");
	//int idx2 = InputKey(argc, argv, "-p");
	//if (idx0 < 2)
	//	exit(-1);
	__GET_IDX__

	img = __LoadImage(argv[idx0]);
	printf("width = %d height = %d\n", img->sSizeX, img->sSizeY);
	if (strcmp(argv[idx2], "r") == 0)
	{
		res = awpFlip(&img, AWP_FLIP_RGHT);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "l") == 0)
	{
		res = awpFlip(&img, AWP_FLIP_LEFT);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "v") == 0)
	{
		res = awpFlip(&img, AWP_FLIP_VERT);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "h") == 0)
	{
		res = awpFlip(&img, AWP_FLIP_HRZT);
		CHECK_RESULT
	}
	else
	{
		printf("unknown flip option %s\n", argv[idx2]);
		exit(-1);
	}

	__SaveImage(argv[idx1], img);
	_AWP_SAFE_RELEASE_(img);
}
void Resize(int argc, char **argv)
{
	awpImage* img = NULL;
	awpImage* imgo = NULL;
	int w;
	int h;
	char key;
	int k,res;
	__GET_IDX__
	k = sscanf(argv[idx2], "%i:%i:%c", &w, &h, &key);
	if (k != 3)
	{
		printf("invalid resize params = %s\n", argv[idx2]);
		exit(-1);
	}
	img = __LoadImage(argv[idx0]);

	if (key == 'r')
	{
		res=awpResize(img, w, h);
		CHECK_RESULT
	}
	else if (key == 'b')
	{
		res=awpResizeBilinear(img, w, h);
		CHECK_RESULT
	}
	else if (key == 'n')
	{
		res=awpResizeNip(img, &imgo, w, h);
		CHECK_RESULT
		_AWP_SAFE_RELEASE_(img);
		img = imgo;
	}
	else
	{
		printf("invalid resize params = %s\n", argv[idx2]);
		exit(-1);
	}

    __SaveImage(argv[idx1], img);
	_AWP_SAFE_RELEASE_(img);
}
void Rescale(int argc, char **argv)
{
	awpImage* img = NULL;
	float w;
	float h;
	int k , res;
	__GET_IDX__
		k = sscanf(argv[idx2], "%f:%f", &w, &h);
	if (k != 2)
	{
		printf("invalid rescale params = %s\n", argv[idx2]);
		exit(-1);
	}
	img = __LoadImage(argv[idx0]);

	res=awpRescale(img, w, h);
	CHECK_RESULT
	__SaveImage(argv[idx1], img);
	_AWP_SAFE_RELEASE_(img);
}
void Rotate(int argc, char **argv)
{
	awpImage* img = NULL;
	int angle;
	char key;
	int k;
	int res;
	__GET_IDX__
		k = sscanf(argv[idx2], "%i:%c", &angle, &key);
	if (k != 2)
	{
		printf("invalid rotate params = %s\n", argv[idx2]);
		exit(-1);
	}
	img = __LoadImage(argv[idx0]);
	if (key == 'r')
	{
		res=awpRotate(img, angle);
		CHECK_RESULT
	}
	else if (key == 'b')
	{
		res=awpRotateBilinear(img, angle);
		CHECK_RESULT
	}
	else
	{
		printf("invalid rotate params = %s\n", argv[idx2]);
		exit(-1);
	}
	__SaveImage(argv[idx1], img);
	_AWP_SAFE_RELEASE_(img);
}
void RotateCenter(int argc, char **argv)
{
	awpImage* img = NULL;
	int angle;
	int x;
	int y;
	int k,res;
	__GET_IDX__
		k = sscanf(argv[idx2], "%i:%i:%i", &angle, &x, &y);
	if (k != 3)
	{
		printf("invalid rotate params = %s\n", argv[idx2]);
		exit(-1);
	}
	img = __LoadImage(argv[idx0]);
		
	res=awpRotateCenter(img, angle, x, y);
	CHECK_RESULT

	__SaveImage(argv[idx1], img);
	_AWP_SAFE_RELEASE_(img);
}

void Crop(int argc, char **argv)
{
	awpImage* img = NULL;
	awpImage* imgo = NULL;
	awpRect* rect = NULL;
	rect = (awpRect*)malloc(sizeof(awpRect));
	int left = 0;// rect->left;
	int top = 0;// rect->top;
	int right = 0;// rect->right;
	int bottom = 0;// rect->bottom;
	int k;
	__GET_IDX__
			k = sscanf(argv[idx2], "%i:%i:%i:%i", &left, &top, &right, &bottom);
		if (k != 4)
		{
			printf("invalid crop params = %s\n", argv[idx2]);
			exit(-1);
		}
	rect->left = left;
	rect->top = top;
	rect->right = right;
	rect->bottom = bottom;
	img = __LoadImage(argv[idx0]);
	if (awpCopyRect(img, &imgo, rect) == AWP_OK)
	{	
		__SaveImage(argv[idx1], imgo);
		_AWP_SAFE_RELEASE_(imgo);
	}
	else
	{
		printf("invalid rect %s\n", argv[idx2]);
		exit(-1);
	}
	free(rect);
	_AWP_SAFE_RELEASE_(img);
}
void Contrast(int argc, char **argv)
{
	awpImage* img = NULL;
	__GET_IDX__

	img = __LoadImage(argv[idx0]);
	if (strcmp(argv[idx2], "a") == 0)
	{
		AWPRESULT res = AWP_OK;
		res = awpAutoLevels(img);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "h") == 0)
	{
		AWPRESULT res = AWP_OK;
		res = awpHistogrammEqualize(img);
		CHECK_RESULT
	}
	else
	{
		printf("unknown contrast option %s\n", argv[idx2]);
		exit(-1);
	}
	__SaveImage(argv[idx1], img);
	_AWP_SAFE_RELEASE_(img);
}
void Filter(int argc, char **argv)
{
	int res;
	awpImage* img = NULL;
	__GET_IDX__

		img = __LoadImage(argv[idx0]);
	awpConvert(img, AWP_CONVERT_3TO1_BYTE);
	if (strcmp(argv[idx2], "b") == 0)
	{
		res=awpFilter(img, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "bm") == 0)
	{
		res=awpFilter(img, AWP_BLUR_MORE, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "s") == 0)
	{
		res=awpFilter(img, AWP_SHARPEN, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "sm") == 0)
	{
		res=awpFilter(img, AWP_SHARPEN_MORE, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "se") == 0)
	{
		res=awpFilter(img, AWP_SHARPEN_EDGES, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "fe") == 0)
	{
		res=awpFilter(img, AWP_FIND_EDGES, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "fe1") == 0)
	{
		res=awpFilter(img, AWP_FIND_EDGES1, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "en") == 0)
	{
		res=awpFilter(img, AWP_EMBOSS_NORTH, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "eno") == 0)
	{
		res=awpFilter(img, AWP_EMBOSS_NORTH_OST, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "eo") == 0)
	{
		res=awpFilter(img, AWP_EMBOSS_OST, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "eso") == 0)
	{
		res=awpFilter(img, AWP_EMBOSS_SOUTH_OST, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "es") == 0)
	{
		res=awpFilter(img, AWP_EMBOSS_SOUTH, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "esw") == 0)
	{
		res=awpFilter(img, AWP_EMBOSS_SOUTH_WEST, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "ew") == 0)
	{
		res=awpFilter(img, AWP_EMBOSS_WEST, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "enw") == 0)
	{
		res=awpFilter(img, AWP_EMBOSS_NORTH_WEST, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "pv") == 0)
	{
		res=awpFilter(img, AWP_PREWITT_V, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "ph") == 0)
	{
		res=awpFilter(img, AWP_PREWITT_H, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "sv") == 0)
	{
		res=awpFilter(img, AWP_SOBEL_V, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else if (strcmp(argv[idx2], "sh") == 0)
	{
		res=awpFilter(img, AWP_SOBEL_H, AWP_FILTER_METHOD_ORDINARY);
		CHECK_RESULT
	}
	else
	{
		printf("unknown filter option %s\n", argv[idx2]);
		exit(-1);
	}
	__SaveImage(argv[idx1], img);
	_AWP_SAFE_RELEASE_(img);
}
int main (int argc, char **argv)
{
  int i = 0;
  /*
   printf("Hello awpipl!\n");
   printf("argc = %d\n", argc);
   for (i = 0; i < argc; i++)
	printf("argv[%d] = %s\n", i, argv[i]);
   */
  if (argc < 2)
  {
	  return -1;
  }
   char* arg1 = argv[1];
   if (strcmp(arg1, "--help") == 0)
   {
	   Help(argc, argv);
   }
   else if (strcmp(arg1, "--info") == 0)
   {
	   Info(argc, argv);
   }
   else if (strcmp(arg1, "--flip") == 0)
   {
	   Flip(argc, argv);
   }
   else if (strcmp(arg1, "--resize") == 0)
   {
	   Resize(argc, argv);
   }
   else if (strcmp(arg1, "--rescale") == 0)
   {
	   Rescale(argc, argv);
   }
   else if (strcmp(arg1, "--rotate") == 0)
   {
	   Rotate(argc, argv);
   }
   else if (strcmp(arg1, "--rotatecenter") == 0)
   {
	   RotateCenter(argc, argv);
   }
   else if (strcmp(arg1, "--calc") == 0)
   {
	   Calc(argc, argv);
   }
   else if (strcmp(arg1, "--crop") == 0)
   {
	   Crop(argc, argv);
   }
   else if (strcmp(arg1, "--filter") == 0)
   {
	   Filter(argc, argv);
   }
   else if (strcmp(arg1, "--contrast") == 0)
   {
	   Contrast(argc, argv);
   }
   else if (strcmp(arg1, "--hst") == 0)
   {
   }
   else if (strcmp(arg1, "--stat") == 0)
   {
   }
   else if (strcmp(arg1, "--convert") == 0)
   {
	   Convert(argc, argv);
   }
   else if (strcmp(arg1, "--draw") == 0)
   {
   }
   else if (strcmp(arg1, "--stroke") == 0)
   {
   }
   else if (strcmp(arg1, "--contour") == 0)
   {
   }
   else if (strcmp(arg1, "--blobs") == 0)
   {
   }
   else if (strcmp(arg1, "--distanse") == 0)
   {
   }
   else if (strcmp(arg1, "--detect") == 0)
   {
   }
   else if (strcmp(arg1, "--lbp") == 0)
   {
   }
   else if (strcmp(arg1, "--backproject") == 0)
   {
   }
   else if (strcmp(arg1, "--camera") == 0)
   {
   }

   awpImage* img = NULL;
   if (awpCreateImage(&img, 512,512,3, AWP_BYTE) != AWP_OK)
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
   point.X = 256;
   point.Y = 256;	

   awpDrawRect(img, &rect, 1, 255, 1);
   awpDrawEllipse(img, point, 50, 150, 30,2, 200, 1);

   awpSaveImage("test.awp", img);  	
   _AWP_SAFE_RELEASE_(img);  
   return 0;
}