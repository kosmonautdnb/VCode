#ifndef __C_WBASE_HPP__
#define __C_WBASE_HPP__

#include "vbase.hpp"

class CWBase : public VBase {
public:

  String name;
  int lastMouseX;
  int lastMouseY;
  enum {
    MODE_NONE = 0,
    MODE_MOVE = 1,
    MODE_RESIZE = 2,
  };
  int mode;

  CWBase() : VBase() {
    lastMouseX=0;
    lastMouseY=0;
    mode = MODE_NONE;
  }
  virtual ~CWBase() {
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    lastMouseX=0;
    lastMouseY=0;
    mode = MODE_NONE;
  }
};

#endif //__C_WBASE_HPP__