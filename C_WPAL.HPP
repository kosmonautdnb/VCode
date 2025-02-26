#ifndef __C_WPAL_HPP__
#define __C_WPAL_HPP__

#include "c_wbase.hpp"

class CWPal : public CWBase {
public:

  int foregroundColor;
  int mouseOverColor;

  CWPal() : CWBase() {
    foregroundColor = 0;
    mouseOverColor = 0;
    name = "Palette";
  }
  virtual ~CWPal() {
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    foregroundColor = 0;
    mouseOverColor = 0;
  }
};

#endif //__C_WPAL_HPP__