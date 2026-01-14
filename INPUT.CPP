#include "input.hpp"
#include "blit.hpp"
#include <i86.h>
#include <dos.h>
#include <string.h>
#include <conio.h>

#ifdef __WATCOMC__
unsigned int int16_1(); // we need the zero flag it's not well exposed by the watcom api sorry that's why some asm here
#pragma aux int16_1="mov ah,0x01""int 0x16""jz do""mov eax,0""jmp dont""do:""mov eax,1""dont:" value[eax] modify[eax ebx ecx edx]
#endif
#ifdef __DJGPP__
 int16_1() {
 unsigned int ret;
  __asm__ __volatile__("\tmovb $1,%%ah\n""\tint $0x16\n""\tjz 1f\n""\tmovl $0,%%eax\n""\tjmp 2f\n""1:\n""\tmovl $1,%%eax\n""2:\n" : "=a" (ret));
  return ret;
}
#endif

int mouseX = 0;
int mouseY = 0;
int mouseB = 0;
int lastMouseB = 0;
int key = 0;
bool shiftKey = false;
bool ctrlKey = false;
bool altKey = false;

void updateKeyboard() {
    key = 0;
    if (!int16_1()) {
      union REGS regs;
      memset(&regs,0,sizeof(regs));
      regs.w.ax = 0x0000;
      int386(0x16, &regs, &regs);
      key = regs.w.ax;
      if ((key & 0xff) != 0x00) key &= 0xff;
    }

    int keyState = *((char *)0x417);
    shiftKey = (keyState & 0x03) != 0;
    ctrlKey = (keyState & 0x04) != 0;
    altKey = (keyState & 0x08) != 0;
}

void updateMouse() {
    union REGS regs;
    regs.x.eax = 0x03;
    int386(0x33, &regs, &regs);
    mouseX = regs.x.ecx & 0xffff;
    mouseY = regs.x.edx & 0xffff;
    lastMouseB = mouseB;
    mouseB = regs.x.ebx & 0xffff;
}

extern unsigned int frameBufferWidth;
extern unsigned int frameBufferHeight;

void setupMouse() {
    union REGS regs;
    regs.x.eax = 0x07;
    regs.x.ecx = 0;
    regs.x.edx = frameBufferWidth-1;
    int386(0x33, &regs, &regs);
    regs.x.eax = 0x08;
    regs.x.ecx = 0;
    regs.x.edx = frameBufferHeight-1;
    int386(0x33, &regs, &regs);
}