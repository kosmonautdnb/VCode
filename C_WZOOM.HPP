#ifndef __C_WZOOM_HPP__
#define __C_WZOOM_HPP__

#include "c_wbase.hpp"

#define CWZOOMX (3*2+1)
#define CWZOOMY (3*2+1)

class CWZoom : public CWBase {
public:

  uint32_t zoom[CWZOOMX*CWZOOMY];


  CWZoom() : CWBase() {
    name = "Zoom";
  }
  virtual ~CWZoom() {
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
  }
};

#endif //__C_WZOOM_HPP__