#ifndef __DOS_HPP__
#define __DOS_HPP__

#include "String.hpp"
#include "Array.hpp"

typedef struct FileTime {
  int day;
  int month;
  int year;
  int hour;
  int minute;
  int second;
  int millisecond;
} FileTime;

typedef struct FileName {
  String path;
  String name;
  String extension;
} FileName;

typedef long FileSize;

typedef struct File {
  FileName name;
  FileTime date;
  FileSize size;
  bool directory;
} File;


short getFileDate(const String &filePath);
short getFileTime(const String &filePath);

String getName(const String &fileName);
String getExtension(const String &fileName);
String getPath(const String &fileName);
String getCWD();
String getExtension(const String &path);
String dosFileName(const String &dir);
String dateString(const FileTime &time);
String sizeString(long size);
Array<File> getFiles(const String &dir);

int rtcRead(unsigned char reg);
void disableControlBreak();

void createFolder(const String &directoryName);
void deleteFolder(const String &folderName);
void removeFile(const String &fileName);
void renameFile(const String &oldFileName, const String &newFileName);

// long file name extensions (maybe you need doslfn.com)
//---------
#define MAX_LFN_PATH 260
typedef struct {
  uint32_t attributes;
  uint64_t creationTime;
  uint64_t accessTime;
  uint64_t modificationTime;
  uint32_t driveId;
  uint64_t fileSize;
  uint32_t linkCount;
  uint64_t fileId;
} LFNFILEINFO;
bool doslfnCreateDirectory(const char *name);
bool doslfnDeleteDirectory(const char *name);
bool doslfnChangeDirectory(const char *name);
bool doslfnDeleteFiles(const char *name, bool useWildCards = false);
FileTime doslfnGetFileTime(const char *name);
bool doslfnGetCWD(char *name, uint16_t onDrive=0); // no drive letters returned
bool doslfnRename(const char *oldName, const char *newName);
bool doslfnLongPath(char *name, const char *fileName);
bool doslfnShortPath(char *name, const char *fileName);
int32_t doslfnOpen(const char *fileName, bool read);
bool doslfnRead(int32_t fileHandle, void *dest, uint64_t byteCount);
bool doslfnWrite(int32_t fileHandle, void *source, uint64_t byteCount);
bool doslfnSeek(int32_t fileHandle, uint64_t pos, uint32_t seekMode);
bool doslfnClose(int32_t fileHandle);
uint64_t doslfnTell(int32_t fileHandle);
uint64_t doslfnSize(int32_t fileHandle);
LFNFILEINFO doslfnInfo(int32_t fileHandle); // maybe not supported: errno = 0x7100 in that case

// lowlevel DPMI and DOS calls
//---------
typedef struct {
  uint16_t selector; // for protected mode
  uint16_t segment; // for 16 bit dos mode (offset is 0)
  void *ptr; // flat mode memory to write to
} DOSPOINTER;

typedef struct {
  uint16_t di;
  uint16_t si;
  uint16_t bp;
  uint16_t reserved;
  uint16_t bx;
  uint16_t dx;
  uint16_t cx;
  uint16_t ax;
  uint16_t es,ds,fs,gs,ip,cs,sp,ss;
  bool cflag;
} REGS16;

DOSPOINTER dosMalloc(uint32_t size);
bool dosFree(const DOSPOINTER &mem);
uint16_t dosInt386x(uint32_t intNumber, REGS16 *in, REGS16 *out);

#endif //__DOS_HPP__