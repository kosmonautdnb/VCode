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
  int k = 0;
  for (int j = 0; j < buttons.size(); j++) {
    k += buttons[j]->text.pixelWidth();
  }
  k += (buttons.size()-1)*(2+margin*2);
  int x = x0+(x1-x0-k)/2;
  int y = y0+CHAR_HEIGHT*7/2;
  for (int i = 0; i < buttons.size(); i++) {
    buttons[i]->setRect(x-margin,y-margin,x+buttons[i]->text.pixelWidth()+margin, y+CHAR_HEIGHT+margin);
    x += buttons[i]->text.pixelWidth()+margin*2+2;
  }
}