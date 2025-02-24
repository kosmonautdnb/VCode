#include "vbase.hpp"
#include "input.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "vga.hpp"
#include "util.hpp"

VBase *keyboardFocus = NULL;

int VBase::doModal() {
  currentToolString = name;
  reset();
  pushCapturedFrameBuffer();
  pushRect(0,0,frameBufferWidth,frameBufferHeight);
  clipRect = Rect(0,0,frameBufferWidth,frameBufferHeight);
  do {
    updateKeyboard();
  } while(key != VK_NONE);
  int state=0;
  do {
    drawFrameBuffer(lastCapturedFrameBuffer);
    updateKeyboard();
    updateMouse();
    state = update();
    draw();
    submitFullScreenFrameBuffer(frameBuffer);
    drawMouse();
    frameBufferToScreen(frameBuffer);
  } while(key != VK_ESCAPE);
  key = VK_NONE;
  popRect();
  popCapturedFrameBuffer();
  currentToolString = "";
  if (restoreKeyboardFocus!=NULL) setKeyboardFocus(restoreKeyboardFocus);
  return state;
}

void setKeyboardFocus(VBase *window) {
  keyboardFocus = window;
}

bool hasKeyboardFocus(VBase *window) {
  return keyboardFocus == window;
}

bool VBase::mouseIn()
{
  return mouseOver(Rect(position,size));
}

void VBase::captureKeyboard() {
  restoreKeyboardFocus = keyboardFocus;
  setKeyboardFocus(this);
}