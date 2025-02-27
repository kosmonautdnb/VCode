#include "vbutton.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "input.hpp"
#include "vga.hpp"

void VButton::draw() {
  pushRect(position.x,position.y,position.x+size.width,position.y+size.height);
  unsigned int col = mouseIn() ? COLOR_MOUSEOVERLINE:COLOR_WHITE;
  if (selected&&(frameCounter&8)) col = COLOR_SCROLL;
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,col);
  drawRectangle(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_BLACK);
  drawString(position.x+(size.width-text.pixelWidth())/2,position.y+size.height/2-CHAR_HEIGHT/2,text,COLOR_RED);
  popRect();
}

int VButton::update() {
  if (mouseIn()&&lastMouseB==1&&mouseB==0) {
      wasClicked = true;
  }
  if (selected && key == VK_ENTER)
    wasClicked = true;
  return 0;
}

void VButton::select(bool s) {
  selected = s;
}

bool VButton::clicked() {
  return wasClicked;
}

void VButton::init(const String &_text) {
  text = _text;
}