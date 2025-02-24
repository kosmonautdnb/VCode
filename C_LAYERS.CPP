#include "C_Layers.hpp"
#include <string.h>
#include "blit.hpp"
#include <math.h>

void CLayer::init(uint32_t *data, int _width, int _height) {
  if (image != NULL) free(image);
  width = _width;
  height = _height;
  image = (void*)malloc(width*height*4);
  memcpy(image,data,width*height*4);
  xpos = 0;  
  ypos = 0;  
  mulX = 1;
  mulY = 1;
}

void CLayer::blitTo32Bit(uint32_t *dest,uint32_t destWidth,uint32_t destHeight, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int src_x0, int src_y0, int src_x1, int src_y1) {
  uint32_t *im = (uint32_t*)image;
  int o = opacity*255; 
  src_x0 -= (int)floor(xpos);
  src_x1 -= (int)floor(xpos);
  src_y0 -= (int)floor(ypos);
  src_y1 -= (int)floor(ypos);
  dst_x1 = (dst_x1-dst_x0)*mulX+dst_x0;
  dst_y1 = (dst_y1-dst_y0)*mulY+dst_y0;
  for (int y = dst_y0; y < dst_y1; y++) {
    int sy = (src_y1-src_y0)*(y-dst_y0)/(dst_y1-dst_y0)+src_y0;
    if (y < 0 || sy < 0 || y >= destHeight || sy >= height) continue;
    for (int x = dst_x0; x < dst_x1; x++) {
      int sx = (src_x1-src_x0)*(x-dst_x0)/(dst_x1-dst_x0)+src_x0;
      if (x < 0 || sx < 0 || x >= destWidth || sx >= width) continue;
      uint32_t srcRGBA = im[sx+sy*width];
      int srcR = srcRGBA & 255;
      int srcG = (srcRGBA>>8) & 255;
      int srcB = (srcRGBA>>16) & 255;
      int srcA = (srcRGBA>>24) & 255;
      int r,g,b,a;
      switch(layerFunction) {
        case LAYERFUNCTION_NONE: {
          r = srcR;
          g = srcG;
          b = srcB;
          a = srcA;
        } break;
        case LAYERFUNCTION_ALPHA: {
          uint32_t dstRGBA = dest[x+y*destWidth];
          int dstR = srcRGBA & 255;
          int dstG = (srcRGBA>>8) & 255;
          int dstB = (srcRGBA>>16) & 255;
          int dstA = (srcRGBA>>24) & 255;
          int o2 = srcA*o;
          r = (((srcR - dstR)*o2)>>16)+dstR;
          g = (((srcG - dstG)*o2)>>16)+dstG;
          b = (((srcB - dstB)*o2)>>16)+dstB;
          a = (((srcA - dstA)*o2)>>16)+dstA;
          if (r > 255) r = 255;
          if (g > 255) g = 255;
          if (b > 255) b = 255;
          if (a > 255) a = 255;
          if (r < 0) r = 0;
          if (g < 0) g = 0;
          if (b < 0) b = 0;
          if (a < 0) a = 0;
        } break;
      }
      dest[x+y*destWidth] = r|(g<<8)|(b<<16)|(a<<24);
    }
  }
}

void CLayer::reCanvas(int x0, int y0, int x1, int y1) {
  int dWidth=x1-x0;
  int dHeight=y1-y0;
  if (image == NULL) {
    image = (void*)malloc(dWidth*dHeight*4);
    uint32_t *im = (uint32_t*)image;
    for (int i = 0; i < dWidth*dHeight; i++) im[i] = 0xff000000;
  } else {
    uint32_t *m = (uint32_t*)malloc(width*height*4);
    memcpy(m, image, width*height*4);
    free(image);
    image = malloc(dWidth*dHeight*4);
    uint32_t *im = (uint32_t*)image;
    for (int i = 0; i < dWidth*dHeight; i++) im[i] = 0xff000000;
    for (int y = 0; y < height; y++) {
      if (y >= dHeight) continue;;
      for (int x = 0; x < width; x++) {
        if (x >= dWidth) continue;
        im[x+y*dWidth]=m[x+y*width];
      }
    }
    free(m);
  }
  width = dWidth;
  height = dHeight;
  xpos = x0;
  ypos = y0;
}

void CLayerStack::blitTo32Bit(uint32_t *dest,uint32_t destWidth,uint32_t destHeight, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int src_x0, int src_y0, int src_x1, int src_y1) {
  for (int y = 0; y < destHeight; y++) {
    for (int x = 0; x < destWidth; x++) {
      uint32_t col = ((x+y) & 7) ? 0xff000000:0xff808080;
      dest[x+y*destWidth]=col;
    }
  }
  float zoom2 = pow(2.f,zoom);
  src_x0 *= zoom2;
  src_y0 *= zoom2;
  src_x1 *= zoom2;
  src_y1 *= zoom2;
  for (int i = 0; i < layers.size(); i++) {
    CLayer *l = layers[i];
    l->blitTo32Bit(dest,destWidth,destHeight,dst_x0,dst_y0,dst_x1,dst_y1,src_x0,src_y0,src_x1,src_y1);
  }
}

void CLayerStack::reCanvas(int x0, int y0, int x1, int y1) {
  for (int i = 0; i < layers.size(); i++) {
    CLayer *l = layers[i];
    l->reCanvas(x0,y0,x1,y1);
  }
  if (image != NULL) free(image);
  width = x1 - x0;
  height = y1 - y0;
  image = (uint32_t*)malloc(width * height * 4);
  blitTo32Bit(image, width, height, 0,0,width,height, 0,0,width,height);
}

void CLayerStack::drawAtXY(int x, int y) {
  if (image == NULL) return;
  int width2 = width;
  int height2 = height;
  for (int y2 = 0; y2 < height2; y2++) {
    int y3 = y2 + ypos; if (y3 < 0 || y3 >= frameBufferHeight) continue;
    int y4 = (int)(y2); if (y4 < 0 || y4 >= height) continue;
    for (int x2 = 0; x2 < width; x2++) {
      int x3 = x2 + xpos; if (x3 < 0 || x3 >= frameBufferWidth) continue;
      int x4 = (int)(x2); if (x4 < 0 || x4 >= width) continue;
      frameBuffer[x3+y3*frameBufferWidth] = image[x4+y4*width];
    }
  }
}

void CLayerStack::reBlit() {
  blitTo32Bit(image, width, height, 0,0,width,height, 0,0,width,height);
}

void CLayerStack::doublePixels(bool _doublePixels) {
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL) layers[i]->mulX=_doublePixels?2:1;
  }
}

void CLayerStack::newStack(int _width, int _height) {
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL) delete layers[i];
  }
  layers.clear();
  layers.push_back(new CLayer());
  width = _width;
  height = _height;
  if (image != NULL) free(image);
  image = (uint32_t*)malloc(width*height*4);
}

void CLayerStack::moveLayers(int moveX, int moveY) {
  float zoom2 = pow(2.f,zoom);
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL) {
      layers[i]->xpos += moveX*zoom2;
      layers[i]->ypos += moveY*zoom2;
    }
  }
}
