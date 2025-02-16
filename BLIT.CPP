#include "types.hpp"
#include "blit.hpp"
#include "charset.hpp"
#include "string.hpp"
#include "array.hpp"
#include "constant.hpp"
#include "vga.hpp"
#include <string.h>

unsigned int *frameBuffer = NULL;
unsigned int *capturedFrameBuffer = NULL;
unsigned int frameBufferWidth;
unsigned int frameBufferHeight;

void captureFrameBuffer() {
  memcpy(capturedFrameBuffer, frameBuffer, frameBufferHeight*frameBufferWidth*sizeof(unsigned int));
}

void drawCapturedFrameBuffer() {
  memcpy(frameBuffer, capturedFrameBuffer, frameBufferHeight*frameBufferWidth*sizeof(unsigned int));
}

Array<Rect> clipStack;
Rect clipRect;

void setClipRect(int x0, int y0, int x1, int y1) {
  clipRect.x0 = x0;
  clipRect.y0 = y0;
  clipRect.x1 = x1;
  clipRect.y1 = y1;
}

void setClipRectClipped(int x0, int y0, int x1, int y1) {
  if (x0 < clipRect.x0) x0 = clipRect.x0;
  if (y0 < clipRect.y0) y0 = clipRect.y0;
  if (x1 > clipRect.x1) x1 = clipRect.x1;
  if (y1 > clipRect.y1) y1 = clipRect.y1;
  setClipRect(x0,y0,x1,y1);
}

void setClipRect(const Rect &v) {
  setClipRect(v.x0, v.y0, v.x1, v.y1);
}

bool mouseOver(const Rect &v) {
  return (mouseX>=v.x0)&&(mouseY>=v.y0)&&(mouseX<v.x1)&&(mouseY<v.y1);
}

void pushRect(int x0, int y0, int x1, int y1) {
  clipStack.push_back(clipRect);
  setClipRectClipped(x0,y0,x1,y1);
}

void pushRect(const Point &p0, const Point &p1) {
  clipStack.push_back(clipRect);
  setClipRectClipped(p0.x,p0.y,p1.x,p1.y);
}

void pushRect(const Point &p0, const Size &size) {
  clipStack.push_back(clipRect);
  setClipRectClipped(p0.x,p0.y,p0.x+size.width,p0.y+size.height);
}

void popRect() {
  setClipRect(clipStack.back());
  clipStack.pop_back();
}

void drawCharacter(int &x, int &y, char c, unsigned int color) {
  Letter *l = getLetter(c);
  if (l == NULL) return;
  if (y < clipRect.y1 && y + CHAR_HEIGHT > clipRect.y0) {
    for (int y2 = 0; y2 < l->height; y2++) {
      for (int x2 = 0; x2 < l->width; x2++) {
        if (l->bitmap[x2+y2*l->width]!='0')
          setPixel(x+x2+l->preAddX,y+y2+l->preAddY,color);
      }
    }
  }
  x += l->preAddX+l->width+1+l->postAddX;
}

void drawString(int x, int y, const String &str, unsigned int color) {
  for (int l = 0; l < str.length(); l++) {
    drawCharacter(x,y,str[l],color);
  }
}

void setPixel(int x, int y, unsigned int color) {
  if ((unsigned int)x < frameBufferWidth && (unsigned int)y < frameBufferHeight) {
    if (x >= clipRect.x0 && y >= clipRect.y0 && x < clipRect.x1 && y < clipRect.y1)
      frameBuffer[x+y*frameBufferWidth] = color;
  }
}

void drawRectangle(int x0, int y0, int x1, int y1, unsigned int color) {
  for (int y = y0; y < y1; y++) {
    setPixel(x0,y,color);
    if (x1-1>x0) setPixel(x1-1,y,color);
  }
  for (int x = x0; x < x1; x++) {
    setPixel(x,y0,color);
    if (y1-1>y0) setPixel(x,y1-1,color);
  }
}

void drawBox(int x0, int y0, int x1, int y1, unsigned int color) {
  for (int y = y0; y < y1; y++)
    for (int x = x0; x < x1; x++)
      setPixel(x,y,color);
}

void allocFrameBuffer(unsigned int width, unsigned int height) {
  frameBuffer = new unsigned int[width*height];
  capturedFrameBuffer = new unsigned int[width*height];
  frameBufferWidth = width;
  frameBufferHeight = height;
  memset(frameBuffer,0,width*height*sizeof(unsigned int));
  clipStack.clear();
  setClipRect(0,0,frameBufferWidth,frameBufferHeight);
}

void freeFrameBuffer() {
  delete[] frameBuffer;
  delete[] capturedFrameBuffer;
}

void clearFrameBuffer(unsigned int color) {
  for (int i = 0; i < frameBufferWidth * frameBufferHeight; i++) frameBuffer[i] = color;
}