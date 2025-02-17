#include "vtext.hpp"
#include "blit.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "dos.hpp"

unsigned int hiLightColors[] = {
        COLOR_HILIGHT_NONE,
        COLOR_HILIGHT_SPECIAL,
        COLOR_HILIGHT_MATH,
        COLOR_HILIGHT_NUMBER,
        COLOR_HILIGHT_BOOLEAN,
        COLOR_HILIGHT_SCOPE,
        COLOR_HILIGHT_STRING,
};

void VText::draw() {
  char *hextab = "0123456789ABCDEF";
  String status = "1=TEXT 3=HEX";
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_TEXTBACKGROUND);
  if (mode == MODE_TEXT) {
    Point pos = position - textPos;
    int line = 0;
    while(pos.y < position.y+size.height) {
      String lineNr = String(line);
      drawString(position.x+size.width-lineNr.pixelWidth(),pos.y,lineNr, COLOR_LINENUMBERS);
      pos.y += CHAR_HEIGHT;
      line++;
    }
    pos = position - textPos;
    int i;
    for (i = 0; i < text.length(); i++) {
      char c = text[i];
      switch(c) {
        case '\n': {pos.y += CHAR_HEIGHT; pos.x = position.x - textPos.x;continue;} break;
        case '\t': {pos.x = ((pos.x - (position.x - textPos.x)) / (CHAR_HEIGHT*2)+1)*(CHAR_HEIGHT*2)+(position.x-textPos.x);continue;} break;
      }
      drawChar(pos.x,pos.y,c,hiLightColors[colors[i]]);
      if (pos.y > position.y + size.height) break;
    }
    status += " " + String(textPos.x/CHAR_HEIGHT)+":"+String(textPos.y/CHAR_HEIGHT);
    if (i == text.length())
      drawStringOutlined(pos.x,pos.y,"<EOF>",COLOR_RED,COLOR_YELLOW);
  }
  if (mode == MODE_HEX) {
    int xp2 = 0;
    int ONELINE = 16 * frameBufferWidth / 320;
    for (int i = 0; i < text.length(); i++) {
      char c = text[i];
      unsigned int l = hiLightColors[colors[i]];
      int xp = (i % ONELINE)*CHAR_HEIGHT*2 + CHAR_HEIGHT*7-textPos.x;
      int yp = (i / ONELINE)*CHAR_HEIGHT-textPos.y;
      if ((i % ONELINE) == 0) {
        xp2 = xp+CHAR_HEIGHT*ONELINE*2;
        int xp3 = -textPos.x;
        drawChar(xp3,yp,hextab[(i / 16 / 16 / 16 / 16 / 16 / 16 / 16)&15],COLOR_LINENUMBERS);
        drawChar(xp3,yp,hextab[(i / 16 / 16 / 16 / 16 / 16 / 16)&15],COLOR_LINENUMBERS);
        drawChar(xp3,yp,hextab[(i / 16 / 16 / 16 / 16 / 16)&15],COLOR_LINENUMBERS);
        drawChar(xp3,yp,hextab[(i / 16 / 16 / 16 / 16)&15],COLOR_LINENUMBERS);
        drawChar(xp3,yp,hextab[(i / 16 / 16 / 16)&15],COLOR_LINENUMBERS);
        drawChar(xp3,yp,hextab[(i / 16 / 16)&15],COLOR_LINENUMBERS);
        drawChar(xp3,yp,hextab[(i / 16 )&15],COLOR_LINENUMBERS);
        drawChar(xp3,yp,hextab[(i)&15],COLOR_LINENUMBERS);
      }
      drawChar(xp,yp,hextab[(c / 16)&15],c==0 ? COLOR_HILIGHT_SPECIAL : COLOR_HILIGHT_NONE);
      drawChar(xp,yp,hextab[c &15],c==0 ? COLOR_HILIGHT_SPECIAL : COLOR_HILIGHT_NONE);
      drawChar(xp2,yp,c,l);
    }
    status += " " + String(textPos.x/CHAR_HEIGHT)+":"+String(textPos.y/CHAR_HEIGHT);
  }
  status += " " + sizeString(text.length());
  status += " " + message;
  drawBox(position.x,position.y+size.height-CHAR_HEIGHT,position.x+size.width,position.y+size.height,COLOR_STATUSBAR);
  drawString(position.x,position.y+size.height-CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
  popRect();
}

void VText::update() {
  if (key == VK_UP) {textPos.y -= CHAR_HEIGHT; if (textPos.y < 0) textPos.y = 0;}
  if (key == VK_DOWN) {textPos.y += CHAR_HEIGHT;}
  if (key == VK_PAGEUP) {textPos.y -= CHAR_HEIGHT*16; if (textPos.y < 0) textPos.y = 0;}
  if (key == VK_PAGEDOWN) {textPos.y += CHAR_HEIGHT*16;}
  if (key == VK_LEFT) {textPos.x -= CHAR_HEIGHT; if (textPos.x < 0) textPos.x = 0;}
  if (key == VK_RIGHT) {textPos.x += CHAR_HEIGHT;}
  if (key == '1') mode = MODE_TEXT;
  if (key == '3') mode = MODE_HEX;
}