#ifndef __VCLIPPY_HPP__
#define __VCLIPPY_HPP__

#include "vbase.hpp"
#include "vfleslct.hpp"
#include "c_layers.hpp"
#include "c_mnubar.hpp"
#include "c_wpal.hpp"
#include "c_wzoom.hpp"
#include "c_wpaled.hpp"

class VClippy : public VBase {
public:

  CLayerStack *cLayerStack;
  CMnuBar *cMnuBar;
  VFleSlct *vFleSlct;
  CWPal *cWPal;
  CWZoom *cWZoom;
  CWPalEd *cWPalEd;
  bool doublePixels;
  bool gridOn;
  int gridWidth;
  int gridHeight;
  bool lockedMouse;
  String currentPath;
  int lastMouseX;
  int lastMouseY;
  String function;

  VClippy() : VBase() {
    cLayerStack = new CLayerStack();
    cMnuBar = new CMnuBar();
    vFleSlct = new VFleSlct();
    cWPal = new CWPal();
    cWZoom = new CWZoom();
    cWPalEd = new CWPalEd();
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
    delete cWPal;
    delete cWZoom;
    delete cWPalEd;
  }

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
    function="";
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