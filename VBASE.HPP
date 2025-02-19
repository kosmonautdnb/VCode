#ifndef __VBASE_HPP__
#define __VBASE_HPP__

#include "point.hpp"
#include "size.hpp"
#include "types.hpp"
#include "string.hpp"

class VBase {
public:
  Point position;
  Size size;
  String name;

  VBase() {position=Point(0,0); size = Size(0,0);}
  virtual ~VBase() {;}

  void setRect(int x0, int y0, int x1, int y1) {position.x=x0;position.y=y0;size.width=x1-x0;size.height=y1-y0;}
  void setRect(const Point &p0, const Size &_size) {position=p0; size=_size;}
  void setRect(const Point &p0, const Point &p1) {position=p0; size.width=p1.x-p0.x; size.height=p1.y-p0.y;}
  virtual void draw() = 0;
  virtual int update() = 0;
  virtual void reset() {;}
  virtual int doModal();

  virtual bool mouseIn();
};

extern VBase *keyboardFocus;
void setKeyboardFocus(VBase *window);
bool hasKeyboardFocus(VBase *window);

#endif //__VBASE_HPP__