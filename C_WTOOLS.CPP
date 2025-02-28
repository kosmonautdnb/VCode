#include "c_wtools.hpp"
#include "blit.hpp"
#include "colors.hpp"
#include "constant.hpp"
#include "input.hpp"

typedef struct ToolEntry {
  const char *name;
} ToolEntry;

ToolEntry toolNames[] = {
  {"Move"},
  {"Pen"},
  {""},
  {""},
  {""},
  {""},
  {""},
  {""}
};

static void drawRectangle2(int x0, int y0, int x1, int y1, unsigned int color1, unsigned int color2) {
  for (int y = y0; y < y1; y++) {
    setPixel(x0,y,color1);
    if (x1-1>x0) setPixel(x1-1,y,color2);
  }
  for (int x = x0; x < x1; x++) {
    setPixel(x,y0,color1);
    if (y1-1>y0) setPixel(x,y1-1,color2);
  }
}


void CWTools::draw() {
  CWBase::draw();
  size.height = size.width * frameBufferWidth * CWTOOLSY / CWTOOLSX / frameBufferHeight;
  pushRect(position.x,position.y,position.x+size.width,position.y+size.height);

  mouseOverTool=-1;
  int t = 0;
  for (int y = 0; y < CWTOOLSY; y++) {
    for (int x = 0; x < CWTOOLSX; x++) {
      if(paintIcon(
        position.x+x*size.width/CWTOOLSX,
        position.y+y*size.height/CWTOOLSY,
        position.x+(x+1)*size.width/CWTOOLSX,
        position.y+(y+1)*size.height/CWTOOLSY,toolNames[t].name,t==currentTool)) mouseOverTool=t;
      t++;
    }
  }

  popRect();
}

int CWTools::update() {
  int r = CWBase::update();
  if (r) return r;
  if (mouseB==0&&lastMouseB==1&&mouseOverTool!=-1) 
    currentTool = mouseOverTool;
  return 0;
}

void CWTools::init() {
}

bool CWTools::paintIcon(int x0,int y0,int x1,int y1,const String &name,bool selected) {
  bool over = mouseOver(Rect(x0,y0,x1,y1));
  pushRect(x0,y0,x1,y1);
  drawBox(x0,y0,x1,y1,selected ? 0xffff50ff:(over?0xffffffc0:0xffffff40));
  for (int i = 0; i < 2; i++)
    drawRectangle2(x0+i,y0+i,x1-i,y1-i,0xffff40c0,COLOR_BLACK);
  drawString(x0,y1-CHAR_HEIGHT+1,name,COLOR_BLACK);
  drawString(x0,y1-CHAR_HEIGHT,name,COLOR_WHITE);
  popRect();
  return over;
}

