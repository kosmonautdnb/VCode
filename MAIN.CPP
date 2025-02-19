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

void displayFileSelector() {
    pushCapturedFrameBuffer();
    treeLeft = new VTree();
    treeRight = new VTree();
    treeLeft->init(Rect(0,CHAR_HEIGHT,frameBufferWidth/2-1,frameBufferHeight-16),"c:/");
    treeRight->init(Rect(frameBufferWidth/2,CHAR_HEIGHT,frameBufferWidth,frameBufferHeight-16),currentWorkingDirectory);
    do {
      updateMouse();
      updateKeyboard();
      drawFrameBuffer(lastCapturedFrameBuffer);
      if (key == 0x09) {
        if (hasKeyboardFocus(treeLeft))
          setKeyboardFocus(treeRight);
        else
          setKeyboardFocus(treeLeft);
      }
      treeLeft->draw();
      treeRight->draw();
      drawRectangle(treeLeft->position.x+treeLeft->size.width,treeLeft->position.y-CHAR_HEIGHT,treeLeft->position.x+treeLeft->size.width,treeLeft->position.y+treeLeft->size.height,COLOR_BLACK);
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

void main()
{
    currentWorkingDirectory = getCWD();
    disableControlBreak();
    int gfxMode = getBiosGraphicsMode();
    graphicsMode320x200x256();
    //graphicsMode640x480x16();

    displayFileSelector();

    freeFrameBuffer();
    setBiosGraphicsMode(gfxMode);
}