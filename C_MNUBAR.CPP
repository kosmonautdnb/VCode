#include "c_mnubar.hpp"
#include "blit.hpp"
#include "constants.hpp"
#include "colors.hpp"
#include "input.hpp"
#include "util.hpp"

void CMnuBar::draw() {
  if (!clickedFunctionName.empty())
    return;
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+CHAR_HEIGHT,COLOR_TEXTBACKGROUND);
  Point pos = position;
  Point lpos = pos;
  cursorX = 0;
  cursorY = 0;
  clickedFunctionName="";
  bool specialKeyStart = false;
  int altKey = VK_NONE;
  int lastSelectedXY=0;
  mouseOverItem = false;
  for (int i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c == '+') {
      i++;
      cursorY++;
      Point p2 = lpos;
      p2.y += CHAR_HEIGHT;
      bool nameStarted = false;
      String name;
      bool thisItemSelected=false;
      for(;i<text.length();i++) {
        c=text[i];
        if (c=='+') {p2.x = lpos.x; p2.y += CHAR_HEIGHT; cursorY++;thisItemSelected=false; continue;}
        if (c=='[') {nameStarted = true;name="";continue;}
        if (c==']') {if (nameStarted) {if(thisItemSelected) clickedFunctionName=name;nameStarted=false;};continue;}
        if (nameStarted) {name+=String::chr(c);continue;}
        if (c=='\n') {cursorY=0;break;}
        if (selectedX == cursorX) {
          lastSelectedXY=cursorY;
          bool mOver = mouseOver(Rect(lpos.x,p2.y,p2.x+String::chr(c).pixelWidth()+1,p2.y+CHAR_HEIGHT));
          if (mOver) {
            selectedY = cursorY;
            mouseOverItem = true;
          }
          if (selectedY==cursorY) {
            if (mouseB == 1  || key == VK_ENTER) {
              mouseSelectedOn = cursorY+cursorX*256;
            } else {
              if (mouseSelectedOn == cursorY+cursorX*256) {
                thisItemSelected=true;
                mouseSelectedOn = -1;
              }
            }
          }
          unsigned int bgColor = COLOR_TEXTBACKGROUND;
          if (selectedY == cursorY) bgColor = COLOR_SELECTION;
          drawBox(p2.x,p2.y,p2.x+String::chr(c).pixelWidth()+1,p2.y+CHAR_HEIGHT,bgColor);
          drawChar(p2.x,p2.y,c,COLOR_TEXT);
        }
      }
    }
    if (c == ')') specialKeyStart = false;
    if (specialKeyStart) {
      altKey = c;
      if (key == mapAltKey(altKey)) {selectedX = cursorX;selectedY=1;}
    }
    if (c == '(') specialKeyStart = true;
    if (c == '\n') {cursorX++;pos.x += 10; lpos = pos; continue;}
    if (mouseOver(Rect(lpos.x,pos.y,pos.x+String::chr(c).pixelWidth()+1,pos.y+CHAR_HEIGHT))) {
      selectedX = cursorX;
      mouseOverItem = true;
    }
    unsigned int bgColor = COLOR_TEXTBACKGROUND;
    if (selectedX == cursorX) bgColor = COLOR_SELECTION;
    drawBox(pos.x,pos.y,pos.x+String::chr(c).pixelWidth()+1,pos.y+CHAR_HEIGHT,bgColor);
    drawChar(pos.x,pos.y,c,COLOR_TEXT);
  }
  if (selectedY>lastSelectedXY) selectedY=lastSelectedXY;
  if (mouseB==1&&!mouseOverItem) 
  {
    mouseSelectedOn=-1;
    selectedX=-1;
    selectedY=-1;
    clickedFunctionName="";
  }
  popRect();
}

int CMnuBar::update() {
  if (!clickedFunctionName.empty()) {selectedX=-1;selectedY=-1;return 0;}
  if (selectedX != -1) {
    if (key == VK_DOWN) {selectedY++; key = VK_NONE;}
    if (key == VK_UP) {selectedY--;; key = VK_NONE;}
    if (key == VK_RIGHT) {selectedX++;selectedY=1; key = VK_NONE;}
    if (key == VK_LEFT) {selectedX--;selectedY=1; key = VK_NONE;}
    if (selectedY<1) selectedY = 1;
    if (selectedX<0) selectedX = 0;
    if (key == VK_ESCAPE) {selectedX = -1; key = VK_NONE;}
  }
  if (selectedX!=-1) return 1;
  return 0;
}

void CMnuBar::init(const String &_text) {
  text = _text;
  reset();
}