#include "vtxtalrt.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "input.hpp"

void VTxtAlrt::draw() {
  VAlert::draw();
  vTxtLine->draw();
}

int VTxtAlrt::update() {
  vTxtLine->update();
  return VAlert::update();
}

int VTxtAlrt::box(String &returned,const String &message,const String &button1,const String &button2) {
  VTxtAlrt *txtalrt = new VTxtAlrt();
  Array<VButton*> arr;
  arr.push_back(new VButton());
  arr.push_back(new VButton());
  arr[0]->init(button1);
  arr[1]->init(button2);
  txtalrt->init(message,arr);
  txtalrt->vTxtLine->text = returned;
  int r = 0;
  do {r = txtalrt->doModal();} while(1!=1);
  returned = txtalrt->vTxtLine->text;
  delete txtalrt;
  return r;
}

void VTxtAlrt::init(const String &_text, const Array<VButton*> &_buttons) {
  text = _text;
  buttons = _buttons;
  int margin = 2;
  int boxWidth = text.pixelWidth()+margin * 2;
  int boxHeight = CHAR_HEIGHT*9/2+margin * 2;
  int x0 = frameBufferWidth/2-boxWidth/2;
  int y0 = frameBufferHeight/2-boxHeight/2;
  int x1 = x0 + boxWidth;
  int y1 = y0 + boxHeight;
  setRect(x0,y0,x1,y1);
  vTxtLine->setRect(x0+2,y0+CHAR_HEIGHT*3/2-1,x1-2,y0+CHAR_HEIGHT*5/2+1);
  margin = 2;
  for (int i = 0; i < buttons.size(); i++) {
    int x = (x0+x1)/2-(x1-x0)/2/buttons.size()+(x1-x0)*i/buttons.size();
    int y = y0+CHAR_HEIGHT*7/2;
    buttons[i]->setRect(x-margin-buttons[i]->text.pixelWidth()/2,y-margin,x+buttons[i]->text.pixelWidth()/2+margin, y+CHAR_HEIGHT+margin);
  }
}