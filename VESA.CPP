#include "vesa.hpp"
#include "memory.hpp"
#include <i86.h>

bool vesa_found = false;
VbeInfoBlock vesa_info;
VbeModeInfo vesa_modeInfo;
Array<int> vesa_videoModes;
String vesa_infoString;
String vesa_modeInfoString;
unsigned int *vesa_linearFrameBuffer = NULL;

void *vesaPointer(const void *d) {
  int segment = ((unsigned int)d) >> 16;
  int offset = ((unsigned int)d) & 0xffff;
  return (void*)(((unsigned int)segment<<4)+offset);
}

void vesa_getVideoModes() {
  vesa_videoModes.clear();
  if (vesa_found) {
    uint16_t *v = (uint16_t *)vesaPointer(vesa_info.videoModes);
    while(*v != 0xffff) {
      vesa_videoModes.push_back(*v);
      v++;
    }
  }
}

String vesaString(const char *d) {
  if (d == NULL) return "<not specified>";
  return String((char*)vesaPointer(d));
}

bool vesa_init() {
  union REGS regs;
  struct SREGS sregs;
  int segment;

  allocateDosMemory(512, segment);
  segread(&sregs);
  //printf("%d:\n",segment);
  regs.w.ax = 0x4f00;
  regs.w.di = segment;
  sregs.es = 0;
  int386x(0x10, &regs, &regs,&sregs);
  vesa_found = regs.h.al==0x4f;
  if (vesa_found) {
    memcpy(&vesa_info,(void*)segment,512);
    vesa_infoString = "aaaa\n";
    vesa_infoString[0] = vesa_info.signature[0];
    vesa_infoString[1] = vesa_info.signature[1];
    vesa_infoString[2] = vesa_info.signature[2];
    vesa_infoString[3] = vesa_info.signature[3];
    vesa_infoString += "Version:"+String(vesa_info.version / 256) + "." + String(vesa_info.version & 255)+"\n";
    vesa_infoString += "OEM:" + vesaString(vesa_info.oemString)+"\n";
    vesa_infoString += "Capabilities:" + String(vesa_info.capabilities)+"\n";
    vesa_infoString += "TotalMemory:" + String(vesa_info.totalMemory)+"\n";
    vesa_infoString += "SoftwareRev:" + String(vesa_info.softwareRev)+"\n";
    vesa_infoString += "Vendor:" + vesaString(vesa_info.vendor)+"\n";
    vesa_infoString += "ProductName:" + vesaString(vesa_info.productName)+"\n";
    vesa_infoString += "ProductRev:" + vesaString(vesa_info.productRev)+"\n";
    vesa_getVideoModes();
    vesa_infoString += "VideoModes:\n";
    for (int i = 0; i < vesa_videoModes.size(); i++) {
      if (i != 0) vesa_infoString+=",";
      vesa_infoString+="$"+String::hex(vesa_videoModes[i],4);
    }
    vesa_infoString += "\n";
  }
  releaseDosMemory(segment);
  return vesa_found;
}

bool vesa_initVideoMode(int modeNr) {
  if (!vesa_found)
    return false;

  union REGS regs;
  struct SREGS sregs;
  int segment;

  segread(&sregs);

  allocateDosMemory(256, segment);
  regs.w.ax = 0x4f01;
  regs.w.cx = modeNr;
  regs.w.di = segment;
  sregs.es = 0;
  int386x(0x10, &regs, &regs, &sregs); //this crashes the mouse interrupt, whyever
  memcpy(&vesa_modeInfo,(void*)segment,256);
  releaseDosMemory(segment);

  vesa_modeInfoString = "Attributes:" + String(vesa_modeInfo.attributes) +"\n";
  vesa_modeInfoString += "WindowA:"+String(vesa_modeInfo.windowA) +"\n";
  vesa_modeInfoString += "WindowB:"+String(vesa_modeInfo.windowB) +"\n";
  vesa_modeInfoString += "Granularity:"+String(vesa_modeInfo.granularity) +"\n";
  vesa_modeInfoString += "WindowSize:"+String(vesa_modeInfo.windowSize) +"\n";
  vesa_modeInfoString += "SegmentA:" + String(vesa_modeInfo.segmentA) +"\n";
  vesa_modeInfoString += "SegmentB:" + String(vesa_modeInfo.segmentB) +"\n";
  vesa_modeInfoString += "WinFuncPtr:"+ String(vesa_modeInfo.winFuncPtr) +"\n";
  vesa_modeInfoString += "Pitch:" + String(vesa_modeInfo.pitch) +"\n";
  vesa_modeInfoString += "Width:" + String(vesa_modeInfo.width) +"\n";
  vesa_modeInfoString += "Height:" + String(vesa_modeInfo.height) +"\n";
  vesa_modeInfoString += "wChar:" + String(vesa_modeInfo.wChar) +"\n";
  vesa_modeInfoString += "yChar:" + String(vesa_modeInfo.yChar) +"\n";
  vesa_modeInfoString += "Planes:" + String(vesa_modeInfo.planes) +"\n";
  vesa_modeInfoString += "BitsPerPixel:" + String(vesa_modeInfo.bpp) +"\n";
  vesa_modeInfoString += "Banks:" + String(vesa_modeInfo.banks) +"\n";
  vesa_modeInfoString += "MemoryModel:" + String(vesa_modeInfo.memoryModel) +"\n";
  vesa_modeInfoString += "BankSize:" + String(vesa_modeInfo.bankSize) +"\n";
  vesa_modeInfoString += "ImagePages:" + String(vesa_modeInfo.imagePages) +"\n";
  vesa_modeInfoString += "Reserved0:"+ String(vesa_modeInfo.reserved0) +"\n";
  vesa_modeInfoString += "RedMask:" + String(vesa_modeInfo.redMask) +"\n";
  vesa_modeInfoString += "RedPosition:" + String(vesa_modeInfo.redPosition) +"\n";
  vesa_modeInfoString += "GreenMask:" + String(vesa_modeInfo.greenMask) +"\n";
  vesa_modeInfoString += "GreenPosition:" + String(vesa_modeInfo.greenPosition) +"\n";
  vesa_modeInfoString += "BlueMask:" + String(vesa_modeInfo.blueMask) +"\n";
  vesa_modeInfoString += "BluePosition:" + String(vesa_modeInfo.bluePosition) +"\n";
  vesa_modeInfoString += "ReservedMask:" + String(vesa_modeInfo.reservedMask) +"\n";
  vesa_modeInfoString += "ReservedPosition:" + String(vesa_modeInfo.reservedPosition) +"\n";
  vesa_modeInfoString += "DirectColorModeInfo:" + String(vesa_modeInfo.directColorModeInfo) +"\n";
  vesa_modeInfoString += "LinearFrameBuffer:" + String::hex(vesa_modeInfo.linearFrameBuffer,8) +"\n";
  vesa_modeInfoString += "OffScreenMemOffset:" + String(vesa_modeInfo.offScreenMemOffset) +"\n";
  vesa_modeInfoString += "OffScreenMemSize:"+ String(vesa_modeInfo.offScreenMemSize) +"\n";

  //printf("%s\n",vesa_modeInfoString.c_str());
  //exit(0);

  if (vesa_modeInfo.bpp==32&&vesa_modeInfo.linearFrameBuffer!=NULL) {
    uint64_t lin = mapPhysicalToLinear(vesa_modeInfo.linearFrameBuffer,4096*1024-1);
    vesa_modeInfoString += "LinearFrameBuffer(real):" + String::hex(lin,8) +"\n";
    vesa_linearFrameBuffer = (unsigned int*)lin;
    regs.w.ax = 0x4f02;
    const int linearFrameBufferIsRequested = 0x4000;
    regs.w.bx = modeNr|linearFrameBufferIsRequested;
    int386(0x10, &regs, &regs);

    return true;
  }

  return false;
}

void vesa_force(int modeNr) {
    const int linearFrameBufferIsRequested = 0x4000;
    union REGS regs;
    vesa_found = true;
    vesa_linearFrameBuffer = (unsigned int*)mapPhysicalToLinear(0xf1000000,4096*1024-1);
    regs.w.ax = 0x4f02;
    regs.w.bx = modeNr|linearFrameBufferIsRequested;
    int386(0x10, &regs, &regs);
}