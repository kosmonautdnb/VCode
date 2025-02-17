#include "vcode.hpp"
#include "blit.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "dos.hpp"

void VCode::draw() {
  String status = "" + message;
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
    drawChar(pos.x,pos.y,c, COLOR_TEXT);
    if (i == text.length()-1) eofPos = pos;
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
  cursorRequiresX=-1;
  cursorRequiresI = -1;
  if (cursorPos.x > position.x + size.width) textPos.x += CHAR_HEIGHT;
  if (cursorPos.x < position.x) {textPos.x -= CHAR_HEIGHT; if (textPos.x < 0) textPos.x = 0;}
  if (selectionEnd==-2) {selectionEnd = cursorI;}
  drawStringOutlined(eofPos.x,eofPos.y,"<EOF>",COLOR_RED,COLOR_YELLOW);
  drawCursor();
  status += " " + String(cursorX)+":"+String(cursorY)+":"+sizeString(text.length());

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
      zwischenAblage=text.substr(selectionStart,selectionEnd);
      selectionStart = -1;
      selectionEnd = -1;
      key = VK_NONE;
    }
  }
  if (key == VK_V_CTRL && strgKey && cursorI>=0) {
    expand();
    text.insert(cursorI,zwischenAblage);
    colors.insert(cursorI,zwischenAblage);
    key = VK_NONE;
  }

  if (key == VK_ESCAPE) {}
  else
  if (key == VK_POS1) {
    if (cursorI != -1) {
      if (cursorI>text.length()) {
        cursorRequiresI = cursorI;
      } else
        for (int i = cursorI-1; i >= 0; i--) {
          if (text[i]=='\n') break;
          cursorRequiresI = i;
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
  if (key == VK_UP) {cursorRequiresX=cursorPos.x;cursorY--; if (cursorY < 0) cursorY = 0; if (cursorY<topLine) textPos.y-=CHAR_HEIGHT; if (textPos.y<0) textPos.y=0;}
  else
  if (key == VK_DOWN) {cursorRequiresX=cursorPos.x;cursorY++; if (cursorY >= bottomLine) textPos.y+=CHAR_HEIGHT;}
  else
  if (key == VK_PAGEUP) {cursorY-=16;if(cursorY<0)cursorY=0; if (cursorY < topLine) textPos.y -= (topLine-cursorY)*CHAR_HEIGHT;}
  else
  if (key == VK_PAGEDOWN) {cursorRequiresX=cursorPos.x;cursorY+=16; if (cursorY >= bottomLine) textPos.y+=(cursorY-bottomLine+1)*CHAR_HEIGHT;}
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
      }
      else
      if (key == VK_DELETE) {
        if (cursorI>0) {
          expand();
          text.erase(cursorI-1,1);
          colors.erase(cursorI-1,1);
          cursorRequiresI = cursorI-1;
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
        } else {
          for (int x = 4-(cursorX & 3); x >= 0; x--) {
            text.insert(cursorI,' ');
            colors.insert(cursorI,' ');
            cursorX++;
          }
        }
      }
      else
      if (key < 256 && key != 0) {
        if (key == VK_ENTER) {
          expand();
          text.insert(cursorI,'\n');
          colors.insert(cursorI,'\n');
          cursorY++;
          cursorX=0;
        } else {
          expand();
          text.insert(cursorI,key);
          colors.insert(cursorI,key);
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
      drawBox(cursorPos.x,cursorPos.y,cursorPos.x+1,cursorPos.y+CHAR_HEIGHT,COLOR_CURSOR);
      drawBox(cursorPos.x+1,cursorPos.y,cursorPos.x+2,cursorPos.y+CHAR_HEIGHT,COLOR_RED);
    }
  }
}