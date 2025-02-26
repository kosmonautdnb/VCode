#ifndef __VTXTLINE_HPP__
#define __VTXTLINE_HPP__

#include "vbase.hpp"
#include "string.hpp"

class VTxtLine : public VBase {
public:
  String text;
  int cursorX;
  int scrollX;

  VTxtLine() : VBase() {
    text = "";
    cursorX = 0;
    scrollX = 0;
  }
  virtual ~VTxtLine() {;}

  virtual void draw();
  virtual int update();

  void init(const String &_text);
};

#endif //__VTXTLINE_HPP__