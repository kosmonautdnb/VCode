#include "vhex.hpp"
#include "blit.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "dos.hpp"
#include "util.hpp"

void VHex::draw() {
  static int time = 0; time++;
  char *hextab = "0123456789ABCDEF";
  String status = "";
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_TEXTBACKGROUND);
  int xp2 = 0;
  int ONELINE = 16 * frameBufferWidth / 320;
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
    drawChar(xp,yp,hextab[(c / 16)&15], col1);
    drawChar(xp,yp,hextab[c &15], col2);
    drawChar(xp2,yp,c,col);
  }
  int textLastLine = (text.length()+ONELINE-1)/ONELINE;
  if (textLastLine>0) {
    scrollBoxY = textPos.y*frameBufferHeight/(textLastLine*CHAR_HEIGHT);
    drawBox(frameBufferWidth-4*frameBufferWidth/320,scrollBoxY,frameBufferWidth,scrollBoxY+2*frameBufferHeight/200,COLOR_RED);
  }
  status += " $" + String::hex(cursorX+cursorY*ONELINE,8) + ":" + String(cursorX+cursorY*ONELINE) + ":" + String(currentHex) + ":$" + String::hex(currentHex,2) + "'" + String::chr(currentHex) + "'" + ":$" + String::hex(lastHex,2);
  status += " " + sizeString(text.length());
  status += " " + message;
  drawBox(position.x,position.y+size.height-CHAR_HEIGHT,position.x+size.width,position.y+size.height,COLOR_STATUSBAR);
  drawString(position.x,position.y+size.height-CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
  popRect();
}

void VHex::update() {

  if (strgKey) {
    if (key==VK_L_CTRL && (!fileName.empty())) {
      cursorX = 0; cursorY = 0; first = false;
      text = stringFromFile(fileName);
      originalText = text;
      return;
    }
    if (key==VK_S_CTRL && (!fileName.empty())) {
      stringToFile(text,fileName);
      return;
    }
  }

  int ONELINE = 16 * frameBufferWidth / 320;

  if (mouseB==1&&mouseX>position.x+size.width-4*frameBufferWidth/320) {
    int ONELINE = 16 * frameBufferWidth / 320;
    int textLastLine = (text.length()+ONELINE-1)/ONELINE;
    textPos.y = (mouseY-position.y)*textLastLine*CHAR_HEIGHT/size.height;
    if (cursorY<textPos.y/CHAR_HEIGHT) cursorY = textPos.y/CHAR_HEIGHT+1;
    if (cursorY>=(textPos.y+size.height-CHAR_HEIGHT)/CHAR_HEIGHT) cursorY = (textPos.y+size.height-CHAR_HEIGHT)/CHAR_HEIGHT-1;
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
}