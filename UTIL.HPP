#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include "string.hpp"
#include "array.hpp"

extern bool fileWasWritten;

void make0d0ato0a(String &r);
void make0ato0d0a(String &r);

extern String currentToolString;
String currentTimeString();
String stringFromFile(const String &fileName, bool textFile);
void stringToFile(const String &string,const String &fileName, bool textFile);
bool copyFile(const String &destFileName, const String &sourceFileName);
bool moveFile(const String &destFileName, const String &sourceFileName);
bool copyFiles(const String &destPath, Array<String> &sourcePaths);
bool moveFiles(const String &destPath, Array<String> &sourcePaths);
bool removeFiles(Array<String> &sourcePaths);
void saveScreenShot(const char *fileName, unsigned int *data, int width,int height);

#endif //__UTIL_HPP__