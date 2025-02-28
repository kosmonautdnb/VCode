#include "vhex.hpp"
#include "blit.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "dos.hpp"
#include "util.hpp"
#include "valert.hpp"
#include "vtxtalrt.hpp"

void VHex::draw() {
  static int time = 0; time++;
  char *hextab = "0123456789ABCDEF";
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_TEXTBACKGROUND);

  int ONELINE = 16 * frameBufferWidth / 320;

  int textLastLine = (text.length()+ONELINE-1)/ONELINE;
  if (textLastLine>0) {
    scrollBoxY = textPos.y*size.height/(textLastLine*CHAR_HEIGHT);
    drawBox(position.x+size.width-4*frameBufferWidth/320,position.y+scrollBoxY,position.x+size.width,position.y+scrollBoxY+2*frameBufferHeight/200,COLOR_SCROLL);
  }

  int xp2 = 0;
  int topLine = textPos.y / CHAR_HEIGHT;
  int bottomLine = (textPos.y+size.height-CHAR_HEIGHT) / CHAR_HEIGHT;
  int bottom = bottomLine*ONELINE; if (bottom > text.length()) bottom=text.length();
  int top = topLine*ONELINE; if (top > text.length()) top=text.length();
  int currentHex = 0;
  int lastHex = 0;
  for (int i = top; i < bottom; i++) {
    char c = text[i];
    int x = (i % ONELINE);
    int y = (i / ONELINE);
    int xp = x*CHAR_HEIGHT*2 + CHAR_HEIGHT*7-textPos.x;
    int yp = y*CHAR_HEIGHT-textPos.y;
    if (x == 0) {
      xp2 = xp+CHAR_HEIGHT*ONELINE*2;
      int col = COLOR_LINENUMBERS;
      if (cursorY == y) col = COLOR_CURSOR;
      int xp3 = -textPos.x;
      drawChar(xp3,yp,hextab[(i / 16 / 16 / 16 / 16 / 16 / 16 / 16)&15],col);
      drawChar(xp3,yp,hextab[(i / 16 / 16 / 16 / 16 / 16 / 16)&15],col);
      drawChar(xp3,yp,hextab[(i / 16 / 16 / 16 / 16 / 16)&15],col);
      drawChar(xp3,yp,hextab[(i / 16 / 16 / 16 / 16)&15],col);
      drawChar(xp3,yp,hextab[(i / 16 / 16 / 16)&15],col);
      drawChar(xp3,yp,hextab[(i / 16 / 16)&15],col);
      drawChar(xp3,yp,hextab[(i / 16 )&15],col);
      drawChar(xp3,yp,hextab[(i)&15],col);
    }
    int col = c == 0 ? COLOR_HILIGHT_SPECIAL : COLOR_TEXT;
    if (i < originalText.length()) {
      if (c != originalText[i]) {
        col = COLOR_HILIGHT_NUMBER;
      }
    } else {
      col = COLOR_HILIGHT_NUMBER;
    }
    int col1 = col;
    int col2 = col;
    if (y == cursorY) {col = COLOR_CURSOR;}
    if (x == cursorX && y == cursorY) {if (i<originalText.length()) lastHex=originalText[i];currentHex = c;col1=COLOR_HILIGHT_BOOLEAN;col2=col1;col = COLOR_HILIGHT_SPECIAL;if (time & 8){if (first) col1 = COLOR_HILIGHT_SPECIAL;else col2 = COLOR_HILIGHT_SPECIAL;}}
    if (mouseX>=xp&&mouseY>=yp&&mouseY<yp+CHAR_HEIGHT&&mouseX<xp+CHAR_HEIGHT*2) {
      if (mouseB == 1) {
        cursorX = x; cursorY = y;
      }
    }
    if (mouseX>=xp2&&mouseY>=yp&&mouseY<yp+CHAR_HEIGHT&&mouseX<xp2+CHAR_HEIGHT) {
      if (mouseB == 1) {
        cursorX = x; cursorY = y;
      }
    }
    drawChar(xp,yp,hextab[(c / 16)&15], col1);
    drawChar(xp,yp,hextab[c &15], col2);
    drawChar(xp2,yp,c,col);
  }
  bool updated = originalText != text;
  String status = (updated?"..":"")+String("<CTRL-L/S>");
  status += " $" + String::hex(cursorX+cursorY*ONELINE,8) + " " + String(cursorX+cursorY*ONELINE);
  status += " " + String(currentHex) + " $" + String::hex(currentHex,2) + " $" + String::hex(lastHex,2) + "'" + String::chr(currentHex) + "'";
  status += " " + sizeString(text.length());
  status += " " + fileName.last(20);
  drawBox(position.x,position.y+size.height-CHAR_HEIGHT,position.x+size.width,position.y+size.height,COLOR_STATUSBAR);
  drawString(position.x,position.y+size.height-CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
  popRect();
}

int VHex::update() {

  int ONELINE = 16 * frameBufferWidth / 320;

  if (ctrlKey) {
    if (key==VK_L_CTRL && (!fileName.empty())) {
      cursorX = 0; cursorY = 0; first = false;
      text = stringFromFile(fileName,false);
      originalText = text;
      return 0;
    }
    if (key==VK_S_CTRL && (!fileName.empty())) {
      originalText = text;
      stringToFile(text,fileName,false);
      return 0;
    }
    if (key==VK_G_CTRL) {
      String toFind;
      int a = VTxtAlrt::box(toFind,"Go to address ["+String::hex(text.length(),8)+"]:","YES","NO");
      if (a == 1) {
        long l=0;
        for (int i = 0; i < toFind.length(); i++) {
          char c = toFind[i];
          int nr = -1;
          if (c>='0'&&c<='9') nr = c-'0';
          else
          if (c>='a'&&c<='f') nr = c-'a'+10;
          else
          if (c>='A'&&c<='F') nr = c-'A'+10;
          if (nr >= 0) {
            l=l*16;
            l+=nr;
          }
        }
        if (l<text.length()) {
          cursorY = l/ONELINE;
          cursorX = l%ONELINE;
          textPos.y = cursorY*CHAR_HEIGHT;
        } else {
          VAlert::box("File is only "+String::hex(text.length(),8),"OK");
        }
      }
    }
    if (key==VK_F_CTRL) {
      String toFind = lastFindMode != FINDMODE_NONE ? lastFindString : "";
      int a = VTxtAlrt::box(toFind,"       Find in file       ","Hex","String","Don't");
      if (a == 1) {
        findHex(toFind,0);
      }
      if (a == 2) {
        findString(toFind,0);
      }
    }
  }

  if (key==VK_F3) {
    if (lastFindMode==FINDMODE_STRING) {
      findString(lastFindString,lastFindAddress+1);
    }
    if (lastFindMode==FINDMODE_HEX) {
      findHex(lastFindString,lastFindAddress+1);
    }
  }

  if (mouseY>=position.y&&mouseY<position.y+size.height&&mouseX>position.x+size.width-4*frameBufferWidth/320&&mouseX<position.x+size.width) {
    if (mouseB==1) {
      int ONELINE = 16 * frameBufferWidth / 320;
      int textLastLine = (text.length()+ONELINE-1)/ONELINE;
      textPos.y = (mouseY-position.y)*textLastLine*CHAR_HEIGHT/size.height;
      if (cursorY<textPos.y/CHAR_HEIGHT) cursorY = textPos.y/CHAR_HEIGHT+1;
      if (cursorY>=(textPos.y+size.height-CHAR_HEIGHT)/CHAR_HEIGHT) cursorY = (textPos.y+size.height-CHAR_HEIGHT)/CHAR_HEIGHT-1;
    }
  }

  int entering = -1;
  if (key >= '0' && key <= '9') {entering = key - '0'; key = VK_NONE;}
  if (key >= 'a' && key <= 'f') {entering = key - 'a' + 10; key = VK_NONE;}
  if (entering >= 0) {
    const int address = cursorX+cursorY*ONELINE;
    if (address<text.length()) {
      if (first) {
        text[address]&=0x0f;
        text[address]|=entering*16;
        first=false;
      } else {
        text[address]&=0xf0;
        text[address]|=entering;
        cursorX++;if (cursorX>=ONELINE) {cursorX=0;cursorY++;}
        first=true;
      }
    }
  }
  if (key == VK_INSERT) {
    const int address = cursorX+cursorY*ONELINE;
    text.insert(address,0x00);
    key = VK_NONE;
  }
  if (key == VK_DELETE) {
    if(cursorX>0||cursorY>0) {
      cursorX--;if (cursorX<0) {if (cursorY>0) {cursorX=ONELINE-1;cursorY--;}else{cursorX=0;}}
      const int address = cursorX+cursorY*ONELINE;
      text.erase(address,1);
    }
    key = VK_NONE;
  }
  if (key == VK_ENTF) {
    const int address = cursorX+cursorY*ONELINE;
    text.erase(address,1);
    key = VK_NONE;
  }

  if (key == VK_UP) {first=true;cursorY--;}
  if (key == VK_DOWN) {first=true;cursorY++;}
  if (key == VK_PAGEUP) {first=true;cursorY-=16;}
  if (key == VK_PAGEDOWN) {first=true;cursorY+=16;}
  if (key == VK_LEFT) {first=true;cursorX--;if (cursorX<0) {if (cursorY>0) {cursorX=ONELINE-1;cursorY--;}else{cursorX=0;}}}
  if (key == VK_RIGHT) {first=true;cursorX++;if (cursorX>=ONELINE) {cursorX=0;cursorY++;}}
  if (cursorY<0) cursorY = 0;
  int topLine = textPos.y / CHAR_HEIGHT;
  int bottomLine = (textPos.y+size.height-CHAR_HEIGHT) / CHAR_HEIGHT;
  if (cursorY>bottomLine-1) {
    textPos.y += (cursorY-(bottomLine-1))*CHAR_HEIGHT;
  }
  if (cursorY<topLine) {
    textPos.y -= (topLine-cursorY)*CHAR_HEIGHT;
  }
  if (cursorY>text.length()/ONELINE) {
    cursorY=text.length()/ONELINE;
  }
  if (cursorY==text.length()/ONELINE&&cursorX>=(text.length()%ONELINE)) {
    cursorX=(text.length()%ONELINE)-1;
  }
  if (textPos.y < 0) textPos.y = 0;
  if (textPos.x < 0) textPos.x = 0;

  if (key == VK_ESCAPE) {
    if (originalText != text) {
      int a = VAlert::box("Do you want to save the file?","YES","NO");
      if (a==1) {
        stringToFile(text,fileName,true);
      }
      if (a != 0)
        key = VK_ESCAPE;
    }
  }

  return 0;
}

void VHex::init(const String &_text, const String &_fileName) {
  text = _text;
  originalText = _text;
  fileName = _fileName;
}

bool VHex::findString(const String &toFind, unsigned int start) {
  if (toFind.empty()) {
    lastFindMode = FINDMODE_NONE;
    lastFindString = "";
    lastFindAddress = 0;
    return false;
  }
  int ONELINE = 16 * frameBufferWidth / 320;
  String j; j.resize(toFind.length());
  for (int i = start; i < text.length(); i++) {
    int l = 0;
    if (text[i]==toFind[0]) {
      for (int k = i; k < text.length(); k++) {
        if (String::chr(text[k]).pixelWidth()>0) {
          j[l]=text[k];
          l++;
          if (l == j.length()) break;
        }
      }
      if (l==j.length()) {
        if (j == toFind) {
            cursorY = i/ONELINE;
            cursorX = i%ONELINE;
            if (cursorY>=(textPos.y+size.height-CHAR_HEIGHT)/CHAR_HEIGHT)
              textPos.y = cursorY*CHAR_HEIGHT;
            lastFindMode = FINDMODE_STRING;
            lastFindString = toFind;
            lastFindAddress = i;
            return true;
        }
      }
    }
  }
  int a = VAlert::box("Reached end of file!","OK");
  lastFindAddress = 0;
  return false;
}

bool VHex::findHex(const String &toFind, unsigned int start) {
  if (toFind.empty()) {
    lastFindMode = FINDMODE_NONE;
    lastFindString = "";
    lastFindAddress = 0;
    return false;
  }
  String toFindHex="";
  bool first = true;
  int half = 0;
  for(int r = 0; r < toFind.length(); r++) {
    char c = toFind[r];
    int n = -1;
    if (c>='0'&&c<='9') n = c - '0';
    if (c>='a'&&c<='f') n = c - 'a'+10;
    if (c>='A'&&c<='F') n = c - 'A'+10;
    if (n >= 0) {
      half *= 16;
      half += n;
      first = !first;
      if (first) {
        toFindHex += String::chr(half);
        half = 0;
        first = true;
      }
    }
  }
  int ONELINE = 16 * frameBufferWidth / 320;
  String j; j.resize(toFindHex.length());
  int l;
  for (int i = start; i < text.length(); i++) {
    for (int k = i,l=0; k < text.length() && l<j.length(); k++,l++) {
      j[l]=text[k];
    }
    if (l==j.length()) {
      if (j == toFindHex) {
          cursorY = i/ONELINE;
          cursorX = i%ONELINE;
          if (cursorY>=(textPos.y+size.height-CHAR_HEIGHT)/CHAR_HEIGHT)
            textPos.y = cursorY*CHAR_HEIGHT;
          lastFindMode = FINDMODE_HEX;
          lastFindString = toFind;
          lastFindAddress = i;
          return true;
      }
    }
  }
  int a = VAlert::box("Reached end of file!","OK");
  lastFindAddress = 0;
  return false;
}