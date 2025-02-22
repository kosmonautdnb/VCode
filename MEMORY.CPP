#include "memory.hpp"
#include <i86.h>
#include <dos.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

typedef struct {
  long edi;
  long esi;
  long ebp;
  long reserved;
  long ebx;
  long edx;
  long ecx;
  long eax;
  short flags;
  short es,ds,fs,gs,ip,cs,sp,ss;
} _RMREGS;

#define IN(reg) rmregs.e##reg = in->##reg
#define OUT(reg) out->##reg = rmregs.e##reg

int VESABUF_sel=0x00;
int VESABUF_rseg=0x00;
int VESABUF_len=1024;

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

void DPMI_allocRealSeg(int size, int *sel, int *r_seg) {
  union REGS r;
  r.w.ax = 0x100;
  r.w.bx = (size+0x0f)>>4;
  int386(0x31,&r,&r);
  if (r.w.cflag)
    exit(0);
  *sel = r.w.dx;
  *r_seg = r.w.ax;
}

void DPMI_freeRealSeg(unsigned sel) {
  union REGS r;
  r.w.ax = 0x101;
  r.w.dx = sel;
  int386(0x31,&r,&r);
}

short DPMI_int386(int intno, RMREGS *in, RMREGS *out) {
  _RMREGS rmregs;
  union REGS r;
  struct SREGS sr;

  memset(&rmregs,0,sizeof(rmregs));
  IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);

  segread(&sr);
  r.w.ax = 0x300;
  r.h.bl = intno;
  r.h.bh = 0;
  r.w.cx = 0;
  sr.es = sr.ds;
  r.x.edi = (unsigned)&rmregs;
  int386x(0x31,&r,&r,&sr);

  OUT(ax); OUT(bx); OUT(cx); OUT(dx); OUT(si); OUT(di);
  out->cflag = rmregs.flags & 0x01;
  return out->ax;
}

short DPMI_int386x(int intno, RMREGS *in, RMREGS *out, RMSREGS *sregs) {
  _RMREGS rmregs;
  union REGS r;
  struct SREGS sr;

  memset(&rmregs,0,sizeof(rmregs));
  IN(ax); IN(bx); IN(cx); IN(dx); IN(si); IN(di);
  rmregs.es = sregs->es;
  rmregs.ds = sregs->ds;

  segread(&sr);
  r.w.ax = 0x300;
  r.h.bl = intno;
  r.h.bh = 0;
  r.w.cx = 0;
  sr.es = sr.ds;
  r.x.edi = (unsigned)&rmregs;
  int386x(0x31,&r,&r,&sr);

  OUT(ax); OUT(bx); OUT(cx); OUT(dx); OUT(si); OUT(di);
  sregs->es = rmregs.es;
  sregs->cs = rmregs.cs;
  sregs->ss = rmregs.ss;
  sregs->ds = rmregs.ds;
  out->cflag = rmregs.flags & 0x01;
  return out->ax;
}

static void ExitVBEBuf() {
  DPMI_freeRealSeg(VESABUF_sel);
}

VBE_initRMBuf() {
  if (!VESABUF_sel) {
    DPMI_allocRealSeg(VESABUF_len,&VESABUF_sel,&VESABUF_rseg);
    atexit(ExitVBEBuf);
  }
}

void VBE_callESDI(RMREGS *regs, void *buffer, int size) {
  RMSREGS sregs;
  VBE_initRMBuf();
  sregs.es = VESABUF_rseg;
  regs->di = 0;
  _fmemcpy(MK_FP(VESABUF_sel,0),buffer,size);
  DPMI_int386x(0x10,regs,regs,&sregs);
  _fmemcpy(buffer,MK_FP(VESABUF_sel,0),size);
  //ExitVBEBuf();
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