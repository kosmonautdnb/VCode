#include "valert.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "input.hpp"

void VAlert::draw() {
  int x0 = position.x;
  int y0 = position.y;
  int x1 = position.x+size.width;
  int y1 = position.y+size.height;
  pushRect(x0,y0,x1,y1);
  clipRect = Rect(x0,y0,x1+4,y1+4);
  drawBox(x0+4,y0+4,x1+4,y1+4,COLOR_BLACK);
  clipRect = Rect(x0,y0,x1,y1);
  drawBox(x0,y0,x1,y1,COLOR_WHITE);
  drawRectangle(x0,y0,x1,y1,COLOR_RED);
  drawString((x0+x1)/2-text.pixelWidth()/2,y0+2,text,COLOR_RED);
  for (int i = 0; i < buttons.size(); i++) {
    buttons[i]->selected = keyboardId == i;
    buttons[i]->draw();
  }
  popRect();
}

int VAlert::update() {
  for (int i = 0; i < buttons.size(); i++) {
    buttons[i]->update();
    if (buttons[i]->clicked()) {
      key = VK_ESCAPE;
      return i+1;
    }
  }
  if (key == VK_RIGHT) {keyboardId++;if (keyboardId>=buttons.size()) keyboardId = buttons.size()-1;}
  if (key == VK_LEFT) {keyboardId--;if (keyboardId<0) keyboardId = 0;}
  return 0;
}

int VAlert::box(const String &message,const String &button1,const String &button2) {
  VAlert *alert = new VAlert();
  Array<VButton*> arr;
  arr.push_back(new VButton());
  arr.push_back(new VButton());
  arr[0]->init(button1);
  arr[1]->init(button2);
  alert->init(message,arr);
  int r = 0;
  do {r = alert->doModal();} while(1!=1);
  delete alert;
  return r;
}

void VAlert::init(const String &_text, const Array<VButton*> &_buttons) {
  text = _text;
  buttons = _buttons;
  int margin = 2;
  int boxWidth = text.pixelWidth()+margin * 2;
  int boxHeight = CHAR_HEIGHT*3+margin * 2;
  int x0 = frameBufferWidth/2-boxWidth/2;
  int y0 = frameBufferHeight/2-boxHeight/2;
  int x1 = x0 + boxWidth;
  int y1 = y0 + boxHeight;
  setRect(x0,y0,x1,y1);
  margin = 2;
  for (int i = 0; i < buttons.size(); i++) {
    int x = (x0+x1)/2-(x1-x0)/2/buttons.size()+(x1-x0)*i/buttons.size();
    int y = y0+CHAR_HEIGHT*2;
    buttons[i]->setRect(x-margin-buttons[i]->text.pixelWidth()/2,y-margin,x+buttons[i]->text.pixelWidth()/2+margin, y+CHAR_HEIGHT+margin);
  }
}