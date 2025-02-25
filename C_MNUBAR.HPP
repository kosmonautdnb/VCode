#ifndef __C_MNUBAR_HPP__
#define __C_MNUBAR_HPP__

#include "vbase.hpp"
#include "string.hpp"
#include "array.hpp"

class CMnuItem {
public:
  String name;
  String functionName;
  String hotKey;
  int id;
};

class CMnuTab {
public:
  String name;
  String hotKey;
  Array<CMnuItem> items;
};

class CMnuBar : public VBase {
public:
  String text;
  Array<CMnuTab> menu;

  String clickedFunctionName;
  int selectedX;
  int selectedY;   
  int mouseOverX;
  int mouseOverY;

  CMnuBar() : VBase() {
    clickedFunctionName = "";
    selectedX = -1;
    selectedY = -1;
    mouseOverX = -1;
    mouseOverY = -1;
  }

  virtual ~CMnuBar() {
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    clickedFunctionName = "";
    selectedX = -1;
    selectedY = -1;
    mouseOverX = -1;
    mouseOverY = -1;
  }

  void init(const String &_text);
  void textToItems();
};

#endif //__C_MNUBAR_HPP__