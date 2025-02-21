#include "memory.hpp"
#include <i86.h>

bool allocateDosMemory(int size, int &segment) {
  union REGS r;
  r.w.ax = 0x4800;
  r.w.bx = (size+15)>>4;
  int386(0x21,&r,&r);
  segment = r.w.ax;
  if (r.w.cflag)
    return false;
  return true;
}

bool releaseDosMemory(int segment) {
  union REGS r;
  struct SREGS s;
  segread(&s);
  r.w.ax = 0x4900;
  r.w.bx = 0;
  s.es = segment;
  int386x(0x21,&r,&r,&s);
  if (r.w.cflag)
    return false;
  return true;
}

uint64_t mapPhysicalToLinear(unsigned int physicalAddress, unsigned int size) {
  union REGS r;
  r.w.ax = 0x800;
  r.w.bx = physicalAddress>>16;
  r.w.cx = physicalAddress & 0xffff;
  r.w.si = size >> 16;
  r.w.di = size & 0xffff;
  int386(0x31,&r,&r);
  if (r.x.cflag)
    return 0;
  return (((uint64_t)r.w.bx)<<16)+r.w.cx;
}

bool unmapPhysical(unsigned int linearAddress) {
  union REGS r;
  r.w.ax = 0x801;
  r.w.bx = linearAddress>>16;
  r.w.cx = linearAddress & 0xffff;
  int386(0x31,&r,&r);
  if (r.x.cflag)
    return false;
  return true;
}