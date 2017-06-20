#include "Include/Geometry/Vec.hpp"
#include <math.h>
#include <stdio.h>
#include <ctime>
#include "Include/raytracer_misc.h"
#pragma once
#pragma pack (push)
#pragma pack(1)
struct bmpinfo
{
	// BMP File Header
	unsigned char signature1;//1 byte
	unsigned char signature2;//1 byte
	unsigned int fileSize;//4 byte
	unsigned int reserved;//4 byte
	unsigned int offset;//4 byte
	// BMP Info Header 
	unsigned int headerSize;//4 byte
	unsigned int width;//4 byte
	unsigned int height;//4 byte
	unsigned short colorPlanes;//2 byte
	unsigned short bpp;//2 byte
	// 
	unsigned int compression;//4 byte
	unsigned int imgSize;//4 byte
	unsigned int resolutionHor;//4 byte
	unsigned int resolutionVer;//4 byte
	unsigned int pltColors;//4 byte
	unsigned int impColors;//4 byte
	bmpinfo (unsigned int _width, unsigned int _height)
	{
		// BMP File Header
		signature1 = 'B';//1 byte
		signature2 = 'M';//1 byte
		reserved = 0;//4 byte
		offset = 14 + 40;//4 byte
		// BMP Info Header
		headerSize = 40;//4 byte
		colorPlanes = 1;//2 byte
		bpp = 24;//2 byte
		compression = 0;//4 byte
		resolutionHor = 2795;//4 byte
		resolutionVer = 2795;//4 byte
		pltColors = 0;//4 byte
		impColors = 0;//4 byte
		width = _width;
		height = _height;
		fileSize = 14 + 40 + height * width * 3;
		imgSize = height *  width * 4;

	}
};
#pragma pack(pop)

inline void writeToPpm (Vec *c, const unsigned  int w, const unsigned int h)
{
#ifdef ppm
	FILE *f = fopen ("image.ppm", "w");         // Write image to PPM file. 
	fprintf (f, "P3\n%d %d\n%d\n", w, h, 255);
	for (unsigned int i = 0; i < w*h; ++i)
		fprintf (f, "%d %d %d ", toInt (c[i].x), toInt (c[i].y), toInt (c[i].z));
#endif
}

void writeToBmp (Vec *c, const unsigned  int w, const unsigned int h)
{
	bmpinfo first (w, h);
	char filename [30];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(filename, "%.2d%.2d%.2d_%.2d%.2d", tm.tm_year-100, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min);
	FILE *f = fopen(strcat(filename, ".bmp"), "wb");  // 
	if (f == NULL)
	{
		printf("Cannot open output file\n");
		return;
	}
	fwrite (&first, sizeof(bmpinfo), 1, f);
	int step = (w * 3 + 3) / 4 * 4;
	//unsigned char *data = new unsigned char[step*h];
	unsigned char pixel[3];
	for (int i = 0; i < h; ++i)
	{
		//unsigned char *p = data + step * (h - i);

		for (int j = 0; j < w; ++j)
		{
			pixel[0] = toChar (c[i*w + j].z);
			pixel[1] = toChar (c[i*w + j].y);
			pixel[2] = toChar (c[i*w + j].x);
			if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0)
				int aaa =1;
			//pixel[3] = 0;
			fwrite(pixel, sizeof(unsigned char) * 3, 1, f);
			
			/**p++ = toChar (c[i*w + j].z);
			*p++ = toChar (c[i*w + j].y);
			*p++ = toChar (c[i*w + j].x);
			*/
		}
	}

	fclose (f);

}