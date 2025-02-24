#include "vclippy.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constant.hpp"
#include "valert.hpp"
#include "input.hpp"
#include "pngload.hpp"

void VClippy::draw() {
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,COLOR_TEXTBACKGROUND);
  cLayerStack->drawAtXY(0,0);
  cMnuBar->setRect(position.x,position.y,position.x+size.width,position.y+size.height);
  cMnuBar->text = getMenuString();
  if (!lockedMouse) 
    cMnuBar->draw();
  String status = "";
  drawBox(position.x,position.y+size.height-CHAR_HEIGHT,position.x+size.width,position.y+size.height,COLOR_STATUSBAR);
  drawString(position.x,position.y+size.height-CHAR_HEIGHT,status,COLOR_STATUSBARTEXT);
  popRect();
}

int VClippy::update() {
  if (!lockedMouse) 
      if (cMnuBar->update()!=0) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        return 0;
      }
  toolMoveLayers();
  if (cMnuBar->clickedFunctionName == "LOAD") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = true;
    vFleSlct->doModal();
    load(vFleSlct->fileNameSelected);
  }

  if (cMnuBar->clickedFunctionName == "SAVE") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = false;
    vFleSlct->doModal();
    String fileToSave = toLower(vFleSlct->fileNameSelected);
  }

  if (cMnuBar->clickedFunctionName == "DOUBLEPIXELS") {
    doublePixels = true;
    cLayerStack->doublePixels(doublePixels);
    cLayerStack->reBlit();
  }

  if (cMnuBar->clickedFunctionName == "SINGLEPIXELS") {
    doublePixels = false;
    cLayerStack->doublePixels(doublePixels);
    cLayerStack->reBlit();
  }

  if (key == VK_ESCAPE) {
    int a = VAlert::box("Exit Clippy?","YES","NO");
    if (a == 1)
      key = VK_ESCAPE;
  }

  if (key == '+') {
    cLayerStack->zoom += 0.25f;
    cLayerStack->reBlit();
  }

  if (key == '-') {
    cLayerStack->zoom -= 0.25f;
    cLayerStack->reBlit();
  }

  cMnuBar->clickedFunctionName = "";
  lastMouseX = mouseX;
  lastMouseY = mouseY;
  return 0;
}

String VClippy::getMenuString() {
  String r = "(A)File+Load[LOAD]+Save[SAVE]\n";
  r+= "(B)Edit+Undo+Redo[REDO]+Cut[CUT]+Copy[COPY]\n";
  r+= "(C)Layer\n";
  r+= "(D)Image\n";
  r+= "(E)Font\n";
  r+= "(F)Window";
  if (!doublePixels) r += "+DoubledWidth[DOUBLEPIXELS]\n";
  else r += "+NormalWidth[SINGLEPIXELS]\n";
  r+= "(G)Filter\n";
  return r;
}

void VClippy::init(const String &path, int width, int height) {
  currentPath = path;
  cLayerStack->newStack(width,height);
  cLayerStack->reBlit();
  cMnuBar->init(getMenuString());
}

void VClippy::load(const String &fileName) {
  if (getExtension(toLower(fileName))==".png") {
    ImageLayer l;
    if (loadPNG(&l, fileName.c_str())) {
      cLayerStack->newStack(cLayerStack->width,cLayerStack->height);
      cLayerStack->layers[0]->init(l.data,l.width,l.height);
      cLayerStack->reBlit();
    }
  }
}

void VClippy::toolMoveLayers() {
  if (mouseB == 1) {
    lockedMouse = true;
  }
  if (mouseB == 0) {
    lockedMouse = false;
  } 

  if (mouseB == 1) {
    cLayerStack->moveLayers(mouseX-lastMouseX,mouseY-lastMouseY);
    cLayerStack->reBlit();
  }
}
