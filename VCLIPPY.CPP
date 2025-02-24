#include "vclippy.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constant.hpp"
#include "valert.hpp"
#include "input.hpp"

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

extern String currentWorkingDirectory;

int VClippy::update() {
  cMnuBar->update();

  if (cMnuBar->clickedFunctionName == "LOAD") {
    vFleSlct->init(currentWorkingDirectory);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = true;
    vFleSlct->doModal();
  }

  if (cMnuBar->clickedFunctionName == "SAVE") {
    vFleSlct->init(currentWorkingDirectory);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = false;
    vFleSlct->doModal();
  }

  if (key == VK_ESCAPE) {
    int a = VAlert::box("Exit Clippy?","YES","NO");
    if (a == 1)
      key = VK_ESCAPE;
  }

  return 0;
}

void VClippy::init(int width, int height) {
  cLayerStack->reCanvas(0,0,width,height);
  cMnuBar->init(
      "(A)File+Load[LOAD]+Save[SAVE]\n"
      "(B)Edit+Undo+Redo[REDO]+Cut[CUT]+Copy[COPY]\n"
      "(C)Layer\n"
      "(D)Image\n"
      "(E)Font\n"
      "(F)Window\n"
      "(G)Filter\n");
}
