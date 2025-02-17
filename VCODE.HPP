#ifndef __VCODE_HPP__
#define __VCODE_HPP__

#include "vbase.hpp"
#include "string.hpp"
#include "constants.hpp"
#include "input.hpp"
#include "blit.hpp"
#include "vga.hpp"

void updateSyntaxHiLight(const String &text, String &colors);

class VCode : public VBase {
public:
  String text;
  String colors;
  String message;
  String fileName;
  Point textPos;
  int cursorX;
  int cursorY;
  int cursorI;
  int cursorIX;
  Point cursorPos;
  int cursorRequiresX;
  int cursorRequiresI;
  int bottomLine;
  int topLine;
  bool shiftReleased;
  int selectionStart;
  int selectionEnd;
  int textLastLine;

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual void update();
  void expand();

  virtual void doModal() {
    pushRect(0,0,frameBufferWidth,frameBufferHeight);
    clipRect = Rect(0,0,frameBufferWidth,frameBufferHeight);
    do {
      updateKeyboard();
    } while(key != VK_NONE);
    cursorX = 0;
    cursorY = 0;
    cursorRequiresX = 0;
    cursorRequiresI = -1;
    cursorIX = -1;
    cursorI = -1;
    topLine = -1;
    bottomLine = -1;
    selectionStart = -1;
    selectionEnd = -1;
    do {
      drawCapturedFrameBuffer();
      updateKeyboard();
      updateMouse();
      draw();
      update();
      drawMouse();
      frameBufferToScreen(frameBuffer);
    } while(key != VK_ESCAPE && key != VK_F4);
    key = VK_NONE;
    popRect();
  }

  void init(const String &_text, const String &_fileName) {
    fileName = _fileName;
    text = _text;
    colors = _text;
    updateSyntaxHiLight(text,colors);
    textPos = Point(0,0);
  }

  void setMessage(const String &_message) {
        message = _message;
  }

  void drawCursor();
};

#endif //__VCODE_HPP__