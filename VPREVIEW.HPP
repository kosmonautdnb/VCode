#ifndef __VPREVIEW_HPP__
#define __VPREVIEW_HPP__

#include "vbase.hpp"
#include "vtext.hpp"
#include "vhex.hpp"
#include "vcode.hpp"
#include "array.hpp"
#include "string.hpp"
#include "colors.hpp"
#include "util.hpp"
#include "dos.hpp"

class VPreview : public VBase {
public:
  String fileName;
  String lastFileName;
  unsigned int *data;

  VPreview() : VBase() {
    fileName = "";
    lastFileName = "";
    data = NULL;
  }

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual void update();

  void init(const String &_fileName);
};

#endif //__VPREVIEW_HPP__