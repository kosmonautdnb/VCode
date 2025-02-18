#ifndef __SYNTAX_HPP__
#define __SYNTAX_HPP__

#include "string.hpp"
#include "array.hpp"

#define HIGHLIGHT_EMPTY 180
#define HIGHLIGHT_COMMENT (HIGHLIGHT_EMPTY+1)
#define HIGHLIGHT_STRING (HIGHLIGHT_EMPTY+2)
#define HIGHLIGHT_NUMBER (HIGHLIGHT_EMPTY+3)
#define HIGHLIGHT_MATH (HIGHLIGHT_EMPTY+4)
#define HIGHLIGHT_BOOLEAN (HIGHLIGHT_EMPTY+5)
#define HIGHLIGHT_SCOPE (HIGHLIGHT_EMPTY+6)
#define HIGHLIGHT_SPECIAL (HIGHLIGHT_EMPTY+7)

extern Array<const char *> hiLightTable;

void initSyntaxHiLighting();
void updateSyntaxHiLight(const String &text, String &colors);
unsigned int getHiLightColor(int l);

#endif //__SYNTAX_HPP__