#include "vclippy.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constant.hpp"
#include "valert.hpp"
#include "input.hpp"
#include "pngload.hpp"

extern bool showAppTime;

void VClippy::draw() {
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,0xff300060);
  cLayerStack->drawAtXY(0,0);
  cMnuBar->setRect(position.x,position.y,position.x+size.width,position.y+size.height);
  cMnuBar->text = getMenuString();
  cMnuBar->draw();
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
  if (function == "LOAD") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = true;
    vFleSlct->doModal();
    load(vFleSlct->fileNameSelected);
  }

  if (function == "SAVE") {
    vFleSlct->init(currentPath);
    vFleSlct->setRect(frameBufferWidth/4,frameBufferHeight/4,frameBufferWidth*3/4,frameBufferHeight*3/4);
    vFleSlct->modeLoading = false;
    vFleSlct->doModal();
    String fileToSave = toLower(vFleSlct->fileNameSelected);
  }

  if (function == "DOUBLEPIXELS") {
    doublePixels = true;
    cLayerStack->doublePixels(doublePixels);
    cLayerStack->reBlit();
  }

  if (function == "SINGLEPIXELS") {
    doublePixels = false;
    cLayerStack->doublePixels(doublePixels);
    cLayerStack->reBlit();
  }

  if (function == "SWITCHGRID") {
    gridOn = !gridOn;
    cLayerStack->enableGrid(gridOn,gridWidth,gridHeight);
    cLayerStack->reBlit();
  }

  showAppTime = false;
  if (key == VK_ESCAPE || function == "EXIT") {
    int a = VAlert::box("Exit Clippy?","YES","NO");
    if (a == 1) {
      key = VK_ESCAPE;
      showAppTime = true;
    }
  }

  if (key == '-') {
    cLayerStack->addZoom(0.25f,mouseX,mouseY);
    cLayerStack->reBlit();
  }

  if (key == '+') {
    cLayerStack->addZoom(-0.25f,mouseX,mouseY);
    cLayerStack->reBlit();
  }

  if (key == 'a' || function=="ROTATE90") {
    cLayerStack->layers[0]->selected = true;
    cLayerStack->rotateLayers(90.f);
    cLayerStack->reBlit();
  }

  if (function=="MIRRORX") {
    cLayerStack->layers[0]->selected = true;
    cLayerStack->mirrorLayers(-1,1);
    cLayerStack->reBlit();
  }

  if (function=="MIRRORY") {
    cLayerStack->layers[0]->selected = true;
    cLayerStack->mirrorLayers(1,-1);
    cLayerStack->reBlit();
  }

  function = cMnuBar->clickedFunctionName; // one frame delay for redraw menubar
  cMnuBar->clickedFunctionName = "";
  lastMouseX = mouseX;
  lastMouseY = mouseY;
  return 0;
}

String VClippy::getMenuString() {
  String r = "(f)File+(L)Load[LOAD]+(S)Save[SAVE]+(Q)Exit[EXIT]\n";
  r+= "Edit+Undo+Redo[REDO]+Cut[CUT]+Copy[COPY]+----------"
  "+(R)Rotate 90 Degrees[ROTATE90]"
  "+(H)Flip horizontaly[MIRRORX]"
  "+(V)Flip verticaly[MIRRORY]"
  "\n";
  r+= "Layer\n";
  r+= "Image\n";
  r+= "Text\n";
  r+= "Window";
  if (!doublePixels) r += "+Doubled width[DOUBLEPIXELS]";  else r += "+Normal width[SINGLEPIXELS]";
  if (!gridOn) r += "+Grid on[SWITCHGRID]"; else r += "+Grid off[SWITCHGRID]";
  r+= "\n";
  r+= "Effects\n";
  return r;
}

void VClippy::init(const String &path, int width, int height) {
  reset();
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
    if (lockedMouse) {
      cLayerStack->finishMoveLayers();
      cLayerStack->reBlit();
    }
    lockedMouse = false;
  }

  if (mouseB == 1) {
    if (shiftKey) {
      cLayerStack->moveStack(mouseX-lastMouseX,mouseY-lastMouseY);
    } else {
      cLayerStack->layers[0]->selected = true;
      cLayerStack->moveLayers(mouseX-lastMouseX,mouseY-lastMouseY);
      cLayerStack->reBlit();
    }
  }
}