#include "pngload.hpp"
#include "png\png.h"
#include <stdlib.h>
#include <stdarg.h>

void savePaletteHeaderFile(const char *headerFileName, unsigned int *palette) {
  int i;
  FILE *out = fopen(headerFileName,"w");
  if (out != NULL) {
    for (i = 0; i < 256; ++i) {
      fprintf(out,"0x%08x,\n",palette[i]);
    }
    fclose(out);
  }
}

bool loadPNG(ImageLayer *layer, const char *fileName) {
  int       i,x,y,width,height;
  unsigned int c;
  FILE      *fp;            // File pointer
  int		channels;		// Number of color channels
  png_structp	pp;			// PNG read pointer
  png_infop	info;			// PNG info pointers
  png_bytep	*rows;			// PNG row pointers
  unsigned char *array;

  layer->width = 0;
  layer->height = 0;

  // Open the PNG file...
  if ((fp = fopen(fileName, "rb")) == NULL) {
    return false;
  }

  // Setup the PNG data structures...
  pp   = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct(pp);

  if (setjmp(pp->jmpbuf))
  {
	    return false;
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

  width = (int)(info->width);
  height = (int)(info->height);

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

  array = new unsigned char[width * height * 4];

  // Allocate pointers...
  rows = new png_bytep[height];

  for (i = 0; i < height; i ++)
    rows[i] = (png_bytep)(array + i * width * channels); // we flip it

  // Read the image, handling interlacing as needed...
  for (i = png_set_interlace_handling(pp); i > 0; i --)
    png_read_rows(pp, rows, NULL, height);

#ifdef WIN32
  // Some Windows graphics drivers don't honor transparency when RGB == white
  if (channels == 4)
  {
    // Convert RGB to 0 when alpha == 0...
    unsigned char *ptr = (unsigned char *)array;
    for (i = width * height; i > 0; i --, ptr += 4) {
      if (!ptr[3]) ptr[0] = ptr[1] = ptr[2] = 0;
    }
  }
#endif // WIN32

  if (channels == 3)
  {
    unsigned char *array2 = new unsigned char[width * height * 4];
    for (i = width * height - 1; i >= 0; i--)
	  {
		  array2[i*4+0] = array[i*3+0];
		  array2[i*4+1] = array[i*3+1];
		  array2[i*4+2] = array[i*3+2];
		  array2[i*4+3] = 255;
	  }
    //for (i = 0; i < width * height * 4; i++)
    //  array[i] = array2[i];
    memcpy(array, array2, width * height * 4);
    delete[] array2;
  }

  // Free memory and return...
  delete[] rows;

  png_read_end(pp, info);
  png_destroy_read_struct(&pp, &info, NULL);

  fclose(fp);

  layer->width = width;
  layer->height = height;
  layer->x0 = 0;
  layer->y0 = 0;
  layer->x1 = width;
  layer->y1 = height;
  layer->data = new unsigned int[width*height];
  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      c = *(unsigned int*)(&array[(x+y*width)*4]);
      if ((unsigned int)x < layer->width && (unsigned int)y < layer->height) layer->data[x+y*layer->width] = c;
    }
  }
  delete[] array;
  return true;
}

void pngabort(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

bool savePNG(ImageLayer *layer, const char* fileName)
{
  png_structp png_ptr;
  png_infop info_ptr;
  int width, height;
  unsigned int *data;
  png_bytep * row_pointers;
  unsigned char *array;
  int i,flipped = 0;
  
  FILE *fp = fopen(fileName, "wb");
  	if (!fp) {
    return false;
  }
      
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  	if (!png_ptr) {
    	fclose(fp);
    return false;
  	}

  	info_ptr = png_create_info_struct(png_ptr);
  	if (!info_ptr) {
    	fclose(fp);
    return false;
  	}
  
  	if (setjmp(png_jmpbuf(png_ptr))) {
    	fclose(fp);
    return false;
  	}

  	png_init_io(png_ptr, fp);

  if (setjmp(png_jmpbuf(png_ptr))) {
    	fclose(fp);
    return false;
  }

  width   = layer->width;
  height  = layer->height;
  data    = layer->data;

  png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  if (setjmp(png_jmpbuf(png_ptr))) {
    	fclose(fp);
    return false;
  	}

  array = new unsigned char[(width * height + 1) * 4];
  for (i = width * height; i >= 0; --i)
  	{
    array[i * 4 + 0] = ((unsigned char*)data)[i * 4 + (flipped ? 2 : 0)];
    array[i * 4 + 1] = ((unsigned char*)data)[i * 4 + 1];
    array[i * 4 + 2] = ((unsigned char*)data)[i * 4 + (flipped ? 0 : 2)];
    array[i * 4 + 3] = ((unsigned char*)data)[i * 4 + 3];
	  }

  	// Allocate pointers...
  row_pointers = new png_bytep[height];
  
  for (i = 0; i < height; i++)
    row_pointers[i] = (png_bytep)(array + (i)* width * 4); // we flip it
  
  	png_write_image(png_ptr, row_pointers);

  if (setjmp(png_jmpbuf(png_ptr))) {
    delete[] row_pointers;
    delete[] array;
    return false;
  }

  	png_write_end(png_ptr, NULL);

  delete[] row_pointers;
  delete[] array;
  fclose(fp);
  return true;
}