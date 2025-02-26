#include "c_wzoom.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "colors.hpp"

void CWZoom::draw() {
  CWBase::draw();
  pushRect(position.x,position.y,position.x+size.width,position.y+size.height);
  for (int y = 0; y < size.height; y++) {
    int ky = y * CWZOOMY / size.height;
    bool yMid = (y * CWZOOMY / size.height) == (CWZOOMY-1)/2;
    bool yMidNext = ((y+1) * CWZOOMY / size.height) == (CWZOOMY-1)/2;
    for (int x = 0; x < size.width; x++) {
      int kx = x * CWZOOMX / size.width;
      bool xMid = (x * CWZOOMX / size.width) == (CWZOOMX-1)/2;
      bool xMidNext = ((x+1) * CWZOOMX / size.width) == (CWZOOMX-1)/2;
      uint32_t color = zoom[kx+ky*CWZOOMX];
      if ((xMid != xMidNext)&&(yMid != yMidNext))
        color = COLOR_WHITE;
      setPixel(x+position.x,y+position.y,color);
    }
  }
  popRect();
}

int CWZoom::update() {
  int r = CWBase::update();
  if (r) return r;
  return mouseIn() ? 1 : 0;
}
