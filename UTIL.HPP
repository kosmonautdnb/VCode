#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include "string.hpp"

extern bool fileWasWritten;

String currentTimeString();
String stringFromFile(const String &fileName);
void stringToFile(const String &string,const String &fileName);

#endif //__UTIL_HPP__