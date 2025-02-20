
#include "types.hpp"
#include "vga.hpp"
#include "blit.hpp"
#include "pngload.hpp"
#include "array.hpp"
#include "string.hpp"
#include "dos.hpp"
#include "constant.hpp"
#include "input.hpp"
#include "vtree.hpp"
#include "util.hpp"
#include "valert.hpp"

String currentWorkingDirectory;
String fileNameToLoad;

void displayFileSelector() {
  pushCapturedFrameBuffer();
  treeLeft = new VTree();
  treeRight = new VTree();
  treeRight->init(Rect(frameBufferWidth/2,CHAR_HEIGHT,frameBufferWidth,frameBufferHeight-16),currentWorkingDirectory);
  treeLeft->init(Rect(0,CHAR_HEIGHT,frameBufferWidth/2-1,frameBufferHeight-16),currentWorkingDirectory);
  if (!fileNameToLoad.empty()) {
    int x = String("FILENAME.NAME").pixelWidth();
    treeLeft->size.width = x - treeLeft->position.x-1;
    treeRight->position.x = x;
    treeRight->size.width = frameBufferWidth-treeRight->position.x;
    treeRight->overlayByFileName(fileNameToLoad);
    setKeyboardFocus(treeRight);
    treeRight->searchText = getName(fileNameToLoad)+getExtension(fileNameToLoad);
    treeRight->searchTextUpdated();
  }
  bool altPressed = false;
  bool altCanceled = false;
  bool grabbed=false;
  do {
    updateMouse();
    updateKeyboard();
    drawFrameBuffer(lastCapturedFrameBuffer);

    if (abs(mouseX-treeRight->position.x)<2&&mouseB==1) {
      grabbed = true;
    }

    if (grabbed) {
      treeLeft->size.width = mouseX - treeLeft->position.x-1;
      treeRight->position.x = mouseX;
      treeRight->size.width = frameBufferWidth-treeRight->position.x;
      if (mouseB!=1)
        grabbed = false;
    }

    if (key == VK_NONE && altKey) altPressed = true;
    if (key != VK_NONE && altPressed && altKey) altCanceled = true;
    bool doSwitch = false;
    if (!altKey) {
      if (altPressed && (!altCanceled)) {
        doSwitch=true;
      }
      altCanceled = false;
      altPressed = false;
    }
    if (treeLeft->treeMode==VTree::TREEMODE_FILETREE&&treeRight->treeMode==VTree::TREEMODE_FILETREE)
      if (key == VK_TAB)
        doSwitch = true;
    if (doSwitch) {
      if (hasKeyboardFocus(treeLeft)) {
        setKeyboardFocus(treeRight);
        int x = String("FILENAME.NAME").pixelWidth();
        treeLeft->size.width = x - treeLeft->position.x-1;
        treeRight->position.x = x;
        treeRight->size.width = frameBufferWidth-treeRight->position.x;
      } else {
        setKeyboardFocus(treeLeft);
        int x = frameBufferWidth-String("FILENAME.NAME").pixelWidth();
        treeLeft->size.width = x - treeLeft->position.x-1;
        treeRight->position.x = x;
        treeRight->size.width = frameBufferWidth-treeRight->position.x;
      }
    }
    if (!grabbed) {
      treeLeft->update();
      treeRight->update();
    }
    treeLeft->draw();
    treeRight->draw();
    if (fileWasWritten) {
      treeLeft->reload = true;
      treeRight->reload = true;
      fileWasWritten = false;
    }
    submitFullScreenFrameBuffer(frameBuffer);
    if (key == VK_ESCAPE) {
      int a = VAlert::box("Exit to Dos?","YES","NO");
      if (a == 1)
        key = VK_ESCAPE;
    }
    drawMouse();
    frameBufferToScreen(frameBuffer);
  } while(key!=VK_ESCAPE);
  popCapturedFrameBuffer();
}

void graphicsMode320x200x256() {
  setBiosGraphicsMode(0x13);
  allocFrameBuffer(320,200);
  randomPalette();
}

void graphicsMode640x480x16() {
  setBiosGraphicsMode(0x12);
  allocFrameBuffer(640,480);
  palette16Colors();
}

int main(int argc, const char *argv[])
{
  if (argc == 2) {
    fileNameToLoad = argv[1];
  }
  currentWorkingDirectory = getCWD();
  disableControlBreak();
  int gfxMode = getBiosGraphicsMode();
  
  //graphicsMode320x200x256();
  graphicsMode640x480x16();

  displayFileSelector();

  freeFrameBuffer();
  setBiosGraphicsMode(gfxMode);
  return 0;
}