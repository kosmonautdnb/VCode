#include "syntax.hpp"
#include "colors.hpp"

Array<const char *> hiLightTable;

void initSyntaxHiLighting() {
  hiLightTable.clear();
  hiLightTable.push_back("const");
  hiLightTable.push_back("void");
  hiLightTable.push_back("signed");
  hiLightTable.push_back("unsigned");
  hiLightTable.push_back("int");
  hiLightTable.push_back("short");
  hiLightTable.push_back("char");
  hiLightTable.push_back("float");
  hiLightTable.push_back("double");
  hiLightTable.push_back("new");
  hiLightTable.push_back("delete");
  hiLightTable.push_back("malloc");
  hiLightTable.push_back("free");
  hiLightTable.push_back("set");
  hiLightTable.push_back("echo");
  hiLightTable.push_back("rem");
  hiLightTable.push_back("cd");
  hiLightTable.push_back("path");
  hiLightTable.push_back("mode");
  hiLightTable.push_back("public");
  hiLightTable.push_back("class");
  hiLightTable.push_back("virtual");
  hiLightTable.push_back("bool");
  hiLightTable.push_back("true");
  hiLightTable.push_back("false");
  hiLightTable.push_back("null");
  hiLightTable.push_back("nullptr");
  hiLightTable.push_back("for");
  hiLightTable.push_back("do");
  hiLightTable.push_back("while");
  hiLightTable.push_back("break");
  hiLightTable.push_back("continue");
  hiLightTable.push_back("return");
  hiLightTable.push_back("template");
  hiLightTable.push_back("typename");
  hiLightTable.push_back("static");
  hiLightTable.push_back("volatile");
  hiLightTable.push_back("extern");
}

void updateSyntaxHiLight(const String &text, String &colors, const String &toFind) {
  if (text.length() != colors.length())
    exit(0);
  const int l = colors.length();
  char c = 0;
  char lc = 0;
  char llc = 0;
  int i;
  // mark comments
  int comment = 0;
  int lineComment = 0;
  for (i = 0; i < l; lc = c,i++) {
    c = text[i];
    if (c == '/' && lc == '/' && comment == 0) {colors[i-1] = HIGHLIGHT_COMMENT;comment++;lineComment++;}
    if (c == '*' && lc == '/' && comment == 0) {colors[i-1] = HIGHLIGHT_COMMENT;comment++;}
    if (c == '\n' && lineComment!=0) {comment--;lineComment--;}
    if (c == '/' && lc == '*' && comment != 0 && lineComment==0) {comment--;}
    if (comment!=0) {
      colors[i] = HIGHLIGHT_COMMENT;
    } else colors[i] = HIGHLIGHT_EMPTY;
  }
  // mark strings
  lc = 0;
  llc = 0;
  int strings = 0;
  int strings2 = 0;
  for (i = 0; i < l; llc = lc, lc = c,i++) {
    c = text[i];
    if (colors[i]!=HIGHLIGHT_EMPTY) continue;
    const bool before = lc == '\\' && (!(llc == '\\'));
    if (strings2 == 0 && c == '"' && (!before) ) {colors[i] = HIGHLIGHT_STRING;strings^=1;}
    if (strings == 0 && c == '\'' && (!before)) {colors[i] = HIGHLIGHT_STRING;strings2^=1;}
    if (strings!=0||strings2!=0) colors[i] = HIGHLIGHT_STRING;
  }
  // mark numbers
  int numbers=0;
  lc = 0;
  for (i = 0; i < l; lc = c,i++) {
    c = text[i];
    if (colors[i]!=HIGHLIGHT_EMPTY) continue;
    const bool isAlphabet = (c >= 'a' && c <= 'z')||(c >= 'A' && c <= 'Z')||c=='_'||c=='#';
    const bool isNumber = (c >= '0' && c <= '9')||c=='.';
    const bool lcWasFull = (lc >= 'a' && lc <= 'z')||(lc >= 'A' && lc <= 'Z')||c=='_'||c=='#';
    const bool lcWasNumber = (lc >= '0' && lc <= '9')||c=='.';
    if (numbers == 0 && isNumber && (!(lcWasFull||lcWasNumber))) numbers=1;
    if (numbers != 0 && (!(isNumber||isAlphabet))) numbers=0;
    if (numbers!=0) colors[i] = HIGHLIGHT_NUMBER;
  }
  // mark single or double special chars
  for (i = 0; i < l; lc = c,i++) {
    c = text[i];
    if (colors[i]!=HIGHLIGHT_EMPTY) continue;
    switch(c) {
      case '$':
      case '@': colors[i] = HIGHLIGHT_SPECIAL; break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '>':
      case '<': colors[i] = HIGHLIGHT_MATH; break;
      case '&':
      case '|':
      case '!':
      case '^':
      case '~': colors[i] = HIGHLIGHT_BOOLEAN; break;
      case '(':
      case ')':
      case '[':
      case ']':
      case '{':
      case '}':
      case '.':
      case ',':
      case ';':
      case ':': colors[i] = HIGHLIGHT_SCOPE; break;
    }
  }
  // mark keywords
  int wordStart = 0;
  bool somethingFound = true;
  for (i = 0; i < l+1; i++) {
    c = i < l ? text[i] : ' ';
    if (colors[i]==HIGHLIGHT_COMMENT||colors[i]==HIGHLIGHT_STRING) continue;
    const bool isAlphabet = (c >= 'a' && c <= 'z')||(c >= 'A' && c <= 'Z')||c=='_'||c=='#';
    const bool isNumber = (c >= '0' && c <= '9');
    if ((!isNumber)&&(!isAlphabet)) {
      if (somethingFound) {
        String word = text.substr(wordStart,i);
        int firstChar = word[0];
        word=toLower(word);
        for (int j = 0; j < hiLightTable.size(); j++) {
          if (word == hiLightTable[j] || (firstChar >= 'A' && firstChar <='Z')) {
            for (int k = wordStart; k < i; k++) {
              colors[k] = HIGHLIGHT_SPECIAL;
            }
            break;
          }
        }
      }
      wordStart = i+1;
      somethingFound = false;
    } else {
      somethingFound = true;
    }
  }
  // mark preprocessor directives
  int preprocessor = 0;
  for (i = 0; i < l; lc = c,i++) {
    c = text[i];
    if (colors[i]!=HIGHLIGHT_EMPTY) continue;
    if (c == '#') {preprocessor=1;}
    if (c == '\n') {preprocessor=0;}
    if (preprocessor!=0) {
      colors[i] = HIGHLIGHT_SPECIAL;
    }
  }
  // mark find
  const int l2 = toFind.length();
  if (l2 > 0) {
    for (i = 0; i < l; i++) {
      int j;
      for (j = 0; j < l2 && j+i<l; j++) {
        if (text[i+j]!=toFind[j]) break;
      }
      if (j == l2) {
        for (j = 0; j < l2; ++j)
          colors[i+j]=HIGHLIGHT_FIND;
      }
    }
  }
}

unsigned int getHiLightColor(int l) {
  switch(l) {
  case HIGHLIGHT_COMMENT: return COLOR_HILIGHT_COMMENT; break;
  case HIGHLIGHT_STRING: return COLOR_HILIGHT_STRING; break;
  case HIGHLIGHT_NUMBER: return COLOR_HILIGHT_NUMBER; break;
  case HIGHLIGHT_MATH: return COLOR_HILIGHT_MATH; break;
  case HIGHLIGHT_BOOLEAN: return COLOR_HILIGHT_BOOLEAN; break;
  case HIGHLIGHT_SCOPE: return COLOR_HILIGHT_SCOPE; break;
  case HIGHLIGHT_SPECIAL: return COLOR_HILIGHT_SPECIAL; break;
  case HIGHLIGHT_FIND: return COLOR_HILIGHT_FIND; break;
  }
  return COLOR_TEXT;
}

unsigned int getHiLightBackgroundColor(int l) {
  switch(l) {
  case HIGHLIGHT_FIND: return COLOR_HILIGHT_FIND_BACKGROUND; break;
  }
  return 0;
}