#ifndef __VTREE_HPP__
#define __VTREE_HPP__

#include "vstrlst.hpp"
#include "rect.hpp"
#include "string.hpp"

class IFiles;
class VPreview;
class VText;
class VCode;
class VHex;

class VTree : public VStrLst {
public:
  IFiles *directory;
  VPreview *preview;
  VText *vText;
  VCode *vCode;
  VHex *vHex;
  String searchText;
  bool reload;
  String lastPathName;

  VTree() : VStrLst() {
    directory = NULL;
    preview = NULL;
    vText = NULL;
    vCode = NULL;
    vHex = NULL;
    searchText = "";
    reload = false;
    lastPathName = "";
  };

  virtual setRect(int x0, int y0, int x1, int y1) {VStrLst::setRect(x0,y0,x1,y1);}
  virtual setRect(const Point &p0, const Size &size) {VStrLst::setRect(p0,size);}
  virtual setRect(const Point &p0, const Point &p1) {VStrLst::setRect(p0,p1);}

  virtual void draw();
  virtual void update();

  virtual void setList(IStrLst *list, int listWidth);

  void init(const Rect &r,const String &path);

  void searchTextUpdated();
};

#endif __VTREE_HPP__