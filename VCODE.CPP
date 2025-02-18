#include "vcode.hpp"
#include "blit.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "dos.hpp"
#include "clipbrd.hpp"
#include "util.hpp"

#define HIGHLIGHT_EMPTY 180
#define HIGHLIGHT_COMMENT (HIGHLIGHT_EMPTY+1)
#define HIGHLIGHT_STRING (HIGHLIGHT_EMPTY+2)
#define HIGHLIGHT_NUMBER (HIGHLIGHT_EMPTY+3)
#define HIGHLIGHT_MATH (HIGHLIGHT_EMPTY+4)
#define HIGHLIGHT_BOOLEAN (HIGHLIGHT_EMPTY+5)
#define HIGHLIGHT_SCOPE (HIGHLIGHT_EMPTY+6)
#define HIGHLIGHT_SPECIAL (HIGHLIGHT_EMPTY+7)

void updateSyntaxHiLight(const String &text, String &colors) {
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
  Array<const char *> wordTable;
  wordTable.push_back("void");
  wordTable.push_back("int");
  wordTable.push_back("unsigned");
  wordTable.push_back("signed");
  wordTable.push_back("const");
  wordTable.push_back("char");
  wordTable.push_back("float");
  wordTable.push_back("double");
  wordTable.push_back("new");
  wordTable.push_back("delete");
  wordTable.push_back("malloc");
  wordTable.push_back("free");
  wordTable.push_back("set");
  wordTable.push_back("echo");
  wordTable.push_back("rem");
  wordTable.push_back("cd");
  wordTable.push_back("path");
  wordTable.push_back("mode");
  wordTable.push_back("public");
  wordTable.push_back("class");
  wordTable.push_back("virtual");
  wordTable.push_back("bool");
  wordTable.push_back("true");
  wordTable.push_back("false");
  wordTable.push_back("null");
  wordTable.push_back("nullptr");
  wordTable.push_back("for");
  wordTable.push_back("do");
  wordTable.push_back("while");
  wordTable.push_back("break");
  wordTable.push_back("continue");
  wordTable.push_back("return");
  wordTable.push_back("template");
  wordTable.push_back("typename");
  wordTable.push_back("static");
  wordTable.push_back("volatile");
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
        for (int j = 0; j < wordTable.size(); j++) {
          if (word == wordTable[j] || (firstChar >= 'A' && firstChar <='Z')) {
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
  }
  return COLOR_TEXT;
}

void VCode::draw() {
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_TEXTBACKGROUND);

  Point pos = position - textPos;
  int line = 0;
  while(pos.y < position.y+size.height) {
    String lineNr = String(line);
    drawString(position.x+size.width-lineNr.pixelWidth(),pos.y,lineNr, COLOR_LINENUMBERS);
    pos.y += CHAR_HEIGHT;
    line++;
  }
  pos = position - textPos;
  int lineX = 0;
  int lineY = 0;
  cursorI = -1;
  topLine = -1;
  bottomLine = -1;
  cursorIX=-1;
  Point eofPos;
  for (int i = 0; i < text.length() || (pos.y < position.y + size.height); i++) {
    char c = i < text.length() ? text[i] : '\n';
    if (i == text.length()-1) {String k=" "; k[0]=c;eofPos = pos; pos.x += k.pixelWidth();}
    char l = i < text.length() ? colors[i] : HIGHLIGHT_EMPTY;
    bool selected = (selectionStart>=0&&selectionEnd>=0)&&((i>=selectionStart&&i<selectionEnd)||(i>=selectionEnd&&i<selectionStart));
    if (i < text.length()) textLastLine=lineY;
    if (i == cursorRequiresI) {
      cursorX = lineX;
      cursorY = lineY;
      cursorRequiresI = -1;
    }
    if (lineY == cursorY) {
      if (cursorRequiresX!=-1 && pos.x>=cursorRequiresX) {cursorRequiresX=-1;cursorX = lineX;}
      if (lineX == cursorX) {
        cursorI = i;
        cursorPos = pos;
      }
    }
    if (pos.y < position.y + size.height) bottomLine = lineY;
    if (topLine == -1 && pos.y >= position.y) topLine = lineY;
    switch(c) {
      case '\n': {if(selected)drawBox(pos.x,pos.y,pos.x+size.width,pos.y+CHAR_HEIGHT,COLOR_SELECTION);if(lineY==cursorY&&cursorI==-1) {cursorI=i;cursorPos=pos;cursorIX=lineX;} lineX=0;lineY++;pos.y += CHAR_HEIGHT; pos.x = position.x - textPos.x;continue;} break;
    }
    if (selected) {
      String k; k.resize(1); k[0]=c;
      drawBox(pos.x,pos.y,pos.x + k.pixelWidth(),pos.y+CHAR_HEIGHT,COLOR_SELECTION);
    }
    drawChar(pos.x, pos.y, c, getHiLightColor(l));
    lineX++;
  }
  if (cursorIX != -1) cursorPos.x += (cursorX - cursorIX)*String(" ").pixelWidth();
  if (cursorRequiresX!=-1) {
    if (cursorPos.x<cursorRequiresX) {
      while(cursorPos.x<cursorRequiresX) {
        cursorPos.x += String(" ").pixelWidth();
        cursorX++;
      }
    } else {
      while(cursorPos.x>cursorRequiresX) {
        cursorPos.x -= String(" ").pixelWidth();
        cursorX--;
      }
    }
  }
  if (textLastLine>0) {
    scrollBoxY = textPos.y*frameBufferHeight/(textLastLine*CHAR_HEIGHT);
    drawBox(frameBufferWidth-4*frameBufferWidth/320,scrollBoxY,frameBufferWidth,scrollBoxY+2*frameBufferHeight/200,COLOR_RED);
  }
  cursorRequiresX=-1;
  cursorRequiresI = -1;
  if (cursorPos.x > position.x + size.width) textPos.x += CHAR_HEIGHT;
  if (cursorPos.x < position.x) {textPos.x -= CHAR_HEIGHT; if (textPos.x < 0) textPos.x = 0;}
  if (selectionEnd==-2) {selectionEnd = cursorI;}
  drawStringOutlined(eofPos.x,eofPos.y,"<EOF>",COLOR_RED,COLOR_YELLOW);
  drawCursor();
  String status = "<CTRL-L> <CTRL-S> " + message + sizeString(text.length()) + "  " + String(cursorX)+":"+String(cursorY);
  drawBox(position.x,position.y+size.height-CHAR_HEIGHT,position.x+size.width,position.y+size.height,COLOR_STATUSBAR);
  drawString(position.x,position.y+size.height-CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
  popRect();
}

void VCode::expand() {
    if (cursorI>text.length()) {
      for (int i = cursorI-text.length()-1; i >= 0; i--) {
        text += "\n";
        colors += "\n";
      }
    }
    if (cursorIX != -1) {
      for (int i = cursorX-cursorIX-1; i >= 0; i--) {
        text.insert(cursorI,' ');
        colors.insert(cursorI,' ');
        cursorI++;
      }
    }
}

void VCode::update() {
  if (cursorY < topLine) textPos.y -= (topLine-cursorY)*CHAR_HEIGHT;
  if (cursorY >= bottomLine) textPos.y+=(cursorY-bottomLine+1)*CHAR_HEIGHT;
  if (mouseB==1&&mouseX>position.x+size.width-4*frameBufferWidth/320) {
    textPos.y = (mouseY-position.y)*textLastLine*CHAR_HEIGHT/size.height;
    cursorY=textPos.y/CHAR_HEIGHT+1;
    return;
  }

  if (strgKey) {
    if (key==VK_L_CTRL && (!fileName.empty())) {
      cursorRequiresI=cursorI;
      text = stringFromFile(fileName);
      colors = text;
      updateSyntaxHiLight(text,colors);
      return;
    }
    if (key==VK_S_CTRL && (!fileName.empty())) {
      stringToFile(text,fileName);
      return;
    }
  }

  if (shiftKey) {
    if (key == VK_UP||key == VK_DOWN||key == VK_LEFT||key==VK_RIGHT||key==VK_PAGEUP||key==VK_PAGEDOWN) {
      if (shiftReleased) selectionStart = cursorI;
      selectionEnd = -2;
      shiftReleased = false;
    }
  } else {
    shiftReleased = true;
    if (key == VK_UP||key == VK_DOWN||key == VK_LEFT||key==VK_RIGHT||key==VK_PAGEUP||key==VK_PAGEDOWN) {
      selectionStart=-1;
      selectionEnd=-1;
    }
  }
  if (selectionStart>=0&&selectionEnd>=0) {
    if (key == VK_ENTF || key == VK_DELETE) {
      if (selectionStart>selectionEnd) {
        int t = selectionStart;
        selectionStart = selectionEnd;
        selectionEnd = t;
      }
      if (selectionEnd>text.length()) selectionEnd = text.length();
      if (selectionStart>text.length()) selectionStart = text.length();
      text.erase(selectionStart,selectionEnd-selectionStart);
      colors.erase(selectionStart,selectionEnd-selectionStart);
      updateSyntaxHiLight(text,colors);
      cursorRequiresI=selectionStart;
      selectionStart = -1;
      selectionEnd = -1;
      key = VK_NONE;
    }
    if (key == VK_C_CTRL && strgKey) {
      if (selectionStart>selectionEnd) {
        int t = selectionStart;
        selectionStart = selectionEnd;
        selectionEnd = t;
      }
      if (selectionEnd>text.length()) selectionEnd = text.length();
      if (selectionStart>text.length()) selectionStart = text.length();
      clipBoard_string=text.substr(selectionStart,selectionEnd);
      selectionStart = -1;
      selectionEnd = -1;
      key = VK_NONE;
    }
  }
  if (key == VK_V_CTRL && strgKey && cursorI>=0) {
    expand();
    text.insert(cursorI,clipBoard_string);
    colors.insert(cursorI,clipBoard_string);
    updateSyntaxHiLight(text,colors);
    key = VK_NONE;
  }

  if (key == VK_ESCAPE) {}
  else
  if (key == VK_POS1) {
    if (cursorI != -1) {
      if (cursorI>text.length()) {
        cursorRequiresI = cursorI;
      } else
        for (int i = cursorI-1; i >= -1; i--) {
          if (i==-1||text[i]=='\n') {cursorRequiresI = i+1;break;}
        }
    }
  } else
  if (key == VK_END) {
    if (cursorI != -1) {
      if (cursorI>=text.length()) {
        cursorRequiresI = cursorI;
      } else
      if (text[cursorI]=='\n') {
        cursorRequiresI = cursorI;
      } else
        for (int i = cursorI; i < text.length(); i++) {
          if (text[i]=='\n') break;
          cursorX++;
        }
    }
  } else
  if (key == VK_UP) {cursorRequiresX=cursorPos.x;cursorY--; if (cursorY < 0) cursorY = 0;}
  else
  if (key == VK_DOWN) {cursorRequiresX=cursorPos.x;cursorY++;}
  else
  if (key == VK_PAGEUP) {cursorY-=16;if(cursorY<0)cursorY=0; }
  else
  if (key == VK_PAGEDOWN) {cursorRequiresX=cursorPos.x;cursorY+=16; }
  else
  if (key == VK_LEFT || key == VK_LEFT_STRG) {int d = strgKey?4:1; cursorX -=d; if (cursorX<0) {cursorX=0;cursorRequiresI = cursorI-1;}}
  else
  if (key == VK_RIGHT || key == VK_RIGHT_STRG) {cursorX+=strgKey?4:1;}
  else {
    if (cursorI != -1) {
      if (key == VK_ENTF) {
          expand();
          text.erase(cursorI,1);
          colors.erase(cursorI,1);
          updateSyntaxHiLight(text,colors);
      }
      else
      if (key == VK_DELETE) {
        if (cursorI>0) {
          expand();
          text.erase(cursorI-1,1);
          colors.erase(cursorI-1,1);
          cursorRequiresI = cursorI-1;
          updateSyntaxHiLight(text,colors);
        }
      }
      else
      if (key == VK_TAB||key == VK_TAB_SHIFT) {
        expand();
        if (selectionStart>=0&&selectionEnd>=0) {
          int searchStart = selectionStart;
          int searchEnd = selectionEnd;
          if (searchStart>searchEnd) {
            int t = searchStart;
            searchStart=searchEnd;
            searchEnd=t;
          }
          searchStart--;
          for(int i = searchStart; i < searchEnd-1; i++) {
            for (int j = 0; j < 2; j++) {
              if ((i==-1)||(text[i]=='\n')) {
                if (!shiftKey) {
                  text.insert(i+1," ");
                  colors.insert(i+1," ");
                  searchEnd++;
                } else {
                  if (text[i+1]==' ') {
                    text.erase(i+1,1);
                    colors.erase(i+1,1);
                    searchEnd--;
                  }
                }
              }
            }
          }
          if (selectionStart>selectionEnd)
            selectionStart=searchEnd;
          else
            selectionEnd=searchEnd;
          updateSyntaxHiLight(text,colors);

        } else {
          for (int x = 4-(cursorX & 3); x >= 0; x--) {
            text.insert(cursorI,' ');
            colors.insert(cursorI,' ');
            cursorX++;
          }
          updateSyntaxHiLight(text,colors);
        }
      }
      else
      if (key == VK_INSERT) {
        overwriteMode = !overwriteMode;
        key = VK_NONE;
      }
      else
      if (key < 256 && key != 0) {
        if (key == VK_ENTER) {
          expand();
          text.insert(cursorI,'\n');
          colors.insert(cursorI,'\n');
          updateSyntaxHiLight(text,colors);
          cursorY++;
          cursorX=0;
        } else {
          expand();
          if (overwriteMode&&text[cursorI]!='\n') {
            text[cursorI] = key;
          } else {
            text.insert(cursorI,key);
            colors.insert(cursorI,key);
          }
          updateSyntaxHiLight(text,colors);
          cursorX++;
        }
      }
    }
  }
}

void VCode::drawCursor() {
  if (cursorI != -1) {
    static int i = 0;i++;
    if (i & 8) {
      if (!overwriteMode) {
        drawBox(cursorPos.x,cursorPos.y,cursorPos.x+1,cursorPos.y+CHAR_HEIGHT,COLOR_CURSOR);
        drawBox(cursorPos.x+1,cursorPos.y,cursorPos.x+2,cursorPos.y+CHAR_HEIGHT,COLOR_RED);
      } else {
        drawBox(cursorPos.x,cursorPos.y,cursorPos.x+CHAR_HEIGHT,cursorPos.y+CHAR_HEIGHT,COLOR_CURSOR);
      }
    }
  }
}