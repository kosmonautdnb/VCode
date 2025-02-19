#ifndef __VBUTTON_HPP__
#define __VBUTTON_HPP__

#include "vbase.hpp"
#include "string.hpp"

class VButton : public VBase {
public:
  String text;
  bool mousePressed;
  bool mouseReleased;
  bool wasClicked;
  bool selected;

  VButton() : VBase() {
    text = "";
    mousePressed = false;
    mouseReleased = false;
    wasClicked = false;
  }
  virtual ~VButton() {;}

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual int update();

  void select(bool s);
  bool clicked();
  void init(const String &_text);
};

#endif //__VBUTTON_HPP__