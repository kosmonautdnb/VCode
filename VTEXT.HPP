#ifndef __VTEXT_HPP__
#define __VTEXT_HPP__

#include "vbase.hpp"
#include "string.hpp"
#include "constants.hpp"
#include "input.hpp"
#include "blit.hpp"
#include "vga.hpp"

class VText : public VBase {
public:
  String text;
  String colors;
  String fileName;
  Point textPos;
  int scrollBoxY;

  VText() : VBase() {
    text = "";
    colors = "";
    fileName = "";
    textPos = Point(0,0);
    scrollBoxY = 0;
  }

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual void update();
  virtual void reset() {
    textPos = Point(0,0);
    scrollBoxY = 0;
  }
  virtual void doModal() {
        VBase::doModal();
  }
  void init(const String &_text, const String &_fileName);
};

#endif //__VTEXT_HPP__