#ifndef __VTXTLINE_HPP__
#define __VTXTLINE_HPP__

#include "vbase.hpp"
#include "string.hpp"

class VTxtLine : public VBase {
public:
  String text;
  int cursorX;
  int scrollX;

  VTxtLine() : VBase() {
    text = "";
    cursorX = 0;
    scrollX = 0;
  }
  virtual ~VTxtLine() {;}

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual int update();

  void init(const String &_text);
};

#endif //__VTXTLINE_HPP__