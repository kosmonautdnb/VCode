#ifndef __C_WPALED_HPP__
#define __C_WPALED_HPP__

#include "c_mnubar.hpp"
#include "c_wbase.hpp"
#include "vtxtline.hpp"
#include "vhslider.hpp"
#include "vvslider.hpp"
#include "vfleslct.hpp"
#include "vbutton.hpp"

class CWPalEd : public CWBase {
public:
  String clipBoard;
  int startColor;
  int endColor;
  VHSlider *slider_R;
  VHSlider *slider_G;
  VHSlider *slider_B;
  VHSlider *slider_A;
  VVSlider *slider_Intens;
  VTxtLine *enter_R;
  VTxtLine *enter_G;
  VTxtLine *enter_B;
  VTxtLine *enter_A;
  VTxtLine *enter_RGBA;
  int red;
  int green;
  int blue;
  int alpha;
  int rgba;
  int mouseOverColorIndex;
  CMnuBar *cMnuBar;
  String function;
  String currentPath;
  VFleSlct *vFleSlct;
  VButton *vButtonFlip;
  VButton *vButtonCopy;
  unsigned int mouseOverColor;

  CWPalEd() : CWBase() {
    name = "Palette editor";
    startColor = 0;
    endColor = 0;
    slider_R = new VHSlider();
    slider_G = new VHSlider();
    slider_B = new VHSlider();
    slider_A = new VHSlider();
    slider_Intens = new VVSlider();
    enter_R = new VTxtLine();
    enter_G = new VTxtLine();
    enter_B = new VTxtLine();
    enter_A = new VTxtLine();
    enter_RGBA = new VTxtLine();
    red = 0;
    green = 0;
    blue = 0;
    alpha = 0;
    rgba = 0;
    mouseOverColorIndex = 0;
    cMnuBar = new CMnuBar();
    vFleSlct = new VFleSlct();
    vButtonFlip = new VButton();
    vButtonCopy = new VButton();
    mouseOverColor = 0;
  }
  virtual ~CWPalEd() {
    delete slider_R;
    delete slider_G;
    delete slider_B;
    delete slider_A;
    delete slider_Intens;
    delete enter_R;
    delete enter_G;
    delete enter_B;
    delete enter_A;
    delete enter_RGBA;
    delete cMnuBar;
    delete vFleSlct;
    delete vButtonFlip;
    delete vButtonCopy;
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    startColor = 0;
    endColor = 0;
    red = 0;
    green = 0;
    blue = 0;
    alpha = 0;
    rgba = 0;
    mouseOverColorIndex = 0;
    mouseOverColor = 0;
  }

  void setColor(int color);
  void init(const String &path, int selectedColor);

  void loadPalette(const String &fileName);
  void savePalette(const String &fileName);
  void paletteFromPicture(const String &fileName);
};

#endif //__C_WPALED_HPP__