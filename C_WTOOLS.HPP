#ifndef __C_WTOOLS_HPP__
#define __C_WTOOLS_HPP__

#include "c_wbase.hpp"

#define CWTOOLSX 2
#define CWTOOLSY 4

class CWTools : public CWBase {
public:
  int currentTool;
  int mouseOverTool;
  enum {
    TOOL_MOVE=0,
    TOOL_PEN=1,
  };

  CWTools() : CWBase() {
    name = "Tools";
    currentTool = TOOL_PEN;
    mouseOverTool=-1;
  }
  virtual ~CWTools() {
  }

  virtual void draw();
  virtual int update();
  virtual void reset() {
    currentTool = TOOL_PEN;
    mouseOverTool=-1;
  }

  void init();
  void resize();
  
  bool paintIcon(int x0,int y0,int x1,int y1,const String &name, bool selected);
};

#endif //__C_WTOOLS_HPP__