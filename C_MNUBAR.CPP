#include "c_mnubar.hpp"
#include "blit.hpp"
#include "constants.hpp"
#include "colors.hpp"
#include "input.hpp"

void CMnuBar::draw() {
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+CHAR_HEIGHT,COLOR_TEXTBACKGROUND);
  Point pos = position;
  Point lpos = pos;
  cursorX = 0;
  cursorY = 0;
  for (int i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c == '+') {
      i++;
      cursorY++;
      Point p2 = lpos;
      p2.y += CHAR_HEIGHT; 
      for(;i<text.length();i++) {
        c=text[i];
        if(c=='\n') {cursorY=0;break;}
        if(c=='+') {p2.x = lpos.x; p2.y += CHAR_HEIGHT; cursorY++; continue;}
        if (cursorX == mouseClickedX) {
          if (mouseOver(Rect(p2.x,p2.y,p2.x+String::chr(c).pixelWidth()+1,p2.y+CHAR_HEIGHT))) {
            mouseOverX = cursorX;
            mouseOverY = cursorY;
            if (mouseB == 1) {
              mouseSelectedOn = cursorY+cursorX*256;
            } else {
              if (mouseSelectedOn == cursorY+cursorX*256) {
                mouseClickedX = cursorX;
                mouseClickedY = cursorY;
                mouseSelectedOn = -1;
              } 
            }
          }
          unsigned int bgColor = COLOR_TEXTBACKGROUND;
          if (mouseOverX == cursorX && mouseOverY == cursorY) bgColor = COLOR_SELECTION;
          drawBox(p2.x,p2.y,p2.x+String::chr(c).pixelWidth()+1,p2.y+CHAR_HEIGHT,bgColor);
          drawChar(p2.x,p2.y,c,COLOR_TEXT);
        }
      }
    }
    if (c == '\n') {cursorX++;pos.x += 10; lpos = pos; continue;}
    if (mouseOver(Rect(pos.x,pos.y,pos.x+String::chr(c).pixelWidth()+1,pos.y+CHAR_HEIGHT))) {
      mouseOverX = cursorX;
      mouseOverY = cursorY;
      if (mouseB == 1) {
        mouseSelectedOn = cursorY+cursorX*256;
      } else {
        if (mouseSelectedOn == cursorY+cursorX*256) {
          mouseClickedX = cursorX;
          mouseClickedY = cursorY;
          mouseSelectedOn = -1;
        } 
      }
    }
    unsigned int bgColor = COLOR_TEXTBACKGROUND;
    if (mouseOverX == cursorX && mouseOverY == cursorY) bgColor = COLOR_SELECTION;
    drawBox(pos.x,pos.y,pos.x+String::chr(c).pixelWidth()+1,pos.y+CHAR_HEIGHT,bgColor);
    drawChar(pos.x,pos.y,c,COLOR_TEXT);
  }
  popRect();
}

int CMnuBar::update() {
  return 0;
}

void CMnuBar::init(const String &_text) {
  text = _text;
  reset();
}