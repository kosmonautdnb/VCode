#include "vhslider.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "input.hpp"


void VHSlider::draw() {
  pushRect(position.x,position.y,position.x+size.width,position.y+size.height);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height, COLOR_HEADERLINE);
  int k = position.x + (value * (size.width-1))/(vmax-vmin);
  drawBox(k,position.y,k+2,position.y+size.height,COLOR_WHITE);
  popRect();
}

int VHSlider::update() {
  lastValue=value;
  if (grabbed) {
    if (mouseB==1) {
      int mouseDX = mouseX-position.x;
      value = mouseDX*(vmax-vmin)/(size.width-1)+vmin;
    }
  }
  if (mouseIn()&&mouseB==1&&lastMouseB==0)
    grabbed = true;
  if (mouseB!=1)
    grabbed = false;
  return grabbed ? 1 : 0;
}


bool VHSlider::hasChanged() {
  return value != lastValue;
}

void VHSlider::init(float _value, float _vmin, float _vmax) {
  value = _value;
  vmin = _vmin;
  vmax = _vmax;
}