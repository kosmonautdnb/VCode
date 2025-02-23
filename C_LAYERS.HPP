#ifndef __C_LAYERS_HPP__
#define __C_LAYERS_HPP__

#include "types.hpp"
#include "array.hpp"
#include <stdlib.h>

class CLayer {
public:
  enum LayerFunction {
    LAYERFUNCTION_NONE = 0,
    LAYERFUNCTION_ALPHA = 1
  };
  enum LayerType {
    LAYERTYPE_NONE = 0,
    LAYERTYPE_RASTER = 1
  };
  void *image;
  int xpos;
  int ypos;
  uint32_t width;
  uint32_t height;
  int mulX;
  int mulY;
  uint32_t layerType;
  uint32_t layerFunction;
  float opacity;

  CLayer() {
    image = NULL;
    xpos = 0;
    ypos = 0;
    width = 0;
    height = 0;
    mulX = 1;
    mulY = 1;
    layerType = LAYERTYPE_NONE;
    layerFunction = LAYERFUNCTION_NONE;
    opacity = 0.f;
  }

  virtual ~CLayer() {
    if (image != NULL) {free(image); image = NULL;}
  }

  void blitTo32Bit(uint32_t *dest,uint32_t destWidth,uint32_t destHeight, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int src_x0, int src_y0, int src_x1, int src_y1);
  void reCanvas(int x0, int y0, int x1, int y1);
};

class CLayerStack {
public:
  Array<CLayer*> layers;
  uint32_t *image;
  uint32_t width;
  uint32_t height;

  CLayerStack() {
    image = NULL;
    width = 0;
    height = 0;
  }

  virtual ~CLayerStack() {
    if (image != NULL) {free(image); image = NULL;}
  }

  void blitTo32Bit(uint32_t *dest,uint32_t destWidth,uint32_t destHeight, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int src_x0, int src_y0, int src_x1, int src_y1);
  void reCanvas(int x0, int y0, int x1, int y1);
  void drawAtXY(int x, int y);
};

#endif //__C_LAYERS_HPP__