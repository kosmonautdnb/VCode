#ifndef __BLIT_HPP__
#define __BLIT_HPP__

#include "Point.hpp"
#include "Size.hpp"
#include "Rect.hpp"
class String;

extern unsigned int *frameBuffer;
extern unsigned int frameBufferWidth;
extern unsigned int frameBufferHeight;

extern Rect clipRect;

void drawMouse();

void captureFrameBuffer();
void drawCapturedFrameBuffer();

bool mouseOver(const Rect &v);

void pushRect(int x0, int y0, int x1, int y1);
void pushRect(const Point &p0, const Point &p1);
void pushRect(const Point &p0, const Size &size);
void popRect();

void drawChar(int &x, int &y, char c, unsigned int color);
void drawString(int x, int y, const String &str, unsigned int color);
void drawStringOutlined(int x, int y, const String &str, unsigned int color,unsigned int outlineColor);
void setPixel(int x, int y, unsigned int color);
void drawRectangle(int x0, int y0, int x1, int y1, unsigned int color);
void drawBox(int x0, int y0, int x1, int y1, unsigned int color);
void clearFrameBuffer(unsigned int color);
void allocFrameBuffer(unsigned int width, unsigned int height);
void freeFrameBuffer();

#endif //__BLIT_HPP__