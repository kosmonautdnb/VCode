#include "vvslider.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "input.hpp"


void VVSlider::draw() {
  pushRect(position.x,position.y,position.x+size.width,position.y+size.height);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height, COLOR_HEADERLINE);
  int k = position.y + (value * (size.height-1))/(vmax-vmin);
  drawBox(position.x,k,position.x+size.width,k+2,COLOR_WHITE);
  popRect();
}

int VVSlider::update() {
  lastValue=value;
  if (grabbed) {
    if (mouseB==1) {
      int mouseDY = mouseY-position.y;
      value = mouseDY*(vmax-vmin)/(size.height-1)+vmin;
      if (value<vmin) value = vmin;
      if (value>vmax) value = vmax;
    }
  }
  if (mouseIn()&&mouseB==1&&lastMouseB==0)
    grabbed = true;
  if (mouseB!=1)
    grabbed = false;
  return grabbed ? 1 : 0;
}


bool VVSlider::hasChanged() {
  return value != lastValue;
}

void VVSlider::init(float _value, float _vmin, float _vmax) {
  value = _value;
  vmin = _vmin;
  vmax = _vmax;
}