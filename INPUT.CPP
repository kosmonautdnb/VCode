#include "input.hpp"
#include "blit.hpp"
#include <i86.h>
#include <conio.h>

int mouseX = 0;
int mouseY = 0;
int mouseB = 0;
int key = 0;
bool shiftKey = false;
bool strgKey = false;
bool altKey = false;

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
    int keyState = *((char *)0x417);
    shiftKey = (keyState & 0x03) != 0;
    strgKey = (keyState & 0x04) != 0;
    altKey = (keyState & 0x08) != 0;
}

void updateMouse() {
    union REGS regs;
    regs.x.eax = 0x03;
    int386(0x33, &regs, &regs);
    mouseX = regs.x.ecx & 0xffff;
    mouseY = regs.x.edx & 0xffff;
    mouseB = regs.x.ebx & 0xffff;
}

extern unsigned int frameBufferWidth;
extern unsigned int frameBufferHeight;

void setupMouse() {
    union REGS regs;
    regs.x.eax = 0x07;
    regs.x.ecx = 0;
    regs.x.edx = frameBufferWidth;
    int386(0x33, &regs, &regs);
    regs.x.eax = 0x08;
    regs.x.ecx = 0;
    regs.x.edx = frameBufferHeight;
    int386(0x33, &regs, &regs);
}