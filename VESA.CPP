#include "vesa.hpp"
#include "memory.hpp"
#include <i86.h>
#include "blit.hpp"
#include "vga.hpp"

int vesa_currentMode = 0x00;

void *vesaPointer(const void *d) {
  int segment = ((unsigned int)d) >> 16;
  int offset = ((unsigned int)d) & 0xffff;
  return (void*)(((unsigned int)segment<<4)+offset);
}

Array<int> vesa_getVideoModes(const VbeInfoBlock &data) {
  Array<int> vesa_videoModes;
  uint16_t *v = (uint16_t *)vesaPointer(data.videoModes);
  while(*v != 0xffff) {
    vesa_videoModes.push_back(*v);
    v++;
  }
  return vesa_videoModes;
}

String vesaString(const char *d) {
  if (d == NULL) return "<not specified>";
  return String((char*)vesaPointer(d));
}


String vesa_getInfoString(const VbeInfoBlock &data) {
    String vesa_infoString;
    vesa_infoString = "aaaa\n";
    vesa_infoString[0] = data.signature[0];
    vesa_infoString[1] = data.signature[1];
    vesa_infoString[2] = data.signature[2];
    vesa_infoString[3] = data.signature[3];
    vesa_infoString += "Version:"+String(data.version / 256) + "." + String(data.version & 255)+"\n";
    vesa_infoString += "OEM:" + vesaString(data.oemString)+"\n";
    vesa_infoString += "Capabilities:" + String(data.capabilities)+"\n";
    vesa_infoString += "TotalMemory:" + String(data.totalMemory)+"\n";
    vesa_infoString += "SoftwareRev:" + String(data.softwareRev)+"\n";
    vesa_infoString += "Vendor:" + vesaString(data.vendor)+"\n";
    vesa_infoString += "ProductName:" + vesaString(data.productName)+"\n";
    vesa_infoString += "ProductRev:" + vesaString(data.productRev)+"\n";
    Array<int> vesa_videoModes = vesa_getVideoModes(data);
    vesa_infoString += "VideoModes:\n";
    for (int i = 0; i < vesa_videoModes.size(); i++) {
      if (i != 0) vesa_infoString+=",";
      vesa_infoString+="$"+String::hex(vesa_videoModes[i],4);
    }
    vesa_infoString += "\n";
    return vesa_infoString;
}

void vesa_getInfo(VbeInfoBlock &data) {
  RMREGS regs;
  regs.ax = 0x4f00;
  VBE_callESDI(&regs,&data,sizeof(VbeInfoBlock));
}

void vesa_getModeInfo(int mode, VbeModeInfo &data) {
  RMREGS regs;
  regs.ax = 0x4f01;
  regs.cx = mode;
  VBE_callESDI(&regs,&data,sizeof(VbeModeInfo));
}

String vesa_getModeInfoString(const VbeModeInfo &data, bool compact) {
  String vesa_modeInfoString;
  if (compact) {
    vesa_modeInfoString += String(data.width) + "x" + String(data.height) + "x" + String(data.bpp);
    return vesa_modeInfoString;
  }
  vesa_modeInfoString = "Attributes:" + String(data.attributes) +"\n";
  vesa_modeInfoString += "WindowA:"+String(data.windowA) +"\n";
  vesa_modeInfoString += "WindowB:"+String(data.windowB) +"\n";
  vesa_modeInfoString += "Granularity:"+String(data.granularity) +"\n";
  vesa_modeInfoString += "WindowSize:"+String(data.windowSize) +"\n";
  vesa_modeInfoString += "SegmentA:" + String(data.segmentA) +"\n";
  vesa_modeInfoString += "SegmentB:" + String(data.segmentB) +"\n";
  vesa_modeInfoString += "WinFuncPtr:"+ String(data.winFuncPtr) +"\n";
  vesa_modeInfoString += "Pitch:" + String(data.pitch) +"\n";
  vesa_modeInfoString += "Width:" + String(data.width) +"\n";
  vesa_modeInfoString += "Height:" + String(data.height) +"\n";
  vesa_modeInfoString += "wChar:" + String(data.wChar) +"\n";
  vesa_modeInfoString += "yChar:" + String(data.yChar) +"\n";
  vesa_modeInfoString += "Planes:" + String(data.planes) +"\n";
  vesa_modeInfoString += "BitsPerPixel:" + String(data.bpp) +"\n";
  vesa_modeInfoString += "Banks:" + String(data.banks) +"\n";
  vesa_modeInfoString += "MemoryModel:" + String(data.memoryModel) +"\n";
  vesa_modeInfoString += "BankSize:" + String(data.bankSize) +"\n";
  vesa_modeInfoString += "ImagePages:" + String(data.imagePages) +"\n";
  vesa_modeInfoString += "Reserved0:"+ String(data.reserved0) +"\n";
  vesa_modeInfoString += "RedMask:" + String(data.redMask) +"\n";
  vesa_modeInfoString += "RedPosition:" + String(data.redPosition) +"\n";
  vesa_modeInfoString += "GreenMask:" + String(data.greenMask) +"\n";
  vesa_modeInfoString += "GreenPosition:" + String(data.greenPosition) +"\n";
  vesa_modeInfoString += "BlueMask:" + String(data.blueMask) +"\n";
  vesa_modeInfoString += "BluePosition:" + String(data.bluePosition) +"\n";
  vesa_modeInfoString += "ReservedMask:" + String(data.reservedMask) +"\n";
  vesa_modeInfoString += "ReservedPosition:" + String(data.reservedPosition) +"\n";
  vesa_modeInfoString += "DirectColorModeInfo:" + String(data.directColorModeInfo) +"\n";
  vesa_modeInfoString += "LinearFrameBuffer:" + String::hex(data.linearFrameBuffer,8) +"\n";
  vesa_modeInfoString += "OffScreenMemOffset:" + String(data.offScreenMemOffset) +"\n";
  vesa_modeInfoString += "OffScreenMemSize:"+ String(data.offScreenMemSize) +"\n";
  return vesa_modeInfoString;
}

void vesa_setAsRenderer(const VbeModeInfo &data) {
    uint64_t lin = mapPhysicalToLinear(data.linearFrameBuffer,4096*1024-1);
    paletteCount = -1;
    if (frameBuffer!=NULL)
      freeFrameBuffer();
    allocFrameBuffer(data.width,data.height);
    setLinearFrameBuffer32Bit((unsigned int*)lin);
}

void vesa_setMode(int mode) {
  vesa_currentMode = mode;
  const int linearFrameBufferIsRequested = 0x4000;
  RMREGS regs;
  regs.ax = 0x4f02;
  regs.bx = mode|linearFrameBufferIsRequested;
  DPMI_int386(0x10,&regs,&regs);
}

int vesa_getMode(int width, int height, int bitsPerPixel, bool linearFramebuffer) {
  VbeInfoBlock info;
  vesa_getInfo(info);
  Array<int> list = vesa_getVideoModes(info);
  for (int i = 0; i < list.size(); i++) {
    VbeModeInfo modeInfo;
    vesa_getModeInfo(list[i], modeInfo);
    if (modeInfo.width==width&&modeInfo.height==height&&modeInfo.bpp==bitsPerPixel&&modeInfo.linearFrameBuffer!=0x00) {
      return list[i];
    }
  }
  return -1;
}
