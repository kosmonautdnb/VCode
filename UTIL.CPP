#include "util.hpp"
#include "types.hpp"
#include "constant.hpp"
#include "dos.hpp"
#include <i86.h>
#include <direct.h>
#include <conio.h>
#include "vga.hpp"
#include "vesa.hpp"
#include "valert.hpp"
#include "pngload.hpp"

String dosFileName(const String &fileName);
bool fileWasWritten = false;

const char *weekdays[8] = {"non","sun","mon","tue","wed","thu","fri","sat"};

String hex(int v) {
  char *h="0123456789ABCDEF";
  String r;
  r.resize(2);
  r[0] = h[v / 16];
  r[1] = h[v & 15];
  return r;
}

String currentToolString="";
String currentTimeString() {
  int rtc_seconds = rtcRead(0);
  int rtc_minutes = rtcRead(2);
  int rtc_hours   = rtcRead(4);
  int rtc_weekday = rtcRead(6);
  int rtc_day     = rtcRead(7);
  int rtc_month   = rtcRead(8);
  int rtc_year    = rtcRead(9);
  return hex(rtc_day)+"."+hex(rtc_month)+"."+hex(rtc_year)+" "+hex(rtc_hours)+":"+hex(rtc_minutes)+"."+hex(rtc_seconds);
}

void make0d0ato0a(String &r) {
  for (int i = 0; i < r.length()-1; i++) {
    char c = r[i];
    if (c==0x0d&&r[i+1]==0x0a) {
      r.erase(i,1);
      i--;
    }
  }
}

void make0ato0d0a(String &r) {
  for (int i = 0; i < r.length(); i++) {
    if (r[i]==0x0a) {
      r.insert(i,String::chr(0x0d));
      i++;
    }
  }
}

String stringFromFile(const String &fileName, bool textFile) {
  FILE *in = fopen(dosFileName(fileName).c_str(),"rb");
  if (in != NULL) {
    setPalette(0,0xffffffff);
    fseek(in,0,SEEK_END);
    long fileSize = ftell(in);
    fseek(in,0,SEEK_SET);
    String r;
    r.resize(fileSize);
    fread(&(r[0]),1,fileSize,in);
    fclose(in);
    if (textFile)
      make0d0ato0a(r);
    setPalette(0,0xff000000);
    return r;
  }
  return String();
}

struct meminfo {
    unsigned LargestBlockAvail;
    unsigned MaxUnlockedPage;
    unsigned LargestLockablePage;
    unsigned LinAddrSpace;
    unsigned NumFreePagesAvail;
    unsigned NumPhysicalPagesFree;
    unsigned TotalPhysicalPages;
    unsigned FreeLinAddrSpace;
    unsigned SizeOfPageFile;
    unsigned Reserved[3];
} MemInfo;

_diskfree_t discFree;
static VbeInfoBlock vesa_info;
static VbeModeInfo vesa_modeInfo;

String getInfoString() {
  String info;
  union REGS regs;
  struct SREGS sregs;

  int drive = _getdrive();
  _getdiskfree(drive, &discFree);

  info += " CONFIGURATION INFO\n";
  info += "----------------------\n";
  info += "\n";
  info += " DISC INFO\n";
  info += "-----------\n";
  info += String("Working directory:")+String(getCWD())+"\n";
  info += String("Drive number:")+drive+"\n";
  info += String("Disc space:")+ sizeString((long)discFree.total_clusters*discFree.sectors_per_cluster*discFree.bytes_per_sector) + "\n";
  info += String("Disc free:")+ sizeString((long)discFree.avail_clusters*discFree.sectors_per_cluster*discFree.bytes_per_sector) + "\n";
  info += String("Disc clusters:")+ discFree.total_clusters + "\n";
  info += String("Available clusters:")+ discFree.avail_clusters + "\n";
  info += String("Sectors per cluster:")+ discFree.sectors_per_cluster + "\n";
  info += String("Bytes per sector:")+ discFree.bytes_per_sector + "\n";
  info += "\n";

  regs.x.eax = 0x00000500;
  memset( &sregs, 0, sizeof(sregs) );
  sregs.es = FP_SEG( &MemInfo );
  regs.x.edi = FP_OFF( &MemInfo );

  int386x( 0x31, &regs, &regs, &sregs );

  info += " MEMORY INFO\n";
  info += "---------------\n";
  info += "Largest block available:";
  info += String(MemInfo.LargestBlockAvail)+"\n";
  info += "Max unlocked page:";
  info += String(MemInfo.MaxUnlockedPage)+"\n";
  info += "Largest lockable page:";
  info += String(MemInfo.LargestLockablePage)+"\n";
  info += "Linear address space:";
  info += String(MemInfo.LinAddrSpace)+"\n";
  info += "Number of free pages available:";
  info += String(MemInfo.NumFreePagesAvail)+"\n";
  info += "Number of physical pages free:";
  info += String(MemInfo.NumPhysicalPagesFree)+"\n";
  info += "Total physical pages:";
  info += String(MemInfo.TotalPhysicalPages)+"\n";
  info += "Free linear address space:";
  info += String(MemInfo.FreeLinAddrSpace)+"\n";
  info += "Size of page file:";
  info += String(MemInfo.SizeOfPageFile)+"\n";
  info += "\n";

  info += " VIDEO INFO\n";
  info += "------------\n";
  info += "Vesa current mode:$" + String::hex(vesa_currentMode,4) + "\n";
  vesa_getInfo(vesa_info);
  info += vesa_getInfoString(vesa_info);
  Array<int> modes;
  modes = vesa_getVideoModes(vesa_info);
  for (int i = 0; i < modes.size(); i++) {
    int mode = modes[i];
    info += "--Mode:$" + String::hex(mode,4) + " ";
    vesa_getModeInfo(mode,vesa_modeInfo);
    info += vesa_getModeInfoString(vesa_modeInfo,true);
    info += "\n";
    if (mode == vesa_currentMode) {
      info += vesa_getModeInfoString(vesa_modeInfo,false);
      info += "\n";
    }
  }
  info += "\n";

  return info;
}

void stringToFile(const String &string,const String &fileName, bool textFile) {
  FILE *in = fopen(dosFileName(fileName).c_str(),"rb");
  bool alreadyThere = in != NULL;
  if (alreadyThere) fclose(in);

  if (alreadyThere) {
    int a = VAlert::box("Overwrite File:" + fileName.last(20), "YES","NO");
    if (a != 1)
      return;
  }

  fileWasWritten = true;
  FILE *out = fopen(dosFileName(fileName).c_str(),"wb");
  if (out != NULL) {
    setPalette(0,0xffffffff);
    String s2 = string;
    if (textFile)
      make0ato0d0a(s2);
    fwrite(&(s2[0]),1,s2.length(),out);
    fclose(out);
    setPalette(0,0xff000000);
  }
}

bool copyFile(const String &destFileName, const String &sourceFileName) {
  FILE *in = fopen(dosFileName(sourceFileName).c_str(),"rb");
  if (in == NULL) return false;
  FILE *out = fopen(dosFileName(destFileName).c_str(),"rb");
  if (out != NULL) {
    fclose(out);
    int a = VAlert::box("Overwrite file:" + destFileName.last(20), "YES","Don't");
    if (a != 1) return false;
  }
  out = fopen(dosFileName(destFileName).c_str(),"wb");
  if (out == NULL) {fclose(in);return false;}
  fseek(in,0,SEEK_END);
  long fileSize = ftell(in);
  fseek(in,0,SEEK_SET);
  Array<unsigned char> data;
  data.resize(65536);
  while(fileSize>0) {
    long count = fread(&(data[0]),1,data.size(),in);
    setPalette(0,0xffffffff);
    fwrite(&(data[0]),1,count,out);
    setPalette(0,0xff000000);
    fileSize -= count;
  }
  fclose(out);
  fclose(in);
  fileWasWritten = true;
  return true;
}

bool moveFile(const String &destFileName, const String &sourceFileName) {
  if (copyFile(destFileName, sourceFileName)) {
    removeFile(sourceFileName);
  }
  return true;
}

bool copyFiles(const String &destPath, Array<String> &sourcePaths) {
  int a = VAlert::box("Copy "+String(sourcePaths.size())+" Files", "YES","NO");
  if (a==1) {
    for (int i = 0; i < sourcePaths.size(); i++) {
      String sourceFileName = sourcePaths[i];
      String sourcePath = getPath(sourceFileName);
      if (sourcePath==destPath) {
        a = VAlert::box("Can't copy files to same folder.", "OK");
        return false;
      }
      String fileName = getName(sourceFileName);
      String extension = getExtension(sourceFileName);
      String destFileName = destPath + fileName + extension;
      copyFile(destFileName, sourceFileName);
    }
    fileWasWritten = true;
  }
  return a == 1;
}
bool moveFiles(const String &destPath, Array<String> &sourcePaths) {
  int a = VAlert::box("Move "+String(sourcePaths.size())+" Files", "YES","NO");
  if (a==1) {
    for (int i = 0; i < sourcePaths.size(); i++) {
      String sourceFileName = sourcePaths[i];
      String sourcePath = getPath(sourceFileName);
      if (sourcePath==destPath) {
        a = VAlert::box("Can't move files to same folder.", "OK");
        return false;
      }
      String fileName = getName(sourceFileName);
      String extension = getExtension(sourceFileName);
      String destFileName = destPath + fileName + extension;
      moveFile(destFileName, sourceFileName);
    }
    fileWasWritten = true;
  }
  return a == 1;
}

bool removeFiles(Array<String> &sourcePaths) {
  int a = VAlert::box("Delete "+String(sourcePaths.size())+" Files", "YES","NO");
  if (a==1) {
    for (int i = 0; i < sourcePaths.size(); i++) {
      String sourceFileName = sourcePaths[i];
      removeFile(sourceFileName);
    }
    fileWasWritten = true;
  }
  return a == 1;
}

void saveScreenShot(const char *fileName, unsigned int *data, int width,int height) {
  ImageLayer l;
  l.data = data;
  l.width = width;
  l.height = height;
  savePNG(&l,fileName);
  fileWasWritten = true;
}

int mapAltKey(int key) {
  int altKeyMapped = VK_NONE;
  switch(key) {
    case 'a':
    case 'A': altKeyMapped = VK_A_ALT; break;
    case 'b':
    case 'B': altKeyMapped = VK_B_ALT; break;
    case 'c':
    case 'C': altKeyMapped = VK_C_ALT; break;
    case 'd':
    case 'D': altKeyMapped = VK_D_ALT; break;
    case 'e':
    case 'E': altKeyMapped = VK_E_ALT; break;
    case 'f':
    case 'F': altKeyMapped = VK_F_ALT; break;
    case 'g':
    case 'G': altKeyMapped = VK_G_ALT; break;
    case 'h':
    case 'H': altKeyMapped = VK_H_ALT; break;
  }
  return altKeyMapped;
}