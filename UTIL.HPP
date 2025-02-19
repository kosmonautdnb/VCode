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
bool copyFiles(const String &destPath, Array<String> &sourcePaths);

#endif //__UTIL_HPP__