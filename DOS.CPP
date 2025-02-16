#include <direct.h>
#include "Dos.hpp"
#include <stdio.h>

String getExtension(const String &fileName) {
  return fileName.substr(fileName.findLast("."));
}

String getName(const String &fileName) {
  return fileName.substr(0,fileName.findLast("."));
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
  r.second = (time) & 31; // dunno & 31?
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
  if (size < 1024)
    return String(size) + "b";
  if (size < 1024 * 1024)
    return String(size/1024) + "." + String(size & 1023) + "kb";
  if (size < 1024 * 1024 * 1024)
    return String(size/1024/1024) + "." + String((size/1024) & 1023) + "mb";
  if (size < 1024 * 1024 * 1024 * 1024)
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
      r.directory = direntp->d_attr == _A_SUBDIR;
      ret.push_back(r);
    } while(1==1);
    closedir( dirp );
  }
  return ret;
}