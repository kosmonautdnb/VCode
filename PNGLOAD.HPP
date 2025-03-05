#ifndef __PNGLOAD_HPP__
#define __PNGLOAD_HPP__

typedef struct ImageLayer {
        int width, height;
        unsigned int *data;
        int x0,y0,x1,y1;
} ImageLayer;

typedef struct GreyImageLayer {
        int width, height;
        unsigned char *data;
        short *derivativesX;
        short *derivativesY;
        int x0,y0,x1,y1;
} GreyImageLayer;

bool loadPNG(struct ImageLayer *layer, const char *fileName);
bool savePNG(struct ImageLayer *layer, const char *fileName);
bool loadTGA(struct ImageLayer *layer, const char *fileName);
bool saveTGA(struct ImageLayer *layer, const char *fileName);

#endif //__PNGLOAD_HPP__