#ifndef __VESA_HPP__
#define __VESA_HPP__

#include "types.hpp"
#include "array.hpp"
#include "string.hpp"

typedef struct VbeInfoBlock {
  char signature[4];
  uint16_t version;
  char *oemString;
  uint32_t capabilities;
  uint16_t *videoModes;
  uint16_t totalMemory;
  uint16_t softwareRev;
  char *vendor;
  char *productName;
  char *productRev;
  char reserved1[222];
  char oemData[256];
} VbeInfoBlock;

typedef struct VbeModeInfo {
  uint16_t attributes;
  uint8_t windowA;
  uint8_t windowB;
  uint16_t granularity;
  uint16_t windowSize;
  uint16_t segmentA;
  uint16_t segmentB;
  uint32_t winFuncPtr;
  uint16_t pitch;
  uint16_t width;
  uint16_t height;
  uint8_t wChar;
  uint8_t yChar;
  uint8_t planes;
  uint8_t bpp;
  uint8_t banks;
  uint8_t memoryModel;
  uint8_t bankSize;
  uint8_t imagePages;
  uint8_t reserved0;
  uint8_t redMask;
  uint8_t redPosition;
  uint8_t greenMask;
  uint8_t greenPosition;
  uint8_t blueMask;
  uint8_t bluePosition;
  uint8_t reservedMask;
  uint8_t reservedPosition;
  uint8_t directColorModeInfo;
  uint32_t linearFrameBuffer;
  uint32_t offScreenMemOffset;
  uint16_t offScreenMemSize;
  uint8_t reserved1[256-50];
} VbeModeInfo;

extern int vesa_currentMode;

Array<int> vesa_getVideoModes(const VbeInfoBlock &data);
String vesa_getInfoString(const VbeInfoBlock &data);
void vesa_getInfo(VbeInfoBlock &data);
void vesa_getModeInfo(int mode, VbeModeInfo &data);
String vesa_getModeInfoString(const VbeModeInfo &data, bool compact);
void vesa_setAsRenderer(const VbeModeInfo &data);
void vesa_setMode(int mode);
int vesa_getMode(int width, int height, int bitsPerPixel, bool linearFramebuffer);

#endif // __VESA_HPP__