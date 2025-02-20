#include "vstrlst.hpp"
#include "blit.hpp"
#include "constant.hpp"
#include "input.hpp"

void VStrLst::draw() {
  pushRect(position.x,position.y-CHAR_HEIGHT,position.x+size.width,position.y+size.height);
  drawBox(position.x,position.y,position.x+size.width,position.y+size.height,backgroundColor);
  if (hasKeyboardFocus(this))
    drawBox(position.x,position.y-CHAR_HEIGHT,position.x+size.width,position.y,COLOR_HEADERLINE);

  textLastLine = -1;
  do {
    textLastLine++;
  } while (items->getString(0,textLastLine)!=NIL);
  if (textLastLine>0) {
    scrollBoxY = scrollY*size.height/(textLastLine*CHAR_HEIGHT);
    drawBox(position.x+size.width-4*frameBufferWidth/320,position.y+scrollBoxY,position.x+size.width,position.y+scrollBoxY+2*frameBufferHeight/200,COLOR_SCROLL);
  }

  int sort = -1;
  for (int xi = 0; xi < listItemCountX; xi++) {
    Point d0(position.x+separators[xi]-scrollX,position.y-CHAR_HEIGHT);
    Point d1(position.x+separators[xi+1]-scrollX,position.y);
    if (d1.x>position.x+size.width) d1.x=position.x+size.width;
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
    if (c1.x>position.x+size.width) c1.x=position.x+size.width;
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
        drawString(x+1,y, k, textColor);
      yi++;
    } while (y < clipRect.y1);
    popRect();
  }
  popRect();
  if (firstVisibleLine != -1 && selectedLine <= firstVisibleLine) {scrollY-=(firstVisibleLine-selectedLine)*CHAR_HEIGHT; if (scrollY<0) scrollY = 0;}
  if (lastVisibleLine != -1 && selectedLine >= lastVisibleLine-1) {scrollY+=(selectedLine-lastVisibleLine+1)*CHAR_HEIGHT;}
  if (sort!=-1) {
    items->sort(sort,true);
    do {
      updateMouse();
    } while(mouseB==1);
  }
}

int VStrLst::update() {
  if (mouseY>=position.y&&mouseY<position.y+size.height&&mouseX>position.x+size.width-4*frameBufferWidth/320&&mouseX<position.x+size.width) {
    if (mouseB == 1) {
      scrollY = (mouseY-position.y)*textLastLine*CHAR_HEIGHT/size.height;
    }
  }
  if (hasKeyboardFocus(this)) {
    int selectedLineBefore = selectedLine;
    if (key == VK_UP) {selectedLine--;} 
    if (key == VK_DOWN) {selectedLine++;}
    if (key == VK_PAGEUP) {selectedLine-=16; if (selectedLine < 0) selectedLine = 0;}
    if (key == VK_PAGEDOWN) {for (int i = 0; i < 16; i++) {if (items->getString(0,selectedLine+1)!=NIL) selectedLine++;}}
    if (key == VK_RIGHT) {scrollX+=CHAR_HEIGHT;if (scrollX+size.width>separators[listItemCountX])scrollX=separators[listItemCountX]-size.width;if (scrollX<0) scrollX = 0;}
    if (key == VK_LEFT) {scrollX-=CHAR_HEIGHT;if(scrollX<0)scrollX=0;}
    if (shiftKey && selectedLine>=0) {
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
    if (items->getString(0,selectedLine)==NIL) selectedLine--;
    if (selectedLine < 0) selectedLine = 0;
  }
  return 0;
}

void VStrLst::setList(IStrLst *list, int listWidth) {
  items = list;
  listItemCountX = listWidth;
  separators.resize(listWidth+1);
  int here = 0;
  selectedLine = 0;
  scrollY = 0;
  scrollX = 0;
  scrollBoxY = 0;
  textLastLine = 0;
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