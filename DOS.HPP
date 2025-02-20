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

String getName(const String &fileName);
String getExtension(const String &fileName);
String getPath(const String &fileName);
String getCWD();
String getExtension(const String &path);
String dosFileName(const String &dir);
String dateString(const FileTime &time);
String sizeString(long size);
Array<File> getFiles(const String &dir);

int rtcRead(int reg);
void disableControlBreak();

void createFolder(const String &directoryName);
void deleteFolder(const String &folderName);
void removeFile(const String &fileName);
void renameFile(const String &oldFileName, const String &newFileName);

#endif //__DOS_HPP__