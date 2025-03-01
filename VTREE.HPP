#ifndef __VTREE_HPP__
#define __VTREE_HPP__

#include "vstrlst.hpp"
#include "rect.hpp"
#include "string.hpp"
#include "ifiles.hpp"
#include "vpreview.hpp"
#include "vtext.hpp"
#include "vcode.hpp"
#include "vhex.hpp"
#include "vtxtalrt.hpp"
#include "vdisasm.hpp"
#include "vclippy.hpp"

class VTree : public VStrLst {
public:
  IFiles *directory;
  VPreview *vPreview;
  VText *vText;
  VCode *vCode;
  VHex *vHex;
  VTxtAlrt *vTxtAlrt;
  VDisAsm *vDisAsm;
  VClippy *vClippy;
  String lastMask;
  String searchText;
  bool reload;
  String lastPathName;
  enum {
    TREEMODE_FILETREE = 0,
    TREEMODE_EDIT = 1
  };
  int treeMode;

  VTree() : VStrLst() {
    name = "Tree";
    directory = NULL;
    vPreview = new VPreview();
    vText = new VText();
    vCode = new VCode();
    vHex = new VHex();
    vTxtAlrt = new VTxtAlrt();
    vDisAsm = new VDisAsm();
    vClippy = new VClippy();
    searchText = "";
    reload = false;
    lastPathName = "";
    treeMode = TREEMODE_FILETREE;
    lastMask = "*.*";
  };
  virtual ~VTree() {
    delete vPreview;
    delete vText;
    delete vCode;
    delete vHex;
    delete vTxtAlrt;
    delete vDisAsm;
    delete vClippy;
  }

  virtual void draw();
  virtual int update();

  virtual void setList(IStrLst *list, int listWidth);

  void init(const Rect &r,const String &path);

  void searchTextUpdated();
  void overlayByFileName(const String &fileName);
  void updateSelection(const String &mask, bool doSelect);
};

extern VTree *treeLeft;
extern VTree *treeRight;

#endif __VTREE_HPP__