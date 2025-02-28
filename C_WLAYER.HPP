#ifndef __C_WLAYER_HPP__
#define __C_WLAYER_HPP__

#include "c_wbase.hpp"
#include "c_mnubar.hpp"
#include "vhslider.hpp"
#include "vvslider.hpp"

class VClippy;

class CWLayer : public CWBase {
public:
  VClippy *data;
  int currentLayer;
  CMnuBar *cMnuBar;
  int lastMouseOverLayer;
  int mouseOverLayer;
  bool mouseOverVisible;
  bool mouseOverSelected;
  VHSlider *opacity;
  VVSlider *scroll;

  CWLayer(VClippy *_data) : CWBase() {
    name = "Layers";
    data = _data;
    currentLayer = 0;
    mouseOverLayer = 0;
    lastMouseOverLayer = 0;
    mouseOverVisible = false;
    mouseOverSelected = false;
    cMnuBar = new CMnuBar();
    opacity = new VHSlider();
    scroll = new VVSlider();
  }
  virtual ~CWLayer() {
    delete cMnuBar;
    delete opacity;
    delete scroll;
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    scroll->value = 0;
  }

  void init();
  void resize();
  void rePosition();
};

#endif //__C_WLAYER_HPP__