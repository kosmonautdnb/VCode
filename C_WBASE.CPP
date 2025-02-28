#include "c_wbase.hpp"
#include "colors.hpp"
#include "constant.hpp"
#include "blit.hpp"
#include "input.hpp"

void CWBase::draw() {
  pushRect(position.x-1,position.y-CHAR_HEIGHT-1,position.x+size.width+1,position.y+size.height+1);
  for (int y = 0; y < CHAR_HEIGHT+1; y++) {
    for (int x = 0; x < CHAR_HEIGHT+1; x++) {
      if (CHAR_HEIGHT-x-2-3 < y)
        setPixel(position.x-1+x,position.y-1-CHAR_HEIGHT+y,COLOR_WHITE);
    }
  }
  drawBox(position.x+CHAR_HEIGHT,position.y-CHAR_HEIGHT-1,position.x+size.width+1,position.y,COLOR_WHITE);
  drawBox(position.x-1,position.y,position.x+size.width+1,position.y+size.height+1,mode == MODE_NONE ? COLOR_WHITE : COLOR_WINDOW_MODIFY);
  pushRect(position.x,position.y-CHAR_HEIGHT,position.x+size.width,position.y);
  drawString(position.x+CHAR_HEIGHT-3,position.y-CHAR_HEIGHT,name,COLOR_BLACK);
  drawString(position.x+size.width-String("X").pixelWidth(),position.y-CHAR_HEIGHT,"x",COLOR_RED);
  popRect();
  popRect();
}

int CWBase::update() {
  if (mouseOver(Rect(position.x-1,position.y-CHAR_HEIGHT,position.x+size.width+1,position.y))) {
    if (mouseB == 1) mode = MODE_MOVE;
  }
  int k = 2*frameBufferWidth/320;
  if (mouseOver(Rect(position.x+size.width-k,position.y+size.height-k,position.x+size.width+k,position.y+size.height+k))) {
    if (mouseB == 1) mode = MODE_RESIZE;
  }

  if (mouseOver(Rect(position.x+size.width-String("x").pixelWidth(),position.y-CHAR_HEIGHT,position.x+size.width,position.y))) {
    if (lastMouseB==1 && mouseB == 0) 
      key = VK_ESCAPE;
  }

  switch(mode) {
    case MODE_MOVE: {
      if (mouseB != 1) mode = MODE_NONE;
      position.x += mouseX-lastMouseX;
      position.y += mouseY-lastMouseY;
      if (position.x + size.width<1) position.x = -size.width+1;
      if (position.y < CHAR_HEIGHT) position.y = CHAR_HEIGHT;
      if (position.x > (int)frameBufferWidth-1) position.x = frameBufferWidth-1;
      if (position.y > (int)frameBufferHeight-1) position.y = frameBufferHeight-1;
    } break;
    case MODE_RESIZE: {
      if (mouseB != 1) mode = MODE_NONE;
      size.width += mouseX-lastMouseX;
      size.height += mouseY-lastMouseY;
      if (size.width < 10) size.width=10;
      if (size.height < 10) size.height=10;
    } break;
  }
  lastMouseX = mouseX;
  lastMouseY = mouseY;
  return mode == MODE_NONE ? 0 : 1;
}
