#include <direct.h>
#include "dos.hpp"
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <conio.h>
#include <i86.h>
#include <fcntl.h>

extern bool fileWasWritten;

short getFileDate(const String &filePath) {
  int handle = open(filePath.c_str(),O_RDONLY | O_TEXT);
  union REGS r;
  r.h.ah = 0x57;
  r.h.al = 0x00;
  r.w.bx = (short)handle;
  int386(0x21,&r,&r);
  close(handle);
  return r.w.dx;
}

short getFileTime(const String &filePath) {
  int handle = open(filePath.c_str(),O_RDONLY | O_TEXT);
  union REGS r;
  r.h.ah = 0x57;
  r.h.al = 0x00;
  r.w.bx = (short)handle;
  int386(0x21,&r,&r);
  close(handle);
  return r.w.cx;
}

String unixFileName(const String &dir) {
  String r = String(dir);
  for (int i = 0; i < r.length(); i++) if (r[i]=='\\') r[i]='/';
  return r;
}

String getCWD() {
  String currentWorkingDirectory;
  currentWorkingDirectory.resize(2048);
  getcwd(&(currentWorkingDirectory[0]),currentWorkingDirectory.length());
  currentWorkingDirectory.resize(strlen(currentWorkingDirectory.c_str()));
  return unixFileName(currentWorkingDirectory+"/");
}

String getExtension(const String &fileName) {
  return fileName.substr(fileName.findLast("."));
}

String getName(const String &fileName) {
  int k0 = fileName.findLast("/");
  int k1 = fileName.findLast("\\");
  int k = 0;
  if (k0 < k1) k0 = k1;
  if (k0>0) k = k0+1;
  return fileName.substr(k,fileName.findLast("."));
}

String getPath(const String &fileName) {
  int k0 = fileName.findLast("/");
  int k1 = fileName.findLast("\\");
  int k = fileName.length();
  if (k0 < k1) k0 = k1;
  if (k0>0) k = k0+1;
  return fileName.substr(0,k);
}

String resolvePath(const String &b) {
  return b;
}

FileName toFileName(const String &path, const char *fileName) {
  FileName r;
  r.extension = getExtension(fileName);
  r.name = getName(fileName);
  if (String(fileName) == "..") {r.name = ".."; r.extension = "";}
  if (String(fileName) == ".") {r.name = "."; r.extension = "";}
  r.path = resolvePath(path + r.name);
  return r;
}

long toFileSize(long fileSize) {
  return fileSize;
}

FileTime toFileTime(int date, int time) {
  FileTime r;
  r.day = date & 31;
  r.month = (date >> 5) & 15;
  r.year = (date>>5>>4)-20+2000;
  r.minute = (time>>5) & 63;
  r.second = ((time) & 31)*2; // dunno & 31?
  r.hour = (time>>5>>6);
  return r;
}

String dosFileName(const String &dir) {
  String r = String(dir);
  for (int i = 0; i < r.length(); i++) if (r[i]=='/') r[i]='\\';
  return r;
}

String digits2(int value) {
  return (value < 10 ? "0" : "") + String(value);
}

String dateString(const FileTime &time) {
  String r = String(time.year)+"."+digits2(time.month)+"."+digits2(time.day);
  r += " " + digits2(time.hour)+":"+digits2(time.minute)+"."+digits2(time.second);
  return r;
}

String sizeString(long size) {
  if (size == 0) return "0";
  long sizea = abs(size);
  if (sizea < 1024)
    return String(size) + "b";
  if (sizea < 1024 * 1024)
    return String(size/1024) + "." + String(size & 1023) + "kb";
  if (sizea < 1024 * 1024 * 1024)
    return String(size/1024/1024) + "." + String((size/1024) & 1023) + "mb";
  if (sizea < 1024 * 1024 * 1024 * 1024)
    return String(size/1024/1024/1024) + "." + String((size/1024/1024) & 1023) + "gb";
  return size;
}

Array<File> getFiles(const String &dir) {
  Array<File> ret;
  DIR *dirp;
  struct dirent *direntp;
  dirp = opendir( dosFileName(dir).c_str() );
  if( dirp != NULL ) {
    do {
      direntp = readdir( dirp );
      if( direntp == NULL )
        break;
      File r;
      r.name = toFileName(dir,direntp->d_name);
      r.size = toFileSize(direntp->d_size);
      r.date = toFileTime(direntp->d_date,direntp->d_time);
      r.directory = (direntp->d_attr & _A_SUBDIR) != 0;
      ret.push_back(r);
    } while(1==1);
    closedir( dirp );
  }
  return ret;
}

int rtcRead(int reg) {
  int volatile v=0;
  for (int j=1; j < 100; j++) {v++;}
  outp(0x70,reg & 0xff);
  for (int i=1; i < 100; i++) {v++;}
  return inp(0x71) & 0xff;
}

void disableControlBreak() {
  union REGS regs;
  regs.h.ah = 0x33;
  regs.h.al = 1;
  regs.h.dl = 0;
  int386(0x21, &regs, &regs);
}

void createFolder(const String &directoryName) {
  mkdir(dosFileName(directoryName).c_str());
  fileWasWritten = true;
}

void deleteFolder(const String &directoryName) {
  rmdir(dosFileName(directoryName).c_str());
  fileWasWritten = true;
}

void removeFile(const String &fileName) {
  remove(dosFileName(fileName).c_str());
  fileWasWritten = true;
}

void renameFile(const String &oldFileName, const String &newFileName) {
  rename(dosFileName(oldFileName).c_str(), dosFileName(newFileName).c_str());
  fileWasWritten = true;
}

// long file name extensions (maybe you need doslfn.com)
//---------
#define dosErrorCode errno
int dosErrorAdditional = 0;

// int 21h long file name functions (not all implemented here)
#define LFN_RESETDRIVE 0x710d
#define LFN_CREATEDIRECTORY 0x7139
#define LFN_REMOVEDIRECTORY 0x713A
#define LFN_SETCURRENTDIRECTORY 0x713B
#define LFN_DELETEFILE 0x7141
#define LFN_FILEATTRIBS 0x7143
#define LFN_GETCURRENTDIRECTORY 0x7147
#define LFN_FINDFIRSTFILE 0x714e
#define LFN_FINDNEXTFILE 0x714f
#define LFN_MOVERENAME 0x7156
#define LFN_TRUENAME 0x7160
#define LFN_EXTENDEDOPEN 0x716C
#define LFN_GETVOLUMEINFO 0x71a0
#define LFN_FINDCLOSE 0x71a1
#define LFN_GETFILEINFO 0x71a6
#define LFN_TIMECONVERSION 0x71a7
#define LFN_GENERATESHORTNAME 0x71a8
#define LFN_SERVERCREATEOPEN 0x71a9
#define LFN_CREATETERMINATE 0x71aa

bool basicDosCallWithString(uint16_t functionNr, const char *str, bool si = false, uint16_t bx = 0, REGS16 *result = NULL, uint16_t dx = 0) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(strlen(str)+1);
  if (dosInterfaceMemory.ptr == NULL) return false;
  sprintf((char*)dosInterfaceMemory.ptr,"%s",str);
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = functionNr;
  regs.bx = bx;
  regs.dx = dx;
  regs.si = si ? 1 : 0;
  regs.ds = dosInterfaceMemory.segment;
  dosInt386x(0x21, &regs, &regs);
  dosFree(dosInterfaceMemory);
  if (regs.cflag) {
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  if (result != NULL) *result = regs;
  return true;
}

bool doslfnCreateDirectory(const char *name) {
  return basicDosCallWithString(0x7139, name);
}

bool doslfnDeleteDirectory(const char *name) {
  return basicDosCallWithString(0x713a, name);
}

bool doslfnChangeDirectory(const char *name) {
  return basicDosCallWithString(0x713b, name);
}

bool doslfnDeleteFiles(const char *name, bool useWildCards) {
  return basicDosCallWithString(0x7141, name, useWildCards);
}

FileTime doslfnGetFileTime(const char *name) {
  REGS16 regs;
  bool r = basicDosCallWithString(0x7143, name,false,4,&regs);
  return r ? toFileTime(regs.di,regs.cx) : toFileTime(0,0);
}

bool doslfnGetCWD(char *name, uint16_t onDrive) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(MAX_LFN_PATH);
  if (dosInterfaceMemory.ptr == NULL) return false;
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x7147;
  regs.dx = onDrive;
  regs.si = 0;
  regs.ds = dosInterfaceMemory.segment;
  dosInt386x(0x21, &regs, &regs);
  if (regs.cflag) {
    dosFree(dosInterfaceMemory);
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  memcpy(name,(char*)dosInterfaceMemory.ptr,strlen((char*)dosInterfaceMemory.ptr)+1);
  dosFree(dosInterfaceMemory);
  return true;
}

bool doslfnRename(const char *oldName, const char *newName) {
  DOSPOINTER dosInterfaceMemory1 = dosMalloc(strlen(oldName)+1);
  if (dosInterfaceMemory1.ptr == NULL) return false;
  DOSPOINTER dosInterfaceMemory2 = dosMalloc(strlen(newName)+1);
  if (dosInterfaceMemory2.ptr == NULL) {
    memcpy((char*)dosInterfaceMemory1.ptr,oldName,strlen(oldName)+1);
    return false;
  }
  memcpy((char*)dosInterfaceMemory1.ptr,oldName,strlen(oldName)+1);
  memcpy((char*)dosInterfaceMemory2.ptr,newName,strlen(newName)+1);
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x7156;
  regs.ds = dosInterfaceMemory1.segment;
  regs.dx = 0;
  regs.es = dosInterfaceMemory2.segment;
  regs.si = 0;
  dosInt386x(0x21, &regs, &regs);
  dosFree(dosInterfaceMemory1);
  dosFree(dosInterfaceMemory2);
  if (regs.cflag) {
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  return true;
}

bool doslfnPath(char *name, const char *fileName, bool longPath) {
  DOSPOINTER dosInterfaceMemory1 = dosMalloc(strlen(fileName)+1);
  if (dosInterfaceMemory1.ptr == NULL) return false;
  memcpy((char*)dosInterfaceMemory1.ptr,fileName,strlen(fileName)+1);
  DOSPOINTER dosInterfaceMemory2 = dosMalloc(MAX_LFN_PATH);
  if (dosInterfaceMemory2.ptr == NULL) {
    dosFree(dosInterfaceMemory1);
    return false;
  }
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x7160;
  regs.cx = (longPath ? 2 : 1)+0x100;
  regs.ds = dosInterfaceMemory1.segment;
  regs.si = 0;
  regs.es = dosInterfaceMemory2.segment;
  regs.di = 0;
  dosInt386x(0x21, &regs, &regs);
  if (regs.cflag) {
    dosFree(dosInterfaceMemory1);
    dosFree(dosInterfaceMemory2);
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  memcpy(name,(char*)dosInterfaceMemory2.ptr,strlen((char*)dosInterfaceMemory2.ptr)+1);
  dosFree(dosInterfaceMemory1);
  dosFree(dosInterfaceMemory2);
  return true;
}

bool doslfnLongPath(char *name, const char *fileName) {
  return doslfnPath(name, fileName, true);
}

bool doslfnShortPath(char *name, const char *fileName) {
  return doslfnPath(name, fileName,false);
}

int32_t doslfnOpen(const char *fileName, bool read) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(strlen(fileName)+1);
  if (dosInterfaceMemory.ptr == NULL) return false;
  memcpy((char*)dosInterfaceMemory.ptr,fileName,strlen(fileName)+1);
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x716c;
#define SHAREMODE(__COMMIT__,__ERRORHANDLING__,__ALIAS__,__COMPRESS__,__BUFFER__,__INHERIT__,__SHARE__,__ACCESS__)\
  ((__COMMIT__)<<14)|\
  ((__ERRORHANDLING__)<<13)|\
  ((__ALIAS__)<<10)|\
  ((__COMPRESS__)<<9)|\
  ((__BUFFER__)<<8)|\
  ((__INHERIT__)<<7)|\
  ((__SHARE__)<<4)|\
  ((__ACCESS__)<<0)
  regs.bx = SHAREMODE(1,0,0,0,0,0,read?2:3,read?0:1);
  regs.cx = 0; // File Attributes
  regs.dx = ((read ? 0 : 1)<<4)|(read?1:2);
  regs.ds = dosInterfaceMemory.segment;
  regs.si = 0;
  dosInt386x(0x21, &regs, &regs);
  dosFree(dosInterfaceMemory);
  if (regs.cflag) {
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return -1;
  }
  int fileHandle = regs.ax;
  return fileHandle;
}

#define FILEIO_BYTES 8192

bool doslfnRead(int32_t fileHandle, void *dest, uint64_t byteCount) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(FILEIO_BYTES);
  if (dosInterfaceMemory.ptr == NULL) return false;

  while(byteCount>0) {
    REGS16 regs;
    memset(&regs,0,sizeof(regs));
    regs.ax = 0x3f00;
    regs.bx = fileHandle & 0xffff;
    uint32_t readCount = byteCount > FILEIO_BYTES ? FILEIO_BYTES : byteCount;
    regs.cx = readCount;
    regs.ds = dosInterfaceMemory.segment;
    regs.dx = 0;
    dosInt386x(0x21, &regs, &regs);
    if (regs.cflag) {
      dosFree(dosInterfaceMemory);
      dosErrorCode = regs.ax;
      dosErrorAdditional = 0;
      return false;
    }
    memcpy(dest,dosInterfaceMemory.ptr,regs.ax);
    if (regs.ax != readCount) {
      dosFree(dosInterfaceMemory);
      dosErrorCode = 0x00;
      dosErrorAdditional = readCount-regs.ax;
      return false;
    }
    byteCount -= readCount;
    dest = (void*)(((unsigned char*)dest)+readCount);
  }
  dosFree(dosInterfaceMemory);
  return true;
}

bool doslfnWrite(int32_t fileHandle, void *source, uint64_t byteCount) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(FILEIO_BYTES);
  if (dosInterfaceMemory.ptr == NULL) return false;

  do {
    REGS16 regs;
    memset(&regs,0,sizeof(regs));
    regs.ax = 0x4000;
    regs.bx = fileHandle & 0xffff;
    uint32_t writeCount = byteCount > FILEIO_BYTES ? FILEIO_BYTES : byteCount;
    regs.cx = writeCount;
    memcpy(dosInterfaceMemory.ptr,source,writeCount);
    regs.ds = dosInterfaceMemory.segment;
    regs.dx = 0;
    dosInt386x(0x21, &regs, &regs);
    if (regs.cflag) {
      dosFree(dosInterfaceMemory);
      dosErrorCode = regs.ax;
      dosErrorAdditional = 0;
      return false;
    }
    if (regs.ax != writeCount) {
      dosFree(dosInterfaceMemory);
      dosErrorCode = 0x00;
      dosErrorAdditional = writeCount-regs.ax;
      return false;
    }
    byteCount -= writeCount;
    source = (void*)(((unsigned char*)source)+writeCount);
  } while(byteCount>0);
  dosFree(dosInterfaceMemory);
  return true;
}

bool doslfnSeek(int32_t fileHandle, uint64_t pos, uint32_t seekMode) {
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x4200+(seekMode==SEEK_SET?0:0)+(seekMode==SEEK_CUR?1:0)+(seekMode==SEEK_END?2:0);
  regs.bx = fileHandle & 0xffff;
  regs.cx = (pos >> 16) & 0xffff;
  regs.dx = pos & 0xffff;
  dosInt386x(0x21, &regs, &regs);
  if (regs.cflag) {
    return false;
  }
  return true;
}

bool doslfnClose(int32_t fileHandle) {
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax  = 0x3e00;
  regs.bx = fileHandle & 0xffff;
  dosInt386x(0x21, &regs, &regs);
  if (regs.cflag) {
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  return true;
}

uint64_t doslfnTell(int32_t fileHandle) {
  FILE k;
  memset(&k,0,sizeof(FILE));
  k._handle = fileHandle;
  return ftell(&k);
}

uint64_t doslfnSize(int32_t fileHandle) {
  uint64_t cp = doslfnTell(fileHandle);
  doslfnSeek(fileHandle,0,SEEK_END);
  uint64_t r = doslfnTell(fileHandle);
  doslfnSeek(fileHandle,cp,SEEK_SET);
  return r;
}

LFNFILEINFO doslfnInfo(int32_t fileHandle) {
  LFNFILEINFO r;
  memset(&r,0,sizeof(LFNFILEINFO));
  DOSPOINTER dosInterfaceMemory = dosMalloc(sizeof(LFNFILEINFO));
  if (dosInterfaceMemory.ptr == NULL) return r;
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x71a6;
  regs.bx = fileHandle & 0xffff;
  regs.cflag = true;
  regs.ds = dosInterfaceMemory.segment;
  regs.dx = 0;
  dosInt386x(0x21, &regs, &regs);
  if (regs.cflag) {
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    dosFree(dosInterfaceMemory);
    return r;
  }
  memcpy(&r,dosInterfaceMemory.ptr,sizeof(LFNFILEINFO));
  dosFree(dosInterfaceMemory);
  return r;
}

// lowlevel DPMI and DOS calls
//---------
__inline DOSPOINTER dosNullPointer() {
  DOSPOINTER k;
  k.selector = 0;
  k.segment = 0;
  k.ptr = NULL;
  return k;
}

__inline DOSPOINTER makeDosPointer(uint16_t selector, uint16_t segment) {
  DOSPOINTER k;
  k.selector = selector;
  k.segment = segment;
  k.ptr = (void*)((unsigned int)segment * 16);
  return k;
}

DOSPOINTER dosMalloc(uint32_t size) {
  union REGS regs;
  regs.w.ax = 0x100;
  regs.w.bx = (size+15)/16;
  regs.w.cflag = false;
  int386(0x31, &regs, &regs);
  if (regs.w.cflag) {
    dosErrorCode = regs.w.ax;
    dosErrorAdditional = (unsigned int)regs.w.bx*16; // largest allocatable block
    return dosNullPointer();
  }
  return makeDosPointer(regs.w.dx,regs.w.ax);
}

bool dosFree(const DOSPOINTER &mem) {
  union REGS regs;
  regs.w.ax = 0x101;
  regs.w.dx = mem.selector;
  regs.w.cflag = false;
  int386(0x31, &regs, &regs);
  if (regs.w.cflag) {
    dosErrorCode = regs.w.ax;
    dosErrorAdditional = 0;
    return false;
  }
  return true;
}

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
} _REGS16;

uint16_t dosInt386x(uint32_t intno, REGS16 *in, REGS16 *out) {
  _REGS16 rmregs;
  union REGS r;
  struct SREGS sr;
  segread(&sr);
  sr.es = sr.ds; // flat model?
  memset(&rmregs,0,sizeof(rmregs));
  rmregs.eax = in->ax; 
  rmregs.ebx = in->bx; 
  rmregs.ecx = in->cx; 
  rmregs.edx = in->dx; 
  rmregs.esi = in->si; 
  rmregs.edi = in->di; 
  rmregs.ds = in->ds;
  rmregs.es = in->es;
  rmregs.flags = in->cflag ? 0x01 : 0x00;
  r.w.ax = 0x300;
  r.h.bl = intno;
  r.h.bh = 0;
  r.w.cx = 0;
  r.x.edi = (unsigned)&rmregs;
  int386x(0x31,&r,&r,&sr);
  out->ax = rmregs.eax; 
  out->bx = rmregs.ebx; 
  out->cx = rmregs.ecx; 
  out->dx = rmregs.edx; 
  out->si = rmregs.esi; 
  out->di = rmregs.edi; 
  out->es = rmregs.es;
  out->cs = rmregs.cs;
  out->ss = rmregs.ss;
  out->ds = rmregs.ds;
  out->cflag = (rmregs.flags & 0x01) ? true : false;
  return out->ax;
}
