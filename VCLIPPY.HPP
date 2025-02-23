#ifndef __VCLIPPY_HPP__
#define __VCLIPPY_HPP__

#include "vbase.hpp"
#include "c_layers.hpp"
#include "c_mnubar.hpp"

class VClippy : public VBase {
public:

  CLayerStack *cLayerStack;
  CMnuBar *cMnuBar;

  VClippy() : VBase() {
    cLayerStack = new CLayerStack();
    cLayerStack->layers.push_back(new CLayer());
    cMnuBar = new CMnuBar();
  }
  virtual ~VClippy() {
    delete cLayerStack;
    delete cMnuBar;
  }

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual int update();
  virtual void reset() {
  }
  virtual int doModal() {
        return VBase::doModal();
  }
  void init(int width, int height);
};

#endif //__VCLIPPY_HPP__