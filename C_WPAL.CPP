#include "c_wpal.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "colors.hpp"
#include "vga.hpp"
#include "input.hpp"

void CWPal::draw() {
  String n = name;
  name += " #" + String::hex(mouseOverColor,8);
  CWBase::draw();
  name = n;
  pushRect(position.x,position.y,position.x+size.width,position.y+size.height);
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < 256; i++) {
      int cx = i % 16;
      int cy = i / 16;
      int x0 = position.x + cx * size.width/16;
      int y0 = position.y + cy * size.height/16;
      int x1 = position.x + (cx+1) * size.width/16;
      int y1 = position.y + (cy+1) * size.height/16;
      if (j == 0) {
        drawBox(x0,y0,x1,y1,palette[i]);
        if (mouseOver(Rect(x0,y0,x1,y1))) {
          mouseOverColor = palette[i];
        }
      }
      if (j == 1 && i == foregroundColor) {
        drawRectangle(x0-2,y0-2,x1+2,y1+2,COLOR_RED);
        drawRectangle(x0-1,y0-1,x1+1,y1+1,COLOR_WHITE);
      }
      if (j == 1 && i == getPaletteIndex(mouseOverColor)) {
        drawRectangle(x0-1,y0-1,x1+1,y1+1,COLOR_BLACK);
      }
    }
  }
  popRect();
}

int CWPal::update() {
  int r = CWBase::update();
  if (r) return r;
  if (mouseIn() && mouseB==1) {
    foregroundColor = getPaletteIndex(mouseOverColor);
    return 1;
  }
  if (lastMouseB==2&&mouseB==0)  {
    foregroundColor = getPaletteIndex(mouseOverColor);
    rightClicked = true;
    return 0;
  }
  return mouseIn() ? 1 : 0;
}
