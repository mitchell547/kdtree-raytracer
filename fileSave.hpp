
#include "Types.hpp"
#include <iostream>
#define bmp
#pragma pack (push)
#pragma pack(1)
struct bmpinfo
{
	unsigned char signature1 = 'B';		//1 byte
	unsigned char signature2 = 'M';		//1 byte
	unsigned int fileSize;				//4 byte
	unsigned int reserved = 0;			//4 byte
	unsigned int offset = 14 + 40;		//4 byte
	unsigned int headerSize = 40;		//4 byte
	unsigned int width;					//4 byte
	unsigned int height;				//4 byte
	unsigned short colorPlanes = 1;		//2 byte
	unsigned short bpp = 24;			//2 byte
	unsigned int compression = 0;		//4 byte
	unsigned int imgSize;				//4 byte
	unsigned int resolutionHor = 2795;	//4 byte
	unsigned int resolutionVer = 2795;	//4 byte
	unsigned int pltColors = 0;			//4 byte
	unsigned int impColors = 0;			//4 byte
	bmpinfo (unsigned int _width, unsigned int _height)
	{
		width = _width;
		height = _height;
		fileSize = 14 + 40 + height * width * 3;
		imgSize = height *  width * 4;
	}
};
#pragma pack(pop)

inline void fileOutput (Vec *c, const unsigned  int w, const unsigned int h)
{
#ifdef ppm
	FILE *f = fopen ("image.ppm", "w");         // Write image to PPM file. 
	fprintf (f, "P3\n%d %d\n%d\n", w, h, 255);
	for (unsigned int i = 0; i < w*h; ++i)
		fprintf (f, "%d %d %d ", toInt (c[i].x), toInt (c[i].y), toInt (c[i].z));
#endif

#ifdef bmp
	bmpinfo first (w, h);
	FILE *f = fopen ("temp1.bmp", "wb");  // 

	fwrite (&first, sizeof(bmpinfo), 1, f);
	int step = (w * 3 + 3) / 4 * 4;
	//unsigned char *data = new unsigned char[step*h];
	unsigned char *data = (unsigned char *)malloc (step*h*sizeof (unsigned char));
	for (int i = h - 1; i >= 0; i--)
	{
		unsigned char *p = data + step * (h - i);

		for (unsigned int j = 0; j < w; j++)
		{
			*p++ = toChar (c[i*w + j].z);
			*p++ = toChar (c[i*w + j].y);
			*p++ = toChar (c[i*w + j].x);
		}
	}

	fwrite (data, step*h, 1, f);
	free (data);
	fclose (f);
#endif
}