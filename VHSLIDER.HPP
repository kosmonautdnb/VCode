#ifndef __VHSLIDER_HPP__
#define __VHSLIDER_HPP__

#include "vbase.hpp"

class VHSlider : public VBase {
public:
  float value;
  float vmin;
  float vmax;
  float lastValue;
  bool grabbed;

  VHSlider() : VBase() {
    value = 0;
    lastValue = 0;
    vmin = 0;
    vmax = 0;
    grabbed = false;
  }
  virtual ~VHSlider() {;}

  virtual void draw();
  virtual int update();

  bool hasChanged();

  void init(float _value, float _vmin, float _vmax);
};

#endif //__VHSLIDER_HPP__