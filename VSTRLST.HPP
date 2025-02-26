#ifndef __VSTRLST_HPP__
#define __VSTRLST_HPP__

#include "vbase.hpp"
#include "array.hpp"
#include "string.hpp"
#include "colors.hpp"
#include "istrlst.hpp"

class VStrLst : public VBase {
public:
  unsigned int backgroundColor;
  unsigned int textColor;
  int listItemCountX;
  IStrLst *items;
  Array<int> separators;
  int selectedLine;
  int firstVisibleLine;
  int lastVisibleLine;
  int scrollY;
  int scrollX;
  int scrollBoxY;
  int textLastLine;

  VStrLst() : VBase() {
    backgroundColor = COLOR_MENUBACKGROUND;
    textColor = COLOR_MENUTEXT;
    listItemCountX = 0;
    items = NULL;
    selectedLine = 0;
    firstVisibleLine = 0;
    lastVisibleLine = 0;
    scrollY = 0;
    scrollX = 0;
    scrollBoxY = 0;
    textLastLine = 0;
  }
  virtual ~VStrLst() {;}

  virtual void draw();
  virtual int update();

  virtual void setList(IStrLst *list, int listWidth);
};

#endif //__VSTRLST_HPP__