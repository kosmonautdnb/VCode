#include "c_mnubar.hpp"
#include "blit.hpp"
#include "constants.hpp"
#include "colors.hpp"
#include "input.hpp"
#include "util.hpp"

void CMnuBar::draw() {
  textToItems();
  pushRect(position,size);
  drawBox(position.x,position.y,position.x+size.width,position.y+CHAR_HEIGHT,COLOR_TEXTBACKGROUND);
  Point pos = position;
  mouseOverX = -1;
  mouseOverY = -1;
  for (int x = 0; x < menu.size(); x++) {
    CMnuTab *tab = &menu[x];
    String tabText = tab->name;
    uint32_t colB = COLOR_WHITE;
    uint32_t colT = COLOR_BLACK;
    if (x == selectedX) {
      colB=COLOR_BLACK;
      colT=COLOR_WHITE;
      Array<CMnuItem> *itms = &menu[x].items;
      int y;
      int maxWidth = 0;
      for (y = 0; y < itms->size(); y++) 
        if ((*itms)[y].name.pixelWidth() > maxWidth) maxWidth = (*itms)[y].name.pixelWidth();
      Point p2 = pos;
      p2.y += CHAR_HEIGHT;
      for (y = 0; y < itms->size(); y++) {
        CMnuItem *itm = &(*itms)[y];
        drawBox(p2.x-1,p2.y,p2.x+maxWidth+1,p2.y+CHAR_HEIGHT+1,COLOR_WHITE);
        p2.y += CHAR_HEIGHT;
      }
      p2 = pos;
      p2.y += CHAR_HEIGHT;
      for (y = 0; y < itms->size(); y++) {
        uint32_t colB = COLOR_WHITE;
        uint32_t colT = COLOR_BLACK;
        CMnuItem *itm = &(*itms)[y];
        bool mse=mouseOver(Rect(p2.x,p2.y,p2.x+maxWidth,p2.y+CHAR_HEIGHT));
        if (mse) {
          mouseOverX=x;
          mouseOverY=y;
        }
        if (mse||selectedY==y) {
          colB = COLOR_BLACK;
          colT = COLOR_WHITE;
          drawBox(p2.x-1,p2.y,p2.x+maxWidth+1,p2.y+CHAR_HEIGHT,colB);
        }
        drawString(p2.x,p2.y,itm->name,colT);
        p2.y += CHAR_HEIGHT;
      }
    }
    bool mse=mouseOver(Rect(pos.x,pos.y,pos.x+tabText.pixelWidth(),pos.y+CHAR_HEIGHT));
    if (mse) {
      colB=COLOR_BLACK;
      colT=COLOR_WHITE;
      mouseOverX=x;
      mouseOverY=-1;
    }
    drawBox(pos.x,pos.y,pos.x + tabText.pixelWidth(), pos.y + CHAR_HEIGHT, colB);
    drawString(pos.x,pos.y,tabText,colT);
    pos.x += tabText.pixelWidth() + 10;
  }
  popRect();
}

bool hotKeyPressed(const String &hotKey) {
  if (hotKey.empty()) return false;
  String lower = toLower(hotKey);
  if (lower == "esc") return key == VK_ESCAPE;
  if (lower == "altf") return key == VK_F_ALT;
  return key == hotKey[0];
}

int CMnuBar::update() {
  if (!clickedFunctionName.empty()) {return 0;}

  // hotKeys
  for (int x = 0; x < menu.size(); x++) {
    if (hotKeyPressed(menu[x].hotKey)) {selectedX = x; selectedY = -1; key = VK_ENTER;}
    for (int y = 0; y < menu[x].items.size(); y++) {
      if (hotKeyPressed(menu[x].items[y].hotKey)) {selectedX = x; selectedY = y; key = VK_ENTER;}
    }
  }

  // mouse
  if (mouseB==1) {
    if (mouseOverX!=-1) {
      selectedX = mouseOverX;
      selectedY = mouseOverY;
    } else {
      selectedX = -1;
      selectedY = -1;
    }
  }

  // cursor
  if (selectedX != -1) {
    if (key == VK_DOWN) {selectedY++; key = VK_NONE;}
    if (key == VK_UP) {selectedY--;; key = VK_NONE;}
    if (key == VK_RIGHT) {selectedY=-1;selectedX++;key = VK_NONE;}
    if (key == VK_LEFT) {selectedY=-1;selectedX--;key = VK_NONE;}
    if (selectedX<0) selectedX = 0;
    if (selectedX>=menu.size()) selectedX = menu.size()-1;
    if (selectedX<menu.size()&&selectedX!=-1) 
      if (selectedY>=menu[selectedX].items.size()) 
        selectedY=menu[selectedX].items.size()-1;
    if (selectedY<0) selectedY = -1;
    if (key == VK_ESCAPE) {selectedX = -1; key = VK_NONE;}
    if (key == VK_ENTER || mouseB==1) {
      if (selectedY>=0) {
        clickedFunctionName = menu[selectedX].items[selectedY].functionName;
        selectedX = -1;
        selectedY = -1;
        key = VK_NONE;
      }
    }
  }

  if (selectedX!=-1) return 1;
  return 0;
}

void CMnuBar::init(const String &_text) {
  text = _text;
  reset();
}

void CMnuBar::textToItems() {

  menu.clear();
  menu.push_back(CMnuTab());
  int id = 0;

  for (int i = 0; i < text.length(); i++) {
    char c = text[i];

    if (c == '+') {

      i++;
      menu.back().items.push_back(CMnuItem());
      CMnuItem  *menuItem = &(menu.back().items.back());
      id++;menuItem->id = id;

      for(;i<text.length();i++) {
        c=text[i];
        if (c == '+') {
          menu.back().items.push_back(CMnuItem());
          menuItem = &(menu.back().items.back());
          id++;menuItem->id = id;
          continue;
        }
        if (c == '[') {
          i++;
          String *functionName = &(menuItem->functionName);
          for(;i<text.length();i++) {
            c=text[i];
            if (c == ']') {
              break;
            }
            *functionName+=String::chr(c);
          }
          continue;
        }
        if (c == '(') {
          i++;
          String *hotKey = &(menuItem->hotKey);
          for(;i<text.length();i++) {
            c=text[i];
            if (c == ')') {
              break;
            }
            *hotKey+=String::chr(c);
          }
          menuItem->name += "[" + menuItem->hotKey + "]";
          continue;
        }
        if (c == '\n') {break;}
        String *name = &(menuItem->name);
        *name+=String::chr(c);
      }

    }

    if (c == '\n') {
      menu.push_back(CMnuTab());
      continue;
    }

    if (c == '(') {
      i++;
      String *hotKey = &(menu.back().hotKey);
      for(;i<text.length();i++) {
        c=text[i];
        if (c == ')') {
          break;
        }
        *hotKey+=String::chr(c);
      }
      menu.back().name += "["+menu.back().hotKey+"]";
      continue;
    }

    menu.back().name += String::chr(c);

  }

  menu.pop_back();
}
