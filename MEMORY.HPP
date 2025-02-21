#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include "types.hpp"

#define SEGMENT(__v__) FP_SEG(__v__)
#define OFFSET(__v__) FP_OFF(__v__)
#define POINTER(__seg__,__ofs__) MK_FP(((unsigned int)__seg__),((unsigned int)__ofs__))

typedef struct {
  short di;
  short si;
  short bp;
  short reserved;
  short bx;
  short dx;
  short cx;
  short ax;
  short flags;
  short es,ds,fs,gs,ip,cs,sp,ss;
  int cflag;
} RMREGS;

typedef struct {
  short es,ds,fs,gs,ip,cs,sp,ss;
} RMSREGS;

bool allocateDosMemory(int size, int &segment);
bool releaseDosMemory(int segment);
void DPMI_allocRealSeg(int size, int *sel, int *r_seg);
void DPMI_freeRealSeg(unsigned sel);
short DPMI_int386(int intno, RMREGS *in, RMREGS *out);
short DPMI_int386x(int intno, RMREGS *in, RMREGS *out, RMSREGS *sregs);
void VBE_callESDI(RMREGS *regs, void *buffer, int size);
uint64_t mapPhysicalToLinear(unsigned int physicalAddress, unsigned int size);
bool unmapPhysical(unsigned int linearAddress);

#endif //__MEMORY_HPP__
