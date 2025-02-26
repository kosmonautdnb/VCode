#ifndef __VPREVIEW_HPP__
#define __VPREVIEW_HPP__

#include "vbase.hpp"
#include "vtext.hpp"
#include "vhex.hpp"
#include "vcode.hpp"
#include "array.hpp"
#include "string.hpp"
#include "colors.hpp"
#include "util.hpp"
#include "dos.hpp"

class VPreview : public VBase {
public:
  String fileName;
  String lastFileName;
  unsigned int *data;

  VPreview() : VBase() {
    fileName = "";
    lastFileName = "";
    data = NULL;
  }

  virtual void draw();
  virtual int update();

  void init(const String &_fileName);
};

#endif //__VPREVIEW_HPP__