#include <i86.h>
#include <conio.h>
#include <stdlib.h>

unsigned int palette[256] = {0};

int mouseX = 0;
int mouseY = 0;
int mouseB = 0;
int key = 0;

void updateKeyboard() {
    union REGS regs;
    regs.x.eax = 0x0600;
    regs.x.edx = 0xff;
    int386(0x21, &regs, &regs);
    key = regs.x.eax & 0xff;
    if (key == 0) {
      regs.x.eax = 0x0600;
      regs.x.edx = 0xff;
      int386(0x21, &regs, &regs);
      key += (regs.x.eax & 0xff)<<8;
    }
}

void updateMouse() {
    union REGS regs;
    regs.x.eax = 0x03;
    int386(0x33, &regs, &regs);
    mouseX = regs.x.ecx & 0xffff;
    mouseY = regs.x.edx & 0xffff;
    mouseB = regs.x.ebx & 0xffff;
    mouseX /= 2;
}

void setBiosGraphicsMode(int mode) {
    union REGS regs;
    regs.x.eax = mode;
    int386(0x10, &regs, &regs);
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

unsigned char getPalette(unsigned int rgba) {
  const int r = rgba & 255;rgba>>=8;
  const int g = rgba & 255;rgba>>=8;
  const int b = rgba & 255;rgba>>=8;
  int ld = 256*256*4;
  int ri = 0;
  for (int i = 0; i < 256; i++) {
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

void frameBufferToScreen(unsigned int *frameBuffer) {
  unsigned char *videoRam = (unsigned char *)0xa0000;
  for (int y = 0; y < 200; y++) {
    for (int x = 0; x < 320; x++) {
      *videoRam++ = getPalette(*frameBuffer++);
    }
  }
}