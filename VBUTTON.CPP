#include "vbutton.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "input.hpp"

void VButton::draw() {
  static int time=0; time++;
  pushRect(position.x,position.y,position.x+size.width,position.y+size.height);
  unsigned int col = mouseIn() ? COLOR_MOUSEOVERLINE:COLOR_WHITE;
  if (selected&&(time&8)) col = COLOR_SCROLL;
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,col);
  drawRectangle(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_BLACK);
  drawString(position.x+(size.width-text.pixelWidth())/2,position.y+size.height/2-CHAR_HEIGHT/2,text,COLOR_RED);
  popRect();
}

int VButton::update() {
  if (mouseIn()) {
    if (mouseB==1) {
      mousePressed = true;
    } else {
      if (mousePressed) mouseReleased = true;
    }
    if (mousePressed && mouseReleased)
      wasClicked = true;
  } else {
    mousePressed = false;
    mouseReleased = false;
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