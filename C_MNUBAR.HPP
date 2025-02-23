#ifndef __C_MNUBAR_HPP__
#define __C_MNUBAR_HPP__

#include "vbase.hpp"
#include "string.hpp"

class CMnuBar : public VBase {
public:
  String text;

  int mouseOverX;
  int mouseOverY;
  int mouseClickedX;
  int mouseClickedY;
  int mouseSelectedOn;
  int cursorX;
  int cursorY;
  
  CMnuBar() : VBase() {
    cursorX = -1;
    cursorY = -1;
    mouseOverX = -1;
    mouseOverY = -1;
    mouseClickedX = -1;
    mouseClickedY = -1;
    mouseSelectedOn = -1;
  }

  virtual ~CMnuBar() {
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    cursorX = -1;
    cursorY = -1;
    mouseOverX = -1;
    mouseOverY = -1;
    mouseClickedX = -1;
    mouseClickedY = -1;
    mouseSelectedOn = -1;
  }

  void init(const String &_text);
};

#endif //__C_MNUBAR_HPP__
