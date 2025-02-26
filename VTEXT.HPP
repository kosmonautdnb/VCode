#ifndef __VTEXT_HPP__
#define __VTEXT_HPP__

#include "vbase.hpp"
#include "string.hpp"
#include "constants.hpp"
#include "input.hpp"
#include "blit.hpp"
#include "vga.hpp"

class VText : public VBase {
public:
  String text;
  String colors;
  String fileName;
  Point textPos;
  int scrollBoxY;
  String toFind;
  int findAddress;

  VText() : VBase() {
    name = "View";
    text = "";
    colors = "";
    fileName = "";
    textPos = Point(0,0);
    scrollBoxY = 0;
    toFind = "";
    findAddress=0;
  }
  virtual ~VText() {;}

  virtual void draw();
  virtual int update();
  virtual void reset() {
    textPos = Point(0,0);
    scrollBoxY = 0;
    toFind = "";
    findAddress=0;
  }
  virtual int doModal() {
        return VBase::doModal();
  }
  void init(const String &_text, const String &_fileName);
  bool find(const String &search, unsigned int address);
};

#endif //__VTEXT_HPP__