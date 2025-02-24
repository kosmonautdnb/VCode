#ifndef __VFLESLCT_HPP__
#define __VFLESLCT_HPP__

#include "vstrlst.hpp"
#include "rect.hpp"
#include "string.hpp"
#include "ifiles.hpp"

class VFleSlct : public VStrLst {
public:
  IFiles *directory;
  String lastPathName;
  bool modeLoading;
  String fileNameSelected;

  VFleSlct() : VStrLst() {
    name = "FileSelect";
    fileNameSelected="";
    directory = NULL;
    modeLoading = true;
  };
  virtual ~VFleSlct() {
  }

  virtual setRect(int x0, int y0, int x1, int y1) {VStrLst::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VStrLst::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VStrLst::setRect(p0,p1);}

  virtual void draw();
  virtual int update();

  virtual void setList(IStrLst *list, int listWidth);

  void init(const String &path);
};

#endif __VFLESLCT_HPP__