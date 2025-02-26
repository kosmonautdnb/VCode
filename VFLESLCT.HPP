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
    name = "File select";
    fileNameSelected="";
    directory = NULL;
    modeLoading = true;
  };
  virtual ~VFleSlct() {
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    fileNameSelected="";
  }

  virtual void setList(IStrLst *list, int listWidth);

  void init(const String &path);
};

#endif __VFLESLCT_HPP__