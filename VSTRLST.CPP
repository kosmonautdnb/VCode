#include "vstrlst.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "input.hpp"

void VStrLst::draw() {
  update();
  pushRect(position.x,position.y-CHAR_HEIGHT,position.x+size.width,position.y+size.height);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,backgroundColor);
  drawBox(position.x,position.y-CHAR_HEIGHT,position.x+size.width,position.y,COLOR_HEADERLINE);
  int sort = -1;
  for (int xi = 0; xi < listItemCountX; xi++) {
    Point d0(position.x+separators[xi]-scrollX,position.y-CHAR_HEIGHT);
    Point d1(position.x+separators[xi+1]-scrollX,position.y);
    pushRect(d0,d1);
    drawString(d0.x,d0.y,items->getString(xi,-1),COLOR_HEADERTEXT);
    if (mouseOver(Rect(d0,d1))) {
      if (mouseB==1) {
        sort = xi;
        setKeyboardFocus(this);
      }
    }
    popRect();
    Point c0(position.x+separators[xi]-scrollX,position.y);
    Point c1(position.x+separators[xi+1]-scrollX,position.y+size.height);
    pushRect(c0,c1);
    int yi = 0;
    firstVisibleLine = -1;
    lastVisibleLine = -1;
    int y,x;
    do {
      x = c0.x;
      y = yi * CHAR_HEIGHT + c0.y - scrollY;
      if (y >= clipRect.y0 && (firstVisibleLine == -1)) firstVisibleLine = yi;
      if (y >= clipRect.y1-CHAR_HEIGHT && (lastVisibleLine == -1)) lastVisibleLine = yi;
      String k = items->getString(xi,yi);
      if (k != NIL && mouseOver(Rect(position.x,y,position.x+size.width,y+CHAR_HEIGHT))) {
        drawBox(c0.x,y,c1.x,y+CHAR_HEIGHT,COLOR_MOUSEOVERLINE);
        if (mouseB==1) {
          selectedLine = yi;
          setKeyboardFocus(this);
        }
      }
      if (items->selected(yi)) {
          drawBox(c0.x,y,c1.x,y+CHAR_HEIGHT,COLOR_SELECTEDLINE);
      }
      if (yi == selectedLine) {
        if (hasKeyboardFocus(this))
          drawBox(c0.x,y,c1.x,y+CHAR_HEIGHT,items->selected(yi) ? COLOR_CURRENTLINESELECTED : COLOR_CURRENTLINE);
      }
      if (k != NIL)
        drawString(x,y, k, textColor);
      yi++;
    } while (y < clipRect.y1);
    popRect();
  }
  popRect();
  if (firstVisibleLine != -1 && selectedLine <= firstVisibleLine) {scrollY-=(firstVisibleLine-selectedLine)*CHAR_HEIGHT; if (scrollY<0) scrollY = 0;}
  if (lastVisibleLine != -1 && selectedLine >= lastVisibleLine-1) {scrollY+=(selectedLine-lastVisibleLine+1)*CHAR_HEIGHT;}
  if (sort!=-1) {
    items->sort(sort,true);
  }
}

void VStrLst::update() {
  if (hasKeyboardFocus(this)) {
    int selectedLineBefore = selectedLine;
    if (key == VK_UP) {selectedLine--; if (selectedLine < 0) selectedLine = 0;}
    if (key == VK_DOWN) {if (items->getString(0,selectedLine+1)!=NIL) selectedLine++;}
    if (key == VK_PAGEUP) {selectedLine-=16; if (selectedLine < 0) selectedLine = 0;}
    if (key == VK_PAGEDOWN) {for (int i = 0; i < 16; i++) {if (items->getString(0,selectedLine+1)!=NIL) selectedLine++;}}
    if (key == VK_RIGHT) {scrollX+=CHAR_HEIGHT;if (scrollX+size.width>separators[listItemCountX])scrollX=separators[listItemCountX]-size.width;if (scrollX<0) scrollX = 0;}
    if (key == VK_LEFT) {scrollX-=CHAR_HEIGHT;if(scrollX<0)scrollX=0;}
    if (shiftKey) {
      if (selectedLineBefore != selectedLine) {
        if (selectedLineBefore<selectedLine) {
          for (int i = selectedLineBefore; i < selectedLine; i++) {
            items->select(i,!items->selected(i));
          }
        } else {
          for (int i = selectedLine+1; i <= selectedLineBefore; i++) {
            items->select(i,!items->selected(i));
          }
        }
      }
    }
  }
}

void VStrLst::setList(IStrLst *list, int listWidth) {
  items = list;
  listItemCountX = listWidth;
  separators.resize(listWidth+1);
  int here = 0;
  selectedLine = 0;
  scrollY = 0;
  scrollX = 0;
  list->assigned();
  for (int i = 0; i < separators.size(); i++) {
    separators[i] = here;
    if(i==separators.size()-1) break;
    int width = 0;
    int y = -1;
    do {
      String k = list->getString(i,y);
      if (k == NIL) break;
      int kWidth = k.pixelWidth();
      if (kWidth > width) width = kWidth;
      y++;
    } while(1==1);
    here += width+2;
  }
}