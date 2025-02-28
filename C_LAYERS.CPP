#include "C_Layers.hpp"
#include <string.h>
#include "blit.hpp"
#include <math.h>

void CLayer::init(uint32_t *data, int _width, int _height) {
  if (image != NULL) delete[] image;
  width = _width;
  height = _height;
  image = (void*)new uint32_t[width*height<1?1:width*height];
  if (data!=NULL)
    memcpy(image,data,width*height*4);
  else {
    uint32_t *d = (uint32_t*)image;
    for (int i = 0; i < _width*_height; i++) d[i] = 0xff000000;
  }
  xpos = 0;
  ypos = 0;
  mulX = 1;
  mulY = 1;
  opacity = 1.f;
  layerType = LAYERTYPE_RASTER;
  layerFunction = LAYERFUNCTION_ALPHA;
}

void CLayer::blitTo32Bit(uint32_t *dest,uint32_t destWidth,uint32_t destHeight, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int src_x0, int src_y0, int src_x1, int src_y1) {
  uint32_t *im = (uint32_t*)image;
  int o = opacity*255;
  float zoom2 = 1.f/pow(2.f,zoom);
  src_x0 *= zoom2;
  src_y0 *= zoom2;
  src_x1 *= zoom2;
  src_y1 *= zoom2;
  src_x0 -= (int)floor(xpos);
  src_x1 -= (int)floor(xpos);
  src_y0 -= (int)floor(ypos);
  src_y1 -= (int)floor(ypos);
  for (int y = dst_y0; y < dst_y1; y++) {
    if (y < 0|| y >= destHeight) continue;
    int sy2 = (src_y1-src_y0)*(y-dst_y0)/(dst_y1-dst_y0)+src_y0;
    sy2 -= height/2;
    for (int x = dst_x0; x < dst_x1; x++) {
      if (x < 0 || x >= destWidth) continue;
      int sx2 = (src_x1-src_x0)*(x-dst_x0)/(dst_x1-dst_x0)+src_x0;
      sx2 -= width/2;
      int sx = (sx2 * matrix[0] + sy2 * matrix[1])/mulX; 
      int sy = (sx2 * matrix[2] + sy2 * matrix[3])/mulY; 
      sx += width/2;
      sy += height/2;
      if (sx < 0 || sx >= width) continue;
      if (sy < 0 || sy >= height) continue;
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
          int dstR = dstRGBA & 255;
          int dstG = (dstRGBA>>8) & 255;
          int dstB = (dstRGBA>>16) & 255;
          int dstA = (dstRGBA>>24) & 255;
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
  xpos = -x0;
  ypos = -y0;
}

void CLayer::newSize(int w, int h) {
  uint32_t *newCanvas = new uint32_t[w*h>0?w*h:1];
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      uint32_t col = 0xff000000;
      if (x < width && y < height) 
        col = ((uint32_t*)image)[x+y*width];
      newCanvas[x+y*w] = col;
    }
  }
  delete[] image;
  image = (void*)newCanvas;
  width=w;
  height=h;
}


void CLayerStack::blitTo32Bit(uint32_t *dest,uint32_t destWidth,uint32_t destHeight, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int src_x0, int src_y0, int src_x1, int src_y1) {
  for (int y = 0; y < destHeight; y++) {
    for (int x = 0; x < destWidth; x++) {
      uint32_t col = ((x+y) & 7) ? 0xff000000:0xff808080;
      dest[x+y*destWidth]=col;
    }
  }
  for (int i = 0; i < layers.size(); i++) {
    CLayer *l = layers[i];
    if (l->visible)
      l->blitTo32Bit(dest,destWidth,destHeight,dst_x0,dst_y0,dst_x1,dst_y1,src_x0,src_y0,src_x1,src_y1);
  }
}

void CLayerStack::reCanvas(int x0, int y0, int x1, int y1) {
  for (int i = 0; i < layers.size(); i++) {
    CLayer *l = layers[i];
    l->reCanvas(x0,y0,x1,y1);
  }
  if (image != NULL) delete[] image;
  width = x1 - x0;
  height = y1 - y0;
  image = (uint32_t*)new uint32_t[width * height];
  blitTo32Bit(image, width, height, 0,0,width,height, 0,0,width,height);
}

void CLayerStack::drawAtXY(int x, int y) {
  if (image == NULL) return;
  float zoom2 = pow(2.f,zoom);
  int width2 = width;
  int height2 = height;
  for (int y2 = 0; y2 < height2*zoom2; y2++) {
    int y3 = y2 + ypos; if (y3 < 0 || y3 >= frameBufferHeight) continue;
    int y4 = (int)floor((float)y2/zoom2); if (y4 < 0 || y4 >= height) continue;
    for (int x2 = 0; x2 < width*zoom2; x2++) {
      int x3 = x2 + xpos; if (x3 < 0 || x3 >= frameBufferWidth) continue;
      int x4 = (int)floor((float)x2/zoom2); if (x4 < 0 || x4 >= width) continue;
      frameBuffer[x3+y3*frameBufferWidth] = image[x4+y4*width];
    }
  }
}

void CLayerStack::reBlit() {
  blitTo32Bit(image, width, height, 0,0,width,height, 0,0,width,height);
  if (gridOn && gridWidth > 0 && gridHeight > 0) {
    for (int y = 0; y < height; y++) {
      bool gridY = (y % gridHeight)==gridHeight-1;
      for (int x = 0; x < width; x++) {
        bool gridX = (x % gridWidth)==gridWidth-1;
        if (gridX || gridY) {
          const unsigned int rgba = image[x+y*width];
          int r = rgba & 255;
          r += 64;
          if (r > 255) r = 255;
          image[x+y*width] |= (rgba & 0xffffff00)|r;
        }
      }
    }
  }
}

void CLayerStack::doublePixels(bool _doublePixels) {
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL && layers[i]->selected) layers[i]->mulX=_doublePixels?2:1;
  }
}

void CLayerStack::newStack(int _width, int _height) {
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL) delete layers[i];
  }
  layers.clear();
  width = _width;
  height = _height;
  if (image != NULL) delete[] image;
  image = (uint32_t*)new uint32_t[width*height<=0?1:width*height];
}

void CLayerStack::moveStack(int moveX, int moveY) {
  xpos += moveX;
  ypos += moveY;
}

void CLayerStack::moveLayers(int moveX, int moveY) {
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL && layers[i]->selected) {
      float zoom2 = pow(2.f,layers[i]->zoom);
      float zoom3 = pow(2.f,zoom);
      layers[i]->xpos += moveX/zoom2/zoom3;
      layers[i]->ypos += moveY/zoom2/zoom3;
    }
  }
}

void CLayerStack::finishMoveLayers() {
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL) {
      layers[i]->xpos = floor(layers[i]->xpos);
      layers[i]->ypos = floor(layers[i]->ypos);
    }
  }
}

void CLayerStack::rotateLayers(float angle) {
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL && layers[i]->selected) {
      float mat1[4]; memcpy(mat1,layers[i]->matrix,sizeof(mat1));
      float mat2[4];
      mat2[0]=cos(angle/360.f*2.f*3.1415927);
      mat2[1]=sin(angle/360.f*2.f*3.1415927);
      mat2[2]=-sin(angle/360.f*2.f*3.1415927);
      mat2[3]=cos(angle/360.f*2.f*3.1415927);
      for (int i0=0;i0<2;i0++) {
        for (int i1=0;i1<2;i1++) {
          float a = 0.f;
          for (int i2=0;i2<2;i2++) {
            a+=mat1[i2+i1*2]*mat2[i0+i2*2];
          }
          if (a > 0.5) a = 1.f; else
          if (a < -0.5) a = -1.f; else
          a = 0.f;
          layers[i]->matrix[i0+i1*2]=a;
        }
      }
    }
  }
}

void CLayerStack::mirrorLayers(float x, float y) {
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL && layers[i]->selected) {
      layers[i]->matrix[0] *= x;
      layers[i]->matrix[1] *= y;
      layers[i]->matrix[2] *= x;
      layers[i]->matrix[3] *= y;
    }
  }
}

void CLayerStack::addZoom(float zoomAdd, int mouseX, int mouseY) {
  float zoom3 = pow(2.f,zoom);
  for(int i = 0; i < layers.size(); i++) {
    if (layers[i] != NULL && layers[i]->selected) {
      float zoom1 = 1.f/pow(2.f,layers[i]->zoom);
      layers[i]->zoom += zoomAdd;
      if (layers[i]->zoom>8) layers[i]->zoom=8;
      if (layers[i]->zoom<-8) layers[i]->zoom=-8;
      float zoom2 = 1.f/pow(2.f,layers[i]->zoom);
      layers[i]->xpos -= (mouseX-xpos)*zoom1/zoom3;
      layers[i]->ypos -= (mouseY-ypos)*zoom1/zoom3;
      layers[i]->xpos += (mouseX-xpos)*zoom2/zoom3;
      layers[i]->ypos += (mouseY-ypos)*zoom2/zoom3;
    }
  }
}

void CLayerStack::addStackZoom(float zoomAdd, int mouseX, int mouseY) {
  float zoom1 = pow(2.f,zoom);
  zoom += zoomAdd;
  if (zoom>8) zoom=8;
  if (zoom<-8) zoom=-8;
  float zoom2 = pow(2.f,zoom);
  float xp=xpos;
  float yp=ypos;
  xpos += (mouseX-xp)*zoom1;
  ypos += (mouseY-yp)*zoom1;
  xpos -= (mouseX-xp)*zoom2;
  ypos -= (mouseY-yp)*zoom2;
}

void CLayerStack::enableGrid(bool _gridOn, int _gridWidth, int _gridHeight) {
  gridOn = _gridOn;
  gridWidth = _gridWidth;
  gridHeight = _gridHeight;
}

uint32_t CLayerStack::getPixel(int mouseX, int mouseY) {
  mouseX -= xpos;
  mouseY -= ypos;
  mouseX /= pow(2.f,zoom);
  mouseY /= pow(2.f,zoom);
  uint32_t k = 0x00000000;
  blitTo32Bit(&k,1,1, 0,0,1,1, mouseX,mouseY,mouseX+1,mouseY+1);
  return k;
}


void CLayerStack::mousePosToLayerPos(float mouseX, float mouseY, CLayer *l, int &retX, int &retY) {
  float zoomS = pow(2.f,zoom);
  float xposS = xpos;
  float yposS = ypos;
  float x0 = (mouseX-xposS)/zoomS;
  float y0 = (mouseY-yposS)/zoomS;
  float zoomL = 1.f/pow(2.f,l->zoom);
  x0*=zoomL;
  y0*=zoomL;
  x0-=l->xpos;
  y0-=l->ypos;
  x0-=l->width/2;
  y0-=l->height/2;
  float x1 = (x0 * l->matrix[0] + y0 * l->matrix[1])/l->mulX; 
  float y1 = (x0 * l->matrix[2] + y0 * l->matrix[3])/l->mulY; 
  x0 = x1;
  y0 = y1;
  x0 += l->width/2;
  y0 += l->height/2;
  retX = (int)floor(x0);
  retY = (int)floor(y0);
}

void CLayerStack::layerPosToMousePos(float layerX, float layerY, CLayer *l, float &retX, float &retY) {
  layerX -= l->width/2;
  layerY -= l->height/2;
  layerX *= l->mulX;
  layerY *= l->mulY;
  float x0 = (layerX * l->matrix[0] + layerY * l->matrix[2]); 
  float y0 = (layerX * l->matrix[1] + layerY * l->matrix[3]); 
  x0 += l->width/2;
  y0 += l->height/2;
  x0 += l->xpos;
  y0 += l->ypos;
  float zoomL = 1.f/pow(2.f,l->zoom);
  x0 /= zoomL;
  y0 /= zoomL;
  float zoomS = pow(2.f,zoom);
  x0 *= zoomS;
  y0 *= zoomS;
  float xposS = xpos;
  float yposS = ypos;
  x0 += xposS;
  y0 += yposS;
  retX = x0;
  retY = y0;  
}
