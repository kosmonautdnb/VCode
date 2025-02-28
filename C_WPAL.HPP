#ifndef __C_WPAL_HPP__
#define __C_WPAL_HPP__

#include "c_wbase.hpp"
#include "vga.hpp"

class CWPal : public CWBase {
public:

  int foregroundColor;
  int mouseOverColor;
  bool rightClicked;

  CWPal() : CWBase() {
    foregroundColor = getPaletteIndex(0xffffffff);
    mouseOverColor = 0;
    rightClicked = false;
    name = "Palette";
  }
  virtual ~CWPal() {
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    foregroundColor = getPaletteIndex(0xffffffff);
    mouseOverColor = 0;
    rightClicked = false;
  }
};

#endif //__C_WPAL_HPP__