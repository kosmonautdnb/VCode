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

String getExtension(const String &path);
String dosFileName(const String &dir);
String dateString(const FileTime &time);
String sizeString(long size);
Array<File> getFiles(const String &dir);

#endif //__DOS_HPP__