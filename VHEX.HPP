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
  Point textPos;
  int scrollBoxY;
  int cursorX;
  int cursorY;
  bool first;

  VHex() : VBase() {
    text = "";
    originalText = "";
    fileName = "";
    textPos = Point(0,0);
    scrollBoxY = 0;
    cursorX = 0;
    cursorY = 0;
    first = false;
  }

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual void update();
  virtual void reset() {
    textPos = Point(0,0);
    scrollBoxY = 0;
    cursorX = 0;
    cursorY = 0;
    first = false;
  }
  virtual void doModal() {
    VBase::doModal();
  }

  void init(const String &_text, const String &_fileName);
};

#endif //__VHEX_HPP__