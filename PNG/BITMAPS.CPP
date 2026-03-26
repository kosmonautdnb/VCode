#define _CRT_SECURE_NO_WARNINGS

#include "bitmaps.h"
#include "png/png.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <istream>
#include <fstream>
#include <string>
//#include "psd/psd.h"
#include "psd2/psdimage.h"
#include <fstream>

//#include "tiff/tiff.h"
//#include "tiff/tiffio.h"

#define PALENTRY(__a__) 0x##__a__

unsigned int palette[256];
unsigned int palette2[256];
unsigned int paletteC64[17] = {
	PALENTRY(FF000000), // palette r is >> 16
	PALENTRY(FFFFFFFF),
	PALENTRY(FF7E352B),
	PALENTRY(FF6EB7C1),
	PALENTRY(FF7F3BA6),
	PALENTRY(FF5CA035),
	PALENTRY(FF332799),
	PALENTRY(FFCBD765),
	PALENTRY(FF85531C),
	PALENTRY(FF503C00),
	PALENTRY(FFB46B61),
	PALENTRY(FF4A4A4A),
	PALENTRY(FF757575),
	PALENTRY(FFA3E77C),
	PALENTRY(FF7064D6),
	PALENTRY(FFA3A3A3),

	PALENTRY(FFFF00FF)
};

std::vector<std::string> split(const std::string& s, char seperator)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;
	while ((pos = s.find(seperator, pos)) != std::string::npos)
	{
		std::string substring(s.substr(prev_pos, pos - prev_pos));
		output.push_back(substring);
		prev_pos = ++pos;
	}
	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
	return output;
}

void setupPalette(const std::string& paletteKichyDir, const std::string& paletteNeroDir) {
	std::ifstream file(paletteKichyDir + "Plus4ColsMeasured.asc");
	std::string zeile;
	int nr = 0;
	while (std::getline(file, zeile) && zeile != "") {
		std::vector<std::string> values = split(zeile, ' ');
		if (values.size() == 3) {
			int r = atoi(values[0].c_str());
			int g = atoi(values[1].c_str());
			int b = atoi(values[2].c_str());
			palette[nr] = b | (g << 8) | (r << 16) | 0xff000000;
			nr++;
		}
	}
	file.close();

	FILE* in = fopen((paletteNeroDir + "PLU4COLORS.bin").c_str(), "rb");
	fread(palette2, 1, 1024, in);
	fclose(in);
	for (int i = 0; i < 256; ++i)
		palette2[i] = ((palette2[i] >> 16) & 255) + ((palette2[i] & 0xff) << 16) + (palette2[i] & 0xff00) + 0xff000000;
}

//--------------------------------------------------------
unsigned int getPalette2(unsigned char color) {
	return ((palette2[color] >> 16) & 255) | (palette2[color] & 0x0000ff00) | ((palette2[color] << 16) & 0x00ff0000) | 0xff000000;
}

unsigned char getColor2(unsigned int color)
{
	int r = color & 255;
	int g = (color >> 8) & 255;
	int b = (color >> 16) & 255;
	int lastDelta = 10000 * 10000;
	int ri = 0;
	unsigned int colorEnabled[16] = { 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1 };

	for (int i = 0; i < 128; ++i)
	{
		int cr = (palette2[i] >> 16) & 255;
		int cg = (palette2[i] >> 8) & 255;
		int cb = palette2[i] & 255;
		int dr = cr - r;
		int dg = cg - g;
		int db = cb - b;
		int d = dr * dr + dg * dg + db * db;

		if (d < lastDelta && (colorEnabled[i & 15] != 0))
		{
			lastDelta = d;
			ri = i;
		}
	}
	if ((ri & 0x0f) == 0x00)
		ri = 0;
	return ri;
}

unsigned int getPalette(unsigned char color) {
	return ((palette[color] >> 16) & 255) | (palette[color] & 0x0000ff00) | ((palette[color] << 16) & 0x00ff0000) | 0xff000000;
}

unsigned int getPaletteC64(unsigned char color) {
	return ((paletteC64[color] >> 16) & 255) | (paletteC64[color] & 0x0000ff00) | ((paletteC64[color] << 16) & 0x00ff0000) | 0xff000000;
}

unsigned char getColor(unsigned int color)
{
	bool a = (color >> 24) > 128;
	int r = color & 255;
	int g = (color >> 8) & 255;
	int b = (color >> 16) & 255;
	int lastDelta = 10000 * 10000;
	int ri = 0;
	unsigned int colorEnabled[16] = { 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1 };

	for (int i = 0; i < 128; ++i)
	{
		int cr = (palette[i] >> 16) & 255;
		int cg = (palette[i] >> 8) & 255;
		int cb = palette[i] & 255;
		int dr = cr - r;
		int dg = cg - g;
		int db = cb - b;
		int d = dr * dr + dg * dg + db * db;

		if (d < lastDelta && (colorEnabled[i & 15] != 0))
		{
			lastDelta = d;
			ri = i;
		}
	}
	if ((ri & 0x0f) == 0x00)
		ri = 0;
	return ri;
}

unsigned char getColorC64(unsigned int color)
{
	bool a = (color >> 24) > 128;
	int r = color & 255;
	int g = (color >> 8) & 255;
	int b = (color >> 16) & 255;
	int lastDelta = 10000 * 10000;
	int ri = 0;

	for (int i = 0; i < 17; ++i)
	{
		int cr = (paletteC64[i] >> 16) & 255;
		int cg = (paletteC64[i] >> 8) & 255;
		int cb = paletteC64[i] & 255;
		int dr = cr - r;
		int dg = cg - g;
		int db = cb - b;
		int d = dr * dr + dg * dg + db * db;
		if (d < lastDelta)
		{
			lastDelta = d;
			ri = i;
		}
	}
	return ri;
}

unsigned char bmpBuffer2[MAXWIDTH * MAXHEIGHT * 4 + 0x36];
unsigned int pictureS[MAXWIDTH*MAXHEIGHT];
unsigned int pictureWriteOut[MAXWIDTH*MAXHEIGHT];
int pictureWidth;
int pictureHeight;

extern "C"
{
#if defined(HAVE_LIBPNG) && defined(HAVE_LIBZ)
#  include <zlib.h>
#  ifdef HAVE_PNG_H
#    include <png.h>
#  else
#    include <libpng/png.h>
#  endif // HAVE_PNG_H
#endif // HAVE_LIBPNG && HAVE_LIBZ
}

Plus4Bitmap toPlus4Bitmap(bool generateTexture) {
	Plus4Bitmap ret;
	ret.width = pictureWidth;
	ret.height = pictureHeight;
	for (int y = 0; y < pictureHeight; ++y)
		for (int x = 0; x < pictureWidth; ++x) {
			unsigned int a = getColor(pictureS[x + y * pictureWidth]);
			bool transparent = (pictureS[x + y * pictureWidth] >> 24) < 10 ? true : false;
			if (transparent)
				ret.pixels.push_back(255);
			else
				ret.pixels.push_back(a);
		}

	if (generateTexture) {
		ret.upload(false);
	}

	return ret;
}

/**
  The constructor loads the named PNG image from the given png filename.
  <P>The destructor free all memory and server resources that are used by
  the image.
*/
void LoadPNG(const char *png) // I - File to read
{
  int		i;			// Looping var
  FILE		*fp;			// File pointer
  int		channels;		// Number of color channels
  png_structp	pp;			// PNG read pointer
  png_infop	info;			// PNG info pointers
  png_bytep	*rows;			// PNG row pointers

  memset(pictureS, 0, sizeof(pictureS));

  // Open the PNG file...
  if ((fp = fopen(png, "rb")) == NULL) return;

  // Setup the PNG data structures...
  pp   = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct(pp);

  if (setjmp(pp->jmpbuf))
  {
	  int debug = 1;
	  return;
  }

  // Initialize the PNG read "engine"...
  png_init_io(pp, fp);

  // Get the image dimensions and convert to grayscale or RGB...
  png_read_info(pp, info);

  if (info->color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_expand(pp);

  if (info->color_type & PNG_COLOR_MASK_COLOR)
    channels = 3;
  else
    channels = 1;

  if ((info->color_type & PNG_COLOR_MASK_ALPHA) || info->num_trans)
    channels ++;

  int w = (int)(info->width);
  int h = (int)(info->height);
  int d = channels;
  pictureWidth = w;
  pictureHeight = h;

  if (info->bit_depth < 8)
  {
    png_set_packing(pp);
    png_set_expand(pp);
  }
  else if (info->bit_depth == 16)
    png_set_strip_16(pp);

#  if defined(HAVE_PNG_GET_VALID) && defined(HAVE_PNG_SET_TRNS_TO_ALPHA)
  // Handle transparency...
  if (png_get_valid(pp, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(pp);
#  endif // HAVE_PNG_GET_VALID && HAVE_PNG_SET_TRNS_TO_ALPHA

  unsigned char *array = (unsigned char *)pictureS;

  // Allocate pointers...
  rows = new png_bytep[h];

  for (i = 0; i < h; i ++)
    rows[i] = (png_bytep)(array + i * w * d); // we flip it

  // Read the image, handling interlacing as needed...
  for (i = png_set_interlace_handling(pp); i > 0; i --)
    png_read_rows(pp, rows, NULL, h);

#ifdef WIN32
  // Some Windows graphics drivers don't honor transparency when RGB == white
  if (channels == 4)
  {
    // Convert RGB to 0 when alpha == 0...
    unsigned char *ptr = (unsigned char *)array;
    for (i = w * h; i > 0; i --, ptr += 4)
      if (!ptr[3]) ptr[0] = ptr[1] = ptr[2] = 0;
  }
#endif // WIN32

  if (channels == 3)
  {
	  unsigned char *array2 = new unsigned char[pictureWidth * pictureHeight * 4];
	  for (int i = w * h - 1; i >= 0; --i)
	  {
		  array2[i*4+0] = array[i*3+0];
		  array2[i*4+1] = array[i*3+1];
		  array2[i*4+2] = array[i*3+2];
		  array2[i*4+3] = 255;
	  }
	  memcpy(array, array2, w * h * 4);
	  delete[] array2;
  }

  // Free memory and return...
  delete[] rows;

  png_read_end(pp, info);
  png_destroy_read_struct(&pp, &info, NULL);

  fclose(fp);
}

void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

void SavePNG(const char* file_name, bool flipped)
{
	png_structp png_ptr;
	png_infop info_ptr;

	/* create file */
	FILE *fp = fopen(file_name, "wb");
	if (!fp)
		abort_("[write_png_file] File %s could not be opened for writing", file_name);


	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		fclose(fp);
		abort_("[write_png_file] png_create_write_struct failed");
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fclose(fp);
		abort_("[write_png_file] png_create_info_struct failed");
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		abort_("[write_png_file] Error during init_io");
	}

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		abort_("[write_png_file] Error during writing header");
	}

	png_set_IHDR(png_ptr, info_ptr, pictureWidth, pictureHeight,
		8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		abort_("[write_png_file] Error during writing bytes");
	}

	png_bytep * row_pointers;

	unsigned char *array = new unsigned char[pictureWidth * pictureHeight * 4 + 4];
	for (int i = pictureWidth * pictureHeight; i >= 0; --i)
	{
		array[i * 4 + 0] = ((unsigned char*)pictureWriteOut)[i * 4 + (flipped ? 2 : 0)];
		array[i * 4 + 1] = ((unsigned char*)pictureWriteOut)[i * 4 + 1];
		array[i * 4 + 2] = ((unsigned char*)pictureWriteOut)[i * 4 + (flipped ? 0 : 2)];
		array[i * 4 + 3] = ((unsigned char*)pictureWriteOut)[i * 4 + 3];
	}


	// Allocate pointers...
	row_pointers = new png_bytep[pictureHeight];

	for (int i = 0; i < pictureHeight; i++)
		row_pointers[i] = (png_bytep)(array + (i)* pictureWidth * 4); // we flip it

	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during end of write");

	png_write_end(png_ptr, NULL);

	/* cleanup heap allocation */
	delete[] row_pointers;

	fclose(fp);
}

void LoadBMP(char *name)
{
	FILE *in;
	in = fopen(name, "rb");
	fseek(in, 0, SEEK_END);
	int FileSize = ftell(in);
	fseek(in, 0, SEEK_SET);
	fread(bmpBuffer2, FileSize, 1, in ) ;
	fclose (in);

	pictureWidth  = bmpBuffer2[0x12] +  (bmpBuffer2[0x13]<<8) + (bmpBuffer2[0x14]<<16) + (bmpBuffer2[0x15]<<24);
	pictureHeight = bmpBuffer2[0x16] +  (bmpBuffer2[0x17]<<8) + (bmpBuffer2[0x18]<<16) + (bmpBuffer2[0x19]<<24);

	for (int y = 0; y < pictureHeight; ++y)
	for (int x = 0; x < pictureWidth; ++x)
	{
		int y2 = pictureHeight - y - 1;
		int b = bmpBuffer2[(x + y2 * pictureWidth) * 4 + 0x36 + 0+1+16];
		int g = bmpBuffer2[(x + y2 * pictureWidth) * 4 + 0x36 + 1+1+16];
		int r = bmpBuffer2[(x + y2 * pictureWidth) * 4 + 0x36 + 2+1+16];
		int a = bmpBuffer2[(x + y2 * pictureWidth) * 4 + 0x36 + 3+1+16];
		pictureS[x + y * pictureWidth] = r + (g << 8) + (b << 16);
	}
}

std::vector<Plus4Bitmap> LoadPSD(const char* psd, unsigned int transparentColor) {
	std::vector<Plus4Bitmap> ret;

	PSDImage* img = new PSDImage();
	if (!img->Load((char*)psd)) // load image file and store image data for all layers
	{
		printf("Error loading %s\n", psd);
		return std::vector<Plus4Bitmap>();
	}

	int imageWidth = img->header.Columns;
	int imageHeight = img->header.Rows;

	for (int i = 0; i < img->NumLayers(); ++i) // iterate over all layers (if desired)
	{
		// dimensions of this layer (outer boundaries of image content)
		int width;
		int height;
		// upper-left coordinate of bounding box, use this to place the layer accurately within the image
		int offset_x;
		int offset_y;
		ADWORD* imagedata; // pixel array in ARGB format, need not be allocated (will point to stored image data)
		char name[33] = { 0 };
		memcpy(name, img->GetLayer(i)->name, 32);

		imagedata = img->GetLayerData(i, width, height, offset_x, offset_y); // get pointer to layer image, and info

		/*
		if (imagedata == NULL)
			printf("Layer %i: FAILED (%s)\n", i, name);
		else
			printf("Layer %i: \"%s\", width:%d, height:%d, x:%d, y:%d%s\n", i, name, width, height, offset_x, offset_y,
				img->LayerHidden(i) ? ", HIDDEN" : "");
		*/

		//if (img->LayerHidden(i))
		//    printf("layer %i (%s) is hidden, but can be used anyway", i, name);
		if (width > 0 && height > 0)
		{
			int width2 = imageWidth;
			int height2 = imageHeight;

			std::vector<unsigned char> data(width2 * height2);
			std::fill(data.begin(), data.end(), transparentColor);
			int offs = 0;
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					char tmp, * pb = (char*)&imagedata[offs];
					// swap RB
					tmp = pb[0];
					pb[0] = pb[2];
					pb[2] = tmp;
					unsigned int a = imagedata[offs];
					int x2 = x + offset_x;
					int y2 = y + offset_y;
					if ((unsigned int)x2 < width2 && (unsigned int)y2 < height2) {
						if (((a >> 24) & 255) > 128)
							data[x2 + y2 * width2] = getColor(a);
						else
							data[x2 + y2 * width2] = transparentColor;
					}
					offs++;
				}
			}

			Plus4Bitmap d;
			d.width = imageWidth;
			d.height = imageHeight;
			d.name = name;
			d.pixels = data;
			ret.push_back(d);
		}
	}
	return ret;
}

std::vector<C64Bitmap> LoadPSDC64(const char* psd, unsigned int transparentColor) {
	std::vector<C64Bitmap> ret;

	PSDImage* img = new PSDImage();
	if (!img->Load((char*)psd)) // load image file and store image data for all layers
	{
		printf("Error loading %s\n", psd);
		return std::vector<C64Bitmap>();
	}

	int imageWidth = img->header.Columns;
	int imageHeight = img->header.Rows;

	for (int i = 0; i < img->NumLayers(); ++i) // iterate over all layers (if desired)
	{
		// dimensions of this layer (outer boundaries of image content)
		int width;
		int height;
		// upper-left coordinate of bounding box, use this to place the layer accurately within the image
		int offset_x;
		int offset_y;
		ADWORD* imagedata; // pixel array in ARGB format, need not be allocated (will point to stored image data)
		char name[33] = { 0 };
		memcpy(name, img->GetLayer(i)->name, 32);

		imagedata = img->GetLayerData(i, width, height, offset_x, offset_y); // get pointer to layer image, and info

		/*
		if (imagedata == NULL)
			printf("Layer %i: FAILED (%s)\n", i, name);
		else
			printf("Layer %i: \"%s\", width:%d, height:%d, x:%d, y:%d%s\n", i, name, width, height, offset_x, offset_y,
				img->LayerHidden(i) ? ", HIDDEN" : "");
		*/

		//if (img->LayerHidden(i))
		//    printf("layer %i (%s) is hidden, but can be used anyway", i, name);
		if (width > 0 && height > 0)
		{
			int width2 = imageWidth;
			int height2 = imageHeight;

			std::vector<unsigned char> data(width2 * height2);
			std::fill(data.begin(), data.end(), transparentColor);
			int offs = 0;
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					char tmp, * pb = (char*)&imagedata[offs];
					// swap RB
					tmp = pb[0];
					pb[0] = pb[2];
					pb[2] = tmp;
					unsigned int a = imagedata[offs];
					int x2 = x + offset_x;
					int y2 = y + offset_y;
					if ((unsigned int)x2 < width2 && (unsigned int)y2 < height2) {
						if (((a >> 24) & 255) > 128)
							data[x2 + y2 * width2] = getColorC64(a);
						else
							data[x2 + y2 * width2] = transparentColor;
					}
					offs++;
				}
			}

			C64Bitmap d;
			d.width = imageWidth;
			d.height = imageHeight;
			d.name = name;
			d.pixels = data;
			ret.push_back(d);
		}
	}
	return ret;
}

/*
std::vector<Plus4Bitmap> LoadTIFF(const char* tifName, unsigned int transparentColor) {
	TIFF* tif = TIFFOpen(tifName, "r");
	if (tif) {
	}
	return std::vector<Plus4Bitmap>();
}
*/