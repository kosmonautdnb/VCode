#ifndef __VALERT_HPP__
#define __VALERT_HPP__

#include "vbase.hpp"
#include "vbutton.hpp"
#include "string.hpp"
#include "array.hpp"

class VAlert : public VBase {
public:
  String text;
  Array<VButton*> buttons;
  int keyboardId;

  VAlert() : VBase() {
    text = "";
    keyboardId = 0;
  }
  virtual ~VAlert() {
    for (int i = 0; i < buttons.size(); i++) {
      delete buttons[i];
    }
  }

  virtual setRect(int x0, int y0, int x1, int y1) {VBase::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VBase::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VBase::setRect(p0,p1);}
  virtual void draw();
  virtual int update();
  virtual void reset() {}
  virtual int doModal() {
    return VBase::doModal();
  }

  static int box(const String &message,const Array<VButton*> _buttons) {
    VAlert *alert = new VAlert();
    alert->init(message,_buttons);
    int r = 0;
    do {r = alert->doModal();} while(1!=1);
    delete alert;
    return r;
  }

  static int box(const String &message,const String &button1) {
    VAlert *alert = new VAlert();
    Array<VButton*> _buttons;
    _buttons.push_back(new VButton());
    _buttons[0]->init(button1);
    return VAlert::box(message,_buttons);
  }

  static int box(const String &message,const String &button1,const String &button2) {
    VAlert *alert = new VAlert();
    Array<VButton*> _buttons;
    _buttons.push_back(new VButton());
    _buttons.push_back(new VButton());
    _buttons[0]->init(button1);
    _buttons[1]->init(button2);
    return VAlert::box(message,_buttons);
  }

  void init(const String &_text, const Array<VButton*> &_buttons);
};

#endif //__VALERT_HPP__