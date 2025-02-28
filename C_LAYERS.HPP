#ifndef __C_LAYERS_HPP__
#define __C_LAYERS_HPP__

#include "types.hpp"
#include "array.hpp"
#include "string.hpp"
#include <stdlib.h>

class CLayer {
public:
  String name;
  enum LayerFunction {
    LAYERFUNCTION_NONE = 0,
    LAYERFUNCTION_ALPHA = 1
  };
  enum LayerType {
    LAYERTYPE_NONE = 0,
    LAYERTYPE_RASTER = 1
  };
  void *image;
  float xpos;
  float ypos;
  uint32_t width;
  uint32_t height;
  int mulX;
  int mulY;
  uint32_t layerType;
  uint32_t layerFunction;
  float opacity;
  bool selected;
  bool visible;
  float matrix[2*2];   
  float zoom;

  CLayer() {
    image = NULL;
    matrix[0] = 1;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 1;
    xpos = 0;
    ypos = 0;
    width = 0;
    height = 0;
    mulX = 1;
    mulY = 1;
    zoom = 0.f;
    layerType = LAYERTYPE_NONE;
    layerFunction = LAYERFUNCTION_NONE;
    opacity = 0.f;
    selected = true;
    visible = true;
    static int counter = 0; counter++;
    name = String("Layer ") + counter;
  }

  virtual ~CLayer() {
    if (image != NULL) {delete[] image; image = NULL;}
  }

  void init(uint32_t *data, int _width, int _height);
  void blitTo32Bit(uint32_t *dest,uint32_t destWidth,uint32_t destHeight, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int src_x0, int src_y0, int src_x1, int src_y1);
  void reCanvas(int x0, int y0, int x1, int y1);
  void newSize(int w, int h);
};
                                              
class CLayerStack {
public:
  Array<CLayer*> layers;
  uint32_t *image;
  uint32_t width;
  uint32_t height;
  float xpos;
  float ypos;
  bool gridOn;
  int gridWidth;
  int gridHeight;
  float zoom;
  

  CLayerStack() {
    image = NULL;
    width = 0;
    height = 0;
    xpos = 0;
    ypos = 0;
    zoom = 0.f;
    gridOn = false;
    gridWidth = 8;
    gridHeight = 8;
  }

  virtual ~CLayerStack() {
    if (image != NULL) {delete[] image; image = NULL;}
    for (int i = 0; i < layers.size(); i++) 
      delete layers[i];
  }

  void reset() {
    xpos = 0;
    ypos = 0;
    gridOn = false;
    gridWidth = 8;
    gridHeight = 8;
  }
  void blitTo32Bit(uint32_t *dest,uint32_t destWidth,uint32_t destHeight, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int src_x0, int src_y0, int src_x1, int src_y1);
  void reCanvas(int x0, int y0, int x1, int y1);
  void drawAtXY(int x, int y);

  void reBlit();
  void doublePixels(bool _doublePixels);
  void newStack(int _width, int _height);
  void moveStack(int moveX, int moveY);
  void moveLayers(int moveX, int moveY);
  void finishMoveLayers();
  void rotateLayers(float angle);
  void mirrorLayers(float x, float y);
  void addZoom(float zoomAdd, int mouseX, int mouseY);
  void addStackZoom(float zoomAdd, int mouseX, int mouseY);
  void enableGrid(bool _gridOn, int _gridX, int _gridY);
  uint32_t getPixel(int mouseX, int mouseY);
  void mousePosToLayerPos(float mouseX, float mouseY, CLayer *l, int &retX, int &retY);
  void layerPosToMousePos(float layerX, float layerY, CLayer *l, float &retX, float &retY);

};

#endif //__C_LAYERS_HPP__