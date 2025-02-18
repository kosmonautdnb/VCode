#include "colors.hpp"
#include "constants.hpp"
#include <i86.h>
#include <conio.h>
#include <stdlib.h>

int paletteCount = 0;
unsigned int palette[256] = {0};

void setBiosGraphicsMode(int mode) {
    union REGS regs;
    regs.x.eax = mode;
    int386(0x10, &regs, &regs);
}

int getBiosGraphicsMode() {
    union REGS regs;
    regs.w.ax = 0x0f00;
    int386 (0x10, &regs, &regs);
    return regs.h.al;
}

void setBiosCursor(int x, int y) {
    union REGS regs;
    regs.x.eax = 0x200;
    regs.h.bh = 0;
    regs.h.dl = x;
    regs.h.dh = y;
    int386(0x10, &regs, &regs);
}

void setPalette(int index, unsigned int color) {
  outp(0x3c8,index);
  outp(0x3c9,(color & 255)>>2);
  outp(0x3c9,((color>>8) & 255)>>2);
  outp(0x3c9,((color>>16) & 255)>>2);
  palette[index] = color;
}

unsigned int getPalette(int index) {
  outp(0x3c8,index);
  int r = inp(0x3c9);
  int g = inp(0x3c9);
  int b = inp(0x3c9);
  return 0xff000000|(r<<2)|(g<<10)|(b<<18);
}

unsigned char getPaletteIndex(unsigned int rgba) {
  const int r = rgba & 255;rgba>>=8;
  const int g = rgba & 255;rgba>>=8;
  const int b = rgba & 255;rgba>>=8;
  int ld = 256*256*4;
  int ri = 0;
  for (int i = 0; i < paletteCount; i++) {
    unsigned int pal = palette[i];
    const int r2 = pal & 255;pal >>= 8;
    const int g2 = pal & 255;pal >>= 8;
    const int b2 = pal & 255;pal >>= 8;
    const int d = (r2-r)*(r2-r)+(g2-g)*(g2-g)+(b2-b)*(b2-b);
    if (d < ld) {
      ld = d;
      ri = i;
    }
  }
  return ri;
}

void randomPalette() {
  paletteCount = 256;
  for (int i = 1; i < 256; i++) {
    int r = rand() & 255;
    int g = rand() & 255;
    int b = rand() & 255;
    if (i > 256-64) {
      r = (i - (256-64))*4;
      g = r;
      b = r;
    }
    setPalette(i,r|(g<<8)|(b<<16)|0xff000000);
  }
}

void palette16Colors() {
  paletteCount = 16;
  palette[EGA_COLOR_BLACK] = _EGA_COLOR_BLACK;
  palette[EGA_COLOR_BLUE] = _EGA_COLOR_BLUE;
  palette[EGA_COLOR_GREEN] = _EGA_COLOR_GREEN;
  palette[EGA_COLOR_CYAN] = _EGA_COLOR_CYAN;
  palette[EGA_COLOR_RED] = _EGA_COLOR_RED;
  palette[EGA_COLOR_VIOLET] = _EGA_COLOR_VIOLET;
  palette[EGA_COLOR_BROWN] = _EGA_COLOR_BROWN;
  palette[EGA_COLOR_GREY_LIGHT] = _EGA_COLOR_GREY_LIGHT;
  palette[EGA_COLOR_GREY_DARK] = _EGA_COLOR_GREY_DARK;
  palette[EGA_COLOR_BLUE_LIGHT] = _EGA_COLOR_BLUE_LIGHT;
  palette[EGA_COLOR_GREEN_LIGHT] = _EGA_COLOR_GREEN_LIGHT;
  palette[EGA_COLOR_CYAN_LIGHT] = _EGA_COLOR_CYAN_LIGHT;
  palette[EGA_COLOR_RED_LIGHT] = _EGA_COLOR_RED_LIGHT;
  palette[EGA_COLOR_VIOLET_LIGHT] = _EGA_COLOR_VIOLET_LIGHT;
  palette[EGA_COLOR_YELLOW] = _EGA_COLOR_YELLOW;
  palette[EGA_COLOR_WHITE] = _EGA_COLOR_WHITE;

  if (false) {
    for (int i = 0; i < 16; i++) palette[i] = getPalette(i);
  }

  if (false) {
    setPalette(0,COLOR_BLACK);
    setPalette(1,COLOR_WHITE);
    setPalette(2,COLOR_RED);
    setPalette(3,COLOR_YELLOW);
    setPalette(4,COLOR_MENUBACKGROUND);
    setPalette(5,COLOR_MENUTEXT);
    setPalette(6,COLOR_SELECTEDLINE);
    setPalette(7,COLOR_CURRENTLINE);
    setPalette(8,COLOR_CURRENTLINESELECTED);
    setPalette(9,COLOR_MOUSEOVERLINE);
    setPalette(10,COLOR_HEADERLINE);
    setPalette(11,COLOR_HEADERTEXT);
    setPalette(12,COLOR_PROMPTTEXT);
    setPalette(13,COLOR_STATUSBARTEXT);
    setPalette(14,COLOR_TEXT);
    setPalette(15,COLOR_TEXTBACKGROUND);
  }
}

extern unsigned int frameBufferWidth;
extern unsigned int frameBufferHeight;

void frameBufferToScreen16Colors(unsigned int *frameBuffer) {
    int x,y,i,j,b;
    unsigned int *read;
    unsigned char *write;
    int width = 640;
    int height = 480;
    for (y = 0; y < height; y++) {
        for (j = 0; j < 4; j++) {
            outp(0x3c4, 2);
            outp(0x3c5, 1<<j);
            read = &frameBuffer[y * frameBufferHeight/height * frameBufferWidth];
            write = (unsigned char *)(0xa0000 + (width/8)*y);
            for (x = 0; x < width/8; x++) {
                b = 0;
                for (i = 0; i < 8; i++) {
                    int xi = x*8+i;
                    b |= ((getPaletteIndex(read[xi*frameBufferWidth/width])>>j) & 1)<<(7-i);
                }
                *write++ = b;
            }
        }
    }
}

void frameBufferToScreen(unsigned int *frameBuffer) {
  if (frameBufferWidth == 640 && frameBufferHeight == 480) {
    frameBufferToScreen16Colors(frameBuffer);
    return;
  }
  unsigned char *videoRam = (unsigned char *)0xa0000;
  for (int y = 0; y < 200; y++) {
    for (int x = 0; x < 320; x++) {
      *videoRam++ = getPaletteIndex(frameBuffer[x*frameBufferWidth/320+y*frameBufferHeight/200*frameBufferWidth]);
    }
  }
}