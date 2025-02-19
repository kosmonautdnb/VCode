#ifndef __VTXTALRT_HPP__
#define __VTXTALRT_HPP__

#include "valert.hpp"
#include "vtxtline.hpp"

class VTxtAlrt : public VAlert {
public:
  VTxtLine *vTxtLine;

  VTxtAlrt() : VAlert() {
    vTxtLine = new VTxtLine();
  }
  virtual ~VTxtAlrt() {
    delete vTxtLine;
  }

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual int update();
  virtual void reset() {}
  virtual int doModal() {
    return VAlert::doModal();
  }

  static int box(String &returned,const String &message,const String &button1,const String &button2);

  void init(const String &_text, const Array<VButton*> &_buttons);
};

#endif //__VTXTALRT_HPP__