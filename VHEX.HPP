#ifndef __VHEX_HPP__
#define __VHEX_HPP__

#include "vbase.hpp"
#include "string.hpp"
#include "constants.hpp"
#include "input.hpp"
#include "blit.hpp"
#include "vga.hpp"

class VHex : public VBase {
public:
  String text;
  String originalText;
  String fileName;
  String message;
  Point textPos;
  int scrollBoxY;
  int cursorX;
  int cursorY;
  bool first;

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual void update();

  virtual void doModal() {
    pushRect(0,0,frameBufferWidth,frameBufferHeight);
    clipRect = Rect(0,0,frameBufferWidth,frameBufferHeight);
    do {
      updateKeyboard();
    } while(key != VK_NONE);
    cursorX = 0;
    cursorY = 0;
    first = true;
    do {
      drawCapturedFrameBuffer();
      updateKeyboard();
      updateMouse();
      draw();
      update();
      drawMouse();
      frameBufferToScreen(frameBuffer);
    } while(key != VK_ESCAPE && key != VK_F3);
    key = VK_NONE;
    popRect();
  }

  void init(const String &_text, const String &_fileName) {
    text = _text;
    originalText = _text;
    fileName = _fileName;
    textPos = Point(0,0);
  }

  void setMessage(const String &_message) {
        message = _message;
  }
};

#endif //__VHEX_HPP__