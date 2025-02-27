#include "types.hpp"
#include "blit.hpp"
#include "charset.hpp"
#include "string.hpp"
#include "array.hpp"
#include "constant.hpp"
#include "colors.hpp"
#include "input.hpp"
#include "util.hpp"
#include <string.h>

unsigned int *frameBuffer = NULL;
unsigned int *fullScreenFrameBuffer = NULL;
unsigned int *lastCapturedFrameBuffer = NULL;
unsigned int frameBufferWidth;
unsigned int frameBufferHeight;

Array<unsigned int*> capturedFrameBuffers;

const char *mouseShape=
  "00000."
  "011110"
  "01110."
  "0110.."
  "010..."
  ".0....";
bool showAppTime = true;

void drawMouse() {
  for (int y = 0; y < 6; y++) {
    for (int x = 0; x < 6; x++) {
      char c = mouseShape[x+y*6];
      if (c == '0') setPixel(mouseX+x,mouseY+y,COLOR_BLACK);
      if (c == '1') setPixel(mouseX+x,mouseY+y,COLOR_WHITE);
    }
  }
  drawString(mouseX+7,mouseY,String(key & 255)+":"+String(key>>8),COLOR_WHITE);
  if (showAppTime) {
    String s = currentToolString+" "+currentTimeString();
    drawStringOutlined(frameBufferWidth-s.pixelWidth(),frameBufferHeight-CHAR_HEIGHT*3/2,s,COLOR_WHITE,COLOR_BLACK);
  }
}


void pushCapturedFrameBuffer() {
  lastCapturedFrameBuffer = new unsigned int[frameBufferHeight*frameBufferWidth];
  memcpy(lastCapturedFrameBuffer, fullScreenFrameBuffer, frameBufferHeight*frameBufferWidth*sizeof(unsigned int));
  capturedFrameBuffers.push_back(lastCapturedFrameBuffer);
}

void popCapturedFrameBuffer() {
  memcpy(frameBuffer,lastCapturedFrameBuffer, frameBufferHeight*frameBufferWidth*sizeof(unsigned int));
  delete[] capturedFrameBuffers.back();
  capturedFrameBuffers.pop_back();
  if (capturedFrameBuffers.empty())
    lastCapturedFrameBuffer = NULL;
  else
  {
    lastCapturedFrameBuffer = capturedFrameBuffers.back();
  }
}

void submitFullScreenFrameBuffer(unsigned int *frameBuffer) {
  memcpy(fullScreenFrameBuffer,frameBuffer,frameBufferWidth*frameBufferHeight*sizeof(unsigned int));
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

void drawFrameBuffer(unsigned int *_frameBuffer) {
  memcpy(frameBuffer,_frameBuffer,sizeof(unsigned int)*frameBufferWidth*frameBufferHeight);
}

void drawChar(int &x, int &y, char c, unsigned int color) {
  Letter *l = getLetter(c);
  if (l == NULL) return;
  if (y < clipRect.y1 && y + CHAR_HEIGHT > clipRect.y0) {
    int x0 = x + l->preAddX;
    int y0 = y + l->preAddY;
    int x1 = x + l->width+l->preAddX;
    int y1 = y + l->height+l->preAddY;
    bool clippedX = x0 < 0 || x0 < clipRect.x0 || x1 > frameBufferWidth || x1 > clipRect.x1;
    bool clippedY = y0 < 0 || y0 < clipRect.y0 || y1 > frameBufferHeight || y1 > clipRect.y1;
    if (clippedX||clippedY) {
      for (int y2 = 0; y2 < l->height; y2++) {
        for (int x2 = 0; x2 < l->width; x2++) {
          if (l->bitmap[x2+y2*l->width]!='0')
            setPixel(x+x2+l->preAddX,y+y2+l->preAddY,color);
        }
      }
    } else {
      for (int y2 = 0; y2 < l->height; y2++) {
        unsigned int *d = frameBuffer + (y0+y2)*frameBufferWidth+x0;
        for (int x2 = 0; x2 < l->width; x2++) {
          if (l->bitmap[x2+y2*l->width]!='0')
            *d = color;
          d++;
        }
      }
    }
  }
  x += l->preAddX+l->width+1+l->postAddX;
}

void drawString(int x, int y, const String &str, unsigned int color) {
  for (int l = 0; l < str.length(); l++) {
    drawChar(x,y,str[l],color);
  }
}

void drawStringOutlined(int x, int y, const String &str, unsigned int color, unsigned int outlineColor) {
  drawString(x-1,y,str,outlineColor);
  drawString(x+1,y,str,outlineColor);
  drawString(x,y-1,str,outlineColor);
  drawString(x,y+1,str,outlineColor);
  drawString(x-1,y-1,str,outlineColor);
  drawString(x+1,y-1,str,outlineColor);
  drawString(x-1,y-1,str,outlineColor);
  drawString(x-1,y+1,str,outlineColor);
  drawString(x-1,y+1,str,outlineColor);
  drawString(x+1,y+1,str,outlineColor);
  drawString(x+1,y-1,str,outlineColor);
  drawString(x+1,y+1,str,outlineColor);
  drawString(x,y,str,color);
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
  if (x0 < 0) x0 = 0;
  if (x0 < clipRect.x0) x0 = clipRect.x0;
  if (y0 < 0) y0 = 0;
  if (y0 < clipRect.y0) y0 = clipRect.y0;
  if (x1 > (int)frameBufferWidth) x1 = frameBufferWidth;
  if (x1 > clipRect.x1) x1 = clipRect.x1;
  if (y1 > (int)frameBufferHeight) y1 = frameBufferHeight;
  if (y1 > clipRect.y1) y1 = clipRect.y1;
  for (int y = y0; y < y1; y++) {
    unsigned int *d = &frameBuffer[y*frameBufferWidth+x0];
    for (int x = x0; x < x1; x++) {
      *d++=color;
    }
  }
}

void allocFrameBuffer(unsigned int width, unsigned int height) {
  frameBuffer = new unsigned int[width*height];
  fullScreenFrameBuffer = new unsigned int[width*height];
  frameBufferWidth = width;
  frameBufferHeight = height;
  for (int i = 0; i < width*height; i++) {
    frameBuffer[i] = 0xff000000;
    fullScreenFrameBuffer[i] = 0xff000000;
  }
  clipStack.clear();
  setClipRect(0,0,frameBufferWidth,frameBufferHeight);
}

void freeFrameBuffer() {
  delete[] frameBuffer;
  delete[] fullScreenFrameBuffer;
}

void clearFrameBuffer(unsigned int color) {
  for (int i = 0; i < frameBufferWidth * frameBufferHeight; i++) frameBuffer[i] = color;
}