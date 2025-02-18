#include "vbase.hpp"
#include "input.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "vga.hpp"

VBase *keyboardFocus = NULL;

void VBase::doModal() {
  reset();
  pushCapturedFrameBuffer();
  pushRect(0,0,frameBufferWidth,frameBufferHeight);
  clipRect = Rect(0,0,frameBufferWidth,frameBufferHeight);
  do {
    updateKeyboard();
  } while(key != VK_NONE);
  do {
    drawFrameBuffer(lastCapturedFrameBuffer);
    updateKeyboard();
    updateMouse();
    update();
    draw();
    drawMouse();
    frameBufferToScreen(frameBuffer);
  } while(key != VK_ESCAPE && key != VK_F4);
  key = VK_NONE;
  popRect();
  popCapturedFrameBuffer();
}

void setKeyboardFocus(VBase *window) {
  keyboardFocus = window;
}

bool hasKeyboardFocus(VBase *window) {
  return keyboardFocus == window;
}