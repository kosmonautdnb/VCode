#ifndef __VVSLIDER_HPP__
#define __VVSLIDER_HPP__

#include "vbase.hpp"

class VVSlider : public VBase {
public:
  float value;
  float vmin;
  float vmax;
  float lastValue;

  VVSlider() : VBase() {
    value = 0;
    lastValue = 0;
    vmin = 0;
    vmax = 0;
  }
  virtual ~VVSlider() {;}

  virtual void draw();
  virtual int update();

  bool hasChanged();

  void init(float _value, float _vmin, float _vmax);
};

#endif //__VVSLIDER_HPP__