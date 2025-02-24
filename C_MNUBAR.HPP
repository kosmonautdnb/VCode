#ifndef __C_MNUBAR_HPP__
#define __C_MNUBAR_HPP__

#include "vbase.hpp"
#include "string.hpp"

class CMnuBar : public VBase {
public:
  String text;

  String clickedFunctionName;
  int mouseSelectedOn;
  int cursorX;
  int cursorY;
  int selectedX;
  int selectedY;   
  bool mouseOverItem;

  CMnuBar() : VBase() {
    clickedFunctionName = "";
    mouseSelectedOn = -1;
    cursorX = -1;
    cursorY = -1;
    selectedX = -1;
    selectedY = -1;
    mouseOverItem = false;
  }

  virtual ~CMnuBar() {
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    clickedFunctionName = "";
    mouseSelectedOn = -1;
    cursorX = -1;
    cursorY = -1;
    selectedX = -1;
    selectedY = -1;
  }

  void init(const String &_text);
};

#endif //__C_MNUBAR_HPP__