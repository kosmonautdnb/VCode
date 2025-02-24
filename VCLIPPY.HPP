#ifndef __VCLIPPY_HPP__
#define __VCLIPPY_HPP__

#include "vbase.hpp"
#include "vfleslct.hpp"
#include "c_layers.hpp"
#include "c_mnubar.hpp"

class VClippy : public VBase {
public:

  CLayerStack *cLayerStack;
  CMnuBar *cMnuBar;
  VFleSlct *vFleSlct;
  bool doublePixels;
  bool gridOn;
  int gridWidth;
  int gridHeight;
  bool lockedMouse;
  String currentPath;
  int lastMouseX;
  int lastMouseY;

  VClippy() : VBase() {
    cLayerStack = new CLayerStack();
    cMnuBar = new CMnuBar();
    vFleSlct = new VFleSlct();
    doublePixels=false;
    gridOn = false;
    gridWidth=8;
    gridHeight=8;
    lockedMouse=false;
    lastMouseX=0;
    lastMouseY=0;
  }
  virtual ~VClippy() {
    delete cLayerStack;
    delete cMnuBar;
    delete vFleSlct;
  }

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual int update();
  virtual void reset() {
    cLayerStack->reset();
    doublePixels=false;
    gridOn = false;
    gridWidth=8;
    gridHeight=8;
    lockedMouse=false;
    lastMouseX=0;
    lastMouseY=0;
    cMnuBar->reset();
  }
  virtual int doModal() {
        return VBase::doModal();
  }
  String getMenuString();
  void init(const String &path, int width, int height);
  
  void load(const String &fileName);

  void toolMoveLayers();
};

#endif //__VCLIPPY_HPP__