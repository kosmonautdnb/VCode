#include "vclippy.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constant.hpp"

void VClippy::draw() {
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_TEXTBACKGROUND);
  cLayerStack->drawAtXY(0,0);
  cMnuBar->setRect(position.x,position.y,position.x+size.width,position.y+size.height);
  cMnuBar->draw();
  String status = "";
  drawBox(position.x,position.y+size.height-CHAR_HEIGHT,position.x+size.width,position.y+size.height,COLOR_STATUSBAR);
  drawString(position.x,position.y+size.height-CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
  popRect();
}

int VClippy::update() {
  return 0;
}

void VClippy::init(int width, int height) {
  cLayerStack->reCanvas(0,0,width,height);
  cMnuBar->init(
      "File+Load+Save\n"
      "Edit+Undo+Redo+Cut+Copy\n"
      "Layer\n"
      "Image\n"
      "Font\n"
      "Window\n"
      "Filter\n");
}
